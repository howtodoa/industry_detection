#ifndef REZULTINFO_H
#define REZULTINFO_H
#include <QObject>
#include "fileoperator.h"
#include "rangeclass.h"
#include "limits.h"
#include "typdef.h"
#include <QPair> 
#include <vector>



class RezultInfo : public QObject
{
    Q_OBJECT
public:
    explicit RezultInfo();
    explicit RezultInfo(Parameters *rangepara, QObject *parent = nullptr);
    virtual ~RezultInfo();
    const QList<ProcessedParam>& getProcessedData() const;
    void updateUnifyParams(AllUnifyParams unifyParams);
    void printProcessedData() const;
    void printScore();
    void updatefix(double fix);
    void updateProcessedData(Parameters* rangePara);
    void updatePaintDataFromScaleArray(const QVector<SimpleParam>& scaleArray);
   virtual void updatePaintData(Parameters* rangePara);
   virtual void printCheckInfo(const QString& paramName, float actualValue, float thresholdValue, bool isUpperLimit, bool outOfRange);
   void printOutPlateResParam(const OutPlateResParam& param);
    virtual int judge_stamp(const OutStampResParam &ret);
    virtual int judge_plate(const  OutPlateResParam &ret);
    virtual int judge_lift(const OutLiftResParam &ret);
    virtual int judge_abut(const OutAbutResParam &ret);
    virtual int judge_pin(const OutPinParam& ret);
    virtual int judge_top(const OutTopParam& ret);
    virtual int judge_side(const OutSideParam& ret);
    virtual int judge_flower_pin(const OutFlowerPinResParam& ret);
    virtual int judge_stamp_min(const OutStampResParam& ret);
    virtual int judge_look(const OutLookPinResParam& ret);
    float getAdjustedLowerThreshold(const QString& key) const;
    float getAdjustedUpperThreshold(const QString& key) const;
    float getCompensationValue(const QString& mappedKey) const;
   void printPaintDataVector(const QVector<PaintDataItem>& dataVector, const QString& description = "");
    void updateActualValues(const OutAbutResParam& ret);
    void updateActualValues(const OutPlateResParam& ret);
	void updateActualValues(const OutFlowerPinResParam& ret);
    void updateActualValues(const OutLookPinResParam& ret);
    template<typename T>
    bool scaleDimensions(T& data, double scaleFactor)
    {
        if (scaleFactor <= 0) {
            qWarning() << "RezultInfo::scaleDimensions: scaleFactor must be positive.";
            return false;
        }

        if constexpr (std::is_same_v<T, OutStampResParam>) {
            // OutStampResParam 
            data.posWidth = data.posWidth * scaleFactor;
            data.negWidth = data.negWidth * scaleFactor;
            data.negErr = data.negErr * scaleFactor * scaleFactor;
            data.posErr= data.posErr * scaleFactor * scaleFactor;
            // phi, posErr, negErr, textNum, textScores, logoScores 不做操作
            qDebug() << "Scaled OutStampResParam dimensions.";
            return true;
        }
        else if constexpr (std::is_same_v<T, OutPlateResParam>) {
            // OutPlateResParam 
			const double epsilon = 1e-6; // 容差值
            data.m_PlateHeight *= scaleFactor;
            data.m_PlateArea *= scaleFactor * scaleFactor;
            data.m_PlateWid *= scaleFactor;
            data.m_PlateHypUpLen *= scaleFactor;
            data.m_PlateHypDownLen *= scaleFactor;
            data.m_PlatePinLeftHeight *= scaleFactor;
            data.m_PlatePinRightHeight *= scaleFactor;
            data.m_PlatePinLeftWid *= scaleFactor;
            data.m_PlatePinRightWid *= scaleFactor;
            data.m_PlatePinLeftExceLen *= scaleFactor;
            data.m_PlatePinRightExceLen *= scaleFactor;
            data.m_PlatePinLeftBendAngle *= scaleFactor;
            data.m_PlatePinRightBendAngle *= scaleFactor;

            for(auto item: m_PaintData){
                if(item.label == "引线总长度"){
                    double finalScale = (std::abs(item.scale - 1.00) < epsilon) ? scaleFactor : item.scale;
                    data.m_PlatePinTotalLen *= finalScale;
                }
			}
            // 其他字段不做操作
            qDebug() << "Scaled OutPlateResParam dimensions.";
            return true;
        }
        else if constexpr (std::is_same_v<T, OutLiftResParam>) {
            // OutLiftResParam 的处理
            data.m_PinWidthLift *= scaleFactor;
            data.m_PinHeightLift *= scaleFactor;
            data.m_disLift *= scaleFactor;
            
            qDebug() << "Scaled OutLiftResParam dimensions.";
            return true;
        }
        else if constexpr (std::is_same_v<T, OutAbutResParam>) {

            //// OutAbutResParam 的处理
            //data.Pin_C *= scaleFactor; // 引脚总长
            //data.shuyao_width *= scaleFactor; // 束腰宽度
            //data.plate_width *= scaleFactor; // 底座宽度
            //data.p_pin_over_pln *= scaleFactor; // 左引脚超板
            //data.n_pin_over_pln *= scaleFactor; // 右引脚超板
            //data.p_pin_H *= scaleFactor; // 左引脚翘脚
            //data.n_pin_H *= scaleFactor; // 右引脚翘脚
            //data.p_n_height_diff *= scaleFactor; // 左右引脚高度差

            //qDebug() << "Scaled OutAbutResParam dimensions.";
            return true;
        }
        else if constexpr (std::is_same_v<T, OutTopParam>) {
            const double epsilon = 1e-6; // 容差值
            for (const auto& item : m_PaintData) {
                if (item.label == "防爆阀半径") {
                    double finalScale = (std::abs(item.scale - 1.00) < epsilon) ? scaleFactor : item.scale;
                    data.m_RadiusTop *= finalScale;
                    return true;
                }
            }
            return false;
        }
        else if constexpr (std::is_same_v<T, OutSideParam>) {
            const double epsilon = 1e-6;
            for (const auto& item : m_PaintData) {
                double finalScale = (std::abs(item.scale - 1.00) < epsilon) ? scaleFactor : item.scale;
                if (item.label == "成型脚距F") {
                    data.m_PinDis *= finalScale;
                }
                else if (item.label == "针脚宽度") {
                    data.m_PinWidth *= finalScale;
                }
                else if (item.label == "套管高度") {
                    data.m_CaseHeigh *= finalScale;
                }
                else if (item.label == "H-H0") {
                    data.m_Case2PinDis *= finalScale;
                }
                else if (item.label == "套管Y差值") {
                    data.m_CaseYSite *= finalScale;
                }
                else if (item.label == "胶带跑上跑下") {
                    data.m_TapeDis *= finalScale;
                }
                else if (item.label == "编带尺寸H") {
                    data.m_Case2TapeDis *= finalScale;
                }
            }
            return true;
        }
        else if constexpr (std::is_same_v<T, OutPinParam>) {
            const double epsilon = 1e-6;
            for (const auto& item : m_PaintData) {
                double finalScale = (std::abs(item.scale - 1.00) < epsilon) ? scaleFactor : item.scale;
                if (item.label == "孔毛刺") {
                    data.m_MaoCiArea *= finalScale * finalScale;
                }
                else if (item.label == "定位圆间距") {
                    data.m_DisCircle *= finalScale;
                }
                else if (item.label == "成型脚距F") {
                    data.m_DisTopPin *= finalScale;
                }
                else if (item.label == "成型不良") {
                    data.m_DisButtomPin *= finalScale;
                }
                else if (item.label == "针最大宽度") {
                    data.m_DisPinWid *= finalScale;
                }
                else if (item.label == "胶带破洞") {
                    data.m_totalArea *= finalScale * finalScale;
                }
                else if (item.label == "粘胶不牢") {
                    data.m_DisTapeWid *= finalScale;
                }
            }
            return true;
        }
        else {
            // 如果传入了未处理的结构体类型
            qWarning() << "RezultInfo::scaleDimensions: Unsupported struct type for scaling.";
            return false;
        }
    }

    const ProcessedParam* getProcessedParam(const QString& mappedKey) const;
    void initStampPaintVector(Parameters* rangepara);
    void initPlatePaintVector(Parameters* rangePara);
    void initLiftPaintVector(Parameters* rangePara);
    void initSidePaintVector(Parameters* rangePara);
    void initPinPaintVector(Parameters* rangePara);
    void initTopPaintVector(Parameters* rangePara);
    QVector<SimpleParam> initScale(QString ScalePath);
    void printPaintDataItems();
    void applyScaleFactors(double scale);
    QVector<PaintDataItem> m_PaintData;
    std::vector<float>score_pos;
    std::vector<float>score_neg;
    int  flag;

signals:
    void UpdateParameters(Parameters m_parameters);
    void BuildUIFromUnifyParameters(const AllUnifyParams& params);
  
public:
    QList<ProcessedParam> m_processedData;
    int flage;
    double fix;
    void processRangeParameters(Parameters *rangePara);
    AllUnifyParams unifyParams;
public:
    float getThresholdValue(const QString& key) const;
    void  appendPaintData(const QString& name, bool check, float value, int result);
};

#endif // REZULTINFO_H


