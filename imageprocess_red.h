#include <QObject>
#include "Cameral.h"
#include <QThread>
#include "typdef.h"
#include "imageprocess.h"


class Imageprocess_Red : public ImageProcess
{
public:
    explicit Imageprocess_Red(Cameral* cam, SaveQueue* m_saveQueue, QObject* parent = nullptr);
    ~Imageprocess_Red() override;

protected:
    void run() override;
};