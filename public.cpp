#include "public.h"

QString SystemPara::VERSION = "电容视觉检测系统";
QString SystemPara::DATA_DIR = "../../../data";
QString SystemPara::LOG_DIR = "../var";
QString SystemPara::OK_DIR = "F:/Industry_Detection/log";
QString SystemPara::NG_DIR = "F:/Industry_Detection/log";
QString SystemPara::ROOT_DIR = "../../../";
QString SystemPara::CAMERAL_DIR = "../../../ini/globe/cameral.json";
QString SystemPara::GLOBAL_DIR = "../../../ini/globe/Global.json"; 

GlobalPara::IoType GlobalPara::ioType = GlobalPara::IoType::VC3000H;
GlobalPara::CameraType GlobalPara::cameraType = GlobalPara::CameraType::DHUA;
GlobalPara::Envirment GlobalPara::envirment = GlobalPara::Envirment::LocationEn;
int GlobalPara::RunPoint = 8;
std::atomic<bool> GlobalPara::changed = false;
std::atomic<bool> GlobalPara::Stagnant = false;
std::atomic<bool> GlobalPara::cheatFlag = false;
std::atomic<bool> GlobalPara::pulse = false;
int GlobalPara::MergePoint = 0;
int GlobalPara::MergePointNum = 2;
int GlobalPara::Light1 = 100;
int GlobalPara::Light2 = 100;
int GlobalPara::Light3 = 100;
int GlobalPara::Light4 = 100;

QHash<QString, std::deque<int>> MergePointVec;
std::mutex g_mutex;
std::condition_variable g_cv;


QString GlobalPara::DeviceId = "Q01";

InStampParam LearnPara::inParam1 = {
	180, 3, 3, false, false,false,true
};

InStampParam LearnPara::inParam2 = {
	90, 3, 3, false, true, false,true
};

InTopParam LearnPara::inParam3 = {
	-90,false ,false,0.5,0.3
};

InSideParam LearnPara::inParam4 = {
 false,-90,90,40,100,false,0.5,0.3
};

InPinParam LearnPara::inParam5 = {
 0,999,0,40,200,false,0,0,0.5,0.3
};

InAbutParam LearnPara::inParam6 = {
	0,        // angleNum
	0.0f,     // p_pin_min
	0.43f,    // p_pin_max
	0.45f,    // b_pln_min
	0.75f,    // b_pln_max
	0.78f,    // n_pin_min
	1.0f,     // n_pin_max
	160.0f,   // plateOffsetY
	100.0f,   // plateHeight
	250.0f,   // waistOffsetY
	50.0f,    // waistHeight
	5,        // gauss_ksize
	60,       // bin_thres_value
	0.1f,     // prodAreaMin
	100.0f,   // pinMcHeight
	100.0f    // plnMcHeight
};

InFlowerPinParam LearnPara::inParam7 = { false,0,220,2,4 };

InFlowerPinParam LearnPara::inParam8 = { false,0,180,2,4 };

InLookPinParam LearnPara::inParam9 = { false,0};
int LearnPara::inNum = 15;
namespace GlobalLog {
	Mz_Log::COperation logger;
}


	void MyImageCallback(cv::Mat & image, void* pUser)
	{
		LOG_DEBUG(GlobalLog::logger, L"cap picture successful");
		if (pUser == nullptr) {
			LOG_DEBUG(GlobalLog::logger, L"ptr null");
			return;
		}

		if (image.empty()) {
			LOG_DEBUG(GlobalLog::logger, L"ptr null");
			qCritical() << "MyImageCallback: Input 'image' is empty before cloning! This is the root cause.";
			 return;
		}

		if (!image.data) {
			LOG_DEBUG(GlobalLog::logger, L"ptr null");
			qCritical() << "MyImageCallback: Input 'image.data' is null before cloning! This is also a root cause.";
			 return;
		}

		qDebug() << "MyImageCallback: Input 'image' valid. Size:" << image.size().width << "x" << image.size().height;

		std::shared_ptr<cv::Mat> currentImageForQueue = std::make_shared<cv::Mat>(image.clone());
		if (!currentImageForQueue) {
			LOG_DEBUG(GlobalLog::logger, L"ptr null");
			return;
		}

		auto* DequePtr = reinterpret_cast<ImageQueuePack*>(pUser);
		if (DequePtr == nullptr) {
			LOG_DEBUG(GlobalLog::logger, L"ptr null");
			return;
		}
		else {
			std::unique_lock<std::mutex> lock(DequePtr->mutex);
			if(DequePtr->queue.size()<10)DequePtr->queue.push_back(currentImageForQueue);
			else
			{
				DequePtr->queue.pop_front();
				DequePtr->queue.push_back(currentImageForQueue);
			}
			qDebug() << "  DequePtr->queue.size():    " << DequePtr->queue.size();
			DequePtr->cond.notify_one();
		}

		if (currentImageForQueue)
			qDebug() << "currentImageForQueue. is not null";
		else
			qDebug() << "currentImageForQueue. is  null";

		currentImageForQueue.reset();
	}


	qint64 getAvailableSystemMemoryMB()
	{
		PROCESS_MEMORY_COUNTERS pmc = { 0 };
		if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
		{
			// pmc.PrivateUsage: 当前进程占用的私有内存
			// pmc.WorkingSetSize: 当前进程占用的物理内存
			// 计算当前进程的可用虚拟内存
			MEMORYSTATUSEX memInfo;
			memInfo.dwLength = sizeof(memInfo);
			if (GlobalMemoryStatusEx(&memInfo))
			{
				qint64 availablePhysMB = memInfo.ullAvailPhys / (1024 * 1024);
				qint64 workingSetMB = pmc.WorkingSetSize / (1024 * 1024);
				// 剩余可分配给当前进程的物理内存近似值
				return availablePhysMB - workingSetMB;
			}
		}

		return 0;
	}


cv::Mat convertQImageToMat(const QImage& image)
{
	if (image.isNull()) {
		qWarning() << "convertQImageToMat: Input QImage is null.";
		return cv::Mat();
	}


	// 2. 根据 QImage 格式转换为 cv::Mat
	switch (image.format()) {
	case QImage::Format_Grayscale8:  // 单通道灰度
		return cv::Mat(image.height(), image.width(), CV_8UC1,
			const_cast<uchar*>(image.bits()),
			static_cast<size_t>(image.bytesPerLine())).clone();

	case QImage::Format_RGB888:      // 3通道 RGB
		return cv::Mat(image.height(), image.width(), CV_8UC3,
			const_cast<uchar*>(image.bits()),
			static_cast<size_t>(image.bytesPerLine())).clone();

	case QImage::Format_RGBA8888:    // 4通道 RGBA
		return cv::Mat(image.height(), image.width(), CV_8UC4,
			const_cast<uchar*>(image.bits()),
			static_cast<size_t>(image.bytesPerLine())).clone();

	default:
		// 其他格式需先转换（例如 ARGB32 -> RGBA8888）
		QImage converted = image.convertToFormat(QImage::Format_RGBA8888);
		return cv::Mat(converted.height(), converted.width(), CV_8UC4,
			const_cast<uchar*>(converted.bits()),
			static_cast<size_t>(converted.bytesPerLine())).clone();
	}

	//return mat;
}


bool containsOnlyAscii(const QString& str)
{
	for (const QChar& c : str) {
		if (c.toLatin1() == '\0' && c.unicode() != '\0') { // 检查是否是多字节字符
			return false;
		}
	}
	return true;
}



