#include "QmitkAmosWorkbenchWindowAdvisor.h"

QmitkAmosWorkbenchWindowAdvisor::QmitkAmosWorkbenchWindowAdvisor(berry::WorkbenchAdvisor *wbAdvisor,
                                berry::IWorkbenchWindowConfigurer::Pointer configurer) :
                       QmitkExtWorkbenchWindowAdvisor(wbAdvisor, configurer)
{
}

bool QmitkAmosWorkbenchWindowAdvisor::PreWindowShellClose()
{
    return true;
}
