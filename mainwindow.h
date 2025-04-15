#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QLabel>
#include <QGridLayout>
#include <QMainWindow>
#include "cameralabelwidget.h"
#include "role.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void CreateMenu();
    void CreateImageGrid();
    void SetupCameraGridLayout(int i, QGridLayout* gridLayout, QVector<CameraLabelWidget*>& cameraLabels, QWidget* window);
    QString CameralName(int &i);
    QWidget* CreateEighthFrame();
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


private slots:

};

#endif // MAINWINDOW_H
