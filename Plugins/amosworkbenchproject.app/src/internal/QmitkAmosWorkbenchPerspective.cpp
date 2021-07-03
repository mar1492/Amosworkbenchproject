/*=========================================================================

 Program:   Medical Imaging & Interaction Toolkit
 Language:  C++
 Date:      $Date$
 Version:   $Revision$

 Copyright (c) German Cancer Research Center, Division of Medical and
 Biological Informatics. All rights reserved.
 See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.

 =========================================================================*/

#include "QmitkAmosWorkbenchPerspective.h"
#include "berryIViewLayout.h"

QmitkAmosWorkbenchPerspective::QmitkAmosWorkbenchPerspective()
{
    
}
 
QmitkAmosWorkbenchPerspective::QmitkAmosWorkbenchPerspective(const QmitkAmosWorkbenchPerspective& other)
: QObject()
{
  Q_UNUSED(other)
  throw std::runtime_error("Copy constructor not implemented");
}

void QmitkAmosWorkbenchPerspective::CreateInitialLayout(berry::IPageLayout::Pointer layout)
{
  QString editorArea = layout->GetEditorArea();

  layout->AddView("org.mitk.views.datamanager", 
    berry::IPageLayout::LEFT, 0.3f, editorArea);

  berry::IViewLayout::Pointer lo = layout->GetViewLayout("org.mitk.views.datamanager");
  lo->SetCloseable(false);

  layout->AddView("org.mitk.views.imagenavigator", 
    berry::IPageLayout::BOTTOM, 0.5f, "org.mitk.views.datamanager");

  berry::IFolderLayout::Pointer bottomFolder = layout->CreateFolder("bottom", berry::IPageLayout::BOTTOM, 0.7f, editorArea);
  bottomFolder->AddView("org.mitk.views.properties");
  bottomFolder->AddView("org.blueberry.views.logview");
}