/*
cv::Mat loadMatFromFile_backup(const QString& filePath)
{
	if (filePath.isEmpty()) {
		qWarning() << "loadMatFromFile: 文件路径为空。";
		return cv::Mat();
	}

	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly)) {
		qWarning() << "loadMatFromFile: 无法打开文件：" << filePath << "，错误：" << file.errorString();
		return cv::Mat();
	}

	QByteArray imageData = file.readAll();
	file.close();

	if (imageData.isEmpty()) {
		qWarning() << "loadMatFromFile: 读取文件内容为空或文件不存在：" << filePath;
		return cv::Mat();
	}

	std::vector<uchar> buffer(imageData.begin(), imageData.end());

	cv::Mat image = cv::imdecode(buffer, cv::IMREAD_UNCHANGED);
	if (image.empty()) {
		qWarning() << "loadMatFromFile: 无法从内存数据解码图像或图像为空：" << filePath;
		return cv::Mat();
	}

	if (image.channels() == 4) {
		qDebug() << "loadMatFromFile: 图像为 4 通道，将其转换为 3 通道 BGR。";
		cv::cvtColor(image, image, cv::COLOR_BGRA2BGR);
	} else if (image.channels() == 1) {
		qDebug() << "loadMatFromFile: 图像为 1 通道 (灰度图)。";
	} else if (image.channels() == 3) {
		qDebug() << "loadMatFromFile: 图像为 3 通道 (彩色图)。";
	} else {
		qWarning() << "loadMatFromFile: 图像通道数不常见，当前为" << image.channels() << "通道。";
		return cv::Mat();
	}

	// 返回直接解码出来的 image（不 clone，也不是指针）
	return image;
}
*/



cv::Mat loadMatFromFile(const QString& filePath)
{
	if (filePath.isEmpty()) {
		qWarning() << "loadMatFromFile: 文件路径为空。";
		return cv::Mat(); // 返回空的 cv::Mat
	}

	// 1. 直接将 QString 转换为 std::string
	//    请再次注意：这种直接转换对于包含非ASCII字符的路径（如中文）可能导致问题，
	//    取决于系统的本地编码设置和OpenCV版本。对于纯ASCII且分隔符正确的路径，这是最直接的方式。
	std::string sFilePath = filePath.toStdString();
	// 如果之前测试过 nativeFilePath.toLocal8Bit().constData() 更可靠，可以继续使用：
	// std::string sFilePath = filePath.toLocal8Bit().constData();


	qDebug() << "loadMatFromFile: 尝试使用 cv::imread。路径: " << filePath;

	// 2. 使用 cv::imread 从文件读取图像
	//    IMREAD_UNCHANGED 标志会加载图像的原始通道数 (1, 3, 或 4)，不进行任何转换。
	cv::Mat image = cv::imread(sFilePath);

	if (image.empty()) {
		qWarning() << "loadMatFromFile: 无法从文件加载或解码图像：" << filePath
			<< "。请检查路径是否存在，文件是否损坏，或OpenCV解码器是否缺失。";
		return cv::Mat(); // 返回空的 cv::Mat
	}

	// 不进行任何通道转换或其他处理，直接返回 image
	return image;
}


/*
std::shared_ptr<cv::Mat> loadImageFromFile_backup(const QString& filePath) {
	if (filePath.isEmpty()) {
		qWarning() << "loadImageFromFile: 文件路径为空。";
		return nullptr;
	}

	// 1. 使用 QFile 读取文件内容到 QByteArray
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly)) {
		qWarning() << "loadImageFromFile: 无法打开文件：" << filePath << "，错误：" << file.errorString();
		return nullptr;
	}

	QByteArray imageData = file.readAll();
	file.close();

	if (imageData.isEmpty()) {
		qWarning() << "loadImageFromFile: 读取文件内容为空或文件不存在：" << filePath;
		return nullptr;
	}

	// 2. 将 QByteArray 转换为 std::vector<uchar>，供 OpenCV 使用
	std::vector<uchar> buffer(imageData.begin(), imageData.end());

	// 3. 使用 cv::imdecode 从内存解码图像，flags 设置为 IMREAD_UNCHANGED
	// 这将根据图像文件本身的通道数加载（灰度1通道，彩色3通道，带alpha4通道）
	cv::Mat image = cv::imdecode(buffer,cv::IMREAD_COLOR);

	if (image.empty()) {
		qWarning() << "loadImageFromFile: 无法从内存数据解码图像或图像为空：" << filePath;
		return nullptr;
	}

	// 4. 根据实际加载的图像通道数进行处理（如果需要的话）
	// 你提到通常只有 8UC1 和 8UC3，所以这里主要处理这两种情况。
	// 如果是 4 通道（例如有 Alpha），则转换为 3 通道 BGR。
	if (image.channels() == 4) {
		qDebug() << "loadImageFromFile: 图像为 4 通道，将其转换为 3 通道 BGR。";
		cv::cvtColor(image, image, cv::COLOR_BGRA2BGR);
	} else if (image.channels() == 1) {
		qDebug() << "loadImageFromFile: 图像为 1 通道 (灰度图)。";
		// 如果是 1 通道，保持不变
	} else if (image.channels() == 3) {
		qDebug() << "loadImageFromFile: 图像为 3 通道 (彩色图)。";
		// 如果是 3 通道，保持不变
		// 注意：imdecode 会将 JPG 加载为 BGR 顺序，这是 OpenCV 默认的彩色顺序，无需再转换。
	} else {
		qWarning() << "loadImageFromFile: 图像通道数不常见，当前为 " << image.channels() << " 通道。";
		// 可以根据需要添加其他通道数的处理逻辑，或直接返回错误
		return nullptr; // 或者根据需求选择默认转换为8U3或8U1
	}

	// 5. 进行一次深拷贝，将图像数据复制到 shared_ptr 管理的内存中
	return std::make_shared<cv::Mat>(image.clone());
}*/

