#include "myqmitkrenderwindow.h"
#include <QMouseEvent>

// #include <mitkPositionTracker.h>
// #include <mitkGlobalInteraction.h>

// #include <QmitkEventAdapter.h>
#include <mitkRotationOperation.h>
#include <mitkInteractionConst.h>
#include <vtkCamera.h>
#include <mitkMousePressEvent.h>
// #include <mitkAffineDataInteractor3D.h>
// #include <mitkAffineInteractor.h>
#include <mitkMouseMoveEvent.h>
#include <mitkInternalEvent.h>

#include <QCoreApplication>

// MyQmitkRenderWindow::MyQmitkRenderWindow(QWidget *parent, QString name, mitk::VtkPropRenderer* /*renderer*//*, mitk::RenderingManager* renderingManager,mitk::BaseRenderer::RenderingMode::Type renderingMode*/) : 
//     QmitkRenderWindow(parent, name, nullptr/*, renderingManager, renderingMode*/),
//     m_FullScreenMode(false), m_MenuWidget(NULL), onlyInAcTool(false)
MyQmitkRenderWindow::MyQmitkRenderWindow(QWidget *parent, const QString &name, mitk::VtkPropRenderer*) : 
    QmitkRenderWindow(parent, name, nullptr),
    m_FullScreenMode(false), m_MyMenuWidget(NULL), onlyInAcTool(false)
{
    
}

MyQmitkRenderWindow::~MyQmitkRenderWindow()
{
}

// void MyQmitkRenderWindow::SetLayoutIndex(unsigned int layoutIndex)
void MyQmitkRenderWindow::SetLayoutIndex(MyQmitkRenderWindowMenu::LayoutIndex layoutIndex)
{
    m_LayoutInd = layoutIndex;
    if (m_MyMenuWidget)
        m_MyMenuWidget->SetLayoutIndex(layoutIndex);    
}

// unsigned int MyQmitkRenderWindow::GetLayoutIndex()
MyQmitkRenderWindowMenu::LayoutIndex MyQmitkRenderWindow::GetLayoutIndex()
{
    if (m_MyMenuWidget != nullptr)
        return m_MyMenuWidget->GetLayoutIndex();
    else
        return MyQmitkRenderWindowMenu::LayoutIndex::AXIAL;
}

// void MyQmitkRenderWindow::LayoutDesignListChanged(int layoutDesignIndex)
// MITK 2021
//void LayoutDesignListChanged(LayoutDesign layoutDesignIndex) override;
void MyQmitkRenderWindow::UpdateLayoutDesignList(QmitkRenderWindowMenu::LayoutDesign layoutDesignIndex)
{
    if (m_MyMenuWidget)
        m_MyMenuWidget->UpdateLayoutDesignList(layoutDesignIndex);
}


void MyQmitkRenderWindow::ActivateMenuWidget(bool state)
{
    if (nullptr == m_MyMenuWidget)
    {
        m_MyMenuWidget = new MyQmitkRenderWindowMenu(this, Qt::WindowFlags(), m_Renderer);
        m_MyMenuWidget->SetLayoutIndex(m_LayoutInd);
    }
    
    m_MenuWidgetActivated = state;
    
    if (m_MenuWidgetActivated)
    {
//         connect(m_MyMenuWidget, &MyQmitkRenderWindowMenu::LayoutDesignChanged, this, &MyQmitkRenderWindow::MyLayoutDesignChanged);
        connect(m_MyMenuWidget, &MyQmitkRenderWindowMenu::LayoutDesignChanged, this, &QmitkRenderWindow::LayoutDesignChanged);        
        connect(m_MyMenuWidget, &MyQmitkRenderWindowMenu::ResetView, this, &MyQmitkRenderWindow::ResetView);
//         connect(m_MyMenuWidget, &QmitkRenderWindowMenu::CrosshairVisibilityChanged, this, &QmitkRenderWindow::CrosshairVisibilityChanged);
//         connect(m_MyMenuWidget, &QmitkRenderWindowMenu::CrosshairRotationModeChanged, this, &QmitkRenderWindow::CrosshairRotationModeChanged);
    }
    else
    {
//         disconnect(m_MyMenuWidget, &MyQmitkRenderWindowMenu::LayoutDesignChanged, this, &MyQmitkRenderWindow::MyLayoutDesignChanged);
        disconnect(m_MyMenuWidget, &MyQmitkRenderWindowMenu::LayoutDesignChanged, this, &MyQmitkRenderWindow::LayoutDesignChanged);        
        disconnect(m_MyMenuWidget, &MyQmitkRenderWindowMenu::ResetView, this, &MyQmitkRenderWindow::ResetView);
//         disconnect(m_MyMenuWidget, &QmitkRenderWindowMenu::CrosshairVisibilityChanged, this, &QmitkRenderWindow::CrosshairVisibilityChanged);
//         disconnect(m_MyMenuWidget, &QmitkRenderWindowMenu::CrosshairRotationModeChanged, this, &QmitkRenderWindow::CrosshairRotationModeChanged);
        
        m_MyMenuWidget->hide();
    }
}

void MyQmitkRenderWindow::mousePressEvent(QMouseEvent *me)
{
    if(onlyInAcTool && GetButtonState(me) == mitk::InteractionEvent::LeftMouseButton) 
        return;  
    QmitkRenderWindow::mousePressEvent(me);
}

void MyQmitkRenderWindow::mouseDoubleClickEvent( QMouseEvent *event )
{
    if(onlyInAcTool && GetButtonState(event) == mitk::InteractionEvent::LeftMouseButton) 
        return;
    QmitkRenderWindow::mouseDoubleClickEvent(event);    
}

void MyQmitkRenderWindow::mouseReleaseEvent( QMouseEvent *event )
{
    if(onlyInAcTool) 
        return;
    QmitkRenderWindow::mouseReleaseEvent(event);    
}

void MyQmitkRenderWindow::mouseMoveEvent(QMouseEvent *me)
{
    this->AdjustRenderWindowMenuVisibility(me->pos());  
    if(onlyInAcTool && GetButtonState(me) == mitk::InteractionEvent::LeftMouseButton) 
        return;
    
    QmitkRenderWindow::mouseMoveEvent(me);
}

void MyQmitkRenderWindow::AdjustRenderWindowMenuVisibility(const QPoint& /*pos*/)
{
    if (m_MyMenuWidget != nullptr)
    {
        m_MyMenuWidget->ShowMenu();
        m_MyMenuWidget->MoveWidgetToCorrectPos(1.0f);
    }
}

void MyQmitkRenderWindow::DeferredHideMenu()
{
    MITK_DEBUG << "QmitkRenderWindow::DeferredHideMenu";
    
    if (m_MyMenuWidget)
        m_MyMenuWidget->HideMenu();
}

mitk::InteractionEvent::MouseButtons MyQmitkRenderWindow::GetButtonState(QMouseEvent* me) const
{
    mitk::InteractionEvent::MouseButtons buttonState = mitk::InteractionEvent::NoButton;
    
    if (me->buttons() & Qt::LeftButton)
    {
        buttonState = buttonState | mitk::InteractionEvent::LeftMouseButton;
    }
    if (me->buttons() & Qt::RightButton)
    {
        buttonState = buttonState | mitk::InteractionEvent::RightMouseButton;
    }
    if (me->buttons() & Qt::MidButton)
    {
        buttonState = buttonState | mitk::InteractionEvent::MiddleMouseButton;
    }
    return buttonState;
}

mitk::InteractionEvent::ModifierKeys MyQmitkRenderWindow::GetModifiers(QInputEvent* me) const
{
    mitk::InteractionEvent::ModifierKeys modifiers = mitk::InteractionEvent::NoKey;
    
    if (me->modifiers() & Qt::ALT)
    {
        modifiers = modifiers | mitk::InteractionEvent::AltKey;
    }
    if (me->modifiers() & Qt::CTRL)
    {
        modifiers = modifiers | mitk::InteractionEvent::ControlKey;
    }
    if (me->modifiers() & Qt::SHIFT)
    {
        modifiers = modifiers | mitk::InteractionEvent::ShiftKey;
    }
    return modifiers;
}

mitk::InteractionEvent::MouseButtons MyQmitkRenderWindow::GetEventButton(QMouseEvent* me) const
{
    mitk::InteractionEvent::MouseButtons eventButton;
    switch (me->button())
    {
        case Qt::LeftButton:
            eventButton = mitk::InteractionEvent::LeftMouseButton;
            break;
        case Qt::RightButton:
            eventButton = mitk::InteractionEvent::RightMouseButton;
            break;
        case Qt::MidButton:
            eventButton = mitk::InteractionEvent::MiddleMouseButton;
            break;
        default:
            eventButton = mitk::InteractionEvent::NoButton;
            break;
    }
    return eventButton;
}

void MyQmitkRenderWindow::leaveEvent(QEvent *e)
{
    if (m_MyMenuWidget)
        m_MyMenuWidget->smoothHide();    
    
    QmitkRenderWindow::leaveEvent(e);
}

void MyQmitkRenderWindow::setACandBCTools(QStringList toolsList)
{
    ACandBCToolsList = toolsList;
}

void MyQmitkRenderWindow::OnOnlyInACTool2DSelected(std::string name)
{
    if(name != "" && !ACandBCToolsList.contains(QString(name.c_str())))    
        onlyInAcTool = true;
    else 
        onlyInAcTool = false;
}

// void MyQmitkRenderWindow::OnChangeLayoutDesign(QmitkRenderWindowMenu::LayoutDesign layoutDesignIndex)
// {
//     emit MyLayoutDesignChanged(layoutDesignIndex);
// }
    
