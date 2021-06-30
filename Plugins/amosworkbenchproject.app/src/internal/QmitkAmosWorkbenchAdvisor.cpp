/*=========================================================================
 * 
 * Program:   Medical Imaging & Interaction Toolkit
 * Language:  C++
 * Date:      $Date$
 * Version:   $Revision$
 * 
 * Copyright (c) German Cancer Research Center, Division of Medical and
 * Biological Informatics. All rights reserved.
 * See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.
 * 
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the above copyright notices for more information.
 * 
 * =========================================================================*/

#include "QmitkAmosWorkbenchAdvisor.h"
#include "QmitkAmosWorkbenchWindowAdvisor.h"
#include "internal/mitkAppPluginActivator.h"

// #include <QmitkExtWorkbenchWindowAdvisor.h>

#include <mitkWorkbenchUtil.h>

const QString QmitkAmosWorkbenchAdvisor::DEFAULT_PERSPECTIVE_ID =
"amosworkbenchproject.AmosWorkbenchPerspective";

void
QmitkAmosWorkbenchAdvisor::Initialize(berry::IWorkbenchConfigurer::Pointer configurer)
{
    berry::QtWorkbenchAdvisor::Initialize(configurer);
    
    configurer->SetSaveAndRestore(true);
    
    ctkPluginContext* context = mitk::AppPluginActivator::GetDefault()->GetPluginContext();
    mitk::WorkbenchUtil::SetDepartmentLogoPreference(":/App/AmosWorkbenchLogo.png", context);
}

berry::WorkbenchWindowAdvisor*
QmitkAmosWorkbenchAdvisor::CreateWorkbenchWindowAdvisor(
    berry::IWorkbenchWindowConfigurer::Pointer configurer)
{
    // -------------------------------------------------------------------
    // Here you could pass your custom Workbench window advisor
    // -------------------------------------------------------------------
//     QmitkExtWorkbenchWindowAdvisor* advisor = new
//     QmitkExtWorkbenchWindowAdvisor(this, configurer);
    QmitkAmosWorkbenchWindowAdvisor* advisor = new
    QmitkAmosWorkbenchWindowAdvisor(this, configurer);    
    
    advisor->SetWindowIcon(":/App/icon_research.xpm");
    
    QStringList viewExcludeList = advisor->GetViewExcludeList();
    viewExcludeList.push_back("org.mitk.views.modules");
    viewExcludeList.push_back("org.mitk.views.deformableclippingplane");
    advisor->SetViewExcludeList(viewExcludeList);  
    return advisor;
}

QString QmitkAmosWorkbenchAdvisor::GetInitialWindowPerspectiveId()
{
    return DEFAULT_PERSPECTIVE_ID;
}

// bool QmitkAmosWorkbenchAdvisor::PreShutdown()
// {
//     return true;
// }