void renameJpgFiles(const QString& directoryPath)
{
	// 1. 创建QDir对象并检查路径有效性
	QDir dir(directoryPath);
	if (!dir.exists()) {
		qDebug() << "错误：" << directoryPath << "不是一个有效的目录。";
		return;
	}

	// 2. 使用 QDir::entryInfoList 获取该目录下的所有文件信息
	//    QDir::Files: 只列出文件
	//    QDir::NoDotAndDotDot: 排除 '.' 和 '..'
	//    QDir::Name: 按文件名排序
	QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);

	// 3. 筛选出所有以 .jpg 或 .JPG 结尾的文件
	QFileInfoList jpgFiles;
	for (const QFileInfo& fileInfo : fileInfoList) {
		if (fileInfo.suffix().compare("jpg", Qt::CaseInsensitive) == 0) {
			jpgFiles.append(fileInfo);
		}
	}

	if (jpgFiles.isEmpty()) {
		qDebug() << "在" << directoryPath << "中没有找到 .jpg 文件。";
		return;
	}

	qDebug() << "在" << directoryPath << "中找到了" << jpgFiles.size() << "个 .jpg 文件，开始重命名...";

	// 4. 遍历文件列表并重命名，从 1 开始计数
	int currentNumber = 1;
	for (const QFileInfo& fileInfo : jpgFiles) {
		QString oldPath = fileInfo.absoluteFilePath();
		QString newFileName = QString("%1.jpg").arg(currentNumber);
		QString newPath = QDir(directoryPath).filePath(newFileName);

		// 检查新文件名是否已经存在，如果存在则跳过以防覆盖
		if (QFile::exists(newPath)) {
			qDebug() << "警告：" << newPath << "已存在，跳过重命名" << fileInfo.fileName() << "。";
			continue;
		}

		// 5. 执行重命名操作
		if (QFile::rename(oldPath, newPath)) {
			qDebug() << "'" << fileInfo.fileName() << "' -> '" << newFileName << "'";
			currentNumber++;
		}
		else {
			qDebug() << "错误：无法重命名 '" << fileInfo.fileName() << "'。";
		}
	}

	qDebug() << "重命名完成。";
}

std::shared_ptr<cv::Mat> loadImageFromFile(const QString& filePath) {
	if (filePath.isEmpty()) {
		qWarning() << "loadImageFromFile: 文件路径为空。";
		return nullptr;
	}


	std::string sFilePath = filePath.toStdString();
	// std::string sFilePath = filePath.toLocal8Bit().constData();

	qDebug() << "loadImageFromFile: 尝试使用 cv::imread。路径: " << filePath;

	// 2. 使用 cv::imread 从文件读取图像
	cv::Mat image = cv::imread(sFilePath);

	if (image.empty()) {
		qWarning() << "loadImageFromFile: 无法从文件加载或解码图像：" << filePath
			<< "。请检查路径是否存在，文件是否损坏，或OpenCV解码器是否缺失。";
		return nullptr;
	}

	// 3. 进行一次深拷贝，将图像数据复制到 shared_ptr 管理的内存中
	//    这确保了 shared_ptr 持有的是图像数据的独立深拷贝，不受 image 局部变量生命周期影响。
	return std::make_shared<cv::Mat>(image.clone());
}

