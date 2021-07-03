#ifndef MITKPLUGINACTIVATOR_H
#define MITKPLUGINACTIVATOR_H

#include <ctkPluginActivator.h>

namespace mitk {

class PluginActivator :
  public QObject, public ctkPluginActivator
{
  Q_OBJECT
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  Q_PLUGIN_METADATA(IID "amosworkbenchproject_plugin")
#endif
  Q_INTERFACES(ctkPluginActivator)

public:

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);
  
  static ctkPluginContext* getContext();
  
  private:
    static ctkPluginContext* m_context;  

}; // PluginActivator

}

#endif // MITKPLUGINACTIVATOR_H
