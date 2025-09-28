#include "cameral.h"
#include <iostream>
Cameral::Cameral(QObject * parent) :QObject(parent)
{
}

Cameral::~Cameral()
{
    if(camOp) delete camOp;
    if(RI) delete RI;
    // {
    //     std::unique_lock<std::mutex> lock(imageQueue.mutex);
    //     imageQueue.stop_flag=true;
    // }
    // imageQueue.cond.notify_all();

}



