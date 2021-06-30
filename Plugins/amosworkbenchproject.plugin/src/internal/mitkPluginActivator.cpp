#include "mitkPluginActivator.h"
#include "QmitkAmosWorkbenchView.h"
#include "QmitkSegmentationView.h"
#include "MyQmitkToolSelectionBox.h"
#include "QmitkCreatePolygonModelAction.h"
#include "../QmitkSegmentationPreferencePage.h"

ctkPluginContext* mitk::PluginActivator::m_context = nullptr;

namespace mitk {
    
    void PluginActivator::start(ctkPluginContext* context)
    {
        BERRY_REGISTER_EXTENSION_CLASS(QmitkAmosWorkbenchView, context)
        BERRY_REGISTER_EXTENSION_CLASS(QmitkSegmentationPreferencePage, context)
        BERRY_REGISTER_EXTENSION_CLASS(QmitkSegmentationView, context)
        BERRY_REGISTER_EXTENSION_CLASS(MyQmitkToolSelectionBox, context)
        BERRY_REGISTER_EXTENSION_CLASS(QmitkCreatePolygonModelAction, context)
        m_context = context;
    }
    
    void PluginActivator::stop(ctkPluginContext* context)
    {
        Q_UNUSED(context)
        m_context = NULL;
    }
    
    ctkPluginContext* PluginActivator::getContext()
    {
        return m_context;
    }
    
}


#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QtPlugin>
Q_EXPORT_PLUGIN2(amosworkbenchproject_plugin, mitk::PluginActivator)
#endif
