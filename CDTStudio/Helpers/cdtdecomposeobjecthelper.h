#ifndef CDTDECOMPOSEOBJECTHELPER_H
#define CDTDECOMPOSEOBJECTHELPER_H

#include "cdtbasethread.h"

class CDTDecomposeObjectHelperPrivate;
class CDTDecomposeObjectHelper : public CDTBaseThread
{
    Q_OBJECT
public:
    explicit CDTDecomposeObjectHelper(
            QString imagePath,
            QString markfilePath,
            QString dir,
            bool withMask,
            QObject *parent = 0);
    ~CDTDecomposeObjectHelper();

    bool isValid();
    void run();
private:
    CDTDecomposeObjectHelperPrivate *p;
    bool withMask;
};

#endif // CDTDECOMPOSEOBJECTHELPER_H
