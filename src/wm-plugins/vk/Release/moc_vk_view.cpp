/****************************************************************************
** Meta object code from reading C++ file 'vk_view.h'
**
** Created: Tue 6. Nov 23:06:09 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../view/vk_view.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vk_view.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Ui__VkView[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      27,   12,   11,   11, 0x0a,
      65,   47,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Ui__VkView[] = {
    "Ui::VkView\0\0progress,state\0"
    "updateView(int,int)\0error,login,token\0"
    "oAuthFinished(RESULT,QString,QString)\0"
};

void Ui::VkView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VkView *_t = static_cast<VkView *>(_o);
        switch (_id) {
        case 0: _t->updateView((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->oAuthFinished((*reinterpret_cast< RESULT(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Ui::VkView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Ui::VkView::staticMetaObject = {
    { &PluginView::staticMetaObject, qt_meta_stringdata_Ui__VkView,
      qt_meta_data_Ui__VkView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Ui::VkView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Ui::VkView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Ui::VkView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Ui__VkView))
        return static_cast<void*>(const_cast< VkView*>(this));
    return PluginView::qt_metacast(_clname);
}

int Ui::VkView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PluginView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE