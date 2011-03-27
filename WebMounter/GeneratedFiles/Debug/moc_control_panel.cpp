/****************************************************************************
** Meta object code from reading C++ file 'control_panel.h'
**
** Created: Sun 27. Mar 21:28:06 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../control_panel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'control_panel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Ui__ControlPanel[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      20,   18,   17,   17, 0x0a,
      52,   48,   17,   17, 0x0a,
      89,   83,   17,   17, 0x08,
     109,  102,   17,   17, 0x08,
     158,   17,   17,   17, 0x08,
     192,  175,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Ui__ControlPanel[] = {
    "Ui::ControlPanel\0\0,\0showMsgBox(QString,QString)\0"
    "msg\0showNotification(Notification)\0"
    "index\0setIcon(int)\0reason\0"
    "iconActivated(QSystemTrayIcon::ActivationReason)\0"
    "messageClicked()\0current,previous\0"
    "changePage(QListWidgetItem*,QListWidgetItem*)\0"
};

const QMetaObject Ui::ControlPanel::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Ui__ControlPanel,
      qt_meta_data_Ui__ControlPanel, 0 }
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
    return QDialog::qt_metacast(_clname);
}

int Ui::ControlPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: showMsgBox((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 1: showNotification((*reinterpret_cast< const Notification(*)>(_a[1]))); break;
        case 2: setIcon((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: iconActivated((*reinterpret_cast< QSystemTrayIcon::ActivationReason(*)>(_a[1]))); break;
        case 4: messageClicked(); break;
        case 5: changePage((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
