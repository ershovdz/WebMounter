/****************************************************************************
** Meta object code from reading C++ file 'view.h'
**
** Created: Sun 27. Mar 21:28:20 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../view.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'view.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Ui__View[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,
      22,    9,    9,    9, 0x05,
      33,    9,    9,    9, 0x05,
      70,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
     104,   89,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Ui__View[] = {
    "Ui::View\0\0startSync()\0stopSync()\0"
    "connectPlugin(Data::PluginSettings&)\0"
    "disconnectPlugin()\0progress,state\0"
    "updateView(int,int)\0"
};

const QMetaObject Ui::View::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Ui__View,
      qt_meta_data_Ui__View, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Ui::View::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Ui::View::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Ui::View::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Ui__View))
        return static_cast<void*>(const_cast< View*>(this));
    return QWidget::qt_metacast(_clname);
}

int Ui::View::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: startSync(); break;
        case 1: stopSync(); break;
        case 2: connectPlugin((*reinterpret_cast< Data::PluginSettings(*)>(_a[1]))); break;
        case 3: disconnectPlugin(); break;
        case 4: updateView((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void Ui::View::startSync()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Ui::View::stopSync()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Ui::View::connectPlugin(Data::PluginSettings & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Ui::View::disconnectPlugin()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}
QT_END_MOC_NAMESPACE
