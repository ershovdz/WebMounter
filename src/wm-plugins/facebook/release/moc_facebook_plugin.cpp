/****************************************************************************
** Meta object code from reading C++ file 'facebook_plugin.h'
**
** Created: Sat 10. Nov 02:07:13 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../facebook_plugin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'facebook_plugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FacebookPlugin[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_FacebookPlugin[] = {
    "FacebookPlugin\0"
};

const QMetaObject FacebookPlugin::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_FacebookPlugin,
      qt_meta_data_FacebookPlugin, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FacebookPlugin::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FacebookPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FacebookPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FacebookPlugin))
        return static_cast<void*>(const_cast< FacebookPlugin*>(this));
    if (!strcmp(_clname, "PluginInterface"))
        return static_cast< PluginInterface*>(const_cast< FacebookPlugin*>(this));
    if (!strcmp(_clname, "com.jsoft.webmounter.plugin/1.0"))
        return static_cast< PluginInterface*>(const_cast< FacebookPlugin*>(this));
    return QObject::qt_metacast(_clname);
}

int FacebookPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
