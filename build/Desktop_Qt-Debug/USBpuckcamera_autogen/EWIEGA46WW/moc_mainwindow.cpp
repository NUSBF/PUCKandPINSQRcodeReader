/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../mainwindow.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#include <QtCore/QSet>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_MainWindow_t {
    uint offsetsAndSizes[32];
    char stringdata0[11];
    char stringdata1[22];
    char stringdata2[1];
    char stringdata3[6];
    char stringdata4[18];
    char stringdata5[8];
    char stringdata6[10];
    char stringdata7[17];
    char stringdata8[29];
    char stringdata9[29];
    char stringdata10[40];
    char stringdata11[22];
    char stringdata12[9];
    char stringdata13[5];
    char stringdata14[15];
    char stringdata15[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_MainWindow_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
        QT_MOC_LITERAL(0, 10),  // "MainWindow"
        QT_MOC_LITERAL(11, 21),  // "newImageForProcessing"
        QT_MOC_LITERAL(33, 0),  // ""
        QT_MOC_LITERAL(34, 5),  // "image"
        QT_MOC_LITERAL(40, 17),  // "QList<CircleInfo>"
        QT_MOC_LITERAL(58, 7),  // "circles"
        QT_MOC_LITERAL(66, 9),  // "QSet<int>"
        QT_MOC_LITERAL(76, 16),  // "processedCircles"
        QT_MOC_LITERAL(93, 28),  // "on_pushButtonCamera1_clicked"
        QT_MOC_LITERAL(122, 28),  // "on_pushButtonCamera2_clicked"
        QT_MOC_LITERAL(151, 39),  // "on_pushButtonTestQRCodDetecti..."
        QT_MOC_LITERAL(191, 21),  // "handleDataMatrixFound"
        QT_MOC_LITERAL(213, 8),  // "circleId"
        QT_MOC_LITERAL(222, 4),  // "code"
        QT_MOC_LITERAL(227, 14),  // "processedImage"
        QT_MOC_LITERAL(242, 15)   // "updateDebugView"
    },
    "MainWindow",
    "newImageForProcessing",
    "",
    "image",
    "QList<CircleInfo>",
    "circles",
    "QSet<int>",
    "processedCircles",
    "on_pushButtonCamera1_clicked",
    "on_pushButtonCamera2_clicked",
    "on_pushButtonTestQRCodDetection_clicked",
    "handleDataMatrixFound",
    "circleId",
    "code",
    "processedImage",
    "updateDebugView"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_MainWindow[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    3,   50,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       8,    0,   57,    2, 0x08,    5 /* Private */,
       9,    0,   58,    2, 0x08,    6 /* Private */,
      10,    0,   59,    2, 0x08,    7 /* Private */,
      11,    3,   60,    2, 0x08,    8 /* Private */,
      15,    2,   67,    2, 0x08,   12 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QImage, 0x80000000 | 4, 0x80000000 | 6,    3,    5,    7,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QImage,   12,   13,   14,
    QMetaType::Void, QMetaType::Int, QMetaType::QImage,   12,    3,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.offsetsAndSizes,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_MainWindow_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'newImageForProcessing'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QImage &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVector<CircleInfo> &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QSet<int> &, std::false_type>,
        // method 'on_pushButtonCamera1_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pushButtonCamera2_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pushButtonTestQRCodDetection_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleDataMatrixFound'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QImage &, std::false_type>,
        // method 'updateDebugView'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QImage &, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->newImageForProcessing((*reinterpret_cast< std::add_pointer_t<QImage>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QList<CircleInfo>>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QSet<int>>>(_a[3]))); break;
        case 1: _t->on_pushButtonCamera1_clicked(); break;
        case 2: _t->on_pushButtonCamera2_clicked(); break;
        case 3: _t->on_pushButtonTestQRCodDetection_clicked(); break;
        case 4: _t->handleDataMatrixFound((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QImage>>(_a[3]))); break;
        case 5: _t->updateDebugView((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QImage>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 2:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QSet<int> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MainWindow::*)(const QImage & , const QVector<CircleInfo> & , const QSet<int> & );
            if (_t _q_method = &MainWindow::newImageForProcessing; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::newImageForProcessing(const QImage & _t1, const QVector<CircleInfo> & _t2, const QSet<int> & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
namespace {
struct qt_meta_stringdata_ImageProcessor_t {
    uint offsetsAndSizes[30];
    char stringdata0[15];
    char stringdata1[16];
    char stringdata2[1];
    char stringdata3[9];
    char stringdata4[5];
    char stringdata5[15];
    char stringdata6[19];
    char stringdata7[15];
    char stringdata8[11];
    char stringdata9[6];
    char stringdata10[13];
    char stringdata11[18];
    char stringdata12[8];
    char stringdata13[10];
    char stringdata14[17];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_ImageProcessor_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_ImageProcessor_t qt_meta_stringdata_ImageProcessor = {
    {
        QT_MOC_LITERAL(0, 14),  // "ImageProcessor"
        QT_MOC_LITERAL(15, 15),  // "dataMatrixFound"
        QT_MOC_LITERAL(31, 0),  // ""
        QT_MOC_LITERAL(32, 8),  // "circleId"
        QT_MOC_LITERAL(41, 4),  // "code"
        QT_MOC_LITERAL(46, 14),  // "processedImage"
        QT_MOC_LITERAL(61, 18),  // "processingComplete"
        QT_MOC_LITERAL(80, 14),  // "processedCount"
        QT_MOC_LITERAL(95, 10),  // "debugImage"
        QT_MOC_LITERAL(106, 5),  // "image"
        QT_MOC_LITERAL(112, 12),  // "processImage"
        QT_MOC_LITERAL(125, 17),  // "QList<CircleInfo>"
        QT_MOC_LITERAL(143, 7),  // "circles"
        QT_MOC_LITERAL(151, 9),  // "QSet<int>"
        QT_MOC_LITERAL(161, 16)   // "processedCircles"
    },
    "ImageProcessor",
    "dataMatrixFound",
    "",
    "circleId",
    "code",
    "processedImage",
    "processingComplete",
    "processedCount",
    "debugImage",
    "image",
    "processImage",
    "QList<CircleInfo>",
    "circles",
    "QSet<int>",
    "processedCircles"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_ImageProcessor[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    3,   38,    2, 0x06,    1 /* Public */,
       6,    1,   45,    2, 0x06,    5 /* Public */,
       8,    2,   48,    2, 0x06,    7 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      10,    3,   53,    2, 0x0a,   10 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QImage,    3,    4,    5,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Int, QMetaType::QImage,    3,    9,

 // slots: parameters
    QMetaType::Void, QMetaType::QImage, 0x80000000 | 11, 0x80000000 | 13,    9,   12,   14,

       0        // eod
};

Q_CONSTINIT const QMetaObject ImageProcessor::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ImageProcessor.offsetsAndSizes,
    qt_meta_data_ImageProcessor,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_ImageProcessor_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ImageProcessor, std::true_type>,
        // method 'dataMatrixFound'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QImage &, std::false_type>,
        // method 'processingComplete'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'debugImage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QImage &, std::false_type>,
        // method 'processImage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QImage &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVector<CircleInfo> &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QSet<int> &, std::false_type>
    >,
    nullptr
} };

void ImageProcessor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ImageProcessor *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->dataMatrixFound((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QImage>>(_a[3]))); break;
        case 1: _t->processingComplete((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->debugImage((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QImage>>(_a[2]))); break;
        case 3: _t->processImage((*reinterpret_cast< std::add_pointer_t<QImage>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QList<CircleInfo>>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QSet<int>>>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 2:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QSet<int> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ImageProcessor::*)(int , const QString & , const QImage & );
            if (_t _q_method = &ImageProcessor::dataMatrixFound; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ImageProcessor::*)(int );
            if (_t _q_method = &ImageProcessor::processingComplete; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ImageProcessor::*)(int , const QImage & );
            if (_t _q_method = &ImageProcessor::debugImage; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject *ImageProcessor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ImageProcessor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ImageProcessor.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ImageProcessor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void ImageProcessor::dataMatrixFound(int _t1, const QString & _t2, const QImage & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ImageProcessor::processingComplete(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ImageProcessor::debugImage(int _t1, const QImage & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
