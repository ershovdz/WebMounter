/****************************************************************************
** Meta object code from reading C++ file 'plugin_view.h'
**
** Created: Tue 6. Nov 23:06:00 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../plugin_view.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'plugin_view.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Ui__PluginView[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      22,   16,   15,   15, 0x08,
      40,   15,   15,   15, 0x08,
      65,   15,   15,   15, 0x08,
     104,   89,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Ui__PluginView[] = {
    "Ui::PluginView\0\0state\0oauthClicked(int)\0"
    "startPluginClicked(bool)\0"
    "stopPluginClicked(bool)\0progress,state\0"
    "updateView(int,int)\0"
};

void Ui::PluginView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PluginView *_t = static_cast<PluginView *>(_o);
        switch (_id) {
        case 0: _t->oauthClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->startPluginClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->stopPluginClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->updateView((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Ui::PluginView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Ui::PluginView::staticMetaObject = {
    { &View::staticMetaObject, qt_meta_stringdata_Ui__PluginView,
      qt_meta_data_Ui__PluginView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Ui::PluginView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Ui::PluginView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Ui::PluginView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Ui__PluginView))
        return static_cast<void*>(const_cast< PluginView*>(this));
    return View::qt_metacast(_clname);
}

int Ui::PluginView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = View::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE