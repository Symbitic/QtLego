#ifndef QLEGOGLOBAL_H
#define QLEGOGLOBAL_H

#include <QtCore/QtEndian>
#include <QtCore/QtGlobal>
#include <QtCore/QString>
#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE

#ifndef QT_STATIC
#    if defined(QT_BUILD_LEGO_LIB)
#        define Q_LEGO_EXPORT Q_DECL_EXPORT
#    else
#        define Q_LEGO_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define Q_LEGO_EXPORT
#endif

QT_END_NAMESPACE

#endif // QLEGOGLOBAL_H
