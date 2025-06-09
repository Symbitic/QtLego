#!/usr/bin/env bash
#-------------------------------------------------------------------------------------------------------------
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License. See https://go.microsoft.com/fwlink/?linkid=2090316 for license information.
#-------------------------------------------------------------------------------------------------------------
#
# Docs: https://github.com/microsoft/vscode-dev-containers/blob/main/script-library/docs/qt6.md
# Maintainer: The VS Code and Codespaces Teams

set -e

# Clean up
rm -rf /var/lib/apt/lists/*

# User options
VERSION=${VERSION:-"6.8.0"}
HOST=${HOST:-"linux"}
TARGET=${TARGET:-"desktop"}
ARCH=${ARCH:-"linux_gcc_64"}
INSTALL_DIR="${INSTALLDIR:-"/opt"}/Qt"
USERNAME="${USERNAME:-"${_REMOTE_USER}"}"
UPDATE_RC="${UPDATE_RC:-"true"}"

# Support modules
if [ ${#MODULES[@]} -gt 0 ]
then
    QT_MODULES=$(echo ${MODULES[*]} | tr ',' ' ')
    QT_MODULES_FLAG="-m"
    echo "(*) Modules: ${QT_MODULES}"
fi

# Runs apt-get update if needed.
apt_get_update() {
    if [ "$(find /var/lib/apt/lists/* | wc -l)" = "0" ]
    then
        echo "Running apt-get update..."
        apt-get update -y
    fi
}

# Checks if packages are installed and installs them if not.
check_packages() {
    if ! dpkg -s "$@" > /dev/null 2>&1
    then
        apt_get_update
        apt-get -y install --no-install-recommends "$@"
    fi
}

# Update shell config (if enabled).
updaterc() {
    if [ "${UPDATE_RC}" = "true" ]
    then
        if [[ "$(cat /etc/bash.bashrc)" != *"$1"* ]]
        then
            echo "Updating /etc/bash.bashrc..."
            echo -e "$1" >> /etc/bash.bashrc
        fi
        if [ -f "/etc/zsh/zshrc" ] && [[ "$(cat /etc/zsh/zshrc)" != *"$1"* ]]
        then
            echo "Updating /etc/zsh/zshrc..."
            echo -e "$1" >> /etc/zsh/zshrc
        fi
    fi
}

echo "(*) Installing Qt6..."

export DEBIAN_FRONTEND=noninteractive
export QT_ROOT_DIR=${INSTALL_DIR}/${VERSION}/${ARCH/${HOST}_}
export QT_PLUGIN_PATH=${QT_ROOT_DIR}/plugins
export QML2_IMPORT_PATH=${QT_ROOT_DIR}/qml
export PATH=${QT_ROOT_DIR}/bin:${PATH}

if [ -z "${LD_LIBRARY_PATH}" ]
then
  export LD_LIBRARY_PATH="${QT_ROOT_DIR}/lib"
else
  export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${QT_ROOT_DIR}/lib"
fi

if [ "$(id -u)" -ne 0 ]
then
    echo -e 'Script must be run as root. Use sudo, su, or add "USER root" to your Dockerfile before running this script.'
    exit 1
fi

if [ -z "${USERNAME}" ]
then
    echo -e 'Feature script must be executed by a tool that implements the dev container specification. See https://containers.dev/ for more information.'
    exit 1
fi

# Install dependencies
check_packages curl ca-certificates gnupg2 dirmngr unzip build-essential cmake clang-format \
    libgl1-mesa-dev libgstreamer-gl1.0-0 libpulse-dev libxcb-glx0 libxcb-icccm4 libxcb-image0 \
    libxcb-keysyms1 libxcb-randr0 libxcb-render-util0 libxcb-render0 libxcb-shape0 libxcb-shm0 \
    libxcb-sync1 libxcb-util1 libxcb-xfixes0 libxcb-xinerama0 libxcb1 libxkbcommon-dev \
    libxkbcommon-x11-0 libxcb-xkb-dev libxcb-cursor0 python3 python3-pip pipx ninja-build \
    libfontconfig1 libfreetype6

# Ensure that login shells get the correct path if the user updated the PATH using ENV.
rm -f /etc/profile.d/00-restore-env.sh
echo "export PATH=${PATH//$(sh -lc 'echo $PATH')/\$PATH}" > /etc/profile.d/00-restore-env.sh
chmod +x /etc/profile.d/00-restore-env.sh

# Setup pipx
pipx ensurepath
if (( $? > 0 ))
then
    echo "'pipx ensurepath' failed"
    exit 1
fi

# Ensure pipx binaries are in PATH
export PATH=${PATH}:/root/.local/bin

# Install aqtinstall
pipx install aqtinstall
if (( $? > 0 ))
then
    echo "Failed to install aqtinstall"
    exit 1
fi

# Install
echo "(*) Installing Qt6..."
echo aqt install-qt ${HOST} ${TARGET} ${VERSION} ${ARCH} --outputdir "${INSTALL_DIR}" ${QT_MODULES_FLAG} ${QT_MODULES}
aqt install-qt ${HOST} ${TARGET} ${VERSION} ${ARCH} --outputdir "${INSTALL_DIR}" ${QT_MODULES_FLAG} ${QT_MODULES}
if (( $? > 0 ))
then
    echo "Failed to install Qt"
    exit 1
fi

# Add Qt directories into bash/zsh config files (unless disabled)
updaterc "$(cat << EOF
export QT_ROOT_DIR="${QT_ROOT_DIR}"
export QT_PLUGIN_PATH="\${QT_ROOT_DIR}/plugins"
export QML2_IMPORT_PATH="\${QT_ROOT_DIR}/qml"

if [[ "\${PATH}" != *"\${QT_ROOT_DIR}/bin"* ]]
then
    export PATH="\${QT_ROOT_DIR}/bin:\${PATH}"
fi

if [[ -z "\${LD_LIBRARY_PATH}" ]]
then
  export LD_LIBRARY_PATH="\${QT_ROOT_DIR}/lib"
else
  export LD_LIBRARY_PATH="\${LD_LIBRARY_PATH}:\${QT_ROOT_DIR}/lib"
fi

EOF
)"

# Clean up
rm -rf /var/lib/apt/lists/*

echo "Done!"