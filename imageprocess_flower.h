#include <QObject>
#include "Cameral.h"
#include <QThread>
#include "typdef.h"
#include "imageprocess.h"


class Imageprocess_Flower : public ImageProcess
{
public:
    explicit Imageprocess_Flower(Cameral* cam, SaveQueue* m_saveQueue, QObject* parent = nullptr);
    ~Imageprocess_Flower() override;

protected:
    void run() override;
};