/*===================================================================
 * 
 * The Medical Imaging Interaction Toolkit (MITK)
 * 
 * Copyright (c) German Cancer Research Center,
 * Division of Medical and Biological Informatics.
 * All rights reserved.
 * 
 * This software is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.
 * 
 * See LICENSE.txt or http://www.mitk.org for details.
 * 
 * ===================================================================*/

#include "MyQmitkRenderWindowMenu.h"

// mitk core
#include "mitkResliceMethodProperty.h"
#include "mitkProperties.h"

// qt
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QSize>
#include <QPainter>

#include<QGroupBox>
#include<QRadioButton>
#include<QAction>
#include<QLine>
#include<QLabel>
#include<QWidgetAction>

#include <QTimer>

//#include"iconClose.xpm"
#include "../../resources/iconFullScreen.xpm"
#include "../../resources/iconSettings.xpm"
// #include"iconCrosshairMode.xpm"
// //#include"iconHoriSplit.xpm"
// #include"iconSettings.xpm"
//#include"iconVertiSplit.xpm"
#include "../../resources/iconLeaveFullScreen.xpm"

// c++
// #include <cmath.h>
#include <math.h>

#ifdef QMITK_USE_EXTERNAL_RENDERWINDOW_MENU
MyQmitkRenderWindowMenu::MyQmitkRenderWindowMenu(QWidget *parent, Qt::WindowFlags f, mitk::BaseRenderer *b)
:QWidget(NULL, Qt::Tool | Qt::FramelessWindowHint ),

#else
MyQmitkRenderWindowMenu::MyQmitkRenderWindowMenu(QWidget *parent, Qt::WindowFlags f, mitk::BaseRenderer *baseRenderer)
:QWidget(parent,f) //,
#endif

// m_Settings(NULL),
// // m_CrosshairMenu(NULL),
// m_Layout(0),
// m_LayoutDesign(0),
// m_OldLayoutDesign(0),
// m_FullScreenMode(false),
// m_Entered(false),
// m_Hidden(true),
// m_Renderer(b),
// m_Parent(parent)
, m_LayoutActionsMenu(nullptr)
// , m_CrosshairMenu(nullptr)
// , m_CrosshairRotationMode(0)
// , m_CrosshairVisibility(true)
, m_Layout(LayoutIndex::AXIAL)
, m_LayoutDesign(LayoutDesign::DEFAULT)
, m_OldLayoutDesign(LayoutDesign::DEFAULT)
, m_FullScreenMode(false)
, m_Renderer(baseRenderer)
, m_Parent(parent)
{
    
    MITK_DEBUG << "creating renderwindow menu on baserenderer " << baseRenderer;
    
    //Create Menu Widget
    this->CreateMenuWidget();
    this->setMinimumWidth(37); //DIRTY.. If you add or remove a button, you need to change the size.
    this->setMaximumWidth(37);
    this->setAutoFillBackground( true );
    
    //Else part fixes the render window menu issue on Linux bug but caused bugs on Mac OS and Windows
    //for Mac OS see bug 3192
    //for Windows see bug 12130
    //... so Mac OS and Windows must be treated differently:
    #if defined(Q_OS_MAC)
    this->show();
    this->setWindowOpacity(0.0f);
    #else
    this->setVisible(false);
    #endif
    
    //    m_AutoRotationTimer = new QTimer(this);
    //    m_AutoRotationTimer->setInterval(75);
    //  
    //    m_HideTimer = new QTimer(this);
    //    m_HideTimer->setSingleShot(true);
    //  
    //    connect(m_AutoRotationTimer, &QTimer::timeout, this, &QmitkRenderWindowMenu::AutoRotateNextStep);
    //    connect(m_HideTimer, &QTimer::timeout, this, &QmitkRenderWindowMenu::DeferredHideMenu);
    connect( m_Parent, SIGNAL(destroyed()), this, SLOT(deleteLater()));
}

MyQmitkRenderWindowMenu::~MyQmitkRenderWindowMenu()
{
    //   if( m_AutoRotationTimer.isActive() )
    //     m_AutoRotationTimer.stop();
}

void MyQmitkRenderWindowMenu::SetLayoutIndex(LayoutIndex layoutIndex)
{
    m_Layout = layoutIndex;
}

void MyQmitkRenderWindowMenu::UpdateLayoutDesignList(LayoutDesign layoutDesign)
{
    m_LayoutDesign = layoutDesign;
    
    if (nullptr == m_LayoutActionsMenu)
    {
        CreateSettingsWidget();
    }
    
    m_DefaultLayoutAction->setEnabled(true);
    m_All2DTop3DBottomLayoutAction->setEnabled(true);
    m_All2DLeft3DRightLayoutAction->setEnabled(true);
    m_OneBigLayoutAction->setEnabled(true);
    m_Only2DHorizontalLayoutAction->setEnabled(true);
    m_Only2DVerticalLayoutAction->setEnabled(true);
    m_OneTop3DBottomLayoutAction->setEnabled(true);
    m_OneLeft3DRightLayoutAction->setEnabled(true);
    m_AllHorizontalLayoutAction->setEnabled(true);
    m_AllVerticalLayoutAction->setEnabled(true);
    m_RemoveOneLayoutAction->setEnabled(true);
    
    m_LeftACandRightBCLayoutAction->setEnabled(true);
    m_UpACandDownBCLayoutAction->setEnabled(true);   
    
    m_2AxialsWithSegmentLayoutAction->setEnabled(true);
    m_2CoronalWithSegmentLayoutAction->setEnabled(true);
    m_2SagittalWithSegmentLayoutAction->setEnabled(true);
    
    switch (m_LayoutDesign)
    {
        case LayoutDesign::DEFAULT:
        {
            m_DefaultLayoutAction->setEnabled(false);
            break;
        }
        case LayoutDesign::ALL_2D_TOP_3D_BOTTOM:
        {
            m_All2DTop3DBottomLayoutAction->setEnabled(false);
            break;
        }
        case LayoutDesign::ALL_2D_LEFT_3D_RIGHT:
        {
            m_All2DLeft3DRightLayoutAction->setEnabled(false);
            break;
        }
        case LayoutDesign::ONE_BIG:
        {
            m_OneBigLayoutAction->setEnabled(false);
            break;
        }
        case LayoutDesign::ONLY_2D_HORIZONTAL:
        {
            m_Only2DHorizontalLayoutAction->setEnabled(false);
            break;
        }
        case LayoutDesign::ONLY_2D_VERTICAL:
        {
            m_Only2DVerticalLayoutAction->setEnabled(false);
            break;
        }
        case LayoutDesign::ONE_TOP_3D_BOTTOM:
        {
            m_OneTop3DBottomLayoutAction->setEnabled(false);
            break;
        }
        case LayoutDesign::ONE_LEFT_3D_RIGHT:
        {
            m_OneLeft3DRightLayoutAction->setEnabled(false);
            break;
        }
        case LayoutDesign::ALL_HORIZONTAL:
        {
            m_AllHorizontalLayoutAction->setEnabled(false);
            break;
        }
        case LayoutDesign::ALL_VERTICAL:
        {
            m_AllVerticalLayoutAction->setEnabled(false);
            break;
        }
        case LayoutDesign::REMOVE_ONE:
        {
            m_RemoveOneLayoutAction->setEnabled(false);
            break;
        }
        case LayoutDesign::NONE:
        {
            break;
        }
        case LayoutDesign::LEFT_AC_AND_RIGHT_BC:
        {
            m_LeftACandRightBCLayoutAction->setEnabled(false);
            break;
        }
        case LayoutDesign::AC_UP_AND_BC_DOWN:
        {
            m_UpACandDownBCLayoutAction->setEnabled(false);
            break;
        }  
        // JCAM
        case LayoutDesign::TWO_AXIAL_WITH_SEGMENTATION:
        {
            m_2AxialsWithSegmentLayoutAction->setEnabled(false);
            break;
        }
        case LayoutDesign::TWO_SAGITTAL_WITH_SEGMENTATION:
        {
            m_2SagittalWithSegmentLayoutAction->setEnabled(false);
            break;
        }
        case LayoutDesign::TWO_CORONNAL_WITH_SEGMENTATION:
        {
            m_2CoronalWithSegmentLayoutAction->setEnabled(false);
            break;
        }
        // JCAM
    }
}

#ifdef QMITK_USE_EXTERNAL_RENDERWINDOW_MENU
void MyQmitkRenderWindowMenu::MoveWidgetToCorrectPos(float opacity)
#else
void MyQmitkRenderWindowMenu::MoveWidgetToCorrectPos(float /*opacity*/)
#endif
{
#ifdef QMITK_USE_EXTERNAL_RENDERWINDOW_MENU
    int X=floor( double(this->m_Parent->width() - this->width() - 8.0) );
    int Y=7;
    
    QPoint pos = this->m_Parent->mapToGlobal( QPoint(0,0) );
    
    this->move( X+pos.x(), Y+pos.y() );
    
    if(opacity<0) 
        opacity=0;
    else if(opacity>1) 
        opacity=1;
    
    this->setWindowOpacity(opacity);
#else
    int moveX= floor( double(this->m_Parent->width() - this->width() - 4.0) );
    this->move( moveX, 3 );
    this->show();
#endif
} 

void MyQmitkRenderWindowMenu::ShowMenu() {
    MITK_DEBUG << "menu showMenu";
    DeferredShowMenu();    
}

void MyQmitkRenderWindowMenu::HideMenu() {
    MITK_DEBUG << "menu hideEvent";
    DeferredHideMenu();    
}

void MyQmitkRenderWindowMenu::paintEvent( QPaintEvent*  /*e*/ )
{
    QPainter painter(this);
    QColor semiTransparentColor = Qt::black;
    semiTransparentColor.setAlpha(255);
    painter.fillRect(rect(), semiTransparentColor);
}

void MyQmitkRenderWindowMenu::CreateMenuWidget()
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setAlignment( Qt::AlignRight );
    layout->setContentsMargins(1,1,1,1);
    
    QSize size( 13, 13 );
    
    //fullScreenButton
    m_FullScreenButton = new QToolButton(this);
    m_FullScreenButton->setMaximumSize(15, 15);
    m_FullScreenButton->setIconSize(size);  
    m_FullScreenButton->setIcon(QIcon(QPixmap(iconFullScreen_xpm)));
    m_FullScreenButton->setAutoRaise(true);
    layout->addWidget( m_FullScreenButton );
    
    //settingsButton
/*    m_SettingsButton = new QToolButton(this);
    m_SettingsButton->setMaximumSize(15, 15);
    m_SettingsButton->setIconSize(size);
    m_SettingsButton->setIcon(QIcon(QPixmap(iconSettings_xpm)));
    m_SettingsButton->setAutoRaise(true);  
    layout->addWidget( m_SettingsButton ); */ 
   m_LayoutDesignButton = new QToolButton(this);
   m_LayoutDesignButton->setMaximumSize(15, 15);
   m_LayoutDesignButton->setIconSize(size);
   m_LayoutDesignButton->setIcon(QIcon(QPixmap(iconSettings_xpm)));
   m_LayoutDesignButton->setAutoRaise(true);
   layout->addWidget(m_LayoutDesignButton);
    
    //Create Connections -- coming soon?
//     connect( m_FullScreenButton, SIGNAL( clicked(bool) ), this, SLOT(OnFullScreenButton(bool)) );
//     connect( m_SettingsButton, SIGNAL( clicked(bool) ), this, SLOT(OnSettingsButton(bool)) );
   connect(m_FullScreenButton, &QToolButton::clicked, this, &MyQmitkRenderWindowMenu::OnFullScreenButton);
   connect(m_LayoutDesignButton, &QToolButton::clicked, this, &MyQmitkRenderWindowMenu::OnLayoutDesignButton);   
    
}

// void MyQmitkRenderWindowMenu::CreateSettingsWidget()
// {
//     m_Settings = new QMenu(this);
//     
//     m_DefaultLayoutAction = new QAction( "standard layout", m_Settings );
//     m_DefaultLayoutAction->setDisabled( true );
//     
//     m_2DImagesUpLayoutAction = new QAction( "2D images top, 3D bottom", m_Settings );
//     m_2DImagesUpLayoutAction->setDisabled( false );
//     
//     m_2DImagesLeftLayoutAction = new QAction( "2D images left, 3D right", m_Settings );
//     m_2DImagesLeftLayoutAction->setDisabled( false );
//     
//     m_Big3DLayoutAction = new QAction( "Big 3D", m_Settings );
//     m_Big3DLayoutAction->setDisabled( false );
//     
//     m_Widget1LayoutAction = new QAction( "Axial AC", m_Settings );
//     m_Widget1LayoutAction->setDisabled( false );
//     
//     m_Widget2LayoutAction = new QAction( "Axial BC", m_Settings );
//     m_Widget2LayoutAction->setDisabled( false );
//     
//     m_Widget3LayoutAction = new QAction( "Axial Patient", m_Settings );
//     m_Widget3LayoutAction->setDisabled( false );
//     
//     m_RowWidget3And4LayoutAction = new QAction( "Axial Patient top, 3D bottom", m_Settings );
//     m_RowWidget3And4LayoutAction->setDisabled( false );
//     
//     m_ColumnWidget3And4LayoutAction = new QAction( "Axial Patient left, 3D right", m_Settings );
//     m_ColumnWidget3And4LayoutAction->setDisabled( false );
//     
//     m_SmallUpperWidget2Big3and4LayoutAction = new QAction( "Axial BC top, Axial Patient n 3D bottom", m_Settings );
//     m_SmallUpperWidget2Big3and4LayoutAction->setDisabled( false );
//     
//     m_2x2Dand3DWidgetLayoutAction = new QAction( "Axial AC and Axial BC left, 3D right", m_Settings );
//     m_2x2Dand3DWidgetLayoutAction->setDisabled( false );
//     
//     m_Left2Dand3DRight2DLayoutAction = new QAction( "Axial AC and 3D left, Axial BC right", m_Settings );
//     m_Left2Dand3DRight2DLayoutAction->setDisabled( false );
//     
//     m_LeftACandRightBCLayoutAction = new QAction( "Axial AC left, Axial BC right", m_Settings );
//     m_LeftACandRightBCLayoutAction->setDisabled( false );
//     
//     m_UpACandDownBCLayoutAction = new QAction( "Axial AC up, Axial BC down", m_Settings );
//     m_UpACandDownBCLayoutAction->setDisabled( false );
//     
//     m_Settings->addAction(m_DefaultLayoutAction);
//     m_Settings->addAction(m_2DImagesUpLayoutAction);
//     m_Settings->addAction(m_2DImagesLeftLayoutAction);
//     m_Settings->addAction(m_Big3DLayoutAction);
//     m_Settings->addAction(m_Widget1LayoutAction);
//     m_Settings->addAction(m_Widget2LayoutAction);
//     m_Settings->addAction(m_Widget3LayoutAction);
//     m_Settings->addAction(m_RowWidget3And4LayoutAction);
//     m_Settings->addAction(m_ColumnWidget3And4LayoutAction);
//     m_Settings->addAction(m_SmallUpperWidget2Big3and4LayoutAction);
//     m_Settings->addAction(m_2x2Dand3DWidgetLayoutAction);
//     m_Settings->addAction(m_Left2Dand3DRight2DLayoutAction);
//     m_Settings->addAction(m_LeftACandRightBCLayoutAction);
//     m_Settings->addAction(m_UpACandDownBCLayoutAction);
//     
//     m_Settings->setVisible( false );
//     
//     connect( m_DefaultLayoutAction, SIGNAL( triggered(bool) ), this, SLOT(OnChangeLayoutToDefault(bool)) );
//     connect( m_2DImagesUpLayoutAction, SIGNAL( triggered(bool) ), this, SLOT(OnChangeLayoutTo2DImagesUp(bool)) );
//     connect( m_2DImagesLeftLayoutAction, SIGNAL( triggered(bool) ), this, SLOT(OnChangeLayoutTo2DImagesLeft(bool)) );
//     connect( m_Big3DLayoutAction, SIGNAL( triggered(bool) ), this, SLOT(OnChangeLayoutToBig3D(bool)) );
//     connect( m_Widget1LayoutAction, SIGNAL( triggered(bool) ), this, SLOT(OnChangeLayoutToWidget1(bool)) );
//     connect( m_Widget2LayoutAction, SIGNAL( triggered(bool) ), this, SLOT(OnChangeLayoutToWidget2(bool)) );
//     connect( m_Widget3LayoutAction  , SIGNAL( triggered(bool) ), this, SLOT(OnChangeLayoutToWidget3(bool)) );
//     connect( m_RowWidget3And4LayoutAction, SIGNAL( triggered(bool) ), this, SLOT(OnChangeLayoutToRowWidget3And4(bool)) );
//     connect( m_ColumnWidget3And4LayoutAction, SIGNAL( triggered(bool) ), this, SLOT(OnChangeLayoutToColumnWidget3And4(bool)) );
//     connect( m_SmallUpperWidget2Big3and4LayoutAction, SIGNAL( triggered(bool) ), this, SLOT(OnChangeLayoutToSmallUpperWidget2Big3and4(bool)) );
//     connect( m_2x2Dand3DWidgetLayoutAction, SIGNAL( triggered(bool) ), this, SLOT(OnChangeLayoutTo2x2Dand3DWidget(bool)) );
//     connect( m_Left2Dand3DRight2DLayoutAction, SIGNAL( triggered(bool) ), this, SLOT(OnChangeLayoutToLeft2Dand3DRight2D(bool)) );
//     connect( m_LeftACandRightBCLayoutAction, SIGNAL( triggered(bool) ), this, SLOT(OnChangeLayoutToLeftACandRightBC(bool)) );
//     connect( m_UpACandDownBCLayoutAction, SIGNAL( triggered(bool) ), this, SLOT(OnChangeLayoutToUpACandDownBC(bool)) );
//     
// }

void MyQmitkRenderWindowMenu::CreateSettingsWidget()
{
    m_LayoutActionsMenu = new QMenu(this);
    
    m_DefaultLayoutAction = new QAction("Standard layout", m_LayoutActionsMenu);
    m_DefaultLayoutAction->setDisabled(true);
    
    m_All2DTop3DBottomLayoutAction = new QAction("All 2D top, 3D bottom", m_LayoutActionsMenu);
    m_All2DTop3DBottomLayoutAction->setDisabled(false);
    
    m_All2DLeft3DRightLayoutAction = new QAction("All 2D left, 3D right", m_LayoutActionsMenu);
    m_All2DLeft3DRightLayoutAction->setDisabled(false);
    
    m_OneBigLayoutAction = new QAction("This big", m_LayoutActionsMenu);
    m_OneBigLayoutAction->setDisabled(false);
    
    m_Only2DHorizontalLayoutAction = new QAction("Only 2D horizontal", m_LayoutActionsMenu);
    m_Only2DHorizontalLayoutAction->setDisabled(false);
    
    m_Only2DVerticalLayoutAction = new QAction("Only 2D vertical", m_LayoutActionsMenu);
    m_Only2DVerticalLayoutAction->setDisabled(false);
    
    m_OneTop3DBottomLayoutAction = new QAction("This top, 3D bottom", m_LayoutActionsMenu);
    m_OneTop3DBottomLayoutAction->setDisabled(false);
    
    m_OneLeft3DRightLayoutAction = new QAction("This left, 3D right", m_LayoutActionsMenu);
    m_OneLeft3DRightLayoutAction->setDisabled(false);
    
    m_AllHorizontalLayoutAction = new QAction("All horizontal", m_LayoutActionsMenu);
    m_AllHorizontalLayoutAction->setDisabled(false);
    
    m_AllVerticalLayoutAction = new QAction("All vertical", m_LayoutActionsMenu);
    m_AllVerticalLayoutAction->setDisabled(false);
    
    m_RemoveOneLayoutAction = new QAction("Remove this", m_LayoutActionsMenu);
    m_RemoveOneLayoutAction->setDisabled(false);
    
    m_LeftACandRightBCLayoutAction = new QAction( "Axial AC left, Axial BC right", m_LayoutActionsMenu );
    m_LeftACandRightBCLayoutAction->setDisabled( false );
    
    m_UpACandDownBCLayoutAction = new QAction( "Axial AC up, Axial BC down", m_LayoutActionsMenu );
    m_UpACandDownBCLayoutAction->setDisabled( false );    
    
    /* JCAM. Create 2 axials with segments menu option */
    m_2AxialsWithSegmentLayoutAction = new QAction( "2 Axials with segments", m_LayoutActionsMenu );
    m_2AxialsWithSegmentLayoutAction->setDisabled( false );    
    
    /* JCAM. Create 2 Sagittals with segments menu option */
    m_2SagittalWithSegmentLayoutAction = new QAction( "2 Sagittals with segmentation", m_LayoutActionsMenu );
    m_2SagittalWithSegmentLayoutAction->setDisabled( false );    
    
    /* JCAM. Create 2 Coranals with segments menu option */
    m_2CoronalWithSegmentLayoutAction = new QAction( "2 Coronals with segments", m_LayoutActionsMenu );
    m_2CoronalWithSegmentLayoutAction->setDisabled( false );    
    
    m_LayoutActionsMenu->addAction(m_DefaultLayoutAction);
    m_LayoutActionsMenu->addAction(m_All2DTop3DBottomLayoutAction);
    m_LayoutActionsMenu->addAction(m_All2DLeft3DRightLayoutAction);
    m_LayoutActionsMenu->addAction(m_OneBigLayoutAction);
    m_LayoutActionsMenu->addAction(m_Only2DHorizontalLayoutAction);
    m_LayoutActionsMenu->addAction(m_Only2DVerticalLayoutAction);
    m_LayoutActionsMenu->addAction(m_OneTop3DBottomLayoutAction);
    m_LayoutActionsMenu->addAction(m_OneLeft3DRightLayoutAction);
    m_LayoutActionsMenu->addAction(m_AllHorizontalLayoutAction);
    m_LayoutActionsMenu->addAction(m_AllVerticalLayoutAction);
    m_LayoutActionsMenu->addAction(m_RemoveOneLayoutAction);
    
    m_LayoutActionsMenu->addAction(m_LeftACandRightBCLayoutAction);
    m_LayoutActionsMenu->addAction(m_UpACandDownBCLayoutAction);  
    
    /* JCAM. Add new menu options */
    m_LayoutActionsMenu->addAction(m_2AxialsWithSegmentLayoutAction);  
    m_LayoutActionsMenu->addAction(m_2CoronalWithSegmentLayoutAction);  
    m_LayoutActionsMenu->addAction(m_2SagittalWithSegmentLayoutAction);  
    
    
    m_LayoutActionsMenu->setVisible(false);
    
    connect(m_DefaultLayoutAction, &QAction::triggered, [this]() { this->OnSetLayout(LayoutDesign::DEFAULT); });
    connect(m_All2DTop3DBottomLayoutAction, &QAction::triggered, [this]() { this->OnSetLayout(LayoutDesign::ALL_2D_TOP_3D_BOTTOM); });
    connect(m_All2DLeft3DRightLayoutAction, &QAction::triggered, [this]() { this->OnSetLayout(LayoutDesign::ALL_2D_LEFT_3D_RIGHT); });
    connect(m_OneBigLayoutAction, &QAction::triggered, [this]() { this->OnSetLayout(LayoutDesign::ONE_BIG); });
    connect(m_Only2DHorizontalLayoutAction, &QAction::triggered, [this]() { this->OnSetLayout(LayoutDesign::ONLY_2D_HORIZONTAL); });
    connect(m_Only2DVerticalLayoutAction, &QAction::triggered, [this]() { this->OnSetLayout(LayoutDesign::ONLY_2D_VERTICAL); });
    connect(m_OneTop3DBottomLayoutAction, &QAction::triggered, [this]() { this->OnSetLayout(LayoutDesign::ONE_TOP_3D_BOTTOM); });
    connect(m_OneLeft3DRightLayoutAction, &QAction::triggered, [this]() { this->OnSetLayout(LayoutDesign::ONE_LEFT_3D_RIGHT); });
    connect(m_AllHorizontalLayoutAction, &QAction::triggered, [this]() { this->OnSetLayout(LayoutDesign::ALL_HORIZONTAL); });
    connect(m_AllVerticalLayoutAction, &QAction::triggered, [this]() { this->OnSetLayout(LayoutDesign::ALL_VERTICAL); });
    connect(m_RemoveOneLayoutAction, &QAction::triggered, [this]() { this->OnSetLayout(LayoutDesign::REMOVE_ONE); });
    
    connect(m_LeftACandRightBCLayoutAction, &QAction::triggered, [this]() { this->OnSetLayout(LayoutDesign::LEFT_AC_AND_RIGHT_BC); });
    connect(m_UpACandDownBCLayoutAction, &QAction::triggered, [this]() { this->OnSetLayout(LayoutDesign::AC_UP_AND_BC_DOWN); });
    
    /// JCAM
    connect(m_2AxialsWithSegmentLayoutAction, &QAction::triggered, [this]() { this->OnSetLayout(LayoutDesign::TWO_AXIAL_WITH_SEGMENTATION); });
    connect(m_2CoronalWithSegmentLayoutAction, &QAction::triggered, [this]() { this->OnSetLayout(LayoutDesign::TWO_CORONNAL_WITH_SEGMENTATION); });
    connect(m_2SagittalWithSegmentLayoutAction, &QAction::triggered, [this]() { this->OnSetLayout(LayoutDesign::TWO_SAGITTAL_WITH_SEGMENTATION); });
}

void MyQmitkRenderWindowMenu::ChangeFullScreenIcon()
{
    m_FullScreenButton->setIcon(m_FullScreenMode ? QPixmap(iconLeaveFullScreen_xpm) : QPixmap(iconFullScreen_xpm));
}

void MyQmitkRenderWindowMenu::DeferredShowMenu()
{
    
    MITK_DEBUG << "deferred show menu";
//     m_HideTimer->stop();
    
    //Else part fixes the render window menu issue on Linux bug but caused bugs on Mac OS and Windows
    //for Mac OS see bug 3192
    //for Windows see bug 12130
    //... so Mac OS and Windows must be treated differently:
#if defined(Q_OS_MAC)
    this->setWindowOpacity(1.0f);
#else
    this->setVisible(true);
    this->raise();
#endif
}

void MyQmitkRenderWindowMenu::DeferredHideMenu( )
{
    MITK_DEBUG << "menu deferredhidemenu";
//     if(m_Hidden)
//     {
//         Else part fixes the render window menu issue on Linux bug but caused bugs on Mac OS and Windows
//         for Mac OS see bug 3192
//         for Windows see bug 12130
//         ... so Mac OS and Windows must be treated differently:
//         #if defined(Q_OS_MAC)
//         this->setWindowOpacity(0.0f);
//         #else
//         this->setVisible(false);
//         #endif
//     }
    
    //    setVisible(false);
    //    setWindowOpacity(0.0f);
    ///hide();
#if defined(Q_OS_MAC)
    this->setWindowOpacity(0.0f);
#else
    this->setVisible(false);
#endif    
}

/* This method is responsible for non fluttering of
 * the renderWindowMenu when mouse cursor moves along the renderWindowMenu*/
void MyQmitkRenderWindowMenu::smoothHide()
{
    
//     MITK_DEBUG<< "menu leaveEvent";
//     
//     m_Entered=false;
//     
//     m_Hidden = true;
//     
//     QTimer::singleShot(10, this, SLOT( DeferredHideMenu( ) ) );

   MITK_DEBUG << "menu leaveEvent";
/*   m_HideTimer->start(10); */   
    
}

void MyQmitkRenderWindowMenu::enterEvent( QEvent * /*e*/ )
{
    MITK_DEBUG << "menu enterEvent";
    DeferredShowMenu();
    
//     m_Entered=true;
//     
//     m_Hidden=false;
    
}

void MyQmitkRenderWindowMenu::leaveEvent( QEvent * /*e*/ )
{
    MITK_DEBUG << "menu leaveEvent";    
    smoothHide();
    
}

void MyQmitkRenderWindowMenu::AutoRotateNextStep()
{
    //   if(m_Renderer->GetCameraRotationController())
    //     m_Renderer->GetCameraRotationController()->GetSlice()->Next();
}

void MyQmitkRenderWindowMenu::OnAutoRotationActionTriggered()
{
    //   if(m_AutoRotationTimer.isActive())
    //   {
    //     m_AutoRotationTimer.stop();
    //     m_Renderer->GetCameraRotationController()->GetSlice()->PingPongOff();
    //   }
    //   else
    //   {
    //     m_Renderer->GetCameraRotationController()->GetSlice()->PingPongOn();
    //     m_AutoRotationTimer.start();
    //   }
}

void MyQmitkRenderWindowMenu::OnTSNumChanged(int /*num*/)
{
//    MITK_DEBUG << "Thickslices num: " << num << " on renderer " << m_Renderer.GetPointer();
//  
//    if (m_Renderer.IsNotNull())
//    {
//      unsigned int thickSlicesMode = 0;
//      //determine the state of the thick-slice mode
//      mitk::ResliceMethodProperty *resliceMethodEnumProperty = nullptr;
//  
//      if(m_Renderer->GetCurrentWorldPlaneGeometryNode()->GetProperty(resliceMethodEnumProperty, "reslice.thickslices") && resliceMethodEnumProperty)
//      {
//        thickSlicesMode = resliceMethodEnumProperty->GetValueAsId();
//        if(thickSlicesMode!=0)
//          m_DefaultThickMode = thickSlicesMode;
//      }
//  
//      if(thickSlicesMode==0 && num>0) //default mode only for single slices
//      {
//        thickSlicesMode = m_DefaultThickMode; //mip default
//        m_Renderer->GetCurrentWorldPlaneGeometryNode()->SetProperty("reslice.thickslices.showarea",
//                                                                    mitk::BoolProperty::New(true));
//      }
//      if(num<1)
//      {
//        thickSlicesMode = 0;
//        m_Renderer->GetCurrentWorldPlaneGeometryNode()->SetProperty("reslice.thickslices.showarea",
//                                                                    mitk::BoolProperty::New(false));
//      }
//  
//      m_Renderer->GetCurrentWorldPlaneGeometryNode()->SetProperty("reslice.thickslices",
//                                                                  mitk::ResliceMethodProperty::New(thickSlicesMode));
//      m_Renderer->GetCurrentWorldPlaneGeometryNode()->SetProperty("reslice.thickslices.num",
//                                                                  mitk::IntProperty::New(num));
//  
//      m_TSLabel->setText(QString::number(num * 2 + 1));
//      m_Renderer->SendUpdateSlice();
//      mitk::RenderingManager::GetInstance()->RequestUpdateAll();
//    }
}

// void QmitkRenderWindowMenu::OnCrosshairMenuAboutToShow()
// {
//     QMenu *crosshairModesMenu = m_CrosshairMenu;
//     
//     crosshairModesMenu->clear();
//     
//     QAction *resetViewAction = new QAction(crosshairModesMenu);
//     resetViewAction->setText("Reset view");
//     crosshairModesMenu->addAction(resetViewAction);
//     connect(resetViewAction, &QAction::triggered, this, &QmitkRenderWindowMenu::ResetView);
//     
//     Show hide crosshairs
//     {
//         QAction *showHideCrosshairVisibilityAction = new QAction(crosshairModesMenu);
//         showHideCrosshairVisibilityAction->setText("Show crosshair");
//         showHideCrosshairVisibilityAction->setCheckable(true);
//         showHideCrosshairVisibilityAction->setChecked(m_CrosshairVisibility);
//         crosshairModesMenu->addAction(showHideCrosshairVisibilityAction);
//         connect(showHideCrosshairVisibilityAction, &QAction::toggled, this, &QmitkRenderWindowMenu::OnCrosshairVisibilityChanged);
//     }
//     
//     Rotation mode
//     {
//         QAction *rotationGroupSeparator = new QAction(crosshairModesMenu);
//         rotationGroupSeparator->setSeparator(true);
//         rotationGroupSeparator->setText("Rotation mode");
//         crosshairModesMenu->addAction(rotationGroupSeparator);
//         
//         QActionGroup *rotationModeActionGroup = new QActionGroup(crosshairModesMenu);
//         rotationModeActionGroup->setExclusive(true);
//         
//         QAction *noCrosshairRotation = new QAction(crosshairModesMenu);
//         noCrosshairRotation->setActionGroup(rotationModeActionGroup);
//         noCrosshairRotation->setText("No crosshair rotation");
//         noCrosshairRotation->setCheckable(true);
//         noCrosshairRotation->setChecked(m_CrosshairRotationMode == 0);
//         noCrosshairRotation->setData(0);
//         crosshairModesMenu->addAction(noCrosshairRotation);
//         
//         QAction *singleCrosshairRotation = new QAction(crosshairModesMenu);
//         singleCrosshairRotation->setActionGroup(rotationModeActionGroup);
//         singleCrosshairRotation->setText("Crosshair rotation");
//         singleCrosshairRotation->setCheckable(true);
//         singleCrosshairRotation->setChecked(m_CrosshairRotationMode == 1);
//         singleCrosshairRotation->setData(1);
//         crosshairModesMenu->addAction(singleCrosshairRotation);
//         
//         QAction *coupledCrosshairRotation = new QAction(crosshairModesMenu);
//         coupledCrosshairRotation->setActionGroup(rotationModeActionGroup);
//         coupledCrosshairRotation->setText("Coupled crosshair rotation");
//         coupledCrosshairRotation->setCheckable(true);
//         coupledCrosshairRotation->setChecked(m_CrosshairRotationMode == 2);
//         coupledCrosshairRotation->setData(2);
//         crosshairModesMenu->addAction(coupledCrosshairRotation);
//         
//         QAction *swivelMode = new QAction(crosshairModesMenu);
//         swivelMode->setActionGroup(rotationModeActionGroup);
//         swivelMode->setText("Swivel mode");
//         swivelMode->setCheckable(true);
//         swivelMode->setChecked(m_CrosshairRotationMode == 3);
//         swivelMode->setData(3);
//         crosshairModesMenu->addAction(swivelMode);
//         
//         connect(rotationModeActionGroup, &QActionGroup::triggered, this, &QmitkRenderWindowMenu::OnCrosshairRotationModeSelected);
//     }
//     
//     auto rotation support
//     if (m_Renderer.IsNotNull() && m_Renderer->GetMapperID() == mitk::BaseRenderer::Standard3D)
//     {
//         QAction *autoRotationGroupSeparator = new QAction(crosshairModesMenu);
//         autoRotationGroupSeparator->setSeparator(true);
//         crosshairModesMenu->addAction(autoRotationGroupSeparator);
//         
//         QAction *autoRotationAction = crosshairModesMenu->addAction("Auto Rotation");
//         autoRotationAction->setCheckable(true);
//         autoRotationAction->setChecked(m_AutoRotationTimer->isActive());
//         connect(autoRotationAction, &QAction::triggered, this, &QmitkRenderWindowMenu::OnAutoRotationActionTriggered);
//     }
//     
//     Thickslices support
//     if (m_Renderer.IsNotNull() && m_Renderer->GetMapperID() == mitk::BaseRenderer::Standard2D)
//     {
//         QAction *thickSlicesGroupSeparator = new QAction(crosshairModesMenu);
//         thickSlicesGroupSeparator->setSeparator(true);
//         thickSlicesGroupSeparator->setText("ThickSlices mode");
//         crosshairModesMenu->addAction(thickSlicesGroupSeparator);
//         
//         QActionGroup *thickSlicesActionGroup = new QActionGroup(crosshairModesMenu);
//         thickSlicesActionGroup->setExclusive(true);
//         
//         int currentMode = 0;
//         {
//             mitk::ResliceMethodProperty::Pointer m = dynamic_cast<mitk::ResliceMethodProperty *>(
//                 m_Renderer->GetCurrentWorldPlaneGeometryNode()->GetProperty("reslice.thickslices"));
//             if (m.IsNotNull())
//                 currentMode = m->GetValueAsId();
//         }
//         
//         int currentNum = 1;
//         {
//             mitk::IntProperty::Pointer m = dynamic_cast<mitk::IntProperty *>(
//                 m_Renderer->GetCurrentWorldPlaneGeometryNode()->GetProperty("reslice.thickslices.num"));
//             if (m.IsNotNull())
//             {
//                 currentNum = m->GetValue();
//             }
//         }
//         
//         if (currentMode == 0)
//             currentNum = 0;
//         
//         QSlider *m_TSSlider = new QSlider(crosshairModesMenu);
//         m_TSSlider->setMinimum(0);
//         m_TSSlider->setMaximum(50);
//         m_TSSlider->setValue(currentNum);
//         
//         m_TSSlider->setOrientation(Qt::Horizontal);
//         
//         connect(m_TSSlider, &QSlider::valueChanged, this, &QmitkRenderWindowMenu::OnTSNumChanged);
//         
//         QHBoxLayout *tsLayout = new QHBoxLayout;
//         tsLayout->setContentsMargins(4, 4, 4, 4);
//         tsLayout->addWidget(new QLabel("TS: "));
//         tsLayout->addWidget(m_TSSlider);
//         tsLayout->addWidget(m_TSLabel = new QLabel(QString::number(currentNum * 2 + 1), this));
//         
//         QWidget *tsWidget = new QWidget;
//         tsWidget->setLayout(tsLayout);
//         
//         QWidgetAction *m_TSSliderAction = new QWidgetAction(crosshairModesMenu);
//         m_TSSliderAction->setDefaultWidget(tsWidget);
//         crosshairModesMenu->addAction(m_TSSliderAction);
//     }
// }
// 
// void QmitkRenderWindowMenu::OnCrosshairVisibilityChanged(bool visible)
// {
//     m_CrosshairVisibility = visible;
//     emit CrosshairVisibilityChanged(visible);
// }
// 
// void QmitkRenderWindowMenu::OnCrosshairRotationModeSelected(QAction *action)
// {
//     m_CrosshairRotationMode = action->data().toInt();
//     emit CrosshairRotationModeChanged(m_CrosshairRotationMode);
// }

/// \brief
// void MyQmitkRenderWindowMenu::OnFullScreenButton( bool  /*checked*/ )
// {
//     if( !m_FullScreenMode )
//     {
//         m_FullScreenMode = true;
//         m_OldLayoutDesign = m_LayoutDesign;
//         
//         switch( m_Layout )
//         {
//             case AXIAL:
//             {
//                 emit LayoutDesignChanged( LAYOUT_AXIAL );
//                 break;
//             }
//             
//             case SAGITTAL:
//             {
//                 emit LayoutDesignChanged( LAYOUT_SAGITTAL );
//                 break;
//             }
//             case CORONAL:
//             {
//                 emit LayoutDesignChanged( LAYOUT_CORONAL );
//                 break;
//             }
//             case THREE_D:
//             {
//                 emit LayoutDesignChanged( LAYOUT_BIG3D );
//                 break;
//             }
//         }
//         
//         Move Widget and show again
//         this->MoveWidgetToCorrectPos(1.0f);
//         
//         change icon
//         this->ChangeFullScreenIcon();
//         
//     }
//     else
//     {
//         m_FullScreenMode = false;
//         emit LayoutDesignChanged( m_OldLayoutDesign );
//         
//         //Move Widget and show again
//         this->MoveWidgetToCorrectPos(1.0f);
//         
//         //change icon
//         this->ChangeFullScreenIcon();
//     }
//     
//     DeferredShowMenu( );
//     
// }

void MyQmitkRenderWindowMenu::OnFullScreenButton(bool /*checked*/)
{
    if (!m_FullScreenMode)
    {
        m_FullScreenMode = true;
        m_OldLayoutDesign = m_LayoutDesign;
        
        emit LayoutDesignChanged(LayoutDesign::ONE_BIG);
    }
    else
    {
        m_FullScreenMode = false;
        emit LayoutDesignChanged(m_OldLayoutDesign);
    }
    
    MoveWidgetToCorrectPos(1.0f);
    ChangeFullScreenIcon();
    
    DeferredShowMenu();
}

/// \brief
// void MyQmitkRenderWindowMenu::OnSettingsButton( bool  /*checked*/ )
// change m_Settings to m_LayoutActionsMenu
// change m_SettingsButton to m_LayoutDesignButton
void MyQmitkRenderWindowMenu::OnLayoutDesignButton( bool  /*checked*/ )
{
    if( m_LayoutActionsMenu == nullptr )
        this->CreateSettingsWidget();
    
    QPoint point = this->mapToGlobal( m_LayoutDesignButton->geometry().topLeft() );
    m_LayoutActionsMenu->setVisible( true );
    m_LayoutActionsMenu->exec( point );
}

