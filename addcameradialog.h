#ifndef ADDCAMERADIALOG_H
#define ADDCAMERADIALOG_H

#include <QDialog>
#include <QVector>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QGroupBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QFileInfo>
#include <QIntValidator>
#include "public.h"
#include "fileoperator.h"

// 用于在 UI 和 Camerinfo 数据之间映射的辅助结构体
struct CameraInputWidgets {
    int cameraIndex; // 对应的相机在 m_cameras 列表中的索引
    QLineEdit *nameEdit;
    QLineEdit *ipEdit;
    QLineEdit *portEdit;
    QLineEdit *snEdit;
    QLineEdit *pathEdit;
    QCheckBox *checkCheckBox;
    QPushButton *deleteButton; // 新增的删除按钮
};

class AddCameraDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddCameraDialog(const QString& configFilePath, QWidget *parent = nullptr);
    ~AddCameraDialog();

private slots:
    void addNewCamera();
    void saveCameras();
    void deleteCamera(); // 删除相机槽函数
    void onInputChanged(); // 监听输入变化以更新数据

private:
    void setupUI();
    void refreshCameraUI(); // 刷新UI以显示 m_cameras 中的数据
    void loadCameras();     // 从文件加载相机配置到 m_cameras
    void updateDataFromUI(); // 将UI上的数据更新到 m_cameras

    QString m_configFilePath; // 配置文件的路径
    QVector<Camerinfo> m_cameras; // 存储所有相机信息的列表

    // UI elements
    QScrollArea *scrollArea;
    QWidget *scrollAreaContent;
    QVBoxLayout *camerasLayout;
    QVector<CameraInputWidgets> m_cameraWidgets; // 存储每个相机对应的UI控件
};

#endif // ADDCAMERADIALOG_H
