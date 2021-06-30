#ifndef DIFFSLICELISTOPERATIONAPPLIER_H
#define DIFFSLICELISTOPERATIONAPPLIER_H

#include <mitkOperationActor.h>

class DiffSliceListOperationApplier : public mitk::OperationActor
{
  public:

    mitkClassMacroNoParent(DiffSliceListOperationApplier)

    /** \brief Returns an instance of the class */
    static DiffSliceListOperationApplier* GetInstance();

    /** \brief Executes a DiffSliceOperation.
      \sa DiffSliceOperation
      Note:
        Only DiffSliceListOperation is supported.
    */    
    void ExecuteOperation(mitk::Operation* operation);
    
protected:

    DiffSliceListOperationApplier();

    virtual ~DiffSliceListOperationApplier();    
};

#endif // DIFFSLICELISTOPERATIONAPPLIER_H
