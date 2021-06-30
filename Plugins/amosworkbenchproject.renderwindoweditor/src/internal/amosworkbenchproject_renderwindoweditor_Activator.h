/*=========================================================================

 Program:   MITK Platform
 Language:  C++
 Date:      $Date$
 Version:   $Revision: 17020 $

 Copyright (c) German Cancer Research Center, Division of Medical and
 Biological Informatics. All rights reserved.
 See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.

 =========================================================================*/


#ifndef amosworkbenchproject_renderwindoweditor_Activator_H_
#define amosworkbenchproject_renderwindoweditor_Activator_H_

#include <ctkPluginActivator.h>

/**
 * \ingroup amosworkbenchproject_renderwindoweditor
 */
class amosworkbenchproject_renderwindoweditor_Activator : public QObject, public ctkPluginActivator
{
  Q_OBJECT
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  Q_PLUGIN_METADATA(IID "amosworkbenchproject_renderwindoweditor")
#endif
  Q_INTERFACES(ctkPluginActivator)

public:

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

};

#endif /* amosworkbenchproject_renderwindoweditor_Activator_H_ */

