#include "diffslicelistoperation.h"
//#include <mitkDiffSliceOperation.h>

DiffSliceListOperation::DiffSliceListOperation() : Operation(1)
{
}

DiffSliceListOperation::DiffSliceListOperation(QList<Operation*> operations) : Operation(1)
{
    operationList = operations;
}

void DiffSliceListOperation::appendOperation(Operation* operation)
{
    operationList.append(operation);
}

DiffSliceListOperation::~DiffSliceListOperation()
{
/*    for(int i = 0; i < undoOperationList.size(); i++) {
        DiffSliceOperation* undoOperation = undoOperationList[i];
        delete undoOperation;
    }
    for(int i = 0; i < doOperationList.size(); i++)
        delete doOperationList.at(i); */   
}

QList<mitk::Operation* > DiffSliceListOperation::getOperations()
{
    return operationList;
}

bool DiffSliceListOperation::IsValid()
{
    return true; //TODO improve
}

// std::vector< std::std::__cxx11::basic_string< char, std::char_traits< char >, std::std::allocator< char > >, std::allocator< std::std::__cxx11::basic_string< char, std::char_traits< char >, std::std::allocator< char > > > > DiffSliceListOperation::GetClassHierarchy() const
// {
// 
// }
// 
// const char* DiffSliceListOperation::GetNameOfClass() const
// {
// 
// }
// 
// DiffSliceListOperation::DiffSliceListOperation(OperationType operationType)
// {
// 
// }