void MyQmitkRenderWindowMenu::OnSetLayout(LayoutDesign layoutDesign)
{
    m_FullScreenMode = false;
    ChangeFullScreenIcon();
    
    m_LayoutDesign = layoutDesign;
    emit LayoutDesignChanged(m_LayoutDesign);
    
    DeferredShowMenu();
}

// void MyQmitkRenderWindowMenu::HideMenu( )
// {
//   MITK_DEBUG << "menu hideEvent";
// 
//   m_Hidden = true;
// 
//   if( ! m_Entered )
//   {
//      Else part fixes the render window menu issue on Linux bug but caused bugs on Mac OS and Windows
//      for Mac OS see bug 3192
//      for Windows see bug 12130
//      ... so Mac OS and Windows must be treated differently:
// #if defined(Q_OS_MAC)
//     this->setWindowOpacity(0.0f);
// #else
//     this->setVisible(false);
// #endif
//   }
// }

// void MyQmitkRenderWindowMenu::ChangeFullScreenMode( bool state )
// {
//   this->OnFullScreenButton( state );
// }

// void MyQmitkRenderWindowMenu::OnChangeLayoutTo2DImagesUp(bool)
// {
//   set Full Screen Mode to false, if Layout Design was changed by the LayoutDesign_List
//   m_FullScreenMode = false;
//   this->ChangeFullScreenIcon();
// 
//   m_LayoutDesign = LAYOUT_2DIMAGEUP;
//   emit LayoutDesignChanged( LAYOUT_2DIMAGEUP );
// 
//   DeferredShowMenu( );
// 
// }
// void MyQmitkRenderWindowMenu::OnChangeLayoutTo2DImagesLeft(bool)
// {
//   set Full Screen Mode to false, if Layout Design was changed by the LayoutDesign_List
//   m_FullScreenMode = false;
//   this->ChangeFullScreenIcon();
// 
//   m_LayoutDesign = LAYOUT_2DIMAGELEFT;
//   emit LayoutDesignChanged( LAYOUT_2DIMAGELEFT );
// 
//   DeferredShowMenu( );
// }
// void MyQmitkRenderWindowMenu::OnChangeLayoutToDefault(bool)
// {
//   set Full Screen Mode to false, if Layout Design was changed by the LayoutDesign_List
//   m_FullScreenMode = false;
//   this->ChangeFullScreenIcon();
// 
//   m_LayoutDesign = LAYOUT_DEFAULT;
//   emit LayoutDesignChanged( LAYOUT_DEFAULT );
// 
//   DeferredShowMenu( );
// 
// }

// void MyQmitkRenderWindowMenu::OnChangeLayoutToBig3D(bool)
// {
//   MITK_DEBUG << "OnChangeLayoutToBig3D";
// 
//   set Full Screen Mode to false, if Layout Design was changed by the LayoutDesign_List
//   m_FullScreenMode = false;
//   this->ChangeFullScreenIcon();
// 
//   m_LayoutDesign = LAYOUT_BIG3D;
//   emit LayoutDesignChanged( LAYOUT_BIG3D );
// 
//   DeferredShowMenu( );
// 
// }
// 
// void MyQmitkRenderWindowMenu::OnChangeLayoutToWidget1(bool)
// {
//   set Full Screen Mode to false, if Layout Design was changed by the LayoutDesign_List
//   m_FullScreenMode = false;
//   this->ChangeFullScreenIcon();
// 
//   m_LayoutDesign = LAYOUT_AXIAL;
//   emit LayoutDesignChanged( LAYOUT_AXIAL );
// 
//   DeferredShowMenu( );
// }
// void MyQmitkRenderWindowMenu::OnChangeLayoutToWidget2(bool)
// {
//   set Full Screen Mode to false, if Layout Design was changed by the LayoutDesign_List
//   m_FullScreenMode = false;
//   this->ChangeFullScreenIcon();
// 
//   m_LayoutDesign = LAYOUT_SAGITTAL;
//   emit LayoutDesignChanged( LAYOUT_SAGITTAL );
// 
//   DeferredShowMenu( );
// }
// void MyQmitkRenderWindowMenu::OnChangeLayoutToWidget3(bool)
// {
//   set Full Screen Mode to false, if Layout Design was changed by the LayoutDesign_List
//   m_FullScreenMode = false;
//   this->ChangeFullScreenIcon();
// 
//   m_LayoutDesign = LAYOUT_CORONAL;
//   emit LayoutDesignChanged( LAYOUT_CORONAL );
// 
//   DeferredShowMenu( );
// }
// void MyQmitkRenderWindowMenu::OnChangeLayoutToRowWidget3And4(bool)
// {
//   set Full Screen Mode to false, if Layout Design was changed by the LayoutDesign_List
//   m_FullScreenMode = false;
//   this->ChangeFullScreenIcon();
// 
//   m_LayoutDesign = LAYOUT_ROWWIDGET3AND4;
//   emit LayoutDesignChanged( LAYOUT_ROWWIDGET3AND4 );
// 
//   DeferredShowMenu( );
// }
// void MyQmitkRenderWindowMenu::OnChangeLayoutToColumnWidget3And4(bool)
// {
//   set Full Screen Mode to false, if Layout Design was changed by the LayoutDesign_List
//   m_FullScreenMode = false;
//   this->ChangeFullScreenIcon();
// 
//   m_LayoutDesign = LAYOUT_COLUMNWIDGET3AND4;
//   emit LayoutDesignChanged( LAYOUT_COLUMNWIDGET3AND4 );
// 
//   DeferredShowMenu( );
// }
// 
// void MyQmitkRenderWindowMenu::OnChangeLayoutToSmallUpperWidget2Big3and4(bool)
// {
//   set Full Screen Mode to false, if Layout Design was changed by the LayoutDesign_List
//   m_FullScreenMode = false;
//   this->ChangeFullScreenIcon();
// 
//   m_LayoutDesign = LAYOUT_SMALLUPPERWIDGET2BIGAND4;
//   emit LayoutDesignChanged( LAYOUT_SMALLUPPERWIDGET2BIGAND4 );
// 
//   DeferredShowMenu( );
// }
// void MyQmitkRenderWindowMenu::OnChangeLayoutTo2x2Dand3DWidget(bool)
// {
//   set Full Screen Mode to false, if Layout Design was changed by the LayoutDesign_List
//   m_FullScreenMode = false;
//   this->ChangeFullScreenIcon();
// 
//   m_LayoutDesign = LAYOUT_2X2DAND3DWIDGET;
//   emit LayoutDesignChanged( LAYOUT_2X2DAND3DWIDGET );
// 
//   DeferredShowMenu( );
// }
// 
// void MyQmitkRenderWindowMenu::OnChangeLayoutToLeft2Dand3DRight2D(bool)
// {
//   set Full Screen Mode to false, if Layout Design was changed by the LayoutDesign_List
//   m_FullScreenMode = false;
//   this->ChangeFullScreenIcon();
// 
//   m_LayoutDesign = LAYOUT_LEFT2DAND3DRIGHT2D;
//   emit LayoutDesignChanged( LAYOUT_LEFT2DAND3DRIGHT2D );
// 
//   DeferredShowMenu( );
// }
// 
// void MyQmitkRenderWindowMenu::OnChangeLayoutTo2DUp3DDown(bool)
// {
//   set Full Screen Mode to false, if Layout Design was changed by the LayoutDesign_List
//   m_FullScreenMode = false;
//   this->ChangeFullScreenIcon();
// 
//   m_LayoutDesign = LAYOUT_2D_UP_AND_3D_DOWN;
//   emit LayoutDesignChanged( LAYOUT_2D_UP_AND_3D_DOWN );
// 
//   DeferredShowMenu( );
// }
// 
// void MyQmitkRenderWindowMenu::OnChangeLayoutToLeftACandRightBC(bool)
// {
//   set Full Screen Mode to false, if Layout Design was changed by the LayoutDesign_List
//   m_FullScreenMode = false;
//   this->ChangeFullScreenIcon();
// 
//   m_LayoutDesign = LEFT_AC_AND_RIGHT_BC;
//   emit LayoutDesignChanged( LEFT_AC_AND_RIGHT_BC );
// 
//   DeferredShowMenu( );    
// }
// 
// void MyQmitkRenderWindowMenu::OnChangeLayoutToUpACandDownBC(bool)
// {
//   set Full Screen Mode to false, if Layout Design was changed by the LayoutDesign_List
//   m_FullScreenMode = false;
//   this->ChangeFullScreenIcon();
// 
//   m_LayoutDesign = AC_UP_AND_BC_DOWN;
//   emit LayoutDesignChanged( AC_UP_AND_BC_DOWN );
// 
//   DeferredShowMenu( );    
// 
// }

// void MyQmitkRenderWindowMenu::UpdateLayoutDesignList( int layoutDesignIndex )
// {
//     m_LayoutDesign = layoutDesignIndex;
//     
//     if( m_Settings == NULL )
//         this->CreateSettingsWidget();
//     
//     switch( m_LayoutDesign )
//     {
//         case LAYOUT_DEFAULT:
//         {
//             m_DefaultLayoutAction->setEnabled(false);
//             m_2DImagesUpLayoutAction->setEnabled(true);
//             m_2DImagesLeftLayoutAction->setEnabled(true);
//             m_Big3DLayoutAction->setEnabled(true);
//             m_Widget1LayoutAction->setEnabled(true);
//             m_Widget2LayoutAction->setEnabled(true);
//             m_Widget3LayoutAction->setEnabled(true);
//             m_RowWidget3And4LayoutAction->setEnabled(true);
//             m_ColumnWidget3And4LayoutAction->setEnabled(true);
//             m_SmallUpperWidget2Big3and4LayoutAction->setEnabled(true);
//             m_2x2Dand3DWidgetLayoutAction->setEnabled(true);
//             m_Left2Dand3DRight2DLayoutAction->setEnabled(true);
//             m_LeftACandRightBCLayoutAction->setEnabled(true);
//             m_UpACandDownBCLayoutAction->setEnabled(true);
//             break;
//         }
//         
//         case LAYOUT_2DIMAGEUP:
//         {
//             m_DefaultLayoutAction->setEnabled(true);
//             m_2DImagesUpLayoutAction->setEnabled(false);
//             m_2DImagesLeftLayoutAction->setEnabled(true);
//             m_Big3DLayoutAction->setEnabled(true);
//             m_Widget1LayoutAction->setEnabled(true);
//             m_Widget2LayoutAction->setEnabled(true);
//             m_Widget3LayoutAction->setEnabled(true);
//             m_RowWidget3And4LayoutAction->setEnabled(true);
//             m_ColumnWidget3And4LayoutAction->setEnabled(true);
//             m_SmallUpperWidget2Big3and4LayoutAction->setEnabled(true);
//             m_2x2Dand3DWidgetLayoutAction->setEnabled(true);
//             m_Left2Dand3DRight2DLayoutAction->setEnabled(true);
//             m_LeftACandRightBCLayoutAction->setEnabled(true);
//             m_UpACandDownBCLayoutAction->setEnabled(true);
//             break;
//         }
//         case LAYOUT_2DIMAGELEFT:
//         {
//             m_DefaultLayoutAction->setEnabled(true);
//             m_2DImagesUpLayoutAction->setEnabled(true);
//             m_2DImagesLeftLayoutAction->setEnabled(false);
//             m_Big3DLayoutAction->setEnabled(true);
//             m_Widget1LayoutAction->setEnabled(true);
//             m_Widget2LayoutAction->setEnabled(true);
//             m_Widget3LayoutAction->setEnabled(true);
//             m_RowWidget3And4LayoutAction->setEnabled(true);
//             m_ColumnWidget3And4LayoutAction->setEnabled(true);
//             m_SmallUpperWidget2Big3and4LayoutAction->setEnabled(true);
//             m_2x2Dand3DWidgetLayoutAction->setEnabled(true);
//             m_Left2Dand3DRight2DLayoutAction->setEnabled(true);
//             m_LeftACandRightBCLayoutAction->setEnabled(true);
//             m_UpACandDownBCLayoutAction->setEnabled(true);
//             break;
//         }
//         case LAYOUT_BIG3D:
//         {
//             m_DefaultLayoutAction->setEnabled(true);
//             m_2DImagesUpLayoutAction->setEnabled(true);
//             m_2DImagesLeftLayoutAction->setEnabled(true);
//             m_Big3DLayoutAction->setEnabled(false);
//             m_Widget1LayoutAction->setEnabled(true);
//             m_Widget2LayoutAction->setEnabled(true);
//             m_Widget3LayoutAction->setEnabled(true);
//             m_RowWidget3And4LayoutAction->setEnabled(true);
//             m_ColumnWidget3And4LayoutAction->setEnabled(true);
//             m_SmallUpperWidget2Big3and4LayoutAction->setEnabled(true);
//             m_2x2Dand3DWidgetLayoutAction->setEnabled(true);
//             m_Left2Dand3DRight2DLayoutAction->setEnabled(true);
//             m_LeftACandRightBCLayoutAction->setEnabled(true);
//             m_UpACandDownBCLayoutAction->setEnabled(true);
//             break;
//         }
//         case LAYOUT_AXIAL:
//         {
//             m_DefaultLayoutAction->setEnabled(true);
//             m_2DImagesUpLayoutAction->setEnabled(true);
//             m_2DImagesLeftLayoutAction->setEnabled(true);
//             m_Big3DLayoutAction->setEnabled(true);
//             m_Widget1LayoutAction->setEnabled(false);
//             m_Widget2LayoutAction->setEnabled(true);
//             m_Widget3LayoutAction->setEnabled(true);
//             m_RowWidget3And4LayoutAction->setEnabled(true);
//             m_ColumnWidget3And4LayoutAction->setEnabled(true);
//             m_SmallUpperWidget2Big3and4LayoutAction->setEnabled(true);
//             m_2x2Dand3DWidgetLayoutAction->setEnabled(true);
//             m_Left2Dand3DRight2DLayoutAction->setEnabled(true);
//             m_LeftACandRightBCLayoutAction->setEnabled(true);
//             m_UpACandDownBCLayoutAction->setEnabled(true);
//             break;
//         }
//         case LAYOUT_SAGITTAL:
//         {
//             m_DefaultLayoutAction->setEnabled(true);
//             m_2DImagesUpLayoutAction->setEnabled(true);
//             m_2DImagesLeftLayoutAction->setEnabled(true);
//             m_Big3DLayoutAction->setEnabled(true);
//             m_Widget1LayoutAction->setEnabled(true);
//             m_Widget2LayoutAction->setEnabled(false);
//             m_Widget3LayoutAction->setEnabled(true);
//             m_RowWidget3And4LayoutAction->setEnabled(true);
//             m_ColumnWidget3And4LayoutAction->setEnabled(true);
//             m_SmallUpperWidget2Big3and4LayoutAction->setEnabled(true);
//             m_2x2Dand3DWidgetLayoutAction->setEnabled(true);
//             m_Left2Dand3DRight2DLayoutAction->setEnabled(true);
//             m_LeftACandRightBCLayoutAction->setEnabled(true);
//             m_UpACandDownBCLayoutAction->setEnabled(true);
//             break;
//         }
//         case LAYOUT_CORONAL:
//         {
//             m_DefaultLayoutAction->setEnabled(true);
//             m_2DImagesUpLayoutAction->setEnabled(true);
//             m_2DImagesLeftLayoutAction->setEnabled(true);
//             m_Big3DLayoutAction->setEnabled(true);
//             m_Widget1LayoutAction->setEnabled(true);
//             m_Widget2LayoutAction->setEnabled(true);
//             m_Widget3LayoutAction->setEnabled(false);
//             m_RowWidget3And4LayoutAction->setEnabled(true);
//             m_ColumnWidget3And4LayoutAction->setEnabled(true);
//             m_SmallUpperWidget2Big3and4LayoutAction->setEnabled(true);
//             m_2x2Dand3DWidgetLayoutAction->setEnabled(true);
//             m_Left2Dand3DRight2DLayoutAction->setEnabled(true);
//             m_LeftACandRightBCLayoutAction->setEnabled(true);
//             m_UpACandDownBCLayoutAction->setEnabled(true);
//             break;
//         }
//         case LAYOUT_2X2DAND3DWIDGET:
//         {
//             m_DefaultLayoutAction->setEnabled(true);
//             m_2DImagesUpLayoutAction->setEnabled(true);
//             m_2DImagesLeftLayoutAction->setEnabled(true);
//             m_Big3DLayoutAction->setEnabled(true);
//             m_Widget1LayoutAction->setEnabled(true);
//             m_Widget2LayoutAction->setEnabled(true);
//             m_Widget3LayoutAction->setEnabled(true);
//             m_RowWidget3And4LayoutAction->setEnabled(true);
//             m_ColumnWidget3And4LayoutAction->setEnabled(true);
//             m_SmallUpperWidget2Big3and4LayoutAction->setEnabled(true);
//             m_2x2Dand3DWidgetLayoutAction->setEnabled(false);
//             m_Left2Dand3DRight2DLayoutAction->setEnabled(true);
//             m_LeftACandRightBCLayoutAction->setEnabled(true);
//             m_UpACandDownBCLayoutAction->setEnabled(true);
//             break;
//         }
//         case LAYOUT_ROWWIDGET3AND4:
//         {
//             m_DefaultLayoutAction->setEnabled(true);
//             m_2DImagesUpLayoutAction->setEnabled(true);
//             m_2DImagesLeftLayoutAction->setEnabled(true);
//             m_Big3DLayoutAction->setEnabled(true);
//             m_Widget1LayoutAction->setEnabled(true);
//             m_Widget2LayoutAction->setEnabled(true);
//             m_Widget3LayoutAction->setEnabled(true);
//             m_RowWidget3And4LayoutAction->setEnabled(false);
//             m_ColumnWidget3And4LayoutAction->setEnabled(true);
//             m_SmallUpperWidget2Big3and4LayoutAction->setEnabled(true);
//             m_2x2Dand3DWidgetLayoutAction->setEnabled(true);
//             m_Left2Dand3DRight2DLayoutAction->setEnabled(true);
//             m_LeftACandRightBCLayoutAction->setEnabled(true);
//             m_UpACandDownBCLayoutAction->setEnabled(true);
//             break;
//         }
//         case LAYOUT_COLUMNWIDGET3AND4:
//         {
//             m_DefaultLayoutAction->setEnabled(true);
//             m_2DImagesUpLayoutAction->setEnabled(true);
//             m_2DImagesLeftLayoutAction->setEnabled(true);
//             m_Big3DLayoutAction->setEnabled(true);
//             m_Widget1LayoutAction->setEnabled(true);
//             m_Widget2LayoutAction->setEnabled(true);
//             m_Widget3LayoutAction->setEnabled(true);
//             m_RowWidget3And4LayoutAction->setEnabled(true);
//             m_ColumnWidget3And4LayoutAction->setEnabled(false);
//             m_SmallUpperWidget2Big3and4LayoutAction->setEnabled(true);
//             m_2x2Dand3DWidgetLayoutAction->setEnabled(true);
//             m_Left2Dand3DRight2DLayoutAction->setEnabled(true);
//             m_LeftACandRightBCLayoutAction->setEnabled(true);
//             m_UpACandDownBCLayoutAction->setEnabled(true);
//             break;
//         }
//         case LAYOUT_SMALLUPPERWIDGET2BIGAND4:
//         {
//             m_DefaultLayoutAction->setEnabled(true);
//             m_2DImagesUpLayoutAction->setEnabled(true);
//             m_2DImagesLeftLayoutAction->setEnabled(true);
//             m_Big3DLayoutAction->setEnabled(true);
//             m_Widget1LayoutAction->setEnabled(true);
//             m_Widget2LayoutAction->setEnabled(true);
//             m_Widget3LayoutAction->setEnabled(true);
//             m_RowWidget3And4LayoutAction->setEnabled(true);
//             m_ColumnWidget3And4LayoutAction->setEnabled(true);
//             m_SmallUpperWidget2Big3and4LayoutAction->setEnabled(false);
//             m_2x2Dand3DWidgetLayoutAction->setEnabled(true);
//             m_Left2Dand3DRight2DLayoutAction->setEnabled(true);
//             m_LeftACandRightBCLayoutAction->setEnabled(true);
//             m_UpACandDownBCLayoutAction->setEnabled(true);
//             break;
//         }
//         case LAYOUT_LEFT2DAND3DRIGHT2D:
//         {
//             m_DefaultLayoutAction->setEnabled(true);
//             m_2DImagesUpLayoutAction->setEnabled(true);
//             m_2DImagesLeftLayoutAction->setEnabled(true);
//             m_Big3DLayoutAction->setEnabled(true);
//             m_Widget1LayoutAction->setEnabled(true);
//             m_Widget2LayoutAction->setEnabled(true);
//             m_Widget3LayoutAction->setEnabled(true);
//             m_RowWidget3And4LayoutAction->setEnabled(true);
//             m_ColumnWidget3And4LayoutAction->setEnabled(true);
//             m_SmallUpperWidget2Big3and4LayoutAction->setEnabled(true);
//             m_2x2Dand3DWidgetLayoutAction->setEnabled(true);
//             m_Left2Dand3DRight2DLayoutAction->setEnabled(false);
//             m_LeftACandRightBCLayoutAction->setEnabled(true);
//             m_UpACandDownBCLayoutAction->setEnabled(true);
//             break;
//         }
//         case LEFT_AC_AND_RIGHT_BC:
//         {
//             m_DefaultLayoutAction->setEnabled(true);
//             m_2DImagesUpLayoutAction->setEnabled(true);
//             m_2DImagesLeftLayoutAction->setEnabled(true);
//             m_Big3DLayoutAction->setEnabled(true);
//             m_Widget1LayoutAction->setEnabled(true);
//             m_Widget2LayoutAction->setEnabled(true);
//             m_Widget3LayoutAction->setEnabled(true);
//             m_RowWidget3And4LayoutAction->setEnabled(true);
//             m_ColumnWidget3And4LayoutAction->setEnabled(true);
//             m_SmallUpperWidget2Big3and4LayoutAction->setEnabled(true);
//             m_2x2Dand3DWidgetLayoutAction->setEnabled(true);
//             m_Left2Dand3DRight2DLayoutAction->setEnabled(true);
//             m_LeftACandRightBCLayoutAction->setEnabled(false);
//             m_UpACandDownBCLayoutAction->setEnabled(true);
//             break;
//         } 
//         case AC_UP_AND_BC_DOWN:
//         {
//             m_DefaultLayoutAction->setEnabled(true);
//             m_2DImagesUpLayoutAction->setEnabled(true);
//             m_2DImagesLeftLayoutAction->setEnabled(true);
//             m_Big3DLayoutAction->setEnabled(true);
//             m_Widget1LayoutAction->setEnabled(true);
//             m_Widget2LayoutAction->setEnabled(true);
//             m_Widget3LayoutAction->setEnabled(true);
//             m_RowWidget3And4LayoutAction->setEnabled(true);
//             m_ColumnWidget3And4LayoutAction->setEnabled(true);
//             m_SmallUpperWidget2Big3and4LayoutAction->setEnabled(true);
//             m_2x2Dand3DWidgetLayoutAction->setEnabled(true);
//             m_Left2Dand3DRight2DLayoutAction->setEnabled(true);
//             m_LeftACandRightBCLayoutAction->setEnabled(true);
//             m_UpACandDownBCLayoutAction->setEnabled(false);
//             break;
//         }     
//     }
// }

// void MyQmitkRenderWindowMenu::SetCrossHairVisibility( bool /*state*/ )
// {
//   if(m_Renderer.IsNotNull())
//   {
//     mitk::DataNode *n;
//     if(this->m_MultiWidget)
//     {
//       n = this->m_MultiWidget->GetWidgetPlane1(); if(n) n->SetVisibility(state);
//       n = this->m_MultiWidget->GetWidgetPlane2(); if(n) n->SetVisibility(state);
//       n = this->m_MultiWidget->GetWidgetPlane3(); if(n) n->SetVisibility(state);
//       m_Renderer->GetRenderingManager()->RequestUpdateAll();
//     }
//   }
// }

// void MyQmitkRenderWindowMenu::NotifyNewWidgetPlanesMode( int /*mode*/ )
// {
//   //currentCrosshairRotationMode = mode;
// }
