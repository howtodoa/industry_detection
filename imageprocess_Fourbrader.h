#include <QObject>
#include "Cameral.h"
#include <QThread>
#include "typdef.h"
#include "imageprocess.h"


class Imageprocess_FourBrader : public ImageProcess
{
public:
    explicit Imageprocess_FourBrader(Cameral* cam, SaveQueue* m_saveQueue, QObject* parent = nullptr);
    ~Imageprocess_FourBrader() override;

protected:
    void run() override;
};