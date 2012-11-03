/****************************************************************************
** Meta object code from reading C++ file 'yaf_oauth.h'
**
** Created: Sat 3. Nov 19:27:39 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../view/yaf_oauth.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'yaf_oauth.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Ui__WebView[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   13,   12,   12, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_Ui__WebView[] = {
    "Ui::WebView\0\0error\0finished(RESULT)\0"
};

void Ui::WebView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        WebView *_t = static_cast<WebView *>(_o);
        switch (_id) {
        case 0: _t->finished((*reinterpret_cast< RESULT(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Ui::WebView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Ui::WebView::staticMetaObject = {
    { &QWebView::staticMetaObject, qt_meta_stringdata_Ui__WebView,
      qt_meta_data_Ui__WebView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Ui::WebView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Ui::WebView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Ui::WebView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Ui__WebView))
        return static_cast<void*>(const_cast< WebView*>(this));
    return QWebView::qt_metacast(_clname);
}

int Ui::WebView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWebView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void Ui::WebView::finished(RESULT _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_Ui__YafOAuth[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      32,   14,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      71,   69,   13,   13, 0x0a,
     120,  114,   13,   13, 0x0a,
     151,  145,   13,   13, 0x0a,
     168,  114,   13,   13, 0x0a,
     209,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Ui__YafOAuth[] = {
    "Ui::YafOAuth\0\0error,login,token\0"
    "authFinished(RESULT,QString,QString)\0"
    ",\0ignoreSSL(QNetworkReply*,QList<QSslError>)\0"
    "reply\0finished(QNetworkReply*)\0error\0"
    "finished(RESULT)\0"
    "handleUnsupportedContent(QNetworkReply*)\0"
    "slotOAuthTimeout()\0"
};

void Ui::YafOAuth::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        YafOAuth *_t = static_cast<YafOAuth *>(_o);
        switch (_id) {
        case 0: _t->authFinished((*reinterpret_cast< RESULT(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 1: _t->ignoreSSL((*reinterpret_cast< QNetworkReply*(*)>(_a[1])),(*reinterpret_cast< const QList<QSslError>(*)>(_a[2]))); break;
        case 2: _t->finished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 3: _t->finished((*reinterpret_cast< RESULT(*)>(_a[1]))); break;
        case 4: _t->handleUnsupportedContent((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 5: _t->slotOAuthTimeout(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Ui::YafOAuth::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Ui::YafOAuth::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Ui__YafOAuth,
      qt_meta_data_Ui__YafOAuth, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Ui::YafOAuth::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Ui::YafOAuth::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Ui::YafOAuth::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Ui__YafOAuth))
        return static_cast<void*>(const_cast< YafOAuth*>(this));
    return QObject::qt_metacast(_clname);
}

int Ui::YafOAuth::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void Ui::YafOAuth::authFinished(RESULT _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
