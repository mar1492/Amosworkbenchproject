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


#ifndef QMITKAMOSWORKBENCHPERSPECTIVE_H_
#define QMITKAMOSWORKBENCHPERSPECTIVE_H_

#include <berryIPerspectiveFactory.h>

class QmitkAmosWorkbenchPerspective : public QObject, public berry::IPerspectiveFactory
{
  Q_OBJECT
  Q_INTERFACES(berry::IPerspectiveFactory)
  
public:

  QmitkAmosWorkbenchPerspective();
  QmitkAmosWorkbenchPerspective(const QmitkAmosWorkbenchPerspective& other);
  
  void CreateInitialLayout(berry::IPageLayout::Pointer layout);

};

#endif /* QMITKAMOSWORKBENCHPERSPECTIVE_H_ */
