/*=========================================================================

Program:   Medical Imaging & Interaction Toolkit
Language:  C++
Date:      $Date$
Version:   $Revision: 18127 $

Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "amosworkbenchproject_renderwindoweditor_Activator.h"

#include "internal/QmitkAmosWorkbenchRenderWindowEditor.h"
#include "internal/QmitkAmosWidgetEditorPreferencePage.h"

void
amosworkbenchproject_renderwindoweditor_Activator::start(ctkPluginContext* context)
{
  BERRY_REGISTER_EXTENSION_CLASS(QmitkAmosWorkbenchRenderWindowEditor, context)
  BERRY_REGISTER_EXTENSION_CLASS(QmitkAmosWidgetEditorPreferencePage, context)
}

void
amosworkbenchproject_renderwindoweditor_Activator::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  Q_EXPORT_PLUGIN2(amosworkbenchproject_renderwindoweditor, amosworkbenchproject_renderwindoweditor_Activator)
#endif
