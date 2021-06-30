#ifndef QMITKAMOSWORKBENCHWINDOWADVISOR_H
#define QMITKAMOSWORKBENCHWINDOWADVISOR_H

#include <QmitkExtWorkbenchWindowAdvisor.h>

class QmitkAmosWorkbenchWindowAdvisor : public QmitkExtWorkbenchWindowAdvisor
{
public:
    QmitkAmosWorkbenchWindowAdvisor(berry::WorkbenchAdvisor *wbAdvisor,
                                    berry::IWorkbenchWindowConfigurer::Pointer configurer);
    bool PreWindowShellClose() override;
};

#endif // QMITKAMOSWORKBENCHWINDOWADVISOR_H
