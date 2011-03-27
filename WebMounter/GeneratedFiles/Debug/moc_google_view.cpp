/****************************************************************************
** Meta object code from reading C++ file 'google_view.h'
**
** Created: Sun 27. Mar 21:28:16 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../plugins/google/view/google_view.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'google_view.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Ui__GoogleView[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      31,   16,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Ui__GoogleView[] = {
    "Ui::GoogleView\0\0progress,state\0"
    "updateView(int,int)\0"
};

const QMetaObject Ui::GoogleView::staticMetaObject = {
    { &PluginView::staticMetaObject, qt_meta_stringdata_Ui__GoogleView,
      qt_meta_data_Ui__GoogleView, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Ui::GoogleView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Ui::GoogleView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Ui::GoogleView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Ui__GoogleView))
        return static_cast<void*>(const_cast< GoogleView*>(this));
    return PluginView::qt_metacast(_clname);
}

int Ui::GoogleView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PluginView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateView((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
