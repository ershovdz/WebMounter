#ifndef QJSON_EXPORT_H
#define QJSON_EXPORT_H

#include <QtCore/qglobal.h>

#ifndef QJSON_EXPORT
# if defined(QJSON_MAKEDLL)
/* We are building this library */
#  define QJSON_EXPORT Q_DECL_EXPORT
# else
/* We are using this library */
#  define QJSON_EXPORT Q_DECL_IMPORT
# endif
#endif

#endif
