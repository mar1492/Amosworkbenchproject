/*============================================================================
 * 
 * The Medical Imaging Interaction Toolkit (MITK)
 * 
 * Copyright (c) German Cancer Research Center (DKFZ)
 * All rights reserved.
 * 
 * Use of this source code is governed by a 3-clause BSD license that can be
 * found in the LICENSE file.
 * 
 * ============================================================================*/

#ifndef MYQMITKRENDERWINDOWWIDGET_H
#define MYQMITKRENDERWINDOWWIDGET_H

// qt widgets module
// #include "MitkQtWidgetsExports.h"
#include "myqmitkrenderwindow.h"

// mitk core
#include <mitkDataStorage.h>
#include <mitkPointSet.h>
#include <mitkRenderWindow.h>

// qt
#include <QmitkRenderWindowWidget.h>
#include <QHBoxLayout>
#include <QMouseEvent>

class vtkCornerAnnotation;

/**
 * @brief The 'QmitkRenderWindowWidget' is a QFrame that holds a render window
 *        and some associates properties, like a crosshair (pointset) and decorations.
 *        Decorations are corner annotation (text and color), frame color or background color
 *        and can be set using this class.
 *        The 'QmitkRenderWindowWidget' is used inside a 'QmitkAbstractMultiWidget', where a map contains
 *        several render window widgets to create the multi widget display.
 */
class AMOSWORKBENCHPROJECT_RENDERWINDOWEDITOR MyQmitkRenderWindowWidget : public QmitkRenderWindowWidget
{
    Q_OBJECT
    
public:
    
    MyQmitkRenderWindowWidget(
        QWidget* parent = nullptr,
        const QString& widgetName = "",
        mitk::DataStorage* dataStorage = nullptr, bool initializeParent = false);
    
    ~MyQmitkRenderWindowWidget() /*override*/;
    
    void SetDataStorage(mitk::DataStorage* dataStorage);
    
    const QString& GetWidgetName() const { return m_WidgetName; };
    MyQmitkRenderWindow* GetRenderWindow() const { return dynamic_cast<MyQmitkRenderWindow*>(m_RenderWindow); };
    
    mitk::SliceNavigationController* GetSliceNavigationController() const;
    
    void RequestUpdate();
    void ForceImmediateUpdate();
    
    void SetGradientBackgroundColors(const mitk::Color& upper, const mitk::Color& lower);
    void ShowGradientBackground(bool enable);
    std::pair<mitk::Color, mitk::Color> GetGradientBackgroundColors() const { return m_GradientBackgroundColors; };
    bool IsGradientBackgroundOn() const;
    
    void SetDecorationColor(const mitk::Color& color);
    mitk::Color GetDecorationColor() const { return m_DecorationColor; };
    
    void ShowColoredRectangle(bool show);
    bool IsColoredRectangleVisible() const;
    
    void ShowCornerAnnotation(bool show);
    bool IsCornerAnnotationVisible() const;
    void SetCornerAnnotationText(const std::string& cornerAnnotation);
    std::string GetCornerAnnotationText() const;
    
    bool IsRenderWindowMenuActivated() const;
    
    void ActivateCrosshair(bool activate);
    
Q_SIGNALS:
    
    void MouseEvent(QMouseEvent* e);
    
protected:
    
    void InitializeGUI();
//     void InitializeDecorations();
//     
//     void SetCrosshair(mitk::Point3D selectedPoint);
//     
//     QString m_WidgetName;
//     QHBoxLayout* m_Layout;
//     
//     mitk::DataStorage* m_DataStorage;
    
//     MyQmitkRenderWindow* m_RenderWindow;
    
//     mitk::DataNode::Pointer m_PointSetNode;
//     mitk::PointSet::Pointer m_PointSet;
//     
//     std::pair<mitk::Color, mitk::Color> m_GradientBackgroundColors;
//     mitk::Color m_DecorationColor;
//     vtkSmartPointer<vtkCornerAnnotation> m_CornerAnnotation;
    
};

#endif // MYQMITKRENDERWINDOWWIDGET_H
