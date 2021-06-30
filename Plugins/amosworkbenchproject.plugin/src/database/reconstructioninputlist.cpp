#include "reconstructioninputlist.h"
#include <qewextensibledialog.h>
#include "amosimage.h"

ReconstructionInputList::ReconstructionInputList(QewExtensibleDialog* sender) : sender(sender)
{

}

ReconstructionInputList::~ReconstructionInputList()
{

}

QList<ReconstructionInputInfo> ReconstructionInputList::getList(QewExtensibleDialog* receiver)
{
    QList<ReconstructionInputInfo> listInfo;
    for(int i = 0; i < list.size(); i++) {
        if(list.at(i)->getSource() == receiver->windowTitle())
            continue;
        ReconstructionInputInfo info(*(list.at(i)));
        listInfo.append(info);
    }
    return listInfo;
}

void ReconstructionInputList::updateList(QewExtensibleDialog *source, QString sourceUuid,
                     QList<AmosImage *> images)
{
    QList<ReconstructionInputInfo*> listInfoPtr = hashDialogs.values(source);
    for(int i = 0; i < listInfoPtr.size(); i++) {
        list.removeOne(listInfoPtr.at(i));
        delete listInfoPtr.at(i);
    }
    hashDialogs.remove(source);
    for(int i = 0; i < images.size(); i++) {
        hashImages.remove(images.at(i));
        addInputInfo(images.at(i), source, sourceUuid);
    }
}

void ReconstructionInputList::addInputInfo(AmosImage const *image, QewExtensibleDialog const *receiver,
                                           QString receiverUuid)
{
    QString source = receiver->windowTitle();
    QString type = image->getTypeName();
    QString name = image->getFileName();
    QString imageUuid = image->getImageUuid().toString();
    ReconstructionInputInfo* inputInfo = new ReconstructionInputInfo(source, type, name,
                                              imageUuid, receiverUuid);
    list.append(inputInfo);
    hashDialogs.insert(receiver, inputInfo);
    hashImages.insert(image, inputInfo);

}

void ReconstructionInputList::removeInputInfo(ReconstructionInputInfo inputInfo, QewExtensibleDialog *receiver)
{
    if(inputInfo.getSource() != receiver->windowTitle())
        return;

    for(int i = 0; i < list.size(); i++) {
        if(inputInfo == *(list.at(i))) {
            list.removeAt(i);
            break;
        }
    }
}

void ReconstructionInputList::clear(QewExtensibleDialog *sender)
{
    if(sender == this->sender) {
        list.clear();
        hashImages.clear();
        hashDialogs.clear();
    }
}
