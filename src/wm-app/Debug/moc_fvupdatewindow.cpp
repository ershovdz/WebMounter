/****************************************************************************
** Meta object code from reading C++ file 'fvupdatewindow.h'
**
** Created: Sat 3. Nov 19:25:47 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../updater/fvupdatewindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'fvupdatewindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Ui__FvUpdateWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x05,
      39,   19,   19,   19, 0x05,
      62,   19,   19,   19, 0x05,
      85,   19,   19,   19, 0x05,
     103,   19,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
     121,  112,   19,   19, 0x0a,
     138,   19,   19,   19, 0x0a,
     155,  151,   19,   19, 0x0a,
     188,  173,   19,   19, 0x0a,
     221,   19,   19,   19, 0x08,
     238,   19,   19,   19, 0x08,
     252,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Ui__FvUpdateWindow[] = {
    "Ui::FvUpdateWindow\0\0installRequested()\0"
    "skipInstallRequested()\0remindLaterRequested()\0"
    "cancelRequested()\0closed()\0percents\0"
    "onProgress(uint)\0onFinished()\0msg\0"
    "onFailed(QString)\0proposedUpdate\0"
    "onShowWindow(FvAvailableUpdate*)\0"
    "installClicked()\0skipClicked()\0"
    "remindClicked()\0"
};

void Ui::FvUpdateWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FvUpdateWindow *_t = static_cast<FvUpdateWindow *>(_o);
        switch (_id) {
        case 0: _t->installRequested(); break;
        case 1: _t->skipInstallRequested(); break;
        case 2: _t->remindLaterRequested(); break;
        case 3: _t->cancelRequested(); break;
        case 4: _t->closed(); break;
        case 5: _t->onProgress((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 6: _t->onFinished(); break;
        case 7: _t->onFailed((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: _t->onShowWindow((*reinterpret_cast< FvAvailableUpdate*(*)>(_a[1]))); break;
        case 9: _t->installClicked(); break;
        case 10: _t->skipClicked(); break;
        case 11: _t->remindClicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Ui::FvUpdateWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Ui::FvUpdateWindow::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Ui__FvUpdateWindow,
      qt_meta_data_Ui__FvUpdateWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Ui::FvUpdateWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Ui::FvUpdateWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Ui::FvUpdateWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Ui__FvUpdateWindow))
        return static_cast<void*>(const_cast< FvUpdateWindow*>(this));
    return QWidget::qt_metacast(_clname);
}

int Ui::FvUpdateWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void Ui::FvUpdateWindow::installRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Ui::FvUpdateWindow::skipInstallRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Ui::FvUpdateWindow::remindLaterRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void Ui::FvUpdateWindow::cancelRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void Ui::FvUpdateWindow::closed()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}
QT_END_MOC_NAMESPACE
