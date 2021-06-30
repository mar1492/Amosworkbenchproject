/*=========================================================================
 * 
 * Program:   BlueBerry Platform
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

#include "mitkAppPluginActivator.h"
#include "QmitkAmosWorkbenchApplication.h"
#include "QmitkAmosWorkbenchPerspective.h"

#include <berryLog.h>

#include <mitkVersion.h>

#include <QFileInfo>
#include <QDateTime>

namespace mitk {
    
    AppPluginActivator* AppPluginActivator::inst = 0;
    
    AppPluginActivator::AppPluginActivator()
    {
        inst = this;
    }
    
    AppPluginActivator::~AppPluginActivator()
    {
    }
    
    AppPluginActivator* AppPluginActivator::GetDefault()
    {
        return inst;
    }
    
    void AppPluginActivator::start(ctkPluginContext* context)
    {
        berry::AbstractUICTKPlugin::start(context);
        
        this->context = context;
        
        BERRY_REGISTER_EXTENSION_CLASS(QmitkAmosWorkbenchApplication, context);
        BERRY_REGISTER_EXTENSION_CLASS(QmitkAmosWorkbenchPerspective, context);
        
        // QString collectionFile = GetQtHelpCollectionFile();
        
        // berry::QtAssistantUtil::SetHelpCollectionFile(collectionFile);
        // berry::QtAssistantUtil::SetDefaultHelpUrl("qthelp://amosworkbenchproject.app/bundle/index.html");
    }
    
    ctkPluginContext* AppPluginActivator::GetPluginContext() const
    {
        return context;
    }
    
    QString AppPluginActivator::GetQtHelpCollectionFile() const
    {
        if (!helpCollectionFile.isEmpty())
        {
            return helpCollectionFile;
        }
        
//         QString collectionFilename = "AppQtHelpCollection.qch";
        QString collectionFilename = "AppQtHelpCollection.qhc";
        
        QFileInfo collectionFileInfo = context->getDataFile(collectionFilename);
        QString path = collectionFileInfo.absoluteFilePath();
        QFileInfo pluginFileInfo = QFileInfo(QUrl(context->getPlugin()->getLocation()).toLocalFile());
        QString path2 = pluginFileInfo.absoluteFilePath();
        if (!collectionFileInfo.exists() ||
            pluginFileInfo.lastModified() > collectionFileInfo.lastModified())
        {
            // extract the qch file from the plug-in
            QByteArray content = context->getPlugin()->getResource(collectionFilename);
            if (content.isEmpty())
            {
                BERRY_WARN << "Could not get plug-in resource: " << collectionFilename.toStdString();
            }
            else
            {
                QFile file(collectionFileInfo.absoluteFilePath());
                file.open(QIODevice::WriteOnly);
                file.write(content);
                file.close();
            }
        }
        
        if (QFile::exists(collectionFileInfo.absoluteFilePath()))
        {
            helpCollectionFile = collectionFileInfo.absoluteFilePath();
        }
        
        return helpCollectionFile;
    }
    
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QtPlugin>
Q_EXPORT_PLUGIN2(amosworkbenchproject_app, mitk::AppPluginActivator)
#endif
