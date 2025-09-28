#include "displayinfowidget.h"

#include <QGridLayout>

#include <QLabel>

#include <QFont>



DisplayInfoWidget::DisplayInfoWidget(Parameters* params, QWidget* parent)

	: QWidget(parent)

{

	m_gridLayout = new QGridLayout(this);

	m_gridLayout->setContentsMargins(10, 10, 10, 10);

	m_gridLayout->setSpacing(5);

	this->setStyleSheet("background-color: #2E2E2E; color: white;");

	// 构造函数直接调用内部函数，完成所有解析和UI构建

	buildUIFromParameters(params);

}



DisplayInfoWidget::~DisplayInfoWidget() {}

void DisplayInfoWidget::updateLimitLabels(Parameters* params)
{
	if (!params) {
		qWarning() << "updateLimitLabels - 参数指针为空。";
		return;
	}

	// 1. 将传入的 Parameters 解析为 ProcessedParam
	QVector<ProcessedParam> processedParams;
	const QMap<QString, DetectionProject>& projects = params->detectionProjects;
	for (const auto& project : projects) {
		for (auto it = project.params.constBegin(); it != project.params.constEnd(); ++it) {
			const ParamDetail& detail = it.value();
			if (it.key().isEmpty()) continue;
			ProcessedParam p;
			p.originalName = it.key();
			p.mappedVariable = detail.name;
			p.value = detail.value;
			processedParams.append(p);
		}
	}

	// 2. 将 ProcessedParam 配对并分组
	QMap<QString, QPair<QVariant, QVariant>> groupedLimits;
	for (const auto& p : processedParams) {
		QString fullName = p.originalName;
		QString baseName = fullName;

		if (baseName.endsWith("(上限)")) {
			baseName.chop(4);
			groupedLimits[baseName].second = p.value;
		}
		else if (baseName.endsWith("(下限)")) {
			baseName.chop(4);
			groupedLimits[baseName].first = p.value;
		}
	}

	// 3. 遍历UI行，更新上下限标签
	for (auto it = groupedLimits.constBegin(); it != groupedLimits.constEnd(); ++it) {
		const QString& baseName = it.key();
		const QPair<QVariant, QVariant>& limits = it.value();

		if (m_uiRows.contains(baseName)) {
			const auto& rowWidgets = m_uiRows.value(baseName);

			// 更新下限值
			rowWidgets.lowerLimitLabel->setText(limits.first.isValid() ? limits.first.toString() : "--");

			// 更新上限值
			rowWidgets.upperLimitLabel->setText(limits.second.isValid() ? limits.second.toString() : "--");
		}
	}
}

void DisplayInfoWidget::buildUIFromParameters(Parameters* params)

{

	// 1. 清理旧UI和数据

	QLayoutItem* item;

	while ((item = m_gridLayout->takeAt(0)) != nullptr) {

		if (item->widget()) delete item->widget();

		delete item;

	}

	m_uiRows.clear();

	m_ngCounters.clear();



	// 2. 创建表头

	QStringList headers = { "参数名称", "下限值", "上限值", "实测值", "不符数" };

	for (int i = 0; i < headers.size(); ++i) {

		auto headerLabel = new QLabel(headers[i]);

		headerLabel->setFont(QFont("微软雅黑", 12, QFont::Bold));

		headerLabel->setStyleSheet("background-color: #444; padding: 5px; border-radius: 3px;");

		headerLabel->setAlignment(Qt::AlignCenter);

		m_gridLayout->addWidget(headerLabel, 0, i);

	}



	if (!params) return; // 安全检查



	// 3. 将传入的 Parameters 解析为 ProcessedParam (逻辑内聚)

	QVector<ProcessedParam> processedParams;

	const QMap<QString, DetectionProject>& projects = params->detectionProjects;

	for (const auto& project : projects) {
		for (auto it = project.params.constBegin(); it != project.params.constEnd(); ++it) {
			const ParamDetail& detail = it.value();
			if (it.key().isEmpty()) continue; // 检查key是否为空
			ProcessedParam p;
			p.originalName = it.key();       
			p.mappedVariable = detail.name; 
			p.value = detail.value;
			processedParams.append(p);
		}
	}



	// 4. 将 ProcessedParam 配对并创建UI行 (逻辑内聚)

	QMap<QString, QPair<QVariant, QVariant>> groupedLimits;
	QList<QString> orderedBaseNames;

	for (const auto& p : processedParams) {
		QString fullName = p.originalName;
		QString baseName = fullName;

		// 智能地移除中文后缀，得到“基础中文名”
		if (baseName.endsWith("(上限)")) {
			baseName.chop(4); // 移除后4个字符 "(上限)"
			groupedLimits[baseName].second = p.value; // 存入上限值
		}
		else if (baseName.endsWith("(下限)")) {
			baseName.chop(4); // 移除后4个字符 "(下限)"
			groupedLimits[baseName].first = p.value; // 存入下限值
		}
		else {
			// 对于没有上下限后缀的独立参数
			groupedLimits[baseName].second = p.value;
		}

		// 记录参数的出现顺序，确保UI顺序和配置文件一致
		if (!orderedBaseNames.contains(baseName)) {
			orderedBaseNames.append(baseName);
		}
	}


	// 5. 创建UI控件

	QFont valueFont("Arial", 12, QFont::Bold);
	int currentRow = 1;
	for (const QString& baseName : orderedBaseNames) {
		const auto& limits = groupedLimits.value(baseName);

		auto nameLabel = new QLabel(baseName); // <<< 使用正确的基础中文名
		auto lowerLimitLabel = new QLabel(limits.first.isValid() ? limits.first.toString() : "--");
		auto upperLimitLabel = new QLabel(limits.second.isValid() ? limits.second.toString() : "--");
		auto measuredValueLabel = new QLabel("-");
		auto ngCountLabel = new QLabel("0");

		nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		lowerLimitLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
		upperLimitLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
		measuredValueLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
		ngCountLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
		ngCountLabel->setStyleSheet("color: #DC3545;");

		m_gridLayout->addWidget(nameLabel, currentRow, 0);
		m_gridLayout->addWidget(lowerLimitLabel, currentRow, 1);
		m_gridLayout->addWidget(upperLimitLabel, currentRow, 2);
		m_gridLayout->addWidget(measuredValueLabel, currentRow, 3);
		m_gridLayout->addWidget(ngCountLabel, currentRow, 4);

		m_uiRows[baseName] = { nameLabel, lowerLimitLabel, upperLimitLabel, measuredValueLabel, ngCountLabel };
		m_ngCounters[baseName] = 0;
		currentRow++;
	}



	m_gridLayout->setColumnStretch(0, 3); // ... (设置列伸展)

}

void DisplayInfoWidget::onUpdateParameters(Parameters m_parameters)
{
	updateLimitLabels(&m_parameters);
}

void DisplayInfoWidget::onPaintSend(QVector<PaintDataItem> paintData)
{
	updateData(paintData);
}

void DisplayInfoWidget::updateData(const QVector<PaintDataItem>& paintData)
{
	
	for (const auto& item : paintData) {
		
		if (m_uiRows.contains(item.label)) {	
    		const auto& rowWidgets = m_uiRows.value(item.label);
			qDebug() << " item.label:  " << item.label;
			rowWidgets.measuredValueLabel->setText(item.value);	
			qDebug() << " item.value:  " << item.value;
			rowWidgets.ngCountLabel->setText(QString::number(item.count));
		}
	}
}

void DisplayInfoWidget::resetCounters()

{

	for (auto it = m_ngCounters.begin(); it != m_ngCounters.end(); ++it) {

		it.value() = 0;

		if (m_uiRows.contains(it.key())) {

			m_uiRows.value(it.key()).ngCountLabel->setText("0");

		}

	}

}