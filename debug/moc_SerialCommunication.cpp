/****************************************************************************
** Meta object code from reading C++ file 'SerialCommunication.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../SerialCommunication.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SerialCommunication.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_SerialCommunication_t {
    QByteArrayData data[14];
    char stringdata0[154];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SerialCommunication_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SerialCommunication_t qt_meta_stringdata_SerialCommunication = {
    {
QT_MOC_LITERAL(0, 0, 19), // "SerialCommunication"
QT_MOC_LITERAL(1, 20, 11), // "MvtFinished"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 12), // "InitFinished"
QT_MOC_LITERAL(4, 46, 13), // "emergencyStop"
QT_MOC_LITERAL(5, 60, 13), // "miseAuPointAv"
QT_MOC_LITERAL(6, 74, 13), // "miseAuPointAr"
QT_MOC_LITERAL(7, 88, 15), // "miseAuPointStop"
QT_MOC_LITERAL(8, 104, 10), // "initialPic"
QT_MOC_LITERAL(9, 115, 6), // "droite"
QT_MOC_LITERAL(10, 122, 6), // "gauche"
QT_MOC_LITERAL(11, 129, 4), // "haut"
QT_MOC_LITERAL(12, 134, 3), // "bas"
QT_MOC_LITERAL(13, 138, 15) // "handleReadyRead"

    },
    "SerialCommunication\0MvtFinished\0\0"
    "InitFinished\0emergencyStop\0miseAuPointAv\0"
    "miseAuPointAr\0miseAuPointStop\0initialPic\0"
    "droite\0gauche\0haut\0bas\0handleReadyRead"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SerialCommunication[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x06 /* Public */,
       3,    0,   75,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   76,    2, 0x0a /* Public */,
       5,    0,   77,    2, 0x0a /* Public */,
       6,    0,   78,    2, 0x0a /* Public */,
       7,    0,   79,    2, 0x0a /* Public */,
       8,    0,   80,    2, 0x0a /* Public */,
       9,    0,   81,    2, 0x0a /* Public */,
      10,    0,   82,    2, 0x0a /* Public */,
      11,    0,   83,    2, 0x0a /* Public */,
      12,    0,   84,    2, 0x0a /* Public */,
      13,    0,   85,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SerialCommunication::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SerialCommunication *_t = static_cast<SerialCommunication *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->MvtFinished(); break;
        case 1: _t->InitFinished(); break;
        case 2: _t->emergencyStop(); break;
        case 3: _t->miseAuPointAv(); break;
        case 4: _t->miseAuPointAr(); break;
        case 5: _t->miseAuPointStop(); break;
        case 6: _t->initialPic(); break;
        case 7: _t->droite(); break;
        case 8: _t->gauche(); break;
        case 9: _t->haut(); break;
        case 10: _t->bas(); break;
        case 11: _t->handleReadyRead(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SerialCommunication::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialCommunication::MvtFinished)) {
                *result = 0;
            }
        }
        {
            typedef void (SerialCommunication::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialCommunication::InitFinished)) {
                *result = 1;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject SerialCommunication::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SerialCommunication.data,
      qt_meta_data_SerialCommunication,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *SerialCommunication::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SerialCommunication::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_SerialCommunication.stringdata0))
        return static_cast<void*>(const_cast< SerialCommunication*>(this));
    return QWidget::qt_metacast(_clname);
}

int SerialCommunication::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void SerialCommunication::MvtFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void SerialCommunication::InitFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