cv::Mat QPixmapToMat(const QPixmap& pixmap)
{
	// 1. QPixmap -> QImage
	QImage image = pixmap.toImage();

	// 2. 根据 QImage 格式转换为 cv::Mat
	switch (image.format()) {
	case QImage::Format_Grayscale8: // 单通道灰度图像
	{
		// 对于灰度图，无需转换颜色空间，直接克隆数据即可
		return cv::Mat(image.height(), image.width(), CV_8UC1,
			const_cast<uchar*>(image.bits()),
			static_cast<size_t>(image.bytesPerLine())).clone();
	}
	case QImage::Format_RGB888: // 3通道 RGB
	{
		// 先创建一个包装 QImage 数据的 Mat (不复制)
		cv::Mat tempMat(image.height(), image.width(), CV_8UC3,
			const_cast<uchar*>(image.bits()),
			static_cast<size_t>(image.bytesPerLine()));

		// 然后转换颜色空间从 RGB 到 BGR，并返回一个新的 Mat
		cv::Mat resultMat;
		cv::cvtColor(tempMat, resultMat, cv::COLOR_RGB2BGR);
		return resultMat;
	}
	case QImage::Format_RGBA8888: // 4通道 RGBA
	{
		// 先创建一个包装 QImage 数据的 Mat (不复制)
		cv::Mat tempMat(image.height(), image.width(), CV_8UC4,
			const_cast<uchar*>(image.bits()),
			static_cast<size_t>(image.bytesPerLine()));

		// 转换颜色空间从 RGBA 到 BGRA
		cv::Mat resultMat;
		cv::cvtColor(tempMat, resultMat, cv::COLOR_RGBA2BGRA);
		return resultMat;
	}
	default:
	{
		// 其他格式先统一转换为 RGBA，再进行处理
		QImage converted = image.convertToFormat(QImage::Format_RGBA8888);
		cv::Mat tempMat(converted.height(), converted.width(), CV_8UC4,
			const_cast<uchar*>(converted.bits()),
			static_cast<size_t>(converted.bytesPerLine()));

		cv::Mat resultMat;
		cv::cvtColor(tempMat, resultMat, cv::COLOR_RGBA2BGRA);
		return resultMat;
	}
	}
}

bool areMatFormatsIdentical(const cv::Mat& mat1, const cv::Mat& mat2) {
	// 1. 检查是否都为空 (empty())
	// 两个都为空也算格式一致
	if (mat1.empty() && mat2.empty()) {
		std::cout << "Info: Both Mats are empty, considered identical format." << std::endl;
		return true;
	}
	// 一个为空，另一个不为空，则格式不一致
	if (mat1.empty() != mat2.empty()) {
		std::cerr << "Error: One Mat is empty, the other is not. Formats differ." << std::endl;
		return false;
	}
	// 如果执行到这里，说明两个Mat都不为空

	// 2. 检查维度 (dims)
	if (mat1.dims != mat2.dims) {
		std::cerr << "Error: Mat dimensions differ. Mat1 dims: " << mat1.dims
			<< ", Mat2 dims: " << mat2.dims << std::endl;
		return false;
	}

	// 3. 检查尺寸 (size - rows and cols for 2D, or full size array for N-D)
	// 对于2D图像，直接比较rows和cols
	if (mat1.dims == 2) {
		if (mat1.rows != mat2.rows || mat1.cols != mat2.cols) {
			std::cerr << "Error: Mat sizes differ. Mat1 size: " << mat1.size()
				<< ", Mat2 size: " << mat2.size() << std::endl;
			return false;
		}
	}
	else { // 对于N-D Mat，比较每个维度的大小
		for (int i = 0; i < mat1.dims; ++i) {
			if (mat1.size[i] != mat2.size[i]) {
				std::cerr << "Error: Mat sizes differ at dimension " << i << ". Mat1 size: "
					<< mat1.size[i] << ", Mat2 size: " << mat2.size[i] << std::endl;
				return false;
			}
		}
	}


	// 4. 检查数据类型 (type) - 包括深度和通道数
	// cv::Mat::type() 返回一个整数，包含了深度和通道数的信息
	// 例如：CV_8UC3 表示 8位无符号，3通道
	if (mat1.type() != mat2.type()) {
		std::cerr << "Error: Mat types differ. Mat1 type: " << mat1.type()
			<< ", Mat2 type: " << mat2.type() << std::endl;
		// 你也可以打印出更友好的类型字符串，例如：
		// std::cerr << "Error: Mat types differ. Mat1: " << cv::typeToString(mat1.type())
		//           << ", Mat2: " << cv::typeToString(mat2.type()) << std::endl;
		return false;
	}

	// 如果所有检查都通过，则格式完全一致
	std::cout << "Info: Both Mats have identical format." << std::endl;
	return true;
}

