#ifndef RECONSTRUCTIONINPUTLIST_H
#define RECONSTRUCTIONINPUTLIST_H

#include "reconstructioninputinfo.h"
#include <QList>

class QewExtensibleDialog;
class AmosImage;

class ReconstructionInputList
{
public:
    ReconstructionInputList(QewExtensibleDialog* sender);
    ~ReconstructionInputList();
    QList<ReconstructionInputInfo > getList(QewExtensibleDialog* receiver);
    void updateList(QewExtensibleDialog* source, QString sourceUuid, QList<AmosImage*> images);
    void addInputInfo(AmosImage const *image, QewExtensibleDialog const *receiver,
                      QString receiverUuid);
    void removeInputInfo(ReconstructionInputInfo inputInfo, QewExtensibleDialog* receiver);
    void clear(QewExtensibleDialog* sender);

protected:
    QList<ReconstructionInputInfo* > list;
    QewExtensibleDialog* sender;
    QHash<AmosImage const *, ReconstructionInputInfo*> hashImages;
    QMultiHash<QewExtensibleDialog const *, ReconstructionInputInfo*> hashDialogs;

};

#endif // RECONSTRUCTIONINPUTLIST_H
