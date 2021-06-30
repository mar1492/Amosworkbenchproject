/*=========================================================================
 * 
 * Program:   Medical Imaging & Interaction Toolkit
 * Module:    $RCSfile$
 * Language:  C++
 * Date:      $Date$
 * Version:   $Revision: 13820 $
 * 
 * Copyright (c) German Cancer Research Center, Division of Medical and
 * Biological Informatics. All rights reserved.
 * See MITKCopyright.txt or http://www.mitk.org/ for details.
 * 
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the above copyright notices for more information.
 * 
 * =========================================================================*/

#include <mitkBaseApplication.h>

#include <QVariant>

int main(int argc, char** argv)
{
    mitk::BaseApplication myApp(argc, argv);
    myApp.setApplicationName("AmosWorkbenchApp");
    myApp.setOrganizationName("AmosWorkbenchOrg");

    // Supress qt.qpa.xcb: QXcbConnection: XCB error: messages
    qputenv("QT_LOGGING_RULES","*.debug=false;qt.qpa.*=false");    
    
    // -------------------------------------------------------------------
    // Here you can switch to your customizable application:
    // -------------------------------------------------------------------
    
    //  myApp.setProperty(mitk::BaseApplication::PROP_APPLICATION, "org.mitk.qt.extapplication");
    myApp.setProperty(mitk::BaseApplication::PROP_APPLICATION, "amosworkbenchproject.app");
    
    if(argc > 1) {
        QString appargs(argv[1]);
        QString xargs = "--xargs=";
        if(appargs.contains(xargs)) {
            appargs.remove(0, xargs.size());
            QStringList arguments = appargs.split(" ", Qt::SkipEmptyParts);
            int index = arguments.indexOf("advanced");
            if(index >= 0)
                myApp.setProperty("AdvancedMode", true);
            else
                myApp.setProperty("AdvancedMode", false);
        }
    }
    
    return myApp.run();
}
