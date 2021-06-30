#ifndef DIFFSLICELISTOPERATION_H
#define DIFFSLICELISTOPERATION_H

#include <mitkOperation.h>
#include <mitkOperationActor.h>

#include <QList>

//class DiffSliceOperation;

class DiffSliceListOperation : public mitk::Operation
{
public:

    mitkClassMacro(DiffSliceListOperation, mitk::OperationActor)
    /** \brief Creates an empty instance.
      Note that it is not valid yet. The properties of the object have to be set.
    */
    DiffSliceListOperation();

    /** \brief */
    DiffSliceListOperation(QList<Operation* > operations);
    
    void appendOperation(Operation* operation);
    /** \brief Check if it is a valid operation.*/
    bool IsValid();

    QList<Operation* > getOperations();

protected:

    virtual ~DiffSliceListOperation(); 
    /** \brief Callback for image observer.*/
    void OnImageDeleted();

protected:
    QList<Operation*> operationList;
    
};

#endif // DIFFSLICELISTOPERATION_H
