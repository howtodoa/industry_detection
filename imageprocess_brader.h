#include <QObject>
#include "Cameral.h"
#include <QThread>
#include "typdef.h"
#include "imageprocess.h"


class Imageprocess_Brader : public ImageProcess
{
public:
    explicit Imageprocess_Brader(Cameral* cam, SaveQueue* m_saveQueue, QObject* parent = nullptr);
    ~Imageprocess_Brader() override;

protected:
    void run() override;
};