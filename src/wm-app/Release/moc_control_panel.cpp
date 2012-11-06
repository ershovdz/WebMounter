/****************************************************************************
** Meta object code from reading C++ file 'control_panel.h'
**
** Created: Tue 6. Nov 23:06:08 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../control_panel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'control_panel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Ui__ControlPanel[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x05,
      26,   17,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
      38,   36,   17,   17, 0x0a,
      69,   66,   17,   17, 0x0a,
     102,   17,   17,   17, 0x0a,
     132,  126,   17,   17, 0x08,
     152,  145,   17,   17, 0x08,
     201,   17,   17,   17, 0x08,
     235,  218,   17,   17, 0x0a,
     281,   17,   17,   17, 0x08,
     306,  299,   17,   17, 0x08,
     336,   17,   17,   17, 0x08,
     350,   17,   17,   17, 0x08,
     373,   17,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Ui__ControlPanel[] = {
    "Ui::ControlPanel\0\0mount()\0unmount()\0"
    ",\0showMsgBox(QString,QString)\0,,\0"
    "showTrayMsg(int,QString,QString)\0"
    "changeLanguage(QString)\0index\0"
    "setIcon(int)\0reason\0"
    "iconActivated(QSystemTrayIcon::ActivationReason)\0"
    "messageClicked()\0current,previous\0"
    "changePage(QListWidgetItem*,QListWidgetItem*)\0"
    "onUpdateClicked()\0update\0"
    "onUpdates(FvAvailableUpdate*)\0"
    "onNoUpdates()\0onHideCheckingResult()\0"
    "onCloseApp(QString)\0"
};

void Ui::ControlPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ControlPanel *_t = static_cast<ControlPanel *>(_o);
        switch (_id) {
        case 0: _t->mount(); break;
        case 1: _t->unmount(); break;
        case 2: _t->showMsgBox((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: _t->showTrayMsg((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 4: _t->changeLanguage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->setIcon((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->iconActivated((*reinterpret_cast< QSystemTrayIcon::ActivationReason(*)>(_a[1]))); break;
        case 7: _t->messageClicked(); break;
        case 8: _t->changePage((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 9: _t->onUpdateClicked(); break;
        case 10: _t->onUpdates((*reinterpret_cast< FvAvailableUpdate*(*)>(_a[1]))); break;
        case 11: _t->onNoUpdates(); break;
        case 12: _t->onHideCheckingResult(); break;
        case 13: _t->onCloseApp((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Ui::ControlPanel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Ui::ControlPanel::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Ui__ControlPanel,
      qt_meta_data_Ui__ControlPanel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Ui::ControlPanel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Ui::ControlPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Ui::ControlPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Ui__ControlPanel))
        return static_cast<void*>(const_cast< ControlPanel*>(this));
    if (!strcmp(_clname, "IGui"))
        return static_cast< IGui*>(const_cast< ControlPanel*>(this));
    return QDialog::qt_metacast(_clname);
}

int Ui::ControlPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void Ui::ControlPanel::mount()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Ui::ControlPanel::unmount()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
