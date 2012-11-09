/****************************************************************************
** Meta object code from reading C++ file 'qfacebookreply.h'
**
** Created: Sat 10. Nov 02:07:14 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../connector/qfacebookreply.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qfacebookreply.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Connector__QFacebookReply[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      27,   26,   26,   26, 0x05,
      59,   38,   26,   26, 0x05,

 // slots: signature, parameters, type, tag, flags
      89,   26,   26,   26, 0x08,
     103,   26,   26,   26, 0x08,
     118,  113,   26,   26, 0x08,
     162,  155,   26,   26, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Connector__QFacebookReply[] = {
    "Connector::QFacebookReply\0\0finished()\0"
    "bytesSent,bytesTotal\0uploadProgress(qint64,qint64)\0"
    "onParseData()\0doParse()\0code\0"
    "onError(QNetworkReply::NetworkError)\0"
    "errors\0onSslErrors(QList<QSslError>)\0"
};

const QMetaObject Connector::QFacebookReply::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Connector__QFacebookReply,
      qt_meta_data_Connector__QFacebookReply, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Connector::QFacebookReply::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Connector::QFacebookReply::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Connector::QFacebookReply::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Connector__QFacebookReply))
        return static_cast<void*>(const_cast< QFacebookReply*>(this));
    return QObject::qt_metacast(_clname);
}

int Connector::QFacebookReply::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: finished(); break;
        case 1: uploadProgress((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2]))); break;
        case 2: onParseData(); break;
        case 3: doParse(); break;
        case 4: onError((*reinterpret_cast< QNetworkReply::NetworkError(*)>(_a[1]))); break;
        case 5: onSslErrors((*reinterpret_cast< const QList<QSslError>(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void Connector::QFacebookReply::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Connector::QFacebookReply::uploadProgress(qint64 _t1, qint64 _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
