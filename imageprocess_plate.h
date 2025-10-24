#include <QObject>
#include "Cameral.h"
#include <QThread>
#include "typdef.h"
#include "imageprocess.h"


class Imageprocess_Plate : public ImageProcess
{
public:
    explicit Imageprocess_Plate(Cameral* cam, SaveQueue* m_saveQueue, QObject* parent = nullptr);
    ~Imageprocess_Plate() override;

protected:
    void run() override;
};