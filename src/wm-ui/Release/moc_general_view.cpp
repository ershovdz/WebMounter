/****************************************************************************
** Meta object code from reading C++ file 'general_view.h'
**
** Created: Tue 6. Nov 23:06:00 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../general_view.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'general_view.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Ui__GeneralView[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,
      25,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      35,   16,   16,   16, 0x08,
      50,   16,   16,   16, 0x08,
      67,   16,   16,   16, 0x0a,
      77,   16,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Ui__GeneralView[] = {
    "Ui::GeneralView\0\0mount()\0unmount()\0"
    "mountClicked()\0unmountClicked()\0"
    "mounted()\0unmounted()\0"
};

void Ui::GeneralView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        GeneralView *_t = static_cast<GeneralView *>(_o);
        switch (_id) {
        case 0: _t->mount(); break;
        case 1: _t->unmount(); break;
        case 2: _t->mountClicked(); break;
        case 3: _t->unmountClicked(); break;
        case 4: _t->mounted(); break;
        case 5: _t->unmounted(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Ui::GeneralView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Ui::GeneralView::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Ui__GeneralView,
      qt_meta_data_Ui__GeneralView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Ui::GeneralView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Ui::GeneralView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Ui::GeneralView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Ui__GeneralView))
        return static_cast<void*>(const_cast< GeneralView*>(this));
    return QWidget::qt_metacast(_clname);
}

int Ui::GeneralView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void Ui::GeneralView::mount()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Ui::GeneralView::unmount()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
