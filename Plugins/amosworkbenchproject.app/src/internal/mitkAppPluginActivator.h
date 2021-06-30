/*=========================================================================
 
 Program:   BlueBerry Platform
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


#ifndef MITK_APP_PLUGIN_ACTIVATOR_H_
#define MITK_APP_PLUGIN_ACTIVATOR_H_

#include <berryAbstractUICTKPlugin.h>

#include <QString>

namespace mitk {

class AppPluginActivator : public berry::AbstractUICTKPlugin
{
  Q_OBJECT
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  Q_PLUGIN_METADATA(IID "amosworkbenchproject_app")
#endif
  Q_INTERFACES(ctkPluginActivator)
  
public:

  AppPluginActivator();
  ~AppPluginActivator();

  static AppPluginActivator* GetDefault();

  ctkPluginContext* GetPluginContext() const;

  void start(ctkPluginContext*);

  QString GetQtHelpCollectionFile() const;

private:

  static AppPluginActivator* inst;

  ctkPluginContext* context;

  mutable QString helpCollectionFile;
};

}

#endif /* MITK_APP_PLUGIN_ACTIVATOR_H_ */
