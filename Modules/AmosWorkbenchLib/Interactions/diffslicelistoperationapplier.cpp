#include "diffslicelistoperationapplier.h"
#include "diffslicelistoperation.h"
#include <mitkDiffSliceOperationApplier.h>

DiffSliceListOperationApplier::DiffSliceListOperationApplier()
{
}

DiffSliceListOperationApplier::~DiffSliceListOperationApplier()
{
}

DiffSliceListOperationApplier* DiffSliceListOperationApplier::GetInstance()
{
  static DiffSliceListOperationApplier*  s_Instance = new DiffSliceListOperationApplier();
  return s_Instance;
}

void DiffSliceListOperationApplier::ExecuteOperation(mitk::Operation* operation)
{
  DiffSliceListOperation* imageOperation = dynamic_cast<DiffSliceListOperation*>( operation );

  //as we only support DiffSliceOperation return if operation is not type of DiffSliceOperation
  if(!imageOperation)
    return;


  //chak if the operation is valid
  if(!imageOperation->IsValid())
      return;
  
  QList<mitk::Operation*> operationList = imageOperation->getOperations();
  
  for(int i = 0; i < operationList.size(); i++) {
      mitk::DiffSliceOperationApplier::GetInstance()->ExecuteOperation(operationList[i]);
      operationList[i] = NULL;
  }
}
