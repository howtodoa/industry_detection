/****************************************************************************
** Meta object code from reading C++ file 'parawidget.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../parawidget.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'parawidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSParaWidgetENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSParaWidgetENDCLASS = QtMocHelpers::stringData(
    "ParaWidget",
    "saveCameralChanges",
    "",
    "filePath",
    "CameralPara",
    "cam",
    "saveRangeChanges",
    "RangePara",
    "range",
    "saveAlgorithmChanges",
    "AlgorithmPara",
    "algo",
    "closeWindow"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSParaWidgetENDCLASS_t {
    uint offsetsAndSizes[26];
    char stringdata0[11];
    char stringdata1[19];
    char stringdata2[1];
    char stringdata3[9];
    char stringdata4[12];
    char stringdata5[4];
    char stringdata6[17];
    char stringdata7[10];
    char stringdata8[6];
    char stringdata9[21];
    char stringdata10[14];
    char stringdata11[5];
    char stringdata12[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSParaWidgetENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSParaWidgetENDCLASS_t qt_meta_stringdata_CLASSParaWidgetENDCLASS = {
    {
        QT_MOC_LITERAL(0, 10),  // "ParaWidget"
        QT_MOC_LITERAL(11, 18),  // "saveCameralChanges"
        QT_MOC_LITERAL(30, 0),  // ""
        QT_MOC_LITERAL(31, 8),  // "filePath"
        QT_MOC_LITERAL(40, 11),  // "CameralPara"
        QT_MOC_LITERAL(52, 3),  // "cam"
        QT_MOC_LITERAL(56, 16),  // "saveRangeChanges"
        QT_MOC_LITERAL(73, 9),  // "RangePara"
        QT_MOC_LITERAL(83, 5),  // "range"
        QT_MOC_LITERAL(89, 20),  // "saveAlgorithmChanges"
        QT_MOC_LITERAL(110, 13),  // "AlgorithmPara"
        QT_MOC_LITERAL(124, 4),  // "algo"
        QT_MOC_LITERAL(129, 11)   // "closeWindow"
    },
    "ParaWidget",
    "saveCameralChanges",
    "",
    "filePath",
    "CameralPara",
    "cam",
    "saveRangeChanges",
    "RangePara",
    "range",
    "saveAlgorithmChanges",
    "AlgorithmPara",
    "algo",
    "closeWindow"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSParaWidgetENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,   38,    2, 0x08,    1 /* Private */,
       6,    2,   43,    2, 0x08,    4 /* Private */,
       9,    2,   48,    2, 0x08,    7 /* Private */,
      12,    0,   53,    2, 0x08,   10 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, 0x80000000 | 4,    3,    5,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 7,    3,    8,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 10,    3,   11,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject ParaWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSParaWidgetENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSParaWidgetENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSParaWidgetENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ParaWidget, std::true_type>,
        // method 'saveCameralChanges'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const CameralPara &, std::false_type>,
        // method 'saveRangeChanges'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const RangePara &, std::false_type>,
        // method 'saveAlgorithmChanges'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const AlgorithmPara &, std::false_type>,
        // method 'closeWindow'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void ParaWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ParaWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->saveCameralChanges((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<CameralPara>>(_a[2]))); break;
        case 1: _t->saveRangeChanges((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<RangePara>>(_a[2]))); break;
        case 2: _t->saveAlgorithmChanges((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<AlgorithmPara>>(_a[2]))); break;
        case 3: _t->closeWindow(); break;
        default: ;
        }
    }
}

const QMetaObject *ParaWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ParaWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSParaWidgetENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ParaWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
