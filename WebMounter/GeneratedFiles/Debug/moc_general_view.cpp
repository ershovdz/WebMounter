/****************************************************************************
** Meta object code from reading C++ file 'general_view.h'
**
** Created: Sun 27. Mar 21:28:12 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../general_view.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'general_view.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Ui__GeneralView[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,
      47,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      57,   16,   16,   16, 0x08,
      73,   16,   16,   16, 0x08,
      88,   16,   16,   16, 0x08,
     105,   16,   16,   16, 0x08,
     114,   16,   16,   16, 0x08,
     130,   16,   16,   16, 0x0a,
     140,   16,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Ui__GeneralView[] = {
    "Ui::GeneralView\0\0mount(Data::GeneralSettings&)\0"
    "unmount()\0acceptClicked()\0mountClicked()\0"
    "unmountClicked()\0browse()\0changeLang(int)\0"
    "mounted()\0unmounted()\0"
};

const QMetaObject Ui::GeneralView::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Ui__GeneralView,
      qt_meta_data_Ui__GeneralView, 0 }
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
        switch (_id) {
        case 0: mount((*reinterpret_cast< Data::GeneralSettings(*)>(_a[1]))); break;
        case 1: unmount(); break;
        case 2: acceptClicked(); break;
        case 3: mountClicked(); break;
        case 4: unmountClicked(); break;
        case 5: browse(); break;
        case 6: changeLang((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: mounted(); break;
        case 8: unmounted(); break;
        default: ;
        }
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void Ui::GeneralView::mount(Data::GeneralSettings & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Ui::GeneralView::unmount()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
