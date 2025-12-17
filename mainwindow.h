#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QLabel>
#include <QGridLayout>
#include <QMainWindow>
#include "cameralabelwidget.h"
#include "role.h"
#include "include/MZ_ClientControl.h"
#include "tcp_client.h"
#include "imagesavingworker.h"
#include "CapacitanceProgram.h"
#include "MZ_VC3000H.h"
#include "MZ_VC3000.h"
#include "directorymonitorworker.h"
#include "RightControlPanel.h" 
#include "displayinfowidget.h"
#include "DBOperation.h" 
#include "outputhread.h"
#include "Api_Welding.h"
#include "run_task_thread.h"
#include <QProgressBar>
#include <QTimer>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    explicit MainWindow(int mode,QWidget* parent = 0);
    explicit MainWindow(QString str, QWidget* parent = 0);
	void closeEvent(QCloseEvent* event) override;
    ~MainWindow();
    void init_log(); 
    void BackupDir();
    void prospect(int code,std::string temppath);
    void loadjson_layer(const QString& filePath);
    void loadjson_layer2(const QString& filePath);
    void loadjson_layer3(const QString& filePath);
    void initcams(int camnumber);
    void CreateMenu();
    void init_SQLInfo();
    void CreateImageGrid(int camnumber);
    void CreateImageGrid_Braider(int camnumber);
    void SetupDualLayout(int camnumber, QGridLayout* cameraGrid, QVBoxLayout* statsLayout, QVector<CameraLabelWidget*>& cameraLabels, QWidget* window);
    void SetupCameraGridLayout(int i, QGridLayout* gridLayout, QVector<CameraLabelWidget*>& cameraLabels, QWidget* window);
    QString CameralName(int &i);
    QWidget* CreateEighthFrame(int camnumber);
    void setLabel(QVBoxLayout *layout, int row, int col);
    void updateCameraStats();
    void AllCameraConnect();
    void RefreshDir();
    void setupUpdateTimer();
    void toggleAutoStart(QAction* action);
    void test_show();
    void init_algo();
    void init_algo_Braider();
    void init_algo_Flower();
    void startAlgoInitAsync();
	void init_algo_FourBrader();
    void init_cap();
    void initSqlite3Db_Brader();
    void initSqlite3Db_Plater();
	void initSqlite3Db_Unify();
    void showAboutWidget();
    int initPCI_VC3000H();
    int initPCI_VC3000();
    void initCameralPara();
    void updateDB_Brader();
    void updateDB_Plater();
	void updateDB_Unify();
    bool updateDeviceId();
    void ResetCameraNGCounts(Cameral* cam);
    Role role;
public slots:
    void onStartAllCamerasClicked();
    void onPhotoAllCamerasClicked();
    void onStopAllCamerasClicked();
    void onClearAllCameraClicked();
    void onParamClicked(int buttonIndex);
    void onAllSheild(bool check);
	void onAllLearn();
    void onDisconnectAllCameras();
    void update_SQLInfo(const DBInfo& info);
private:    
    Ui::MainWindow *ui;
    QMenu *menuExecute; // "执行"菜单
    QMenu *menuRecord;  // "记录"菜单
    QMenu *menuTools;   // "工具"菜单
    QMenu *menuUser;    // "用户"菜单
    QAction* menuAbout;
    QLabel* m_runtimeLabel;
    QLabel* m_roleLabel;
    SqliteDB::DBOperation* dbManager = nullptr;
    QDate m_lastDate = QDate::currentDate();
   // QAction *loginAction; // 登录动作
    QVector<CameraLabelWidget *> cameraLabels;
    QVector<DisplayInfoWidget*> m_displayInfoWidgets;
    RightControlPanel* m_rightControlPanel;
    QVector<Cameral *> cams;
    QVector<Camerinfo> caminfo;
    QVector<QVector<QLabel*>> dataLabels;
    QTimer *m_updateTimer;
    QTimer* m_databaseTimer;
    //唯一的全局保存队列
    SaveQueue m_saveQueue;

    // 管理子线程和 Worker 对象的指针
    QThread* m_workerThread;
    ImageSaverWorker* m_imageSaverWorker;
    void setupImageSaverThread();//启动保存图片线程

    QThread* m_monitorThread;
    DirectoryMonitorWorker* m_monitorWorker;
    QStringList m_pathsToWatch;
    void setupMonitorThread();

    OutPutThread* m_outputThread = nullptr;
	RunTaskThread* m_runTaskThread = nullptr;
    void setupOutPutThread();
	void setupRunTaskThread();

    void terminateProcessByName(const std::string& processName);

    QProgressBar* m_algoProgressBar = nullptr;
    QTimer* m_algoProgressTimer = nullptr;
    QLabel* m_algoStatusLabel = nullptr;
    QAction* LogAction = nullptr;
    QAction* DataAction = nullptr;
private slots:

};

#endif // MAINWINDOW_H
