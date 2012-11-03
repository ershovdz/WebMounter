/****************************************************************************
** Meta object code from reading C++ file 'general_view.h'
**
** Created: Sat 3. Nov 19:27:30 2012
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
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,
      47,   16,   16,   16, 0x05,
      57,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      81,   16,   16,   16, 0x08,
      97,   16,   16,   16, 0x08,
     112,   16,   16,   16, 0x08,
     129,   16,   16,   16, 0x08,
     138,   16,   16,   16, 0x08,
     154,   16,   16,   16, 0x0a,
     164,   16,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Ui__GeneralView[] = {
    "Ui::GeneralView\0\0mount(Data::GeneralSettings&)\0"
    "unmount()\0changeLanguage(QString)\0"
    "acceptClicked()\0mountClicked()\0"
    "unmountClicked()\0browse()\0changeLang(int)\0"
    "mounted()\0unmounted()\0"
};

void Ui::GeneralView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        GeneralView *_t = static_cast<GeneralView *>(_o);
        switch (_id) {
        case 0: _t->mount((*reinterpret_cast< Data::GeneralSettings(*)>(_a[1]))); break;
        case 1: _t->unmount(); break;
        case 2: _t->changeLanguage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->acceptClicked(); break;
        case 4: _t->mountClicked(); break;
        case 5: _t->unmountClicked(); break;
        case 6: _t->browse(); break;
        case 7: _t->changeLang((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->mounted(); break;
        case 9: _t->unmounted(); break;
        default: ;
        }
    }
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
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
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

// SIGNAL 2
void Ui::GeneralView::changeLanguage(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
