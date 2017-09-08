/****************************************************************************
** Meta object code from reading C++ file 'FocusWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../FocusWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FocusWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_FocusWindow_t {
    QByteArrayData data[7];
    char stringdata0[66];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FocusWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FocusWindow_t qt_meta_stringdata_FocusWindow = {
    {
QT_MOC_LITERAL(0, 0, 11), // "FocusWindow"
QT_MOC_LITERAL(1, 12, 12), // "PictureTaken"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 15), // "OnImageReceived"
QT_MOC_LITERAL(4, 42, 7), // "QImage*"
QT_MOC_LITERAL(5, 50, 5), // "image"
QT_MOC_LITERAL(6, 56, 9) // "SaveImage"

    },
    "FocusWindow\0PictureTaken\0\0OnImageReceived\0"
    "QImage*\0image\0SaveImage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FocusWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   30,    2, 0x0a /* Public */,
       6,    0,   33,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void,

       0        // eod
};

void FocusWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FocusWindow *_t = static_cast<FocusWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->PictureTaken(); break;
        case 1: _t->OnImageReceived((*reinterpret_cast< QImage*(*)>(_a[1]))); break;
        case 2: _t->SaveImage(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (FocusWindow::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FocusWindow::PictureTaken)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject FocusWindow::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_FocusWindow.data,
      qt_meta_data_FocusWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *FocusWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FocusWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_FocusWindow.stringdata0))
        return static_cast<void*>(const_cast< FocusWindow*>(this));
    return QWidget::qt_metacast(_clname);
}

int FocusWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void FocusWindow::PictureTaken()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