std::string generateStamp()
{
	QDateTime currentDateTime = QDateTime::currentDateTime();
	return currentDateTime.toString("yyyyMMdd_hhmmss_zzz").toStdString();
}

bool generateDir(const std::string& dirPath)
{
	QString qDirPath = QString::fromStdString(dirPath);
	QDir dir(qDirPath);

	if (!dir.exists()) {
		if (!dir.mkpath("."))
		{
			GlobalLog::logger.Mz_AddLog(QString("Error: Failed to create directory: %1").arg(qDirPath).toStdWString().c_str());
			return false;
		}
	}
	return true;
}

QImage convertMatToQImage(const cv::Mat& mat) {
	if (mat.empty()) {
		qWarning("MatToQImageConverter: Input Mat is empty.");
		LOG_DEBUG(GlobalLog::logger, _T("afterImagePtr ptr null"));
		return QImage();
	}
	switch (mat.type()) {
	case CV_8UC1: {
		// 创建视图后，立即调用.copy()创建深拷贝
		QImage image(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_Grayscale8);
		return image.copy();
	}
	case CV_8UC3: {
		QImage image(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_BGR888);
		return image.copy();
	}
	case CV_8UC4: {
		cv::Mat rgbaMat;
		cv::cvtColor(mat, rgbaMat, cv::COLOR_BGRA2RGBA);
		// 这里也一样，先创建视图，再拷贝
		QImage image(rgbaMat.data, rgbaMat.cols, rgbaMat.rows, rgbaMat.step, QImage::Format_RGBA8888);
		return image.copy();
	}
	default:
		qWarning("MatToQImageConverter: Unsupported Mat type.");
		return QImage();
	}
}


QPixmap convertMatToPixmap(const cv::Mat& mat)
{
	if (mat.empty()) {
		qWarning("convertMatToPixmap: 输入图像为空");
		return QPixmap();
	}

	QImage image;

	switch (mat.type()) {
	case CV_8UC1:
		image = QImage(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_Grayscale8);
		break;
	case CV_8UC3:
		image = QImage(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_BGR888);
		break;
	case CV_8UC4:
		image = QImage(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_ARGB32);
		break;
	default:
		qWarning("convertMatToPixmap: 不支持的图像类型: %d", mat.type());
		return QPixmap();
	}

	if (image.isNull()) {
		qWarning("convertMatToPixmap: QImage 构造失败");
		return QPixmap();
	}

	// 深拷贝为 QPixmap 返回
	return QPixmap::fromImage(image.copy());
}

void printOutStampResParam(const OutStampResParam& param, const std::string& structName)
{
	std::cout << "--- " << structName << " Details ---" << std::endl;
	std::cout << "phi:        " << param.phi << std::endl;
	std::cout << "posWidth:   " << param.posWidth << std::endl;
	std::cout << "posErr:     " << param.posErr << std::endl;
	std::cout << "negWidth:   " << param.negWidth << std::endl;
	std::cout << "negErr:     " << param.negErr << std::endl;
	std::cout << "textNum:    " << param.textNum << std::endl;

	std::cout << "textScores: [";
	if (param.textScores.empty()) {
		std::cout << "]" << std::endl;
	}
	else {
		// 设置浮点数输出精度
		std::cout << std::fixed << std::setprecision(4);
		for (size_t i = 0; i < param.textScores.size(); ++i) {
			std::cout << param.textScores[i];
			if (i < param.textScores.size() - 1) {
				std::cout << ", ";
			}
		}
		std::cout << "]" << std::endl;
	}

	std::cout << "logoScores: " << std::fixed << std::setprecision(4) << param.logoScores << std::endl;
	std::cout << "--------------------------" << std::endl;
}


