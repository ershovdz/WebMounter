/****************************************************************************
** Meta object code from reading C++ file 'facebook_view.h'
**
** Created: Sat 10. Nov 02:07:14 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../view/facebook_view.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'facebook_view.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Ui__FacebookView[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      33,   18,   17,   17, 0x0a,
      71,   53,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Ui__FacebookView[] = {
    "Ui::FacebookView\0\0progress,state\0"
    "updateView(int,int)\0error,login,token\0"
    "oAuthFinished(RESULT,QString,QString)\0"
};

const QMetaObject Ui::FacebookView::staticMetaObject = {
    { &PluginView::staticMetaObject, qt_meta_stringdata_Ui__FacebookView,
      qt_meta_data_Ui__FacebookView, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Ui::FacebookView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Ui::FacebookView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Ui::FacebookView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Ui__FacebookView))
        return static_cast<void*>(const_cast< FacebookView*>(this));
    return PluginView::qt_metacast(_clname);
}

int Ui::FacebookView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PluginView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateView((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: oAuthFinished((*reinterpret_cast< RESULT(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
