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

#include "QmitkAmosWorkbenchApplication.h"

#include <berryPlatformUI.h>

#include "QmitkAmosWorkbenchAdvisor.h"

#include <QDebug>

QmitkAmosWorkbenchApplication::QmitkAmosWorkbenchApplication()
{
}

QmitkAmosWorkbenchApplication::QmitkAmosWorkbenchApplication(const QmitkAmosWorkbenchApplication& other)
: QObject(other.parent())
{
  Q_UNUSED(other)
  throw std::runtime_error("Copy constructor not implemented");
}

QVariant QmitkAmosWorkbenchApplication::Start(berry::IApplicationContext* /*context*/)
{
  berry::Display* display = berry::PlatformUI::CreateDisplay();

  int code = berry::PlatformUI::CreateAndRunWorkbench(display, new QmitkAmosWorkbenchAdvisor());

  // exit the application with an appropriate return code
  return code == berry::PlatformUI::RETURN_RESTART
              ? EXIT_RESTART : EXIT_OK;
}

void QmitkAmosWorkbenchApplication::Stop()
{

}
