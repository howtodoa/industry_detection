#include "imagesavingworker.h"
#include "public.h"
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <string>
#include <locale>
#include <codecvt>

ImageSaverWorker::ImageSaverWorker(SaveQueue& saveQueue, QObject *parent)
    : QObject(parent),
    m_saveQueue(saveQueue)
{

    //GlobalLog::logger.Mz_AddLog(L"ImageSaverWorker start");
}

ImageSaverWorker::~ImageSaverWorker()
{
    //GlobalLog::logger.Mz_AddLog(L"ImageSaverWorker destructor called.");
    // 在主线程退出
    // thread_stopFlag.store(true); // 立即设置 worker 自身的停止标志
    // {
    //     std::unique_lock<std::mutex> lock(m_saveQueue.mutex);
    //     m_saveQueue.stopFlag = true;
    // }
    // m_saveQueue.cond.notify_all();
   // GlobalLog::logger.Mz_AddLog(L"ImageSaverWorker destructor finished.");
}




void ImageSaverWorker::saveLoop()
{
    GlobalLog::logger.Mz_AddLog(L"Image Saver Worker started in thread.");

    while (!m_saveQueue.stopFlag.load())
    {
        SaveData dataToSave;

        {
            std::unique_lock<std::mutex> lock(m_saveQueue.mutex);

            m_saveQueue.cond.wait(lock, [this]{
                 return m_saveQueue.stopFlag.load() || !m_saveQueue.queue.empty();
            });

            if (m_saveQueue.stopFlag.load() && m_saveQueue.queue.empty()) {
                GlobalLog::logger.Mz_AddLog(L"Image Saver Worker: Stopping gracefully due to queue stop flag and empty queue.");
                return;
            }

            if (!m_saveQueue.queue.empty()) {
            //      std::cout<<"image ptr use count before:  "<<m_saveQueue.queue.front().imagePtr.use_count()<<std::endl;
              dataToSave = std::move(m_saveQueue.queue.front());

                m_saveQueue.queue.pop_front();
                m_saveQueue.cond.notify_one();
            } else {//又有数据了
                GlobalLog::logger.Mz_AddLog(L"Image Saver Worker: Woke up but queue is empty. Re-checking stopFlag or waiting again.");
                continue;
            }
        }

        // 检查 cv::Mat 指针是否有效且不为空
        if (!dataToSave.imagePtr || dataToSave.imagePtr->empty()) 
        { 
           // LOG_DEBUG(GlobalLog::logger, _T("m_pParaDock ptr null"));
            continue;
        }

        QElapsedTimer timer;
        timer.start();  // 开始计时


         // 保证目录存在
         QString qFullPath = QDir(QString::fromStdString(dataToSave.work_path))
                                 .filePath(QString::fromStdString(generateStamp() + ".jpg"));
         QDir().mkpath(QFileInfo(qFullPath).absolutePath());


         // --- -------------- ---
         //cv::Mat& mat_to_check = *dataToSave.imagePtr; // 
         //qDebug() << "准备转换的 Mat 不是空的。";
         //qDebug() << "它的类型ID是: " << mat_to_check.type(); // 打印类型ID
         //qDebug() << "它的通道数是: " << mat_to_check.channels(); // 打印通道数
         //qDebug() << "它的位深度是: " << mat_to_check.depth(); // 打印位深度 (例如 CV_8U 是 0)

         QImage qImage = convertMatToQImage(*dataToSave.imagePtr);
         if (qImage.isNull() == true)
         {
             LOG_DEBUG(GlobalLog::logger, _T("m_pParaDock ptr null"));
             continue;
         }
         //qDebug() << "---------------------------------------";
         //qDebug() << "QImage is null: " << qImage.isNull();
         //if (!qImage.isNull()) {
         //    qDebug() << "QImage width: " << qImage.width();
         //    qDebug() << "QImage height: " << qImage.height();
         //    qDebug() << "QImage format: " << qImage.format(); // 打印QImage::Format枚举值
         //    qDebug() << "QImage bytesPerLine: " << qImage.bytesPerLine();
         //    qDebug() << "QImage depth: " << qImage.depth();
         //    qDebug() << "QImage hasAlphaChannel: " << qImage.hasAlphaChannel();
         //}
         //qDebug() << "目标保存路径: " << qFullPath;
         //qDebug() << "目标目录是否存在: " << QDir().exists(QFileInfo(qFullPath).absolutePath());
         //qDebug() << "---------------------------------------";



         if (qImage.save(qFullPath, "JPG")) {
             std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

             // 2. 将 std::string (UTF-8) 转换为 std::wstring
             std::wstring wstr = converter.from_bytes(dataToSave.info);

             // 3. 传入宏，使用 std::wstring 的 c_str()
             LOG_DEBUG(GlobalLog::logger, wstr.c_str());
         } else {
             GlobalLog::logger.Mz_AddLog(L"QImage save failed");
         }
      //   std::cout<<"image ptr use count:  "<<dataToSave.imagePtr.use_count()<<std::endl;
      //  dataToSave.imagePtr.reset();

        qint64 elapsed = timer.elapsed();  // 获取经过的毫秒数
        qDebug() << "存图耗时：" << elapsed << "毫秒";
     //   Sleep(50);

//opencv
    //    try {

    //        std::cout<<"into try"<<std::endl;
    //        // if (!std::filesystem::exists(dataToSave.work_path))
    //        // {
    //        //     std::cout<<"file is not exsit"<<std::endl;
    //        //       std::cout<<"dataToSave.work_path          "<<dataToSave.work_path;
    //        //     if (!generateDir(dataToSave.work_path))
    //        //     {
    //        //         continue; // 创建失败
    //        //     }
    //        // }

    //        // 生成带时间戳的文件名 (std::string)
    //        std::string filename = generateStamp() + ".jpg"; // 确保使用 .jpg 后缀
    //  //   std::cout<<"dataToSave.work_path          "<<dataToSave.work_path<<std::endl;
    // //      std::string fullPath = dataToSave.work_path + "/" + filename;
    //     QString qFullPath = QDir(QString::fromStdString(dataToSave.work_path))
    //                             .filePath(QString::fromStdString(filename));

    //        qDebug()<<"qFullPath:           "<<qFullPath;
    //        // 使用 cv::imwrite 保存图片
    //        if (  cv::imwrite(qFullPath.toUtf8().constData(), *dataToSave.imagePtr)) {
    //             GlobalLog::logger.Mz_AddLog(L"save img successful");
    //             dataToSave.imagePtr.reset();
    //        } else {
    //            // cv::imwrite 返回 false 时，直接记录失败
    //            GlobalLog::logger.Mz_AddLog(L"cv::imwrite save img fail");
    //        }

    //    } catch (...) { // 捕获所有类型的异常
    //        GlobalLog::logger.Mz_AddLog(L"save img fail error");
    //    }

        

    }
    GlobalLog::logger.Mz_AddLog(L"Image Saver Worker gracefully stopped.");
}
