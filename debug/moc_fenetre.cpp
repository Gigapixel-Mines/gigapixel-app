/****************************************************************************
** Meta object code from reading C++ file 'fenetre.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../fenetre.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'fenetre.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Fenetre_t {
    QByteArrayData data[12];
    char stringdata0[109];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Fenetre_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Fenetre_t qt_meta_stringdata_Fenetre = {
    {
QT_MOC_LITERAL(0, 0, 7), // "Fenetre"
QT_MOC_LITERAL(1, 8, 13), // "LigneFinished"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 9), // "PasGauche"
QT_MOC_LITERAL(4, 33, 9), // "PasDroite"
QT_MOC_LITERAL(5, 43, 8), // "FinCycle"
QT_MOC_LITERAL(6, 52, 13), // "disableButton"
QT_MOC_LITERAL(7, 66, 10), // "ableButton"
QT_MOC_LITERAL(8, 77, 8), // "assign_h"
QT_MOC_LITERAL(9, 86, 8), // "assign_v"
QT_MOC_LITERAL(10, 95, 8), // "compteur"
QT_MOC_LITERAL(11, 104, 4) // "stop"

    },
    "Fenetre\0LigneFinished\0\0PasGauche\0"
    "PasDroite\0FinCycle\0disableButton\0"
    "ableButton\0assign_h\0assign_v\0compteur\0"
    "stop"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Fenetre[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x06 /* Public */,
       3,    0,   65,    2, 0x06 /* Public */,
       4,    0,   66,    2, 0x06 /* Public */,
       5,    0,   67,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   68,    2, 0x08 /* Private */,
       7,    0,   69,    2, 0x08 /* Private */,
       8,    0,   70,    2, 0x08 /* Private */,
       9,    0,   71,    2, 0x08 /* Private */,
      10,    0,   72,    2, 0x08 /* Private */,
      11,    0,   73,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Fenetre::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Fenetre *_t = static_cast<Fenetre *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->LigneFinished(); break;
        case 1: _t->PasGauche(); break;
        case 2: _t->PasDroite(); break;
        case 3: _t->FinCycle(); break;
        case 4: _t->disableButton(); break;
        case 5: _t->ableButton(); break;
        case 6: _t->assign_h(); break;
        case 7: _t->assign_v(); break;
        case 8: _t->compteur(); break;
        case 9: _t->stop(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Fenetre::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Fenetre::LigneFinished)) {
                *result = 0;
            }
        }
        {
            typedef void (Fenetre::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Fenetre::PasGauche)) {
                *result = 1;
            }
        }
        {
            typedef void (Fenetre::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Fenetre::PasDroite)) {
                *result = 2;
            }
        }
        {
            typedef void (Fenetre::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Fenetre::FinCycle)) {
                *result = 3;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Fenetre::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Fenetre.data,
      qt_meta_data_Fenetre,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Fenetre::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Fenetre::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Fenetre.stringdata0))
        return static_cast<void*>(const_cast< Fenetre*>(this));
    return QWidget::qt_metacast(_clname);
}

int Fenetre::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void Fenetre::LigneFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void Fenetre::PasGauche()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void Fenetre::PasDroite()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void Fenetre::FinCycle()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
