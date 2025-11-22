#include <QObject>
#include "Cameral.h"
#include <QThread>
#include "typdef.h"
#include "imageprocess.h"


class Imageprocess_Image : public ImageProcess
{
public:
    explicit Imageprocess_Image(Cameral* cam, SaveQueue* m_saveQueue, QObject* parent = nullptr);
    ~Imageprocess_Image() override;

protected:
    void run() override;
};