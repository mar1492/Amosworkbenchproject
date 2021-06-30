#ifndef MYQMITKRENDERWINDOW_H
#define MYQMITKRENDERWINDOW_H

#include <QmitkRenderWindow.h>
#include "MyQmitkRenderWindowMenu.h"
#include "amosworkbenchproject_renderwindoweditor_Export.h"

class AMOSWORKBENCHPROJECT_RENDERWINDOWEDITOR MyQmitkRenderWindow : public QmitkRenderWindow
{
    Q_OBJECT

public:
    using LayoutDesign = QmitkMultiWidgetLayoutManager::LayoutDesign;
//     MyQmitkRenderWindow(QWidget *parent = nullptr, QString name = "unnamed renderwindow", mitk::VtkPropRenderer* renderer = nullptr/*,
//                     mitk::RenderingManager* renderingManager = NULL,mitk::BaseRenderer::RenderingMode::Type renderingMode = mitk::BaseRenderer::RenderingMode::Standard*/); 
    MyQmitkRenderWindow(QWidget *parent = nullptr, const QString &name = "unnamed renderwindow", mitk::VtkPropRenderer* renderer = nullptr); 
    ~MyQmitkRenderWindow() override;
    
    // Set Layout Index to define the Layout Type
//     void SetLayoutIndex(unsigned int layoutIndex);
    void SetLayoutIndex(QmitkRenderWindowMenu::LayoutIndex layoutIndex); 
    // Get Layout Index to define the Layout Type
//     unsigned int GetLayoutIndex();
    MyQmitkRenderWindowMenu::LayoutIndex GetLayoutIndex();
    //MenuWidget need to update the Layout Design List when Layout had changed
    /*void LayoutDesignListChanged(int layoutDesignIndex);*/ 
    // MITK 2021
    //void LayoutDesignListChanged(LayoutDesign layoutDesignIndex) override;
    void UpdateLayoutDesignList(LayoutDesign layoutDesignIndex) override;
    
    // Activate or Deactivate MenuWidget.
    void ActivateMenuWidget(bool state) override;
    bool GetActivateMenuWidgetFlag()
    {
        return m_MenuWidgetActivated;
    }
    void OnOnlyInACTool2DSelected(std::string name);
    void setACandBCTools(QStringList toolsList);
    
protected:
    void mouseDoubleClickEvent( QMouseEvent *event ) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void AdjustRenderWindowMenuVisibility(const QPoint& pos);
//     mitk::Point2D GetMousePosition(QMouseEvent* me) const;
    mitk::InteractionEvent::MouseButtons GetButtonState(QMouseEvent* me) const;
    mitk::InteractionEvent::ModifierKeys GetModifiers(QInputEvent* me) const;
    mitk::InteractionEvent::MouseButtons GetEventButton(QMouseEvent* me) const;
    void leaveEvent(QEvent*) override;
    
protected Q_SLOTS:
    void DeferredHideMenu();
//     void OnChangeLayoutDesign(QmitkRenderWindowMenu::LayoutDesign layoutDesignIndex);
    
 Q_SIGNALS:
//    void MyLayoutDesignChanged(QmitkRenderWindowMenu::LayoutDesign);    

// public:
//     enum { LAYOUT_DEFAULT = 0, LAYOUT_2D_IMAGES_UP, LAYOUT_2D_IMAGES_LEFT,
// 	    LAYOUT_BIG_3D, LAYOUT_WIDGET1, LAYOUT_WIDGET2, LAYOUT_WIDGET3,
// 	    LAYOUT_2X_2D_AND_3D_WIDGET, LAYOUT_ROW_WIDGET_3_AND_4,
// 	    LAYOUT_COLUMN_WIDGET_3_AND_4, LAYOUT_ROW_WIDGET_SMALL3_AND_BIG4 ,
// 	    LAYOUT_SMALL_UPPER_WIDGET2_BIG3_AND4,LAYOUT_2D_AND_3D_LEFT_2D_RIGHT_WIDGET,
// 	    LAYOUT_2D_UP_AND_3D_DOWN, LAYOUT_2D_AC_RIGHT_AND_BC_LEFT,
// 	    LAYOUT_AC_UP_AND_BC_DOWN };  
// 	    
//     enum {AXIAL, SAGITTAL, CORONAL,THREE_D};
    
protected:
    bool m_FullScreenMode;
//     unsigned int m_LayoutInd;
    MyQmitkRenderWindowMenu::LayoutIndex m_LayoutInd;
    bool m_MenuWidgetActivated;
    MyQmitkRenderWindowMenu* m_MyMenuWidget;
    bool onlyInAcTool;
    QStringList ACandBCToolsList;

};

#endif // MYQMITKRENDERWINDOW_H