qint64 getAvailableVRAM_MB()
{
	// 初始化COM库，DXGI需要
	HRESULT com_hr = CoInitialize(NULL);

	IDXGIFactory* pFactory = nullptr;
	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);

	if (FAILED(hr) || !pFactory) {
		qWarning() << "创建 DXGI Factory 失败。";
		if (SUCCEEDED(com_hr)) CoUninitialize();
		return 0;
	}

	IDXGIAdapter* pAdapter = nullptr;
	IDXGIAdapter3* pAdapter3 = nullptr;
	qint64 availableMemory = 0;

	// 枚举第一个适配器（通常是主显卡）
	if (pFactory->EnumAdapters(0, &pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		// 尝试查询 IDXGIAdapter3 接口，这是获取显存预算所必需的
		hr = pAdapter->QueryInterface(__uuidof(IDXGIAdapter3), (void**)&pAdapter3);
		if (SUCCEEDED(hr) && pAdapter3)
		{
			DXGI_QUERY_VIDEO_MEMORY_INFO memoryInfo = { 0 };
			// 查询本机（非共享）显存段的信息
			hr = pAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memoryInfo);

			if (SUCCEEDED(hr))
			{
				// 可用预算 = 总预算 - 当前使用量
				qint64 availableBudgetBytes = memoryInfo.Budget - memoryInfo.CurrentUsage;
				availableMemory = availableBudgetBytes / (1024 * 1024);
			}
			else {
				qWarning() << "QueryVideoMemoryInfo 失败。";
			}
		}
		else {
			qWarning() << "查询 IDXGIAdapter3 接口失败，您的系统或驱动程序可能不支持此功能。";
		}
	}

	// 释放所有已创建的COM对象，防止资源泄漏
	if (pAdapter3) pAdapter3->Release();
	if (pAdapter) pAdapter->Release();
	if (pFactory) pFactory->Release();
	if (SUCCEEDED(com_hr)) CoUninitialize();

	return availableMemory > 0 ? availableMemory : 0;
}


int CheckRAM()
{
	const qint64 SAFE_RAM_THRESHOLD_MB = 80;  // 系统内存阈值
	const qint64 SAFE_VRAM_THRESHOLD_MB = 256; // 显存阈值

	// 检查系统可用内存 (RAM)
	qint64 availableRAM_MB = getAvailableSystemMemoryMB();
	QString str = QString("当前空闲物理内存: %1 MB").arg(availableRAM_MB);
	LOG_DEBUG(GlobalLog::logger, str.toStdWString().c_str());
	if (availableRAM_MB > 0 && availableRAM_MB < SAFE_RAM_THRESHOLD_MB)
	{

		LOG_DEBUG(GlobalLog::logger, _T("System RAM is low, skipping image processing to prevent crash."));
		return-1; // RAM 不足，提前退出
	}

	// 检查可用显存 (VRAM)
	qint64 availableVRAM_MB = getAvailableVRAM_MB();
	if (availableVRAM_MB > 0 && availableVRAM_MB < SAFE_VRAM_THRESHOLD_MB)
	{

		LOG_DEBUG(GlobalLog::logger, _T("Available VRAM budget is low, skipping image processing."));
		return -1; // VRAM 不足，提前退出
	}
	return 0;
}

int CheckPixmap(const QPixmap& pixmap)
{
	// 1 安全检查：空 pixmap
	if (pixmap.isNull()) {
		qWarning() << "[ZoomableLabel] pixmap is null, skip display.";
		return -1;
	}

	// 2. 安全检查：cacheKey
	if (pixmap.cacheKey() == 0) {
		qWarning() << "[ZoomableLabel] pixmap cacheKey is 0, skip display.";
		return -1;
	}

	// 3. 尺寸检查（防止极端值）
	int w = pixmap.width();
	int h = pixmap.height();
	if (w <= 0 || h <= 0 || w > 10000 || h > 10000) {
		qWarning() << "[ZoomableLabel] pixmap size invalid:" << w << "x" << h;
		return -1;
	}
	return 0;
}