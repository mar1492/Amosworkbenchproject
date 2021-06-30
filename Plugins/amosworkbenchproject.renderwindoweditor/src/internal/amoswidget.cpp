#include "amoswidget.h"
#include <MyQmitkRenderWindowWidget.h>

// #include <QHBoxLayout>
// #include <QVBoxLayout>
// #include <QPalette>
// // #include <QGridLayout>
// 
// #include <vtkCornerAnnotation.h>
// #include <vtkMitkRectangleProp.h>
// #include <vtkCamera.h>
// 
// // #include <mitkGlobalInteraction.h>
// #include <mitkLabelSetImage.h>
// #include <mitkNodePredicateDataType.h>
// #include <mitkRenderingManager.h>
// #include <mitkGeometry3D.h>
// #include <mitkTimeGeometry.h>
// // #include <mitkPositionTracker.h>
// #include <mitkPointSet.h>
// // #include <mitkOverlayManager.h>
// #include <vtkTextProperty.h>
// #include <mitkVtkLayerController.h>
// #include <mitkCameraController.h>
// #include <vtkCornerAnnotation.h>
// #include <mitkPlaneGeometryDataMapper2D.h>
// #include <mitkImage.h>

 // mitk core
 #include <mitkCameraController.h>
 #include <mitkImage.h>
 #include <mitkImagePixelReadAccessor.h>
 #include <mitkInteractionConst.h>
 #include <mitkLine.h>
 #include <mitkNodePredicateBase.h>
 #include <mitkNodePredicateDataType.h>
 #include <mitkNodePredicateNot.h>
 #include <mitkNodePredicateProperty.h>
 #include <mitkPixelTypeMultiplex.h>
 #include <mitkPlaneGeometryDataMapper2D.h>
 #include <mitkPointSet.h>
 #include <mitkProperties.h>
 #include <mitkStatusBar.h>
 #include <mitkDisplayActionEventHandlerStd.h>
 #include <mitkVtkLayerController.h>
 
 // qt
 #include <QList>
 #include <QMouseEvent>
 #include <QTimer>
 
 // vtk
 #include <vtkSmartPointer.h>
 
 // c++
 #include <iomanip>

#include "utils.hpp"
#include "amositkimageio.h"

using namespace mitk;

// AmosWidget::AmosWidget(QWidget* parent, Qt::WindowFlags f,/* mitk::RenderingManager* renderingManager, mitk::BaseRenderer::RenderingMode::Type renderingMode,*/ const QString& name) : QWidget(parent, f, name)/*,
// m_RenderingManager(renderingManager),
// m_GradientBackgroundFlag(true)*/
AmosWidget::AmosWidget(QWidget* parent, Qt::WindowFlags f,const QString& name) : 
// JCAM
QmitkMxNMultiWidget(parent,f,name)
//QmitkAbstractMultiWidget(parent, f, name)
{
/*    if (m_RenderingManager == NULL) {
	    m_RenderingManager = mitk::RenderingManager::GetInstance();
    } */   
//     sliceNaviController = m_RenderingManager->GetTimeNavigationController();
	
//     layout = new QHBoxLayout(this);
//     layout->setContentsMargins(0,0,0,0);
//     this->setLayout(layout);
//     
//     splitter = new QSplitter( this );
//     layout->addWidget(splitter);
//     //   layoutUp->addWidget(splitter,1);
//     
//     splitterVert = new QSplitter( Qt::Vertical, splitter );
//     splitter->addWidget(splitterVert);  
//     
//      //create splitterUp and splitterDown
//     splitterUp = new QSplitter(splitterVert);
//     splitterDown = new QSplitter(splitterVert); 
//     
//      //creae Widget Container
//     renderWindowACContainer = new QWidget(splitterUp);
//     renderWindowBCContainer = new QWidget(splitterUp);
//     renderWindowPatientContainer = new QWidget(splitterDown);
//     renderWindow3DContainer = new QWidget(splitterDown);  
//     
//     renderWindowACContainer->setContentsMargins(0,0,0,0);
//     renderWindowBCContainer->setContentsMargins(0,0,0,0);
//     renderWindowPatientContainer->setContentsMargins(0,0,0,0);
//     renderWindow3DContainer->setContentsMargins(0,0,0,0);
//     
// /*    QPalette Pal(palette());
//     QColor backColor(100, 100, 100);
//     Pal.setColor(QPalette::Window, backColor);
//     emptyContainer->setAutoFillBackground(true);
//     emptyContainer->setPalette(Pal); */    
// 
//      //create Widget Layout
//     QHBoxLayout *renderACLayout = new QHBoxLayout(renderWindowACContainer);
//     QHBoxLayout *renderBCLayout = new QHBoxLayout(renderWindowBCContainer);
//     QHBoxLayout *renderPatientLayout = new QHBoxLayout(renderWindowPatientContainer);
//  //     emptyContainer->setSizePolicy(QSizePolicy ::Minimum , QSizePolicy ::Minimum);
//     QHBoxLayout *render3DLayout = new QHBoxLayout(renderWindow3DContainer); 
//     
//     m_CornerAnnotations[0] = vtkSmartPointer<vtkCornerAnnotation>::New();
//     m_CornerAnnotations[1] = vtkSmartPointer<vtkCornerAnnotation>::New();
//     m_CornerAnnotations[2] = vtkSmartPointer<vtkCornerAnnotation>::New();
//     m_CornerAnnotations[3] = vtkSmartPointer<vtkCornerAnnotation>::New();
// 
//     m_RectangleProps[0] = vtkSmartPointer<vtkMitkRectangleProp>::New();
//     m_RectangleProps[1] = vtkSmartPointer<vtkMitkRectangleProp>::New();
//     m_RectangleProps[2] = vtkSmartPointer<vtkMitkRectangleProp>::New();
//     m_RectangleProps[3] = vtkSmartPointer<vtkMitkRectangleProp>::New();
// 
//     renderACLayout->setMargin(0);
//     renderBCLayout->setMargin(0);
//     renderPatientLayout->setMargin(0);
//     render3DLayout->setMargin(0);  
//     
//      //set Layout to Widget Container
//     renderWindowACContainer->setLayout(renderACLayout);
//     renderWindowBCContainer->setLayout(renderBCLayout);
//     renderWindowPatientContainer->setLayout(renderPatientLayout);
// //     renderWindowPatientContainer->setAccessibleName(name);
//     renderWindow3DContainer->setLayout(render3DLayout);
// 
//     //set SizePolicy
//     renderWindowACContainer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
//     renderWindowBCContainer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
//     renderWindowPatientContainer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
//     renderWindow3DContainer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
// 
//     //insert Widget Container into the splitters
//     splitterUp->addWidget( renderWindowACContainer );
//     splitterUp->addWidget( renderWindowBCContainer );
// 
//     splitterDown->addWidget( renderWindowPatientContainer );
//     splitterDown->addWidget( renderWindow3DContainer );  
//        
// /*    layoutUp = new QHBoxLayout();
//     layout->addLayout(layoutUp);*/  
// 	
//     renderWindowAC = new MyQmitkRenderWindow(this, name + "WidgetAC", NULL, m_RenderingManager, renderingMode);
//     renderWindowAC->SetLayoutIndex(AXIAL);
//     renderACLayout->addWidget(renderWindowAC); 
// 
//     renderWindowBC = new MyQmitkRenderWindow(this, name + "WidgetBC", NULL, m_RenderingManager, renderingMode);
//     renderWindowBC->SetLayoutIndex(SAGITTAL);
//     renderBCLayout->addWidget(renderWindowBC); 
//     
//     renderWindowPatient = new MyQmitkRenderWindow(this, name + "WidgetPatient", NULL, m_RenderingManager, renderingMode);
//     renderWindowPatient->SetLayoutIndex(CORONAL);
//     renderPatientLayout->addWidget(renderWindowPatient);    
// 
//     renderWindow3D = new MyQmitkRenderWindow(this, name + "Widget3D", NULL, m_RenderingManager, renderingMode);
//     renderWindow3D->SetLayoutIndex(THREE_D);
//     render3DLayout->addWidget(renderWindow3D);  
//     
//     //create SignalSlot Connection
//     connect(renderWindowAC, SIGNAL(SignalLayoutDesignChanged(int)), this, SLOT(OnLayoutDesignChanged(int)));
//     connect(renderWindowAC, SIGNAL(ResetView()), this, SLOT(ResetViews()));
//     
//     connect(renderWindowBC, SIGNAL(SignalLayoutDesignChanged(int)), this, SLOT(OnLayoutDesignChanged(int)));
//     connect(renderWindowBC, SIGNAL(ResetView()), this, SLOT(ResetViews()));
//     
//     connect(renderWindowPatient, SIGNAL(SignalLayoutDesignChanged(int)), this, SLOT(OnLayoutDesignChanged(int)));
//     connect(renderWindowPatient, SIGNAL(ResetView()), this, SLOT(ResetViews()));
//     
//     connect(renderWindow3D, SIGNAL(SignalLayoutDesignChanged(int)), this, SLOT(OnLayoutDesignChanged(int)));
//     connect(renderWindow3D, SIGNAL(ResetView()), this, SLOT(ResetViews())); 
//     
//     //Create Level Window Widget
// //     levelWindowWidget = new MyMitkLevelWindowWidget(splitter); //this
//     levelWindowWidget = new QmitkLevelWindowWidget(splitter); //this    
//     levelWindowWidget->setObjectName(QString::fromUtf8("levelWindowWidget"));
//     QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
//     sizePolicy.setHorizontalStretch(0);
//     sizePolicy.setVerticalStretch(0);
//     sizePolicy.setHeightForWidth(levelWindowWidget->sizePolicy().hasHeightForWidth());
//     levelWindowWidget->setSizePolicy(sizePolicy);
//     levelWindowWidget->setMaximumWidth(50);
//     
// //     levelWindowWidgetBC = new MyMitkLevelWindowWidget(splitter);
// //     levelWindowWidget2->SetDataStorage(ds2);
// //     levelWindowWidgetBC->SetExternLevelWindowManager(levelWindowWidget);  
// 
//     //add LevelWindow Widget to mainSplitter
//     splitter->addWidget(levelWindowWidget);
// 
//     //show mainSplitt and add to Layout
//     splitter->show();
// 
//     //resize Image.
// //     this->resize( QSize(900, 900).expandedTo(minimumSizeHint()));
// 
//     //Initialize the widgets.
//     this->InitializeMultiWidget();
// 
//     //Activate Widget Menu
//     this->ActivateMenuWidget(true); 
// 
//     mitk::NodePredicateDataType::Pointer isDwi = mitk::NodePredicateDataType::New("DiffusionImage");
//     mitk::NodePredicateDataType::Pointer isDti = mitk::NodePredicateDataType::New("TensorImage");
//     mitk::NodePredicateDataType::Pointer isQbi = mitk::NodePredicateDataType::New("QBallImage");
//     mitk::NodePredicateOr::Pointer isDiffusionImage = mitk::NodePredicateOr::New(isDwi, isDti);
//     isDiffusionImage = mitk::NodePredicateOr::New(isDiffusionImage, isQbi);    
//     m_IsOfTypeImagePredicate = mitk::NodePredicateOr::New(isDiffusionImage, mitk::TNodePredicateDataType<mitk::Image>::New());
//     m_IsBinaryPredicate = mitk::NodePredicateProperty::New("binary", mitk::BoolProperty::New(true));
//     m_IsNotBinaryPredicate = mitk::NodePredicateNot::New( m_IsBinaryPredicate );
//     m_IsABinaryImagePredicate = mitk::NodePredicateAnd::New( m_IsOfTypeImagePredicate, m_IsBinaryPredicate);
//     m_IsNotABinaryImagePredicate = mitk::NodePredicateAnd::New( m_IsOfTypeImagePredicate, m_IsNotBinaryPredicate);
//     
//     m_IsASegmentationImagePredicate = mitk::NodePredicateOr::New(m_IsABinaryImagePredicate, mitk::TNodePredicateDataType<mitk::LabelSetImage>::New());
//     m_IsAPatientImagePredicate = mitk::NodePredicateAnd::New(m_IsNotABinaryImagePredicate, mitk::NodePredicateNot::New(mitk::TNodePredicateDataType<mitk::LabelSetImage>::New()));     
//     
//     m_SegmentationColor[0][0] = 1.0f;
//     m_SegmentationColor[0][1] = 0.0f;
//     m_SegmentationColor[0][2] = 0.0f;
//     m_SegmentationColor[1][0] = 0.0f;
//     m_SegmentationColor[1][1] = 1.0f;
//     m_SegmentationColor[1][2] = 0.0f;
    
}

AmosWidget::~AmosWidget()
{
// //     sliceNaviController->Disconnect(renderWindowAC->GetSliceNavigationController());
// //     sliceNaviController->Disconnect(renderWindowBC->GetSliceNavigationController());
// //     sliceNaviController->Disconnect(renderWindow3D->GetSliceNavigationController());
    
//     DisablePositionTracking();
// //     DisableNavigationControllerEventListening();

// //     levelWindowWidget->RemoveAllObservers();
// //     levelWindowWidgetBC->RemoveAllObservers(); 
    
    GetDataStorage()->AddNodeEvent.RemoveListener(mitk::MessageDelegate1<AmosWidget, const mitk::DataNode*>(this,&AmosWidget::onNodeAdded));
    GetDataStorage()->RemoveNodeEvent.RemoveListener(mitk::MessageDelegate1<AmosWidget, const mitk::DataNode*>(this,&AmosWidget::onNodeRemoved));    
}

void AmosWidget::InitializeMultiWidget()
{
    m_LayoutManager = new MyQmitkMultiWidgetLayoutManager(this);
//     m_PositionTracker = NULL;

//      //Make all black and overwrite renderwindow 3 and 4
//     this->FillGradientBackgroundWithBlack();
//      //This is #191919 in hex
//     float tmp1[3] = { 0.098f, 0.098f, 0.098f};
//      //This is #7F7F7F in hex
//     float tmp2[3] = { 0.498f, 0.498f, 0.498f};
//  //     m_GradientBackgroundColors[2] = std::make_pair(mitk::Color(tmp1), mitk::Color(tmp2));
//     m_GradientBackgroundColors[3] = std::make_pair(mitk::Color(tmp1), mitk::Color(tmp2));
    
    //Blue is default color for widgetPatient
/*    m_DecorationColorWidget3[0] = 0.0f;
    m_DecorationColorWidget3[1] = 0.502f;
    m_DecorationColorWidget3[2] = 1.0f; */   

    //Yellow is default color for widget3D
    m_DecorationColorWidget4[0] = 1.0f;
    m_DecorationColorWidget4[1] = 1.0f;
    m_DecorationColorWidget4[2] = 0.0f; 
    
    // JCAM
    // SetLayout(2, 3);
    MITK_INFO << "JCAM. Setting layout to 3 x 2" << endl;
    SetLayout(2, 3);
    
    // transfer colors in WorldGeometry-Nodes of the associated Renderer
    mitk::IntProperty::Pointer  layer;
    // of widget 1
    MyQmitkRenderWindow* AC = GetRenderWindowAC();
    // MITK 2021
//     vtkRenderWindow* ACR = AC->GetRenderWindow();
    vtkRenderWindow* ACR = AC->renderWindow();
    mitk::BaseRenderer* rend = mitk::BaseRenderer::GetInstance(ACR);
    m_PlaneNodeAC = rend->GetCurrentWorldPlaneGeometryNode();
//     m_PlaneNodeAC = mitk::BaseRenderer::GetInstance(GetRenderWindowAC()->GetRenderWindow())->GetCurrentWorldPlaneGeometryNode();
    m_PlaneNodeAC->SetColor(GetDecorationColor(0));
    layer = mitk::IntProperty::New(1000);
    m_PlaneNodeAC->SetProperty("layer",layer);

    // ... of widget 2
    // MITK 2021
//     m_PlaneNodeBC = mitk::BaseRenderer::GetInstance(GetRenderWindowBC()->GetRenderWindow())->GetCurrentWorldPlaneGeometryNode();
    m_PlaneNodeBC = mitk::BaseRenderer::GetInstance(GetRenderWindowBC()->renderWindow())->GetCurrentWorldPlaneGeometryNode();
    m_PlaneNodeBC->SetColor(GetDecorationColor(1));
    layer = mitk::IntProperty::New(1000);
    m_PlaneNodeBC->SetProperty("layer",layer);
    
    // ... of widget 3
    // MITK 2021
//     m_PlaneNodePatient = mitk::BaseRenderer::GetInstance(GetRenderWindowPatient()->GetRenderWindow())->GetCurrentWorldPlaneGeometryNode();
    m_PlaneNodePatient = mitk::BaseRenderer::GetInstance(GetRenderWindowPatient()->renderWindow())->GetCurrentWorldPlaneGeometryNode();
    m_PlaneNodePatient->SetColor(GetDecorationColor(2));
    layer = mitk::IntProperty::New(1000);
    m_PlaneNodePatient->SetProperty("layer",layer);    
    
    //The parent node
    // MITK 2021
//     m_ParentNodeForGeometryPlanes = mitk::BaseRenderer::GetInstance(GetRenderWindow3D()->GetRenderWindow())->GetCurrentWorldPlaneGeometryNode();
    m_ParentNodeForGeometryPlanes = mitk::BaseRenderer::GetInstance(GetRenderWindow3D()->renderWindow())->GetCurrentWorldPlaneGeometryNode();
    layer = mitk::IntProperty::New(1000);
    m_ParentNodeForGeometryPlanes->SetProperty("layer",layer);
    
    AddDisplayPlaneSubTree();
    
    SetDisplayActionEventHandler(std::make_unique<mitk::DisplayActionEventHandlerStd>());
 
    auto displayActionEventHandler = GetDisplayActionEventHandler();
    if (nullptr != displayActionEventHandler)
    {
        displayActionEventHandler->InitActions();
    }    

    // All lines below commented for 2018 realease
// //     mitk::OverlayManager::Pointer OverlayManager = mitk::OverlayManager::New();
//     mitk::BaseRenderer::GetInstance(GetRenderWindowAC()->GetRenderWindow())->SetOverlayManager(OverlayManager);
//     mitk::BaseRenderer::GetInstance(renderWindowBC->GetRenderWindow())->SetOverlayManager(OverlayManager);
//     mitk::BaseRenderer::GetInstance(renderWindowPatient->GetRenderWindow())->SetOverlayManager(OverlayManager);    
//     mitk::BaseRenderer::GetInstance(renderWindow3D->GetRenderWindow())->SetOverlayManager(OverlayManager); 
    
//     mitk::BaseRenderer::GetInstance(renderWindow3D->GetRenderWindow())->SetMapperID(mitk::BaseRenderer::Standard3D);
    
//      // Set default view directions for SNCs
//     GetRenderWindowAC->GetSliceNavigationController()->SetDefaultViewDirection(mitk::SliceNavigationController::Axial);
//     renderWindowBC->GetSliceNavigationController()->SetDefaultViewDirection(mitk::SliceNavigationController::Axial);
//     renderWindowPatient->GetSliceNavigationController()->SetDefaultViewDirection(mitk::SliceNavigationController::Axial);    
//     renderWindow3D->GetSliceNavigationController()->SetDefaultViewDirection(mitk::SliceNavigationController::Original);
    
//     SetDecorationProperties("Axial AC", GetDecorationColor(0), 0);
//     SetDecorationProperties("Axial BC", GetDecorationColor(1), 1);
//     SetDecorationProperties("Axial Patient", GetDecorationColor(2), 2);
//     SetDecorationProperties("3D Axial", GetDecorationColor(3), 3);

//    //connect to the "time navigation controller": send time via sliceNavigationControllers
/*/*    sliceNaviController->ConnectGeometrySliceEvent(renderWindowAC->GetRenderer()); 
    sliceNaviController->ConnectGeometrySliceEvent(renderWindowBC->GetRenderer()); 
    sliceNaviController->ConnectGeometrySliceEvent(renderWindow3D->GetRenderer());
    
    renderWindowAC->GetSliceNavigationController()->ConnectGeometrySliceEvent<>(sliceNaviController, false);
    renderWindowBC->GetSliceNavigationController()->ConnectGeometrySliceEvent(sliceNaviController, false);
    renderWindow3D->GetSliceNavigationController()->ConnectGeometrySliceEvent(sliceNaviController, false);  
    
    sliceNaviController->ConnectGeometryTimeEvent(renderWindowAC->GetSliceNavigationController() , false);
    sliceNaviController->ConnectGeometryTimeEvent(renderWindowBC->GetSliceNavigationController() , false);
    sliceNaviController->ConnectGeometryTimeEvent(renderWindow3D->GetSliceNavigationController() , false);
    renderWindowAC->GetSliceNavigationController()->ConnectGeometryTimeEvent(sliceNaviController, false);
    renderWindowBC->GetSliceNavigationController()->ConnectGeometryTimeEvent(sliceNaviController, false);
    renderWindow3D->GetSliceNavigationController()->ConnectGeometryTimeEvent(sliceNaviController, false);
    renderWindowAC->GetSliceNavigationController()->ConnectGeometrySendEvent(mitk::BaseRenderer::GetInstance(renderWindow3D->GetRenderWindow())); */ 
       
// //    sliceNaviController->GetSlice()->SetPos( sliceNaviController->GetSlice()->GetSteps() / 2 );*/ 

/*    GetRenderWindowAC->GetSliceNavigationController()->ConnectGeometrySliceEvent(renderWindowBC->GetSliceNavigationController(), false);
    GetRenderWindowAC->GetSliceNavigationController()->ConnectGeometrySliceEvent(renderWindowPatient->GetSliceNavigationController(), false); */   
// //    renderWindowAC->GetSliceNavigationController()->ConnectGeometrySliceEvent(renderWindow3D->GetSliceNavigationController(), false);
//     renderWindowBC->GetSliceNavigationController()->ConnectGeometrySliceEvent(renderWindowAC->GetSliceNavigationController(), false);
//     renderWindowPatient->GetSliceNavigationController()->ConnectGeometrySliceEvent(renderWindowAC->GetSliceNavigationController(), false);
    
//     m_MouseModeSwitcher = mitk::MouseModeSwitcher::New();
    
//   //  m_LastLeftClickPositionSupplier = mitk::CoordinateSupplier::New("navigation", NULL);
//  //   mitk::GlobalInteraction::GetInstance()->AddListener(m_LastLeftClickPositionSupplier);

    // //setup the department logo rendering
//     m_LogoRendering = mitk::LogoOverlay::New();
//     mitk::BaseRenderer::Pointer renderer3D = mitk::BaseRenderer::GetInstance(renderWindow3D->GetRenderWindow());
//     m_LogoRendering->SetOpacity(0.5);
//     mitk::Point2D offset;
//     offset.Fill(0.03);
//     m_LogoRendering->SetOffsetVector(offset);
//     m_LogoRendering->SetRelativeSize(0.15);
//     m_LogoRendering->SetCornerPosition(1);
//     m_LogoRendering->SetLogoImagePath("DefaultLogo");
//     renderer3D->GetOverlayManager()->AddOverlay(m_LogoRendering.GetPointer(),renderer3D);
    
/*    for(unsigned int i = 0; i < 4; ++i)
    {
	//if(i != 2) { 
	SetGradientBackgroundColorForRenderWindow(m_GradientBackgroundColors[i].first, m_GradientBackgroundColors[i].second, i);
	//}
    }*/  
}

QmitkRenderWindow* AmosWidget::GetRenderWindow(const QString& widgetName) const
{
    if ("axial" == widgetName)
    {
        return GetRenderWindowAC();
    }
    
    if ("axial2" == widgetName)
    {
        return GetRenderWindowBC();
    }
    
    if ("axial3" == widgetName)
    {
        return GetRenderWindowPatient();
    }
    
    if ("3d" == widgetName)
    {
        return GetRenderWindow3D();
    }
    
    return QmitkAbstractMultiWidget::GetRenderWindow(widgetName);
}

QmitkRenderWindow* AmosWidget::GetRenderWindow(const mitk::BaseRenderer::ViewDirection& viewDirection) const
{
    return GetRenderWindow(static_cast<unsigned int>(viewDirection));
}

QmitkRenderWindow* AmosWidget::GetRenderWindow(unsigned int number) const
{
    switch (number)
    {
        case 0:
            return GetRenderWindowAC();
        case 1:
            return GetRenderWindowBC();
        case 2:
            return GetRenderWindowPatient();
        case 3:
            return GetRenderWindow3D();
        default:
            MITK_ERROR << "Requested unknown render window";
            break;
    }
    
    return nullptr;
}

void AmosWidget::SetSelectedPosition(const mitk::Point3D& newPosition, const QString& /*widgetName*/)
{
    GetRenderWindowAC()->GetSliceNavigationController()->SelectSliceByPoint(newPosition);
    GetRenderWindowBC()->GetSliceNavigationController()->SelectSliceByPoint(newPosition);
    GetRenderWindowPatient()->GetSliceNavigationController()->SelectSliceByPoint(newPosition);
    
    RequestUpdateAll();
}

const mitk::Point3D AmosWidget::GetSelectedPosition(const QString& /*widgetName*/) const
{
//     const mitk::PlaneGeometry* plane1 = GetRenderWindow1()->GetSliceNavigationController()->GetCurrentPlaneGeometry();
//     const mitk::PlaneGeometry* plane2 = GetRenderWindow2()->GetSliceNavigationController()->GetCurrentPlaneGeometry();
//     const mitk::PlaneGeometry* plane3 = GetRenderWindow3()->GetSliceNavigationController()->GetCurrentPlaneGeometry();
//     
//     mitk::Line3D line;
//     if ((plane1 != nullptr) && (plane2 != nullptr)
//         && (plane1->IntersectionLine(plane2, line)))
//     {
//         mitk::Point3D point;
//         if ((plane3 != nullptr) && (plane3->IntersectionPoint(line, point)))
//         {
//             return point;
//         }
//     }
    
    return mitk::Point3D();
}

void AmosWidget::SetCrosshairVisibility(bool /*visible*/) 
{
}

bool AmosWidget::GetCrosshairVisibility() const
{
    return false;
}

void AmosWidget::ResetCrosshair() 
{
}

void AmosWidget::SetWidgetPlaneMode(int /*mode*/) 
{
}

// void AmosWidget::FillGradientBackgroundWithBlack()
// {
//     for(unsigned int i = 0; i < 4; ++i) {
// 	float black[3] = {0.0f, 0.0f, 0.0f};
// 	m_GradientBackgroundColors[i] = std::make_pair(mitk::Color(black), mitk::Color(black));
//     }    
// }

// void AmosWidget::SetGradientBackgroundColorForRenderWindow(const mitk::Color &upper, const mitk::Color &lower, unsigned int widgetNumber)
// {
//     if(widgetNumber > 3 /*|| widgetNumber == 2*/)
//     {
// 	MITK_ERROR << "Gradientbackground for unknown widget!";
// 	return;
//     }
//     m_GradientBackgroundColors[widgetNumber].first = upper;
//     m_GradientBackgroundColors[widgetNumber].second = lower;
//     vtkRenderer* renderer = GetRenderWindow(widgetNumber)->GetRenderer()->GetVtkRenderer();
//     renderer->SetBackground2(upper[0], upper[1], upper[2]);
//     renderer->SetBackground(lower[0], lower[1], lower[2]);
//     m_GradientBackgroundFlag = true; 
// }

// std::pair<mitk::Color, mitk::Color> AmosWidget::GetGradientColors(unsigned int widgetNumber)
// {
//   if(widgetNumber > 3 /*|| widgetNumber == 2*/)
//   {
//     MITK_ERROR << "Decoration color for unknown widget!";
//     float black[3] = { 0.0f, 0.0f, 0.0f};
//     return std::make_pair(mitk::Color(black), mitk::Color(black));
//   }
//   return m_GradientBackgroundColors[widgetNumber];
// }

// bool AmosWidget::GetGradientBackgroundFlag() const
// {
//   return m_GradientBackgroundFlag;
// }

void AmosWidget::SetDecorationColor(unsigned int widgetNumber, mitk::Color color)
{
  switch (widgetNumber) {
  case 0:
    if(m_PlaneNodeAC.IsNotNull())
    {
      m_PlaneNodeAC->SetColor(color);
    }
    break;
  case 1:
    if(m_PlaneNodeBC.IsNotNull())
    {
      m_PlaneNodeBC->SetColor(color);
    }
    break;
  case 2:
    if(m_PlaneNodePatient.IsNotNull())
    {
      m_PlaneNodePatient->SetColor(color);
    }
    break;    
  case 3:
    m_DecorationColorWidget4 = color;
    break;
  default:
    MITK_ERROR << "Decoration color for unknown widget!";
    break;
  }
}

mitk::Color AmosWidget::GetDecorationColor(unsigned int widgetNumber)
{
  //The implementation looks a bit messy here, but it avoids
  //synchronization of the color of the geometry nodes and an
  //internal member here.
  //Default colors were chosen for decent visibitliy.
  //Feel free to change your preferences in the workbench.
  float tmp[3] = {0.0f,0.0f,0.0f};
  switch (widgetNumber) {
  case 0:
  {
    if(m_PlaneNodeAC.IsNotNull())
    {
      if(m_PlaneNodeAC->GetColor(tmp))
      {
        return dynamic_cast<mitk::ColorProperty*>(
              m_PlaneNodeAC->GetProperty("color"))->GetColor();
      }
    }
    float red[3] = { 0.753f, 0.0f, 0.0f};//This is #C00000 in hex
    return mitk::Color(red);
  }
  case 1:
  {
    if(m_PlaneNodeBC.IsNotNull())
    {
      if(m_PlaneNodeBC->GetColor(tmp))
      {
        return dynamic_cast<mitk::ColorProperty*>(
              m_PlaneNodeBC->GetProperty("color"))->GetColor();
      }
    }
    float green[3] = { 0.0f, 0.69f, 0.0f};//This is #00B000 in hex
    return mitk::Color(green);
  }
  case 2:
  {
//     return m_DecorationColorWidget3;
    if(m_PlaneNodePatient.IsNotNull())
    {
      if(m_PlaneNodePatient->GetColor(tmp))
      {
        return dynamic_cast<mitk::ColorProperty*>(
              m_PlaneNodePatient->GetProperty("color"))->GetColor();
      }
    }
    float blue[3] = { 0.0f, 0.502f, 1.0f};//This is #0080FF in hex
    return mitk::Color(blue);      
  }
  case 3:
  {
    return m_DecorationColorWidget4;
  }
  default:
    MITK_ERROR << "Decoration color for unknown widget!";
    float black[3] = { 0.0f, 0.0f, 0.0f};
    return mitk::Color(black);
  }
}

void AmosWidget::SetSegmentationColor(unsigned int widgetNumber, mitk::Color color)
{
  switch (widgetNumber) {
  case 0:
    m_SegmentationColor[0] = color;        
    changeSegmentationColor(widgetNumber);
//     m_RenderingManager->RequestUpdate(renderWindowAC->GetRenderWindow());
    break;
  case 1:
    m_SegmentationColor[1] = color;
    changeSegmentationColor(widgetNumber);
//     m_RenderingManager->RequestUpdate(renderWindowBC->GetRenderWindow());
    break;

  default:
    MITK_ERROR << "Segmentation color for unknown widget!";
    break;
  }
}

mitk::Color AmosWidget::GetSegmentationColor(unsigned int widgetNumber)
{
  //The implementation looks a bit messy here, but it avoids
  //synchronization of the color of the geometry nodes and an
  //internal member here.
  //Default colors were chosen for decent visibitliy.
  //Feel free to change your preferences in the workbench.
    
/*  DataStorage::SetOfObjects::ConstPointer binariesImg = ds->GetSubset(m_IsASegmentationImagePredicate);
  std::string typeSeg;
  bool found = false;*/  
  switch (widgetNumber) {
  case 0:
  { 
    float AcColor[3] = {m_SegmentationColor[0][0], m_SegmentationColor[0][1], m_SegmentationColor[0][2]};
//     for(DataStorage::SetOfObjects::ConstIterator it = binariesImg->Begin(); it != binariesImg->End();  ++it) {
// 	DataNode::Pointer node = it.Value();
// 	if(node->GetStringProperty("SegmentationType", typeSeg)) {
// 	    if(!typeSeg.compare("Amos_AC")) {
// 		node->GetColor(AcColor);
// 		found = true;
// 		break;
// 	    }
// 	}
//     }
//     if(!found) {   
// 	AcColor[0] = m_SegmentationColor[0][0];
// 	AcColor[1] = m_SegmentationColor[0][1];
// 	AcColor[2] = m_SegmentationColor[0][2];
//     }
    return mitk::Color(AcColor);
  }
  case 1:
  {
    float BcColor[3] = {m_SegmentationColor[1][0], m_SegmentationColor[1][1], m_SegmentationColor[1][2]};
//     bool found = false;
//     for(DataStorage::SetOfObjects::ConstIterator it = binariesImg->Begin(); it != binariesImg->End();  ++it) {
// 	DataNode::Pointer node = it.Value();
// 	if(node->GetStringProperty("SegmentationType", typeSeg)) {
// 	    if(!typeSeg.compare("Amos_BC")) {
// 		node->GetColor(BcColor);
// 		found = true;
// 		break;
// 	    }
// 	}
//     }
//     if(!found) {   
// 	BcColor[0] = m_SegmentationColor[1][0];
// 	BcColor[1] = m_SegmentationColor[1][1];
// 	BcColor[2] = m_SegmentationColor[1][2];
//     }
    return mitk::Color(BcColor);
  }
  default:
    MITK_ERROR << "Segmentation color for unknown widget!";
    float red[3] = { 1.0f, 0.0f, 0.0f};
    return mitk::Color(red);
  }
}

void AmosWidget::changeSegmentationColor(int widgetNum)
{
    Utils::Segmentation_Type amosType;
    if(widgetNum == 0)
        amosType = Utils::Amos_AC;
    else if(widgetNum == 1)
        amosType = Utils::Amos_BC;
    else
        return;
    auto dataStorage = GetDataStorage();
    DataStorage::SetOfObjects::ConstPointer binariesImg = dataStorage->GetSubset(m_IsASegmentationImagePredicate);
    for(DataStorage::SetOfObjects::ConstIterator it = binariesImg->Begin(); it != binariesImg->End(); ++it) {
	DataNode::Pointer nodeBin = it.Value();
	Utils::Segmentation_Type amosTypeNode = Utils::AmosSegmentationType(nodeBin);
	if(amosType == amosTypeNode) {
	    nodeBin->SetProperty("color", ColorProperty::New(m_SegmentationColor[widgetNum][0], m_SegmentationColor[widgetNum][1], m_SegmentationColor[widgetNum][2]));
	    nodeBin->SetProperty("binaryimage.selectedcolor", ColorProperty::New(m_SegmentationColor[widgetNum][0], m_SegmentationColor[widgetNum][1], m_SegmentationColor[widgetNum][2]));	
	    emit sendSegmentationNodeAdded(nodeBin);
	}
	else if(amosType != Utils::Non_Amos)
	    emit sendSegmentationNodeAdded(nodeBin);
    }
}

// void AmosWidget::SetDecorationProperties(std::string text, mitk::Color color, int widgetNumber)
// {
//     if( widgetNumber > 3 /*|| widgetNumber == 2*/) {
// 	MITK_ERROR << "Unknown render window for annotation.";
// 	return;
//     }
//     vtkRenderer* renderer = this->GetRenderWindow(widgetNumber)->GetRenderer()->GetVtkRenderer();
//     if(!renderer)
// 	return;
//     vtkSmartPointer<vtkCornerAnnotation> annotation = m_CornerAnnotations[widgetNumber];
//     annotation->SetText(0, text.c_str());
//     annotation->SetMaximumFontSize(12);
//     annotation->GetTextProperty()->SetColor(color[0],color[1],color[2]);
//     if(!renderer->HasViewProp(annotation)) {
// 	renderer->AddViewProp(annotation);
//     }
//     vtkSmartPointer<vtkMitkRectangleProp> frame = m_RectangleProps[widgetNumber];
//     frame->SetColor(color[0],color[1],color[2]);
//     if(!renderer->HasViewProp(frame)) {
// 	renderer->AddViewProp(frame);
//     }    
// }

// QmitkRenderWindow* AmosWidget::GetRenderWindow(unsigned int number)
// {
//     switch (number) {
//     case 0:
// 	return this->renderWindowAC;
//     case 1:
// 	return this->renderWindowBC;
//     case 2:
// 	return this->renderWindowPatient;
// // 	MITK_ERROR << "Requested unknown render window";
// // 	break;
//     case 3:
// 	return this->renderWindow3D;
//     default:
// 	MITK_ERROR << "Requested unknown render window";
// 	break;
//     }
//     return NULL;
// }

// std::string AmosWidget::GetCornerAnnotationText(unsigned int widgetNumber)
// {
//   if(widgetNumber > 3 /*|| widgetNumber == 2*/)
//   {
//     MITK_ERROR << "Annotation text for unknown widget!";
//     return std::string("");
//   }
//   return std::string(m_CornerAnnotations[widgetNumber]->GetText(0));
// }

// void AmosWidget::EnablePositionTracking()
// {
//     if (!m_PositionTracker) {
// 	m_PositionTracker = mitk::PositionTracker::New("PositionTracker", NULL);
//     }
//     mitk::GlobalInteraction* globalInteraction = mitk::GlobalInteraction::GetInstance();
//     if (globalInteraction) {
// 	if(ds.IsNotNull())
// 	    ds->Add(m_PositionTrackerNode);
// 	
// /*	if(dsAC.IsNotNull())
// 	    dsAC->Add(m_PositionTrackerNode);
// 	if(dsBC.IsNotNull())
// 	    dsBC->Add(m_PositionTrackerNode);*/	
// 	globalInteraction->AddListener(m_PositionTracker);
//     }
// }
// 
// void AmosWidget::DisablePositionTracking()
// {
//   mitk::GlobalInteraction* globalInteraction = mitk::GlobalInteraction::GetInstance();
// 
//   if(globalInteraction) {
// 	if (ds.IsNotNull())
// 	    ds->Remove(m_PositionTrackerNode);      
// 
// /*	if (dsAC.IsNotNull())
// 	    dsAC->Remove(m_PositionTrackerNode);
// 	if (dsBC.IsNotNull())
// 	    dsBC->Remove(m_PositionTrackerNode);*/	
// 	globalInteraction->RemoveListener(m_PositionTracker);
//   }
// }

void AmosWidget::EnsureDisplayContainsPoint(
  mitk::BaseRenderer* renderer, const mitk::Point3D& p)
{
  mitk::Point2D pointOnDisplay;
  renderer->WorldToDisplay(p,pointOnDisplay);

  if(pointOnDisplay[0] < renderer->GetVtkRenderer()->GetOrigin()[0]
     || pointOnDisplay[1] < renderer->GetVtkRenderer()->GetOrigin()[1]
     || pointOnDisplay[0] > renderer->GetVtkRenderer()->GetOrigin()[0]+renderer->GetViewportSize()[0]
     || pointOnDisplay[1] > renderer->GetVtkRenderer()->GetOrigin()[1]+renderer->GetViewportSize()[1])
  {
    mitk::Point2D pointOnPlane;
    renderer->GetCurrentWorldPlaneGeometry()->Map(p,pointOnPlane);
    renderer->GetCameraController()->MoveCameraToPoint(pointOnPlane);
  }
}

// void AmosWidget::EnableNavigationControllerEventListening()
// {
//     // Let NavigationControllers listen to GlobalInteraction
//     
//     // Enabling this a problem happen when clicking in AC or BC windows
//     // slice change from this_slice to last_slice - this_slice
//     mitk::GlobalInteraction *gi = mitk::GlobalInteraction::GetInstance();
// // 
//     gi->AddListener(renderWindowAC->GetSliceNavigationController());
//     gi->AddListener(renderWindowBC->GetSliceNavigationController());
//     gi->AddListener(renderWindowPatient->GetSliceNavigationController());
//     gi->AddListener(renderWindow3D->GetSliceNavigationController());
// // // 
// //     gi->AddListener(sliceNaviController);
// }

// void AmosWidget::DisableNavigationControllerEventListening()
// {
//     // Do not let NavigationControllers listen to GlobalInteraction
//     mitk::GlobalInteraction *gi = mitk::GlobalInteraction::GetInstance();
// 
//     gi->RemoveListener(renderWindowAC->GetSliceNavigationController());
//     gi->RemoveListener(renderWindowBC->GetSliceNavigationController());
//     gi->RemoveListener(renderWindowPatient->GetSliceNavigationController());
//     gi->RemoveListener(renderWindow3D->GetSliceNavigationController());
// //     
// //     gi->RemoveListener(sliceNaviController);
// }

void AmosWidget::SetWidgetPlaneVisibility(const char* widgetName, bool visible, mitk::BaseRenderer *renderer)
{
    auto dataStorage = GetDataStorage();
    if (dataStorage != nullptr)
    {
        mitk::DataNode* n = dataStorage->GetNamedNode(widgetName);
        if (n != nullptr)
            n->SetVisibility(visible, renderer);
    }
}

void AmosWidget::SetWidgetPlanesVisibility(bool visible, mitk::BaseRenderer *renderer)
{
    if (m_PlaneNodeAC.IsNotNull())
    {
        m_PlaneNodeAC->SetVisibility(visible, renderer);
    }
    if (m_PlaneNodeBC.IsNotNull())
    {
        m_PlaneNodeBC->SetVisibility(visible, renderer);
    }
    if (m_PlaneNodePatient.IsNotNull())
    {
        m_PlaneNodePatient->SetVisibility(visible, renderer);
    }
    //   m_RenderingManager->RequestUpdateAll();
    mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

// void AmosWidget::MouseModeSelected( mitk::MouseModeSwitcher::MouseMode mouseMode)
// {
//   if ( mouseMode == 0 )
//   {
//     this->EnableNavigationControllerEventListening();
//   }
//   else
//   {
//     this->DisableNavigationControllerEventListening();
//   }
// }

void AmosWidget::RemovePlanesFromDataStorage()
{
    
    auto dataStorage = GetDataStorage();
    if (nullptr == dataStorage)
    {
        return;
    }
    
    if (m_PlaneNodeAC.IsNotNull() && m_PlaneNodeBC.IsNotNull() && m_PlaneNodePatient.IsNotNull() && m_ParentNodeForGeometryPlanes.IsNotNull()) {
        dataStorage->Remove(m_PlaneNodeAC);
        dataStorage->Remove(m_PlaneNodeBC);
        dataStorage->Remove(m_PlaneNodePatient);
        dataStorage->Remove(m_ParentNodeForGeometryPlanes);     
        /*    if(dsAC.IsNotNull()) {
         *	dsAC->Remove(m_PlaneNodeAC);
         *	dsAC->Remove(m_ParentNodeForGeometryPlanes);
    }
    if(dsBC.IsNotNull()) {
        dsBC->Remove(m_PlaneNodeBC);
        dsBC->Remove(m_ParentNodeForGeometryPlanes);// Comprobe
    }*/    
    }
}

void AmosWidget::SetLayoutImpl()
{
    CreateRenderWindowWidgets();
    GetMultiWidgetLayoutManager()->SetLayoutDesign(QmitkMultiWidgetLayoutManager::LayoutDesign::DEFAULT);
    
    // Initialize views as axial, sagittal, coronal to all data objects in DataStorage
    auto geo = GetDataStorage()->ComputeBoundingGeometry3D(GetDataStorage()->GetAll());
    mitk::RenderingManager::GetInstance()->InitializeViews(geo);
}

void AmosWidget::CreateRenderWindowWidgets()
{
    // create axial render window (widget)
    using RenderWindowWidgetPointer = std::shared_ptr<MyQmitkRenderWindowWidget>;
    QString renderWindowWidgetName = GetNameFromIndex(0, 0);
    RenderWindowWidgetPointer renderWindowWidget1 = std::make_shared<MyQmitkRenderWindowWidget>(this, renderWindowWidgetName, GetDataStorage());
    auto renderWindow1 = renderWindowWidget1->GetRenderWindow();   
    renderWindow1->GetSliceNavigationController()->SetDefaultViewDirection(mitk::SliceNavigationController::Axial);
    renderWindowWidget1->SetDecorationColor(GetDecorationColor(0));
    renderWindowWidget1->SetCornerAnnotationText("Axial AC");
    renderWindowWidget1->GetRenderWindow()->SetLayoutIndex(ViewDirection::AXIAL);
    AddRenderWindowWidget(renderWindowWidgetName, std::dynamic_pointer_cast<QmitkRenderWindowWidget>(renderWindowWidget1));
    
    // create sagittal render window (widget)
    renderWindowWidgetName = GetNameFromIndex(0, 1);
    RenderWindowWidgetPointer renderWindowWidget2 = std::make_shared<MyQmitkRenderWindowWidget>(this, renderWindowWidgetName, GetDataStorage());
    auto renderWindow2 = renderWindowWidget2->GetRenderWindow();
    renderWindow2->GetSliceNavigationController()->SetDefaultViewDirection(mitk::SliceNavigationController::Axial); //Sagittal
    renderWindowWidget2->SetDecorationColor(GetDecorationColor(1));
    renderWindowWidget2->setStyleSheet("border: 0px");
    renderWindowWidget2->SetCornerAnnotationText("Axial BC");
    renderWindowWidget2->GetRenderWindow()->SetLayoutIndex(ViewDirection::AXIAL); // SAGITAL
    AddRenderWindowWidget(renderWindowWidgetName, std::dynamic_pointer_cast<QmitkRenderWindowWidget>(renderWindowWidget2));
    
    // create coronal render window (widget)
    renderWindowWidgetName = GetNameFromIndex(1, 0);
    RenderWindowWidgetPointer renderWindowWidget3 = std::make_shared<MyQmitkRenderWindowWidget>(this, renderWindowWidgetName, GetDataStorage());
    auto renderWindow3 = renderWindowWidget3->GetRenderWindow();
    renderWindow3->GetSliceNavigationController()->SetDefaultViewDirection(mitk::SliceNavigationController::Axial); //Frontal
    renderWindowWidget3->SetDecorationColor(GetDecorationColor(2));
    renderWindowWidget3->SetCornerAnnotationText("Axial Patient");
    renderWindowWidget3->GetRenderWindow()->SetLayoutIndex(ViewDirection::AXIAL);
    AddRenderWindowWidget(renderWindowWidgetName, std::dynamic_pointer_cast<QmitkRenderWindowWidget>(renderWindowWidget3));
    
    // create 3D render window (widget)
    renderWindowWidgetName = GetNameFromIndex(1, 1);
    RenderWindowWidgetPointer renderWindowWidget4 = std::make_shared<MyQmitkRenderWindowWidget>(this, renderWindowWidgetName, GetDataStorage());
    auto renderWindow4 = renderWindowWidget4->GetRenderWindow();
    renderWindow4->GetSliceNavigationController()->SetDefaultViewDirection(mitk::SliceNavigationController::Original);
    renderWindowWidget4->SetDecorationColor(GetDecorationColor(3));
    renderWindowWidget4->SetCornerAnnotationText("3D Axial");
    renderWindowWidget4->GetRenderWindow()->SetLayoutIndex(ViewDirection::THREE_D);
    // MITK 2021
//     mitk::BaseRenderer::GetInstance(renderWindowWidget4->GetRenderWindow()->GetRenderWindow())->SetMapperID(mitk::BaseRenderer::Standard3D);
    mitk::BaseRenderer::GetInstance(renderWindowWidget4->GetRenderWindow()->renderWindow())->SetMapperID(mitk::BaseRenderer::Standard3D);
    AddRenderWindowWidget(renderWindowWidgetName, std::dynamic_pointer_cast<QmitkRenderWindowWidget>(renderWindowWidget4));
    
    SetActiveRenderWindowWidget(renderWindowWidget1);
    
    // connect to the "time navigation controller": send time via sliceNavigationControllers
//     m_TimeNavigationController->ConnectGeometryTimeEvent(renderWindow1->GetSliceNavigationController(), false);
//     m_TimeNavigationController->ConnectGeometryTimeEvent(renderWindow2->GetSliceNavigationController(), false);
//     m_TimeNavigationController->ConnectGeometryTimeEvent(renderWindow3->GetSliceNavigationController(), false);
//     m_TimeNavigationController->ConnectGeometryTimeEvent(renderWindow4->GetSliceNavigationController(), false);
    // MITK 2021
//     renderWindow1->GetSliceNavigationController()->ConnectGeometrySendEvent(
//         mitk::BaseRenderer::GetInstance(renderWindow4->GetRenderWindow()));
    renderWindow1->GetSliceNavigationController()->ConnectGeometrySendEvent(
        mitk::BaseRenderer::GetInstance(renderWindow4->renderWindow()));
    
    // reverse connection between sliceNavigationControllers and timeNavigationController
//     renderWindow1->GetSliceNavigationController()->ConnectGeometryTimeEvent(m_TimeNavigationController, false);
//     renderWindow2->GetSliceNavigationController()->ConnectGeometryTimeEvent(m_TimeNavigationController, false);
//     renderWindow3->GetSliceNavigationController()->ConnectGeometryTimeEvent(m_TimeNavigationController, false);
    //renderWindow4->GetSliceNavigationController()->ConnectGeometryTimeEvent(m_TimeNavigationController, false);
    
    auto layoutManager = GetMultiWidgetLayoutManager();
    connect(dynamic_cast<MyQmitkRenderWindowWidget*>(renderWindowWidget1.get()), &MyQmitkRenderWindowWidget::MouseEvent, this, &AmosWidget::mousePressEvent);
//     connect(renderWindow1, &QmitkRenderWindow::ResetView, this, &QmitkStdMultiWidget::ResetCrosshair);
//     connect(renderWindow1, &QmitkRenderWindow::CrosshairVisibilityChanged, this, &QmitkStdMultiWidget::SetCrosshairVisibility);
//     connect(renderWindow1, &QmitkRenderWindow::CrosshairRotationModeChanged, this, &QmitkStdMultiWidget::SetWidgetPlaneMode);
    connect(renderWindow1, &QmitkRenderWindow::LayoutDesignChanged, layoutManager, &QmitkMultiWidgetLayoutManager::SetLayoutDesign);
    
    connect(dynamic_cast<MyQmitkRenderWindowWidget*>(renderWindowWidget2.get()), &MyQmitkRenderWindowWidget::MouseEvent, this, &AmosWidget::mousePressEvent);
//     connect(renderWindow2, &QmitkRenderWindow::ResetView, this, &QmitkStdMultiWidget::ResetCrosshair);
//     connect(renderWindow2, &QmitkRenderWindow::CrosshairVisibilityChanged, this, &QmitkStdMultiWidget::SetCrosshairVisibility);
//     connect(renderWindow2, &QmitkRenderWindow::CrosshairRotationModeChanged, this, &QmitkStdMultiWidget::SetWidgetPlaneMode);
    connect(renderWindow2, &QmitkRenderWindow::LayoutDesignChanged, layoutManager, &QmitkMultiWidgetLayoutManager::SetLayoutDesign);
    
    connect(dynamic_cast<MyQmitkRenderWindowWidget*>(renderWindowWidget3.get()), &MyQmitkRenderWindowWidget::MouseEvent, this, &AmosWidget::mousePressEvent);
//     connect(renderWindow3, &QmitkRenderWindow::ResetView, this, &QmitkStdMultiWidget::ResetCrosshair);
//     connect(renderWindow3, &QmitkRenderWindow::CrosshairVisibilityChanged, this, &QmitkStdMultiWidget::SetCrosshairVisibility);
//     connect(renderWindow3, &QmitkRenderWindow::CrosshairRotationModeChanged, this, &QmitkStdMultiWidget::SetWidgetPlaneMode);
    connect(renderWindow3, &QmitkRenderWindow::LayoutDesignChanged, layoutManager, &QmitkMultiWidgetLayoutManager::SetLayoutDesign);
    
    connect(dynamic_cast<MyQmitkRenderWindowWidget*>(renderWindowWidget4.get()), &MyQmitkRenderWindowWidget::MouseEvent, this, &AmosWidget::mousePressEvent);
//     connect(renderWindow4, &QmitkRenderWindow::ResetView, this, &QmitkStdMultiWidget::ResetCrosshair);
//     connect(renderWindow4, &QmitkRenderWindow::CrosshairVisibilityChanged, this, &QmitkStdMultiWidget::SetCrosshairVisibility);
//     connect(renderWindow4, &QmitkRenderWindow::CrosshairRotationModeChanged, this, &QmitkStdMultiWidget::SetWidgetPlaneMode);
    connect(renderWindow4, &QmitkRenderWindow::LayoutDesignChanged, layoutManager, &QmitkMultiWidgetLayoutManager::SetLayoutDesign);
    
    renderWindow1->GetSliceNavigationController()->ConnectGeometrySliceEvent(renderWindow2->GetSliceNavigationController(), false);
    renderWindow1->GetSliceNavigationController()->ConnectGeometrySliceEvent(renderWindow3->GetSliceNavigationController(), false);    
//     renderWindowAC->GetSliceNavigationController()->ConnectGeometrySliceEvent(renderWindow3D->GetSliceNavigationController(), false);
    renderWindow2->GetSliceNavigationController()->ConnectGeometrySliceEvent(renderWindow1->GetSliceNavigationController(), false);
    renderWindow3->GetSliceNavigationController()->ConnectGeometrySliceEvent(renderWindow1->GetSliceNavigationController(), false);    
}

void AmosWidget::AddPlanesToDataStorage()
{
    auto dataStorage = GetDataStorage();
    if (nullptr == dataStorage)
    {
        return;
    }    
    if (m_PlaneNodeAC.IsNotNull() && m_PlaneNodeBC.IsNotNull() && m_PlaneNodePatient.IsNotNull() && m_ParentNodeForGeometryPlanes.IsNotNull()) {
        dataStorage->Add(m_ParentNodeForGeometryPlanes);
        dataStorage->Add(m_PlaneNodeAC, m_ParentNodeForGeometryPlanes);
        dataStorage->Add(m_PlaneNodeBC, m_ParentNodeForGeometryPlanes);
        dataStorage->Add(m_PlaneNodePatient, m_ParentNodeForGeometryPlanes);	      
        /*    if(dsAC.IsNotNull()) {
         d sA*C->Add(m_PlaneNodeAC);
         dsAC->Add(m_ParentNodeForGeometryPlanes);
    }
    if(dsBC.IsNotNull()) {
        dsBC->Add(m_PlaneNodeBC);
        dsBC->Add(m_ParentNodeForGeometryPlanes);// Comprobe
    }*/    
    }  
    // JCAM
    if (m_Plane2AxialSegments.IsNotNull() && m_Plane2SagitalSegments.IsNotNull() && m_ParentNodeForGeometryPlanes.IsNotNull())
    {
        dataStorage->Add(m_Plane2AxialSegments, m_ParentNodeForGeometryPlanes);
        dataStorage->Add(m_Plane2SagitalSegments, m_ParentNodeForGeometryPlanes);
    }
}

void AmosWidget::Fit()
{
    vtkSmartPointer<vtkRenderer> vtkrenderer;
    // MITK 2021
//     vtkrenderer = mitk::BaseRenderer::GetInstance(GetRenderWindowAC()->GetRenderWindow())->GetVtkRenderer();
    vtkrenderer = mitk::BaseRenderer::GetInstance(GetRenderWindowAC()->renderWindow())->GetVtkRenderer();
    if ( vtkrenderer!= nullptr )
        vtkrenderer->ResetCamera();
    
    // MITK 2021
//     vtkrenderer = mitk::BaseRenderer::GetInstance(GetRenderWindowBC()->GetRenderWindow())->GetVtkRenderer();
    vtkrenderer = mitk::BaseRenderer::GetInstance(GetRenderWindowBC()->renderWindow())->GetVtkRenderer();
    if ( vtkrenderer!= nullptr )
        vtkrenderer->ResetCamera();
    
    // MITK 2021
//     vtkrenderer = mitk::BaseRenderer::GetInstance(GetRenderWindowPatient()->GetRenderWindow())->GetVtkRenderer();
    vtkrenderer = mitk::BaseRenderer::GetInstance(GetRenderWindowPatient()->renderWindow())->GetVtkRenderer();
    if ( vtkrenderer!= nullptr )
        vtkrenderer->ResetCamera(); 
    
    // MITK 2021
//     vtkrenderer = mitk::BaseRenderer::GetInstance(GetRenderWindow3D()->GetRenderWindow())->GetVtkRenderer();
    vtkrenderer = mitk::BaseRenderer::GetInstance(GetRenderWindow3D()->renderWindow())->GetVtkRenderer();
    if ( vtkrenderer!= nullptr )
        vtkrenderer->ResetCamera();    
    
    // MITK 2021
//     mitk::BaseRenderer::GetInstance(GetRenderWindowAC()->GetRenderWindow())->GetCameraController()->Fit();
//     mitk::BaseRenderer::GetInstance(GetRenderWindowBC()->GetRenderWindow())->GetCameraController()->Fit();
//     mitk::BaseRenderer::GetInstance(GetRenderWindowPatient()->GetRenderWindow())->GetCameraController()->Fit();    
//     mitk::BaseRenderer::GetInstance(GetRenderWindow3D()->GetRenderWindow())->GetCameraController()->Fit();
    mitk::BaseRenderer::GetInstance(GetRenderWindowAC()->renderWindow())->GetCameraController()->Fit();
    mitk::BaseRenderer::GetInstance(GetRenderWindowBC()->renderWindow())->GetCameraController()->Fit();
    mitk::BaseRenderer::GetInstance(GetRenderWindowPatient()->renderWindow())->GetCameraController()->Fit();    
    mitk::BaseRenderer::GetInstance(GetRenderWindow3D()->renderWindow())->GetCameraController()->Fit();

    
    int w = vtkObject::GetGlobalWarningDisplay();
    vtkObject::GlobalWarningDisplayOff();
    
    vtkObject::SetGlobalWarningDisplay(w);
}

// void AmosWidget::InitPositionTracking()
// {
    //PoinSetNode for MouseOrientation
//     m_PositionTrackerNode = mitk::DataNode::New();
//     m_PositionTrackerNode->SetProperty("name", mitk::StringProperty::New("Mouse Position"));
//     m_PositionTrackerNode->SetData( mitk::PointSet::New() );
//     m_PositionTrackerNode->SetColor(1.0,0.33,0.0);
//     m_PositionTrackerNode->SetProperty("layer", mitk::IntProperty::New(1001));
//     m_PositionTrackerNode->SetVisibility(true);
//     m_PositionTrackerNode->SetProperty("inputdevice", mitk::BoolProperty::New(true) );
//     m_PositionTrackerNode->SetProperty("BaseRendererMapperID", mitk::IntProperty::New(0) );//point position 2D mouse
//     m_PositionTrackerNode->SetProperty("baserenderer", mitk::StringProperty::New("N/A"));
// }

void AmosWidget::AddDisplayPlaneSubTree()
{
  // add the displayed planes of the multiwidget to a node to which the subtree
  // @a planesSubTree points ...

  mitk::PlaneGeometryDataMapper2D::Pointer mapper;

  // ... of widget 1
  // MITK 2021
//   mitk::BaseRenderer* renderer1 = mitk::BaseRenderer::GetInstance(GetRenderWindowAC()->GetRenderWindow());
    mitk::BaseRenderer* renderer1 = mitk::BaseRenderer::GetInstance(GetRenderWindowAC()->renderWindow());

    m_PlaneNodeAC = renderer1->GetCurrentWorldPlaneGeometryNode();
    m_PlaneNodeAC->SetProperty("visible", mitk::BoolProperty::New(true), renderer1);
    m_PlaneNodeAC->SetProperty("name", mitk::StringProperty::New(std::string(renderer1->GetName()) + ".plane"));
    m_PlaneNodeAC->SetProperty("includeInBoundingBox", mitk::BoolProperty::New(false));
    m_PlaneNodeAC->SetProperty("helper object", mitk::BoolProperty::New(true));
    mapper = mitk::PlaneGeometryDataMapper2D::New();
    m_PlaneNodeAC->SetMapper(mitk::BaseRenderer::Standard2D, mapper);

  // ... of widget 2
  // MITK 2021
//   mitk::BaseRenderer* renderer2 = mitk::BaseRenderer::GetInstance(GetRenderWindowBC()->GetRenderWindow());
    mitk::BaseRenderer* renderer2 = mitk::BaseRenderer::GetInstance(GetRenderWindowBC()->renderWindow());
    m_PlaneNodeBC = renderer2->GetCurrentWorldPlaneGeometryNode();
    m_PlaneNodeBC->SetProperty("visible", mitk::BoolProperty::New(true), renderer2);
    m_PlaneNodeBC->SetProperty("name", mitk::StringProperty::New(std::string(renderer2->GetName()) + ".plane"));
    m_PlaneNodeBC->SetProperty("includeInBoundingBox", mitk::BoolProperty::New(false));
    m_PlaneNodeBC->SetProperty("helper object", mitk::BoolProperty::New(true));
    mapper = mitk::PlaneGeometryDataMapper2D::New();
    m_PlaneNodeBC->SetMapper(mitk::BaseRenderer::Standard2D, mapper);
    
    // MITK 2021
//     mitk::BaseRenderer* renderer3 = mitk::BaseRenderer::GetInstance(GetRenderWindowPatient()->GetRenderWindow());
    mitk::BaseRenderer* renderer3 = mitk::BaseRenderer::GetInstance(GetRenderWindowPatient()->renderWindow());
    m_PlaneNodePatient = renderer3->GetCurrentWorldPlaneGeometryNode();
    m_PlaneNodePatient->SetProperty("visible", mitk::BoolProperty::New(true), renderer3);
    m_PlaneNodePatient->SetProperty("name", mitk::StringProperty::New(std::string(renderer3->GetName()) + ".plane"));
    m_PlaneNodePatient->SetProperty("includeInBoundingBox", mitk::BoolProperty::New(false));
    m_PlaneNodePatient->SetProperty("helper object", mitk::BoolProperty::New(true));
    mapper = mitk::PlaneGeometryDataMapper2D::New();
    m_PlaneNodePatient->SetMapper(mitk::BaseRenderer::Standard2D, mapper); 

    // MITK 2021
    // mitk::BaseRenderer* renderer4 = mitk::BaseRenderer::GetInstance(GetRenderWindow3D()->GetRenderWindow());
    mitk::BaseRenderer* renderer4 = mitk::BaseRenderer::GetInstance(GetRenderWindow3D()->renderWindow());
    m_ParentNodeForGeometryPlanes = mitk::DataNode::New();
    m_ParentNodeForGeometryPlanes->SetProperty("name", mitk::StringProperty::New("Widgets"));
    m_ParentNodeForGeometryPlanes->SetProperty("helper object", mitk::BoolProperty::New(true));

    m_PlaneNodeBC->SetProperty("visible", mitk::BoolProperty::New(false), renderer4);
    m_PlaneNodePatient->SetProperty("visible", mitk::BoolProperty::New(false), renderer4);
    
    // JCAM
    mitk::BaseRenderer* renderer5 = mitk::BaseRenderer::GetInstance(GetRenderWindow2AxialWithSegment()->renderWindow());
    m_Plane2AxialSegments = renderer5->GetCurrentWorldPlaneGeometryNode();
    m_Plane2AxialSegments->SetProperty("visible", mitk::BoolProperty::New(true), renderer5);
    m_Plane2AxialSegments->SetProperty("name", mitk::StringProperty::New(std::string(renderer5->GetName()) + ".plane"));
    m_Plane2AxialSegments->SetProperty("includeInBoundingBox", mitk::BoolProperty::New(false));
    m_Plane2AxialSegments->SetProperty("helper object", mitk::BoolProperty::New(true));
    mapper = mitk::PlaneGeometryDataMapper2D::New();
    m_Plane2AxialSegments->SetMapper(mitk::BaseRenderer::Standard2D, mapper);
    // JCAM
    mitk::BaseRenderer* renderer6 = mitk::BaseRenderer::GetInstance(GetRenderWindow2SagitalWithSegment()->renderWindow());
    m_Plane2SagitalSegments = renderer6->GetCurrentWorldPlaneGeometryNode();
    m_Plane2SagitalSegments->SetProperty("visible", mitk::BoolProperty::New(true), renderer6);
    m_Plane2SagitalSegments->SetProperty("name", mitk::StringProperty::New(std::string(renderer6->GetName()) + ".plane"));
    m_Plane2SagitalSegments->SetProperty("includeInBoundingBox", mitk::BoolProperty::New(false));
    m_Plane2SagitalSegments->SetProperty("helper object", mitk::BoolProperty::New(true));
    mapper = mitk::PlaneGeometryDataMapper2D::New();
    m_Plane2SagitalSegments->SetMapper(mitk::BaseRenderer::Standard2D, mapper);
}

// void AmosWidget::EnableStandardLevelWindow()
// {
//     levelWindowWidget->disconnect(this);
//     levelWindowWidget->SetDataStorage(ds);
//  //   levelWindowWidgetBC->SetDataStorage(mitk::BaseRenderer::GetInstance(renderWindowBC->GetRenderWindow())->GetDataStorage());
//     levelWindowWidget->show();
// 
// }

// void AmosWidget::DisableStandardLevelWindow()
// {
//   levelWindowWidget->disconnect(this);
//   levelWindowWidget->hide();
// }

// void AmosWidget::RequestUpdate()
// {
//   m_RenderingManager->RequestUpdate(renderWindowAC->GetRenderWindow());
//   m_RenderingManager->RequestUpdate(renderWindowBC->GetRenderWindow());
//   m_RenderingManager->RequestUpdate(renderWindowPatient->GetRenderWindow());
//   m_RenderingManager->RequestUpdate(renderWindow3D->GetRenderWindow());
// }

// void AmosWidget::ForceImmediateUpdate()
// {
//   m_RenderingManager->ForceImmediateUpdate(renderWindowAC->GetRenderWindow());
//   m_RenderingManager->ForceImmediateUpdate(renderWindowBC->GetRenderWindow());
//   m_RenderingManager->ForceImmediateUpdate(renderWindowPatient->GetRenderWindow());
//   m_RenderingManager->ForceImmediateUpdate(renderWindow3D->GetRenderWindow());
// }

void AmosWidget::wheelEvent( QWheelEvent * e )
{
  emit WheelMoved( e );
}

// void AmosWidget::mousePressEvent(QMouseEvent * e)
// {
//    if (e->buttons() & Qt::LeftButton) {
//        mitk::Point3D pointValue = this->GetLastLeftClickPosition(); // DEPRECATED use mitk::DisplayInteractor
// //        mitk::Point3D pointValue;
//        emit LeftMouseClicked(pointValue);
//    }
// }

void AmosWidget::mousePressEvent(QMouseEvent * e)
{
    if (QEvent::MouseButtonPress != e->type())
    {
        return;
    }
    
    auto renderWindowWidget = dynamic_cast<MyQmitkRenderWindowWidget*>(this->sender());
    if (nullptr == renderWindowWidget)
    {
        return;
    }
    
    auto renderWindowWidgetPointer = GetRenderWindowWidget(renderWindowWidget->GetWidgetName());
    SetActiveRenderWindowWidget(renderWindowWidgetPointer);
}

void AmosWidget::moveEvent( QMoveEvent* e )
{
  QWidget::moveEvent( e );

  // it is necessary to readjust the position of the overlays as the StdMultiWidget has moved
  // unfortunately it's not done by QmitkRenderWindow::moveEvent -> must be done here
    emit Moved();
}

// void AmosWidget::mouseDoubleClickEvent( QMouseEvent *event )
// {
//     setLevelWindowFixed(false);
// }

// mitk::MouseModeSwitcher* AmosWidget::GetMouseModeSwitcher()
// {
//     return m_MouseModeSwitcher;
// }

// void AmosWidget::setLevelWindowFixed(bool levelWindowFixed)
// {
// //     m_LevelWindow = levelWindowWidget->GetManager()->GetLevelWindow();
// //     m_LevelWindow.SetFixed(levelWindowFixed);
// //     levelWindowWidget->GetManager()->SetLevelWindow(m_LevelWindow);
// }

// void AmosWidget::ActivateMenuWidget(bool state)
// {
//   renderWindowAC->ActivateMenuWidget(state);
//   renderWindowBC->ActivateMenuWidget(state);
//   renderWindowPatient->ActivateMenuWidget(state);
//   renderWindow3D->ActivateMenuWidget(state);    
// }

// void AmosWidget::OnLayoutDesignChanged(int layoutDesignIndex)
// {
//   switch( layoutDesignIndex )
//   {
//   case MyQmitkRenderWindow::LAYOUT_DEFAULT:
//     {
//       this->changeLayoutToDefault();
//       break;
//     }
//   case MyQmitkRenderWindow::LAYOUT_2D_IMAGES_UP:
//     {
//       this->changeLayoutTo2DImagesUp();
//       break;
//     }
//   case MyQmitkRenderWindow::LAYOUT_2D_IMAGES_LEFT:
//     {
//       this->changeLayoutTo2DImagesLeft();
//       break;
//     }
//   case MyQmitkRenderWindow::LAYOUT_BIG_3D:
//     {
//       this->changeLayoutToBig3D();
//       break;
//     }
//   case MyQmitkRenderWindow::LAYOUT_WIDGET1:
//     {
//       this->changeLayoutToWidget1();
//       break;
//     }
//   case MyQmitkRenderWindow::LAYOUT_WIDGET2:
//     {
//       this->changeLayoutToWidget2();
//       break;
//     }
//   case MyQmitkRenderWindow::LAYOUT_WIDGET3:
//     {
//       this->changeLayoutToWidget3();
//       break;
//     }
//   case MyQmitkRenderWindow::LAYOUT_2X_2D_AND_3D_WIDGET:
//     {
//       this->changeLayoutTo2x2Dand3DWidget();
//       break;
//     }
//   case MyQmitkRenderWindow::LAYOUT_ROW_WIDGET_3_AND_4:
//     {
//       this->changeLayoutToRowWidget3And4();
//       break;
//     }
//   case MyQmitkRenderWindow::LAYOUT_COLUMN_WIDGET_3_AND_4:
//     {
//       this->changeLayoutToColumnWidget3And4();
//       break;
//     }
//   case MyQmitkRenderWindow::LAYOUT_ROW_WIDGET_SMALL3_AND_BIG4:
//     {
//       this->changeLayoutToRowWidgetSmall3andBig4();
//       break;
//     }
//   case MyQmitkRenderWindow::LAYOUT_SMALL_UPPER_WIDGET2_BIG3_AND4:
//     {
//       this->changeLayoutToSmallUpperWidget2Big3and4();
//       break;
//     }
//   case MyQmitkRenderWindow::LAYOUT_2D_AND_3D_LEFT_2D_RIGHT_WIDGET:
//     {
//       this->changeLayoutToLeft2Dand3DRight2D();
//       break;
//     }
//   case MyQmitkRenderWindow::LAYOUT_2D_AC_RIGHT_AND_BC_LEFT:
//     {
//       this->changeLayoutToLeftACandRightBC();
//       break;
//     }  
//   case MyQmitkRenderWindow::LAYOUT_AC_UP_AND_BC_DOWN:
//     {
//       this->changeLayoutToUpACandDownBC();
//       break;
//     }     
//   };    
// }

// void AmosWidget::UpdateAllWidgets()
// {
//   renderWindowAC->resize( renderWindowACContainer->frameSize().width()-1, renderWindowACContainer->frameSize().height() );
//   renderWindowAC->resize( renderWindowACContainer->frameSize().width(), renderWindowACContainer->frameSize().height() );
// 
//   renderWindowBC->resize( renderWindowBCContainer->frameSize().width()-1, renderWindowBCContainer->frameSize().height() );
//   renderWindowBC->resize( renderWindowBCContainer->frameSize().width(), renderWindowBCContainer->frameSize().height() );
//   
//   renderWindowPatient->resize( renderWindowPatientContainer->frameSize().width()-1, renderWindowPatientContainer->frameSize().height() );
//   renderWindowPatient->resize( renderWindowPatientContainer->frameSize().width(), renderWindowPatientContainer->frameSize().height() );  
// 
//   renderWindow3D->resize( renderWindow3DContainer->frameSize().width()-1, renderWindow3DContainer->frameSize().height() );
//   renderWindow3D->resize( renderWindow3DContainer->frameSize().width(), renderWindow3DContainer->frameSize().height() );
// }

// void AmosWidget::HideAllWidgetToolbars()
// {
//   renderWindowAC->HideRenderWindowMenu();
//   renderWindowBC->HideRenderWindowMenu();
//   renderWindowPatient->HideRenderWindowMenu();
//   renderWindow3D->HideRenderWindowMenu();
// }

// void AmosWidget::ResetViews()
// {
//     if (ds.IsNotNull())
//         m_RenderingManager->InitializeViewsByBoundingObjects(ds);    
//     m_RenderingManager->InitializeViews();  
// }

void AmosWidget::SetDataStorage(mitk::DataStorage* dataStorage)
{
//     this->ds = dataStorage;
// 
//     renderWindowAC->GetRenderer()->SetDataStorage(ds);
//     renderWindowBC->GetRenderer()->SetDataStorage(ds);
//     renderWindowPatient->GetRenderer()->SetDataStorage(ds);
//     renderWindow3D->GetRenderer()->SetDataStorage(ds);
    QmitkAbstractMultiWidget::SetDataStorage(dataStorage);
//     this->ds->AddNodeEvent.AddListener(mitk::MessageDelegate1<AmosWidget, const mitk::DataNode*>(this,&AmosWidget::onNodeAdded));
//     this->ds->RemoveNodeEvent.AddListener(mitk::MessageDelegate1<AmosWidget, const mitk::DataNode*>(this,&AmosWidget::onNodeRemoved));
    dataStorage->AddNodeEvent.AddListener(mitk::MessageDelegate1<AmosWidget, const mitk::DataNode*>(this,&AmosWidget::onNodeAdded));
    dataStorage->RemoveNodeEvent.AddListener(mitk::MessageDelegate1<AmosWidget, const mitk::DataNode*>(this,&AmosWidget::onNodeRemoved));    
}

void AmosWidget::onNodeAdded(const mitk::DataNode* node)
{
    auto dataStorage = GetDataStorage();
    DataStorage::SetOfObjects::ConstPointer binariesImg = dataStorage->GetSubset(m_IsASegmentationImagePredicate);

    Utils::Segmentation_Type amosTypeIn = Utils::AmosSegmentationType(node);
    if(amosTypeIn != Utils::Amos_AC && amosTypeIn != Utils::Amos_BC) 
	return;
    
    for(DataStorage::SetOfObjects::ConstIterator it = binariesImg->Begin(); it != binariesImg->End();  ++it) {
        DataNode::Pointer nodeBin = it.Value();
        Utils::Segmentation_Type amosType = Utils::AmosSegmentationType(nodeBin);
        if(amosType == Utils::Amos_AC) {
            nodeBin->SetVisibility(false, GetRenderWindowBC()->GetRenderer());
            nodeBin->SetVisibility(false, GetRenderWindowPatient()->GetRenderer());
//             nodeBin->SetVisibility(false, GetRenderWindow3D()->GetRenderer());
            
            nodeBin->SetProperty("color", ColorProperty::New(m_SegmentationColor[0][0], m_SegmentationColor[0][1], m_SegmentationColor[0][2]));
            nodeBin->SetProperty("binaryimage.selectedcolor", ColorProperty::New(m_SegmentationColor[0][0], m_SegmentationColor[0][1], m_SegmentationColor[0][2]));
            nodeBin->SetProperty("SegmentationType", mitk::StringProperty::New("Amos_AC"));
            
            if(!segmentationNodes.contains(nodeBin) && segmentationNodes.size() < 2) {
            segmentationNodes.append(nodeBin);
            emit sendSegmentationNodeAdded(nodeBin);
            }
        }
        else if(amosType == Utils::Amos_BC) {
            nodeBin->SetVisibility(false, GetRenderWindowAC()->GetRenderer());
            nodeBin->SetVisibility(false, GetRenderWindowPatient()->GetRenderer());
            nodeBin->SetVisibility(false, GetRenderWindow3D()->GetRenderer());

            nodeBin->SetProperty("color", ColorProperty::New(m_SegmentationColor[1][0], m_SegmentationColor[1][1], m_SegmentationColor[1][2]));
            nodeBin->SetProperty("binaryimage.selectedcolor", ColorProperty::New(m_SegmentationColor[1][0], m_SegmentationColor[1][1], m_SegmentationColor[1][2]));
            nodeBin->SetProperty("SegmentationType", mitk::StringProperty::New("Amos_BC"));
            
            if(!segmentationNodes.contains(nodeBin) && segmentationNodes.size() < 2) {
            segmentationNodes.append(nodeBin);
            emit sendSegmentationNodeAdded(nodeBin);
            }
        }	
	    
    } 

}

void AmosWidget::onNodeRemoved(const mitk::DataNode* node)
{
    if(segmentationNodes.contains(node)) {
        segmentationNodes.removeOne(node);
        emit sendSegmentationNodeRemoved(node);
    }
    
    AmosItkImageIO::removeImageFromList(node);
}

MyQmitkRenderWindow* AmosWidget::GetRenderWindowAC() const
// QmitkRenderWindow* AmosWidget::GetRenderWindowAC() const
{
    //     return renderWindowAC;
//     return QmitkAbstractMultiWidget::GetRenderWindow(GetNameFromIndex(0, 0));
    return dynamic_cast<MyQmitkRenderWindow*>(QmitkAbstractMultiWidget::GetRenderWindow(GetNameFromIndex(0, 0)));
}

MyQmitkRenderWindow* AmosWidget::GetRenderWindowBC() const
// QmitkRenderWindow* AmosWidget::GetRenderWindowBC() const
{
//     return renderWindowBC;
//     return QmitkAbstractMultiWidget::GetRenderWindow(GetNameFromIndex(0, 1));
    return dynamic_cast<MyQmitkRenderWindow*>(QmitkAbstractMultiWidget::GetRenderWindow(GetNameFromIndex(0, 1)));
}

MyQmitkRenderWindow* AmosWidget::GetRenderWindowPatient() const
// QmitkRenderWindow* AmosWidget::GetRenderWindowPatient() const
{
//     return renderWindowPatient;
//     return QmitkAbstractMultiWidget::GetRenderWindow(GetNameFromIndex(1, 0));
    return dynamic_cast<MyQmitkRenderWindow*>(QmitkAbstractMultiWidget::GetRenderWindow(GetNameFromIndex(1, 0)));
}

MyQmitkRenderWindow* AmosWidget::GetRenderWindow3D() const
// QmitkRenderWindow* AmosWidget::GetRenderWindow3D() const
{
//     return renderWindow3D;
//     return QmitkAbstractMultiWidget::GetRenderWindow(GetNameFromIndex(1, 1));
    return dynamic_cast<MyQmitkRenderWindow*>(QmitkAbstractMultiWidget::GetRenderWindow(GetNameFromIndex(1, 1)));
}

// JCAM.
MyQmitkRenderWindow * AmosWidget::GetRenderWindow2AxialWithSegment() const
{
    return dynamic_cast<MyQmitkRenderWindow*>(QmitkAbstractMultiWidget::GetRenderWindow(GetNameFromIndex(0, 2)));
}

MyQmitkRenderWindow * AmosWidget::GetRenderWindow2SagitalWithSegment() const
{
    return dynamic_cast<MyQmitkRenderWindow*>(QmitkAbstractMultiWidget::GetRenderWindow(GetNameFromIndex(1, 2)));
}

// bool AmosWidget::IsColoredRectanglesEnabled() const
// {
//   return m_RectangleProps[0]->GetVisibility() > 0;
// }

// void AmosWidget::EnableColoredRectangles()
// {
//   m_RectangleProps[0]->SetVisibility(1);
//   m_RectangleProps[1]->SetVisibility(1);
//   m_RectangleProps[2]->SetVisibility(1);
//   m_RectangleProps[3]->SetVisibility(1);
// }

// void AmosWidget::DisableColoredRectangles()
// {
//   m_RectangleProps[0]->SetVisibility(0);
//   m_RectangleProps[1]->SetVisibility(0);
//   m_RectangleProps[2]->SetVisibility(0);
//   m_RectangleProps[3]->SetVisibility(0);
// }

// void AmosWidget::EnableGradientBackground()
// {
//   // gradient background is by default only in widget 4, otherwise
//   // interferences between 2D rendering and VTK rendering may occur.
//     for(unsigned int i = 0; i < 4; ++i)
//     {
// 	/*if(i != 2)*/    
// 	GetRenderWindow(i)->GetRenderer()->GetVtkRenderer()->GradientBackgroundOn();
// // 	else {
// // 	    QPalette Pal(palette());	   
// // 	    Pal.setColor(QPalette::Window, Qt::black);
// // 	    emptyContainer->setAutoFillBackground(true);
// // 	    emptyContainer->setPalette(Pal);
// // 	    emptyContainer->show();	    
// // 	}
//     }
//     m_GradientBackgroundFlag = true;
// }

// void AmosWidget::DisableGradientBackground()
// {
//     for(unsigned int i = 0; i < 4; ++i)
//     {
// 	/*if(i != 2)*/ 
// 	GetRenderWindow(i)->GetRenderer()->GetVtkRenderer()->GradientBackgroundOff();
//     }
//   m_GradientBackgroundFlag = false;
// }

// void AmosWidget::SetDepartmentLogoPath( const char * path )
// {
//   m_LogoRendering->SetLogoImagePath(path);
//   mitk::BaseRenderer* renderer = mitk::BaseRenderer::GetInstance(renderWindow3D->GetRenderWindow());
//   m_LogoRendering->Update(renderer);
//   RequestUpdate();
// }

// bool AmosWidget::IsDepartmentLogoEnabled() const
// {
//   return m_LogoRendering->IsVisible(mitk::BaseRenderer::GetInstance(renderWindow3D->GetRenderWindow()));
// }

// void AmosWidget::EnableDepartmentLogo()
// {
//   m_LogoRendering->SetVisibility(true);
//   RequestUpdate();
// }

// void AmosWidget::DisableDepartmentLogo()
// {
//   m_LogoRendering->SetVisibility(false);
//   RequestUpdate();
// }

// bool AmosWidget::IsMenuWidgetEnabled() const
// { 
//     return renderWindowAC->GetActivateMenuWidgetFlag();    
// }

// void AmosWidget::changeLayoutToDefault()
// {
//   //  SMW_INFO << "changing layout to default... " << std::endl;
// 
//   // Hide all Menu Widgets
//    this->HideAllWidgetToolbars();
// 
// //     setLevelWindowFixed(true);     
// 
//     delete layout ;
// 
//     //create Main Layout
//     layout =  new QHBoxLayout( this );
//     layout->setContentsMargins(0,0,0,0);
//     this->setLayout(layout);
//     //create main splitter
//     splitter = new QSplitter( this );
//     layout->addWidget( splitter );
// 
//   ////create splitterVert  and add to the mainSplit
//     splitterVert = new QSplitter( Qt::Vertical, splitter );
//     splitter->addWidget(splitterVert);  
//     
// 
//     //add LevelWindow Widget to mainSplitter
//     splitter->addWidget( levelWindowWidget );
// 
//   //create splitterUp and splitterDown
//     splitterUp = new QSplitter(splitterVert);
//     splitterDown = new QSplitter(splitterVert); 
//     
// 
//   //insert Widget container into the splitters
//     splitterUp->addWidget( renderWindowACContainer );
//     splitterUp->addWidget( renderWindowBCContainer );
// 
//     splitterDown->addWidget( renderWindowPatientContainer );
//     splitterDown->addWidget( renderWindow3DContainer );  
// 
//     set splitter Size
//     QList<int> splitterSize;
//     splitterSize.push_back(1000);
//     splitterSize.push_back(1000);
//     splitterUp->setSizes( splitterSize );
//     splitterDown->setSizes( splitterSize );
//     splitterVert->setSizes( splitterSize );
// 
//     //show mainSplitt and add to Layout
//     splitter->show();
//   
//     renderWindowACContainer->setContentsMargins(0,0,0,0);
//     renderWindowBCContainer->setContentsMargins(0,0,0,0);
//     renderWindowPatientContainer->setContentsMargins(0,0,0,0);
//     renderWindow3DContainer->setContentsMargins(0,0,0,0);  
// 
//     //show Widget if hidden
//     if(renderWindowAC->isHidden()) renderWindowAC->show();
//     if(renderWindowBC->isHidden()) renderWindowBC->show();
//     if(renderWindowPatient->isHidden()) renderWindowPatient->show();
//     if(renderWindow3D->isHidden()) renderWindow3D->show();
// 
//     m_Layout = MyQmitkRenderWindow::LAYOUT_DEFAULT;
// 
//     //update Layout Design List
//   
//     renderWindowAC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_DEFAULT );
//     renderWindowBC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_DEFAULT );
//     renderWindowPatient->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_DEFAULT );
//     renderWindow3D->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_DEFAULT );
// 
//     //update Alle Widgets
//     this->UpdateAllWidgets();
//     
// }

// void AmosWidget::changeLayoutToBig3D()
// {
//   //   SMW_INFO << "changing layout to big 3D ..." << std::endl;
// 
//   //Hide all Menu Widgets
//    this->HideAllWidgetToolbars();
// 
// //     setLevelWindowFixed(true);    
// 
//     delete layout ;
//     //create Main Layout
//     layout =  new QHBoxLayout( this );
// 
//     layout->setContentsMargins(0,0,0,0);
//     this->setLayout(layout);  
//     
//     //create main splitter
//     splitter = new QSplitter( this );
//     layout->addWidget( splitter );
// 
//     //add widget Splitter to main Splitter
//     splitter->addWidget(renderWindow3DContainer);
// 
//     //add LevelWindow Widget to mainSplitter
//     splitter->addWidget( levelWindowWidget );
// 
//     //show mainSplitt and add to Layout
//     splitter->show();
//   
//     renderWindow3DContainer->setContentsMargins(0,0,0,0);  
// 
//     //show/hide Widgets
//     renderWindowAC->hide();
//     renderWindowBC->hide();
//     renderWindowPatient->hide();
//     if ( renderWindow3D->isHidden() ) renderWindow3D->show();
// 
//     m_Layout = MyQmitkRenderWindow::LAYOUT_BIG_3D;
// 
//     //update Layout Design List
//     renderWindowAC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_BIG_3D );
//     renderWindowBC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_BIG_3D );
//     renderWindowPatient->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_BIG_3D );
//     renderWindow3D->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_BIG_3D );
// 
//     //update Alle Widgets
//     this->UpdateAllWidgets();
//   
// }

// void AmosWidget::changeLayoutToWidget1()
// {
//   //   SMW_INFO << "changing layout to big Widget1 ..." << std::endl;
// 
//   //Hide all Menu Widgets
//    this->HideAllWidgetToolbars();
// //     setLevelWindowFixed(true);
//     
//   delete layout ;  
// 
//   //create Main Layout
//   layout =  new QHBoxLayout( this );
//     layout->setContentsMargins(0,0,0,0);
//     this->setLayout(layout);
//     
//   //create main splitter
//   splitter = new QSplitter( this );
//   layout->addWidget( splitter );
// 
//   //add widget Splitter to main Splitter
//   splitter->addWidget( renderWindowACContainer );
// 
//   //add LevelWindow Widget to mainSplitter
//   splitter->addWidget( levelWindowWidget );
// 
//   //show mainSplitt and add to Layout
//   splitter->show();
//   
//     renderWindowACContainer->setContentsMargins(0,0,0,0); 
// 
//   //show/hide Widgets
//   if ( renderWindowAC->isHidden() ) renderWindowAC->show();
//   renderWindowBC->hide();
//   renderWindowPatient->hide();
//   renderWindow3D->hide();
// 
//   m_Layout = MyQmitkRenderWindow::LAYOUT_WIDGET1;
// 
//   //update Layout Design List
//   renderWindowAC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_WIDGET1 );
//   renderWindowBC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_WIDGET1 );
//   renderWindowPatient->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_WIDGET1 );
//   renderWindow3D->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_WIDGET1 );
// 
//   //update Alle Widgets
//   this->UpdateAllWidgets();
//        
// }

// void AmosWidget::changeLayoutToWidget2()
// {
//   //   SMW_INFO << "changing layout to big Widget1 ..." << std::endl;
// 
//   //Hide all Menu Widgets
//    this->HideAllWidgetToolbars();
// //     setLevelWindowFixed(true);
//     
//     delete layout ;
// 
//     create Main Layout
//     layout =  new QHBoxLayout( this );
//     layout->setContentsMargins(0,0,0,0);
//     this->setLayout(layout);    
// 
//     //create main splitter
//     splitter = new QSplitter( this );
//     layout->addWidget( splitter );
// 
//     //add widget Splitter to main Splitter
//     splitter->addWidget( renderWindowBCContainer );
// 
//     //add LevelWindow Widget to mainSplitter
//     splitter->addWidget( levelWindowWidget );
// 
//     //show mainSplitt and add to Layout
//     splitter->show();
// 
//     renderWindowBCContainer->setContentsMargins(0,0,0,0);  
// 
//     //show/hide Widgets
//     renderWindowAC->hide();
//     if ( renderWindowBC->isHidden() ) renderWindowBC->show();  
//     renderWindowPatient->hide();
//     renderWindow3D->hide();
// 
//     m_Layout = MyQmitkRenderWindow::LAYOUT_WIDGET2;
// 
//     //update Layout Design List
//     renderWindowAC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_WIDGET2 );
//     renderWindowBC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_WIDGET2 );
//     renderWindowPatient->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_WIDGET2 );
//     renderWindow3D->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_WIDGET2 );
// 
//     //update Alle Widgets
//     this->UpdateAllWidgets();
// 
// }

// void AmosWidget::changeLayoutToWidget3()
// {
//   //   SMW_INFO << "changing layout to big Widget1 ..." << std::endl;
// 
//   //Hide all Menu Widgets
//    this->HideAllWidgetToolbars();
// //     setLevelWindowFixed(true);
//     
//     delete layout ;
// 
//     //create Main Layout
//     layout =  new QHBoxLayout( this );
//     layout->setContentsMargins(0,0,0,0);
//     this->setLayout(layout);    
// 
//     //create main splitter
//     splitter = new QSplitter( this );
//     layout->addWidget( splitter );
// 
//     //add widget Splitter to main Splitter
//     splitter->addWidget( renderWindowPatientContainer );
// 
//     //add LevelWindow Widget to mainSplitter
//     splitter->addWidget( levelWindowWidget );
// 
//     //show mainSplitt and add to Layout
//     splitter->show();
// 
//     renderWindowPatientContainer->setContentsMargins(0,0,0,0);  
// 
//     //show/hide Widgets
//     renderWindowAC->hide();
//     renderWindowBC->hide();
//     if ( renderWindowPatient->isHidden() ) renderWindowPatient->show();    
//     renderWindow3D->hide();
// 
//     m_Layout = MyQmitkRenderWindow::LAYOUT_WIDGET3;
// 
//     //update Layout Design List
//     renderWindowAC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_WIDGET3 );
//     renderWindowBC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_WIDGET3 );
//     renderWindowPatient->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_WIDGET3 );
//     renderWindow3D->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_WIDGET3 );
// 
//     //update Alle Widgets
//     this->UpdateAllWidgets();
// 
// }

// void AmosWidget::changeLayoutTo2DImagesUp()
// {
//       //   SMW_INFO << "changing layout to 2D images up... " << std::endl;
// 
//     //Hide all Menu Widgets
//        this->HideAllWidgetToolbars();
// 
//     delete layout ;
// 
//     //create Main Layout
//     layout =  new QHBoxLayout( this );
// 
//     //Set Layout to widget
//     this->setLayout(layout);
// 
//     //create main splitter
//     splitter = new QSplitter( this );
//     layout->addWidget( splitter );
// 
//     create splitterVert  and add to the mainSplit
//     splitterVert = new QSplitter( Qt::Vertical, splitter );
//     splitter->addWidget( splitterVert );
// 
//     //add LevelWindow Widget to mainSplitter
//     splitter->addWidget( levelWindowWidget );
// 
//     //create splitterUp and splitterDown
//     splitterUp = new QSplitter( splitterVert );
//     splitterDown = new QSplitter( splitterVert );
// 
//     //insert Widget Container into splitter top
//     splitterUp->addWidget( renderWindowACContainer );
//     splitterUp->addWidget( renderWindowBCContainer );
//     splitterUp->addWidget( renderWindowPatientContainer );
// 
//     //set SplitterSize for splitter top
//     QList<int> splitterSize;
//     splitterSize.push_back(1000);
//     splitterSize.push_back(1000);
//     splitterSize.push_back(1000);
//     splitterUp->setSizes( splitterSize );
// 
//     //insert Widget Container into splitter bottom
//     splitterDown->addWidget( renderWindow3DContainer );
// 
//     set SplitterSize for splitter splitterVert
//     splitterSize.clear();
//     splitterSize.push_back(400);
//     splitterSize.push_back(1000);
//     splitterVert->setSizes( splitterSize );
// 
//     //show mainSplitt
//     splitter->show();
// 
//     //show Widget if hidden
//     if ( renderWindowAC->isHidden() ) renderWindowAC->show();
//     if ( renderWindowBC->isHidden() ) renderWindowBC->show();
//     if ( renderWindowPatient->isHidden() ) renderWindowPatient->show();
//     if ( renderWindow3D->isHidden() ) renderWindow3D->show();
// 
//     //Change Layout Name
//     m_Layout = MyQmitkRenderWindow::LAYOUT_2D_IMAGES_UP;
// 
//     //update Layout Design List
//     renderWindowAC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_2D_IMAGES_UP );
//     renderWindowBC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_2D_IMAGES_UP );
//     renderWindowPatient->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_2D_IMAGES_UP );
//     renderWindow3D->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_2D_IMAGES_UP );
// 
//     //update Alle Widgets
//     this->UpdateAllWidgets();    
// }

// void AmosWidget::changeLayoutTo2DImagesLeft()
// {
//       //   SMW_INFO << "changing layout to 2D images left... " << std::endl;
// 
//     //Hide all Menu Widgets
//        this->HideAllWidgetToolbars();
// 
//     delete layout ;
// 
//     //create Main Layout
//     layout =  new QHBoxLayout( this );
// 
//     //create main splitter
//     splitter = new QSplitter( this );
//     layout->addWidget( splitter );
// 
//     //create splitterVert  and add to the mainSplit
//     splitterVert = new QSplitter( splitter );
//     splitter->addWidget( splitterVert );
// 
//     //add LevelWindow Widget to mainSplitter
//     splitter->addWidget( levelWindowWidget );
// 
//     //create splitterUp and splitterDown
//     splitterUp = new QSplitter( Qt::Vertical, splitterVert );
//     splitterDown = new QSplitter( splitterVert );
// 
//     //insert Widget into the splitters
//     splitterUp->addWidget( renderWindowACContainer );
//     splitterUp->addWidget( renderWindowBCContainer );
//     splitterUp->addWidget( renderWindowPatientContainer );
// 
//     set splitterSize of SubSplit1
//     QList<int> splitterSize;
//     splitterSize.push_back(1000);
//     splitterSize.push_back(1000);
//     splitterSize.push_back(1000);
//     splitterUp->setSizes( splitterSize );
// 
//     splitterDown->addWidget( renderWindow3DContainer );
// 
//     //set splitterSize of Layout Split
//     splitterSize.clear();
//     splitterSize.push_back(400);
//     splitterSize.push_back(1000);
//     splitterVert->setSizes( splitterSize );
// 
//     //show mainSplitt and add to Layout
//     splitter->show();
// 
//     show Widget if hidden
//     if ( renderWindowAC->isHidden() ) renderWindowAC->show();
//     if ( renderWindowBC->isHidden() ) renderWindowBC->show();
//     if ( renderWindowPatient->isHidden() ) renderWindowPatient->show();
//     if ( renderWindow3D->isHidden() ) renderWindow3D->show();
// 
//     //update Layout Name
//     m_Layout = MyQmitkRenderWindow::LAYOUT_2D_IMAGES_LEFT;
// 
//     update Layout Design List
//     renderWindowAC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_2D_IMAGES_LEFT );
//     renderWindowBC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_2D_IMAGES_LEFT );
//     renderWindowPatient->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_2D_IMAGES_LEFT );
//     renderWindow3D->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_2D_IMAGES_LEFT );
// 
//     //update Alle Widgets
//     this->UpdateAllWidgets();    
// }

// void AmosWidget::changeLayoutToRowWidget3And4()
// {
//       //   SMW_INFO << "changing layout to Widget3 and 4 in a Row..." << std::endl;
// 
//     //Hide all Menu Widgets
//        this->HideAllWidgetToolbars();
// 
//     delete layout ;
// 
//     //create Main Layout
//     layout =  new QHBoxLayout( this );
// 
//     //create main splitter
//     splitter = new QSplitter( this );
//     layout->addWidget( splitter );
// 
//     //create splitterVert  and add to the mainSplit
//     splitterVert = new QSplitter( Qt::Vertical, splitter );
//     splitter->addWidget( splitterVert );
// 
//     //add LevelWindow Widget to mainSplitter
//     splitter->addWidget( levelWindowWidget );
// 
//     add Widgets to splitter
//     splitterVert->addWidget( renderWindowPatientContainer );
//     splitterVert->addWidget( renderWindow3DContainer );
// 
//     //set Splitter Size
//     QList<int> splitterSize;
//     splitterSize.push_back(1000);
//     splitterSize.push_back(1000);
//     splitterVert->setSizes( splitterSize );
// 
//     //show mainSplitt and add to Layout
//     splitter->show();
// 
//     //show/hide Widgets
//     renderWindowAC->hide();
//     renderWindowBC->hide();
//     if ( renderWindowPatient->isHidden() ) renderWindowPatient->show();
//     if ( renderWindow3D->isHidden() ) renderWindow3D->show();
// 
//     m_Layout = MyQmitkRenderWindow::LAYOUT_ROW_WIDGET_3_AND_4;
// 
//     //update Layout Design List
//     renderWindowAC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_ROW_WIDGET_3_AND_4 );
//     renderWindowBC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_ROW_WIDGET_3_AND_4 );
//     renderWindowPatient->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_ROW_WIDGET_3_AND_4 );
//     renderWindow3D->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_ROW_WIDGET_3_AND_4 );
// 
//     //update Alle Widgets
//     this->UpdateAllWidgets();    
// }

// void AmosWidget::changeLayoutToColumnWidget3And4()
// {
//       //   SMW_INFO << "changing layout to Widget3 and 4 in one Column..." << std::endl;
// 
//     //Hide all Menu Widgets
//        this->HideAllWidgetToolbars();
// 
//     delete layout ;
// 
//     //create Main Layout
//     layout =  new QHBoxLayout( this );
// 
//     //create main splitter
//     splitter = new QSplitter( this );
//     layout->addWidget( splitter );
// 
//     //create splitterVert  and add to the mainSplit
//     splitterVert = new QSplitter( splitter );
//     splitter->addWidget( splitterVert );
// 
//     //add LevelWindow Widget to mainSplitter
//     splitter->addWidget( levelWindowWidget );
// 
//     //add Widgets to splitter
//     splitterVert->addWidget( renderWindowPatientContainer );
//     splitterVert->addWidget( renderWindow3DContainer );
// 
//     //set SplitterSize
//     QList<int> splitterSize;
//     splitterSize.push_back(1000);
//     splitterSize.push_back(1000);
//     splitterVert->setSizes( splitterSize );
// 
//     //show mainSplitt and add to Layout
//     splitter->show();
// 
//     //show/hide Widgets
//     renderWindowAC->hide();
//     renderWindowBC->hide();
//     if ( renderWindowPatient->isHidden() ) renderWindowPatient->show();
//     if ( renderWindow3D->isHidden() ) renderWindow3D->show();
// 
//     m_Layout = MyQmitkRenderWindow::LAYOUT_COLUMN_WIDGET_3_AND_4;
// 
//     //update Layout Design List
//     renderWindowAC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_COLUMN_WIDGET_3_AND_4 );
//     renderWindowBC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_COLUMN_WIDGET_3_AND_4 );
//     renderWindowPatient->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_COLUMN_WIDGET_3_AND_4 );
//     renderWindow3D->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_COLUMN_WIDGET_3_AND_4 );
// 
//     //update Alle Widgets
//     this->UpdateAllWidgets();    
// }

// void AmosWidget::changeLayoutToRowWidgetSmall3andBig4()
// {
    //   //   SMW_INFO << "changing layout to Widget3 and 4 in a Row..." << std::endl;

//     this->changeLayoutToRowWidget3And4();
// 
//     m_Layout = MyQmitkRenderWindow::LAYOUT_ROW_WIDGET_SMALL3_AND_BIG4;    
// }

// void AmosWidget::changeLayoutToSmallUpperWidget2Big3and4()
// {
//       //   SMW_INFO << "changing layout to Widget3 and 4 in a Row..." << std::endl;
// 
//     //Hide all Menu Widgets
//        this->HideAllWidgetToolbars();
// 
//     delete layout ;
// 
//     //create Main Layout
//     layout =  new QHBoxLayout( this );
// 
//     //create main splitter
//     splitter = new QSplitter( this );
//     layout->addWidget( splitter );
// 
//     //create splitterVert  and add to the mainSplit
//     splitterVert = new QSplitter( Qt::Vertical, splitter );
//     splitter->addWidget( splitterVert );
// 
//     //add LevelWindow Widget to mainSplitter
//     splitter->addWidget( levelWindowWidget );
// 
//     //create splitterUp and splitterDown
//     splitterUp = new QSplitter( Qt::Vertical, splitterVert );
//     splitterDown = new QSplitter( splitterVert );
// 
//     //insert Widget into the splitters
//     splitterUp->addWidget( renderWindowBCContainer );
// 
//     splitterDown->addWidget( renderWindowPatientContainer );
//     splitterDown->addWidget( renderWindow3DContainer );
// 
//     //set Splitter Size
//     QList<int> splitterSize;
//     splitterSize.push_back(1000);
//     splitterSize.push_back(1000);
//     splitterDown->setSizes( splitterSize );
//     splitterSize.clear();
//     splitterSize.push_back(500);
//     splitterSize.push_back(1000);
//     splitterVert->setSizes( splitterSize );
// 
//     //show mainSplitt
//     splitter->show();
// 
//     //show Widget if hidden
//     renderWindowAC->hide();
//     if ( renderWindowBC->isHidden() ) renderWindowBC->show();
//     if ( renderWindowPatient->isHidden() ) renderWindowPatient->show();
//     if ( renderWindow3D->isHidden() ) renderWindow3D->show();
// 
//     m_Layout = MyQmitkRenderWindow::LAYOUT_SMALL_UPPER_WIDGET2_BIG3_AND4;
// 
//     //update Layout Design List
//     renderWindowAC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_SMALL_UPPER_WIDGET2_BIG3_AND4 );
//     renderWindowBC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_SMALL_UPPER_WIDGET2_BIG3_AND4 );
//     renderWindowPatient->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_SMALL_UPPER_WIDGET2_BIG3_AND4 );
//     renderWindow3D->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_SMALL_UPPER_WIDGET2_BIG3_AND4 );
// 
//     //update Alle Widgets
//     this->UpdateAllWidgets();    
// }

// void AmosWidget::changeLayoutTo2x2Dand3DWidget()
// {
//       //   SMW_INFO << "changing layout to 2 x 2D and 3D Widget" << std::endl;
// 
//     //Hide all Menu Widgets
//        this->HideAllWidgetToolbars();
// 
//     delete layout ;
// 
//     //create Main Layout
//     layout =  new QHBoxLayout( this );
// 
//     //create main splitter
//     splitter = new QSplitter( this );
//     layout->addWidget( splitter );
// 
//     //create splitterVert  and add to the mainSplit
//     splitterVert = new QSplitter( splitter );
//     splitter->addWidget( splitterVert );
// 
//     //add LevelWindow Widget to mainSplitter
//     splitter->addWidget( levelWindowWidget );
// 
//     //create splitterUp and splitterDown
//     splitterUp = new QSplitter( Qt::Vertical, splitterVert );
//     splitterDown = new QSplitter( splitterVert );
// 
//     //add Widgets to splitter
//     splitterUp->addWidget( renderWindowACContainer );
//     splitterUp->addWidget( renderWindowBCContainer );
//     splitterDown->addWidget( renderWindow3DContainer );
// 
//     //set Splitter Size
//     QList<int> splitterSize;
//     splitterSize.push_back(1000);
//     splitterSize.push_back(1000);
//     splitterUp->setSizes( splitterSize );
//     splitterVert->setSizes( splitterSize );
// 
//     //show mainSplitt and add to Layout
//     splitter->show();
// 
//     //show/hide Widgets
//     if ( renderWindowAC->isHidden() ) renderWindowAC->show();
//     if ( renderWindowBC->isHidden() ) renderWindowBC->show();
//     renderWindowPatient->hide();
//     if ( renderWindow3D->isHidden() ) renderWindow3D->show();
// 
//     m_Layout = MyQmitkRenderWindow::LAYOUT_2X_2D_AND_3D_WIDGET;
// 
//     //update Layout Design List
//     renderWindowAC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_2X_2D_AND_3D_WIDGET );
//     renderWindowBC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_2X_2D_AND_3D_WIDGET );
//     renderWindowPatient->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_2X_2D_AND_3D_WIDGET );
//     renderWindow3D->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_2X_2D_AND_3D_WIDGET );
// 
//     //update Alle Widgets
//     this->UpdateAllWidgets();    
// }

// void AmosWidget::changeLayoutToLeft2Dand3DRight2D()
// {
//       //   SMW_INFO << "changing layout to 2D and 3D left, 2D right Widget" << std::endl;
// 
//     //Hide all Menu Widgets
//        this->HideAllWidgetToolbars();
// 
//     delete layout ;
// 
//     //create Main Layout
//     layout =  new QHBoxLayout( this );
// 
//     //create main splitter
//     splitter = new QSplitter( this );
//     layout->addWidget( splitter );
// 
//     //create splitterVert  and add to the mainSplit
//     splitterVert = new QSplitter( splitter );
//     splitter->addWidget( splitterVert );
// 
//     //add LevelWindow Widget to mainSplitter
//     splitter->addWidget( levelWindowWidget );
// 
//     //create splitterUp and splitterDown
//     splitterUp = new QSplitter( Qt::Vertical, splitterVert );
//     splitterDown = new QSplitter( splitterVert );
// 
//     //add Widgets to splitter
//     splitterUp->addWidget( renderWindowACContainer );
//     splitterUp->addWidget( renderWindow3DContainer );
//     splitterDown->addWidget( renderWindowBCContainer );
// 
//     //set Splitter Size
//     QList<int> splitterSize;
//     splitterSize.push_back(1000);
//     splitterSize.push_back(1000);
//     splitterUp->setSizes( splitterSize );
//     splitterVert->setSizes( splitterSize );
// 
//     //show mainSplitt and add to Layout
//     splitter->show();
// 
//     //show/hide Widgets
//     if ( renderWindowAC->isHidden() ) renderWindowAC->show();
//     if ( renderWindowBC->isHidden() ) renderWindowBC->show();
//     renderWindowPatient->hide();
//     if ( renderWindow3D->isHidden() ) renderWindow3D->show();
// 
//     m_Layout = MyQmitkRenderWindow::LAYOUT_2D_AND_3D_LEFT_2D_RIGHT_WIDGET;
// 
//     //update Layout Design List
//     renderWindowAC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_2D_AND_3D_LEFT_2D_RIGHT_WIDGET );
//     renderWindowBC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_2D_AND_3D_LEFT_2D_RIGHT_WIDGET );
//     renderWindowPatient->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_2D_AND_3D_LEFT_2D_RIGHT_WIDGET );
//     renderWindow3D->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_2D_AND_3D_LEFT_2D_RIGHT_WIDGET );
// 
//     //update Alle Widgets
//     this->UpdateAllWidgets();    
// }

// void AmosWidget::changeLayoutTo2DUpAnd3DDown()
// {
//       //   SMW_INFO << "changing layout to only 2D AC and BC" << std::endl;
// 
//     //Hide all Menu Widgets
//        this->HideAllWidgetToolbars();
// 
//     delete layout ;
// 
//     //create Main Layout
//     layout =  new QHBoxLayout( this );
// 
//     //Set Layout to widget
//     this->setLayout(layout);
// 
//     //create main splitter
//     splitter = new QSplitter( this );
//     layout->addWidget( splitter );
// 
//     //create splitterVert  and add to the mainSplit
//     splitterVert = new QSplitter( Qt::Vertical, splitter );
//     splitter->addWidget( splitterVert );
// 
//     //add LevelWindow Widget to mainSplitter
//     splitter->addWidget( levelWindowWidget );
// 
//     //create splitterUp and splitterDown
//     splitterUp = new QSplitter( splitterVert );
//     splitterDown = new QSplitter( splitterVert );
// 
//     //insert Widget Container into splitter top
//     splitterUp->addWidget( renderWindowACContainer );
// 
//     //set SplitterSize for splitter top
//     QList<int> splitterSize;
//     //insert Widget Container into splitter bottom
//     splitterDown->addWidget( renderWindow3DContainer );
//     //set SplitterSize for splitter splitterVert
//     splitterSize.clear();
//     splitterSize.push_back(700);
//     splitterSize.push_back(700);
//     splitterVert->setSizes( splitterSize );
// 
//     //show mainSplitt
//     splitter->show();
// 
//     //show/hide Widgets
//     if ( renderWindowAC->isHidden() ) renderWindowAC->show();
//     renderWindowBC->hide();
//     renderWindowPatient->hide();
//     if ( renderWindow3D->isHidden() ) renderWindow3D->show();
// 
//     m_Layout = MyQmitkRenderWindow::LAYOUT_2D_UP_AND_3D_DOWN;
// 
//     //update Layout Design List
//     renderWindowAC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_2D_UP_AND_3D_DOWN );
//     renderWindowBC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_2D_UP_AND_3D_DOWN );
//     renderWindowPatient->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_2D_UP_AND_3D_DOWN );
//     renderWindow3D->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_2D_UP_AND_3D_DOWN );
// 
//     //update all Widgets
//     this->UpdateAllWidgets();    
// }

// void AmosWidget::changeLayoutToLeftACandRightBC()
// {
//     //   SMW_INFO << "changing layout to 2D up and 3D down" << std::endl;
// 
//     //Hide all Menu Widgets
//     this->HideAllWidgetToolbars();
// 
//     delete layout ;
// 
//     //create Main Layout
//     layout =  new QHBoxLayout( this );
// 
//     //Set Layout to widget
//     this->setLayout(layout);
// 
//     //create main splitter
//     splitter = new QSplitter( this );
//     layout->addWidget( splitter );
// 
//     //create splitterVert  and add to the mainSplit
//     splitterUp = new QSplitter(splitter);
//     splitter->addWidget( splitterUp );
// 
//     //add LevelWindow Widget to mainSplitter
//     splitter->addWidget( levelWindowWidget );
// 
//     //create splitterUp and splitterDown
// //     splitterUp = new QSplitter( splitter );
// //     splitterDown = new QSplitter( splitterVert );
// 
//     //insert Widget Container into splitter top
//     splitterUp->addWidget( renderWindowACContainer );
// 
//     //set SplitterSize for splitter top
//     QList<int> splitterSize;
//     //splitterVert Widget Container into splitter bottom
//     splitterUp->addWidget( renderWindowBCContainer );
//     //set SplitterSize for splitter splitterVert
//     splitterSize.clear();
//     splitterSize.push_back(1000);
//     splitterSize.push_back(1000);
//     splitterUp->setSizes( splitterSize );
// 
//     //show mainSplitt
//     splitter->show();
// 
//     //show/hide Widgets
//     if ( renderWindowAC->isHidden() ) renderWindowAC->show();
//     if ( renderWindowBC->isHidden() ) renderWindowBC->show();
//     renderWindowPatient->hide();
//     renderWindow3D->hide();
// 
//     m_Layout = MyQmitkRenderWindow::LAYOUT_2D_AC_RIGHT_AND_BC_LEFT;
// 
//     //update Layout Design List
//     renderWindowAC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_2D_AC_RIGHT_AND_BC_LEFT );
//     renderWindowBC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_2D_AC_RIGHT_AND_BC_LEFT );
//     renderWindowPatient->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_2D_AC_RIGHT_AND_BC_LEFT );
//     renderWindow3D->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_2D_AC_RIGHT_AND_BC_LEFT );
// 
//     //update all Widgets
//     this->UpdateAllWidgets();       
// }

// void AmosWidget::changeLayoutToUpACandDownBC()
// {
//     //   SMW_INFO << "changing layout to 2D up and 3D down" << std::endl;
// 
//     //Hide all Menu Widgets
//        this->HideAllWidgetToolbars();
// 
//     delete layout ;
// 
//     //create Main Layout
//     layout =  new QHBoxLayout( this );
// 
//     //Set Layout to widget
//     this->setLayout(layout);
// 
//     //create main splitter
//     splitter = new QSplitter( this );
//     layout->addWidget( splitter );
// 
//     //create splitterVert  and add to the mainSplit
//     splitterVert = new QSplitter( Qt::Vertical, splitter );
//     splitter->addWidget( splitterVert );
// 
//     //add LevelWindow Widget to mainSplitter
//     splitter->addWidget( levelWindowWidget );
// 
//     //create splitterUp and splitterDown
// //     splitterUp = new QSplitter( splitter );
// //     splitterDown = new QSplitter( splitterVert );
// 
//     //insert Widget Container into splitter top
//     splitterVert->addWidget( renderWindowACContainer );
// 
//     //set SplitterSize for splitter top
//     QList<int> splitterSize;
//     //splitterVert Widget Container into splitter bottom
//     splitterVert->addWidget( renderWindowBCContainer );
//     //set SplitterSize for splitter splitterVert
//     splitterSize.clear();
//     splitterSize.push_back(1000);
//     splitterSize.push_back(1000);
//     splitterVert->setSizes( splitterSize );
// 
//     //show mainSplitt
//     splitter->show();
// 
//     //show/hide Widgets
//     if ( renderWindowAC->isHidden() ) renderWindowAC->show();
//     if ( renderWindowBC->isHidden() ) renderWindowBC->show();
//     renderWindowPatient->hide();
//     renderWindow3D->hide();
// 
//     m_Layout = MyQmitkRenderWindow::LAYOUT_AC_UP_AND_BC_DOWN;
// 
//     //update Layout Design List
//     renderWindowAC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_AC_UP_AND_BC_DOWN );
//     renderWindowBC->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_AC_UP_AND_BC_DOWN );
//     renderWindowPatient->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_AC_UP_AND_BC_DOWN );
//     renderWindow3D->LayoutDesignListChanged( MyQmitkRenderWindow::LAYOUT_AC_UP_AND_BC_DOWN );
// 
//     //update all Widgets
//     this->UpdateAllWidgets();       
// }


mitk::DataNode::Pointer AmosWidget::GetWidgetPlaneAC()
{
  return this->m_PlaneNodeAC;
}

mitk::DataNode::Pointer AmosWidget::GetWidgetPlaneBC()
{
  return this->m_PlaneNodeBC;
}

mitk::DataNode::Pointer AmosWidget::GetWidgetPlanePatient()
{
  return this->m_PlaneNodePatient;
}


// JCAM
mitk::DataNode::Pointer AmosWidget::GetWidgetPlane2AxialWithSegment()
{
    return this->m_Plane2AxialSegments;
}
  
  // JCAM
mitk::DataNode::Pointer AmosWidget::GetWidgetPlane2SagitalWithSegment()
{
    return this->m_Plane2SagitalSegments;
}

    
QmitkMultiWidgetLayoutManager* AmosWidget::GetMultiWidgetLayoutManager() const
{
    return m_LayoutManager;
}

// void AmosWidget::SetWidgetPlanesLocked(bool locked)
// {
//   // do your job and lock or unlock slices.
//   GetRenderWindowAC()->GetSliceNavigationController()->SetSliceLocked(locked);
//   GetRenderWindowBC()->GetSliceNavigationController()->SetSliceLocked(locked);
//   GetRenderWindowPatient()->GetSliceNavigationController()->SetSliceLocked(locked);
// }
