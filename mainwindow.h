#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QLabel>
#include <QGridLayout>
#include <QMainWindow>
#include "cameralabelwidget.h"
#include "role.h"
#include "include/MZ_ClientControl.h"
#include "tcp_client.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void loadjson_layer(const QString& filePath);
    void loadjson_layer2(const QString& filePath);
    void initcams(int camnumber);
    void CreateMenu();
    void CreateImageGrid(int camnumber);
    void SetupCameraGridLayout(int i, QGridLayout* gridLayout, QVector<CameraLabelWidget*>& cameraLabels, QWidget* window);
    QString CameralName(int &i);
    QWidget* CreateEighthFrame(int camnumber);
    void setLabel(QVBoxLayout *layout, int row, int col);
    void test();
    Role role;
private:
    Ui::MainWindow *ui;
    QMenu *menuExecute; // "执行"菜单
    QMenu *menuRecord;  // "记录"菜单
    QMenu *menuTools;   // "工具"菜单
    QMenu *menuUser;    // "用户"菜单
   // QAction *loginAction; // 登录动作
    QVector<CameraLabelWidget *> cameraLabels;
    QVector<Cameral *> cams;
    QVector<Camerinfo> caminfo;
    QVector<QVector<QLabel*>> dataLabels;

private slots:

};

#endif // MAINWINDOW_H
