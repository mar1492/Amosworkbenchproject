#ifndef AMOSWIDGET_H
#define AMOSWIDGET_H

#include <QmitkAbstractMultiWidget.h>
#include <MitkQtWidgetsExports.h>
#include "myqmitkrenderwindow.h"
#include <mitkNodePredicateOr.h>
#include "myqmitkmultiwidgetlayoutmanager.h"
// JCAM
#include <QmitkMxNMultiWidget.h>

/// JCAM
// Define rows of each view
#define AXIAL_ROW_0         0
#define AXIAL_ROW_1         1
#define SAGITTAL_ROW_0      2
#define SAGITTAL_ROW_1      3
#define CORONAL_ROW_0       4
#define CORONAL_ROW_1       5


// 
// #include <QWidget>
// #include <QSplitter>
// #include <QMouseEvent>
// #include <QMoveEvent>
// #include <QWheelEvent>
// 
// // #include <QmitkRenderWindow.h>
// #include <QmitkLevelWindowWidget.h>
// #include <mitkStandaloneDataStorage.h>
// #include <mitkMouseModeSwitcher.h>
// #include <QmitkSliderNavigatorWidget.h>
// #include <QmitkStepperAdapter.h>
// // #include <mitkPositionTracker.h>
// // #include <mitkLogoOverlay.h>
// // #include <mitkCoordinateSupplier.h>
// #include <mitkLevelWindowManager.h>
// #include <mitkNodePredicateOr.h>
// #include <mitkNodePredicateProperty.h>
// #include <mitkNodePredicateAnd.h>
// #include <mitkNodePredicateNot.h>
// 
// class QHBoxLayout;
// class QVBoxLayout;
// class QGridLayout;
// class QSpacerItem;
// // class QmitkLevelWindowWidget;
// class QmitkRenderWindow;
// class vtkCornerAnnotation;
// class vtkMitkRectangleProp;

class AmosWidget : public QmitkMxNMultiWidget //public QmitkAbstractMultiWidget
{
    Q_OBJECT
public:
//     AmosWidget(QWidget* parent = 0, Qt::WindowFlags f = 0, mitk::RenderingManager* renderingManager = 0, mitk::BaseRenderer::RenderingMode::Type renderingMode = mitk::BaseRenderer::RenderingMode::Standard, const QString& name = "AmosMulti");
    AmosWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags(), const QString& name = "stdmulti");    
    virtual ~AmosWidget() override;
    
    virtual void InitializeMultiWidget() override;
    virtual QmitkRenderWindow* GetRenderWindow(const QString& widgetName) const override;
    virtual QmitkRenderWindow* GetRenderWindow(const mitk::BaseRenderer::ViewDirection& viewDirection) const override;
    virtual QmitkRenderWindow *GetRenderWindow(unsigned int number) const;
//     void SetDataStorages(mitk::StandaloneDataStorage::Pointer dsAC, mitk::StandaloneDataStorage::Pointer dsBC);
    virtual void SetDataStorage(mitk::DataStorage* dataStorage) override;
    
//     virtual void ActivateMenuWidget(bool state) override;
    
//     void EnablePositionTracking();
//     void DisablePositionTracking();
    
    void AddPlanesToDataStorage();
    // called when the StdMultiWidget is closed to remove the 3 widget planes and the helper node from the DataStorage
    void RemovePlanesFromDataStorage();    
    
//     void RequestUpdate();
//     void RequestUpdate(const QString& widgetName);
//     void ForceImmediateUpdate();
//     void ForceImmediateUpdate(const QString& widgetName);
    
    virtual void SetSelectedPosition(const mitk::Point3D& newPosition, const QString& widgetName) override;
    virtual const mitk::Point3D GetSelectedPosition(const QString& widgetName) const override;
    virtual void SetCrosshairVisibility(bool visible) override;
    virtual bool GetCrosshairVisibility() const override;
    virtual void ResetCrosshair() override;
    virtual void SetWidgetPlaneMode(int mode) override;
    
    MyQmitkRenderWindow* GetRenderWindowAC() const;
    MyQmitkRenderWindow* GetRenderWindowBC() const;
    MyQmitkRenderWindow* GetRenderWindowPatient() const;
    MyQmitkRenderWindow* GetRenderWindow3D() const;
    
    /**
     * @brief JCAM. Render 2Axial and 2Satigal with segments
     **/
    MyQmitkRenderWindow* GetRenderWindowCoronal() const;
    MyQmitkRenderWindow* GetRenderWindowSagittal() const;
    
//     QmitkRenderWindow* GetRenderWindowAC() const;
//     QmitkRenderWindow* GetRenderWindowBC() const;
//     QmitkRenderWindow* GetRenderWindowPatient() const;
//     QmitkRenderWindow* GetRenderWindow3D() const;    
    
//     bool IsColoredRectanglesEnabled() const;
//     bool IsDepartmentLogoEnabled() const;
//     virtual bool IsMenuWidgetEnabled() const override;
    
//     mitk::MouseModeSwitcher* GetMouseModeSwitcher();
//     void SetDepartmentLogoPath( const char * path );
    
    mitk::DataNode::Pointer GetWidgetPlaneAC();
    mitk::DataNode::Pointer GetWidgetPlaneBC();
    mitk::DataNode::Pointer GetWidgetPlanePatient();
    
    /** 
     * @brief Get panels for Axial and Sagital with segments
     */
    mitk::DataNode::Pointer GetWidgetPlaneCoronal();
    mitk::DataNode::Pointer GetWidgetPlaneSagittal();
    
//     std::pair<mitk::Color, mitk::Color> GetGradientColors(unsigned int widgetNumber);
    
//     bool GetGradientBackgroundFlag() const;
    
  /**
   * @brief FillGradientBackgroundWithBlack Internal helper method to initialize the
   * gradient background colors with black.
   */
//     void FillGradientBackgroundWithBlack();
    
  /**
   * @brief SetGradientBackgroundColorForRenderWindow background for a widget.
   *
   * If two different input colors are, a gradient background is generated.
   *
   * @param upper Upper color of the gradient background.
   * @param lower Lower color of the gradient background.
   * @param widgetNumber The widget (0-3).
   */
//     void SetGradientBackgroundColorForRenderWindow(const mitk::Color &upper, const mitk::Color &lower, unsigned int widgetNumber); 
    
  /**
   * @brief SetDecorationColor Set the color of the decoration of the 4 widgets.
   *
   * This is used to color the frame of the renderwindow and the corner annatation.
   * For the first 3 widgets, this color is a property of the helper object nodes
   * which contain the respective plane geometry. For widget 4, this is a member,
   * since there is no data node for this widget.
   */
  void SetDecorationColor(unsigned int widgetNumber, mitk::Color color); 
  
  void SetSegmentationColor(unsigned int widgetNumber, mitk::Color color); 
  
  /**
   * @brief GetDecorationColorForWidget Get the color for annotation, crosshair and rectangle.
   * @param widgetNumber Number of the renderwindow (0-3).
   * @return Color in mitk format.
   */
    mitk::Color GetDecorationColor(unsigned int widgetNumber);
    
    mitk::Color GetSegmentationColor(unsigned int widgetNumber);
  
  /**
   * @brief SetCornerAnnotation Create a corner annotation for a widget.
   * @param text The text of the annotation.
   * @param color The color.
   * @param widgetNumber The widget (0-3).
   */
//     void SetDecorationProperties(std::string text, mitk::Color color, int widgetNumber);
    
    /**
    * @brief GetRenderWindow convinience method to get a widget.
    * @param number of the widget (0-3)
    * @return The renderwindow widget.
    */
//     QmitkRenderWindow *GetRenderWindow(unsigned int number); 
    
    /**
    * @brief GetCornerAnnotationText Getter for corner annotation text.
    * @param widgetNumber the widget number (0-3).
    * @return The text in the corner annotation.
    */
//     std::string GetCornerAnnotationText(unsigned int widgetNumber);   
    
//     void UpdateAllWidgets();
//     void HideAllWidgetToolbars();
//     QmitkLevelWindowWidget* GetLevelWindowWidget() { return levelWindowWidget; }        
        ///
        void CreateAxialRenderWindowWidgets();    
        void CreateSagittalRenderWindowWidgets();
        void CreateCoronalRenderWindowWidgets();

public slots:
//     void OnLayoutDesignChanged(int layoutDesignIndex);
//     void ResetViews();
    
//     void EnableNavigationControllerEventListening();
//     void DisableNavigationControllerEventListening();

    void Fit();
//     void InitPositionTracking();
    void AddDisplayPlaneSubTree();
//     void EnableStandardLevelWindow();
//     void DisableStandardLevelWindow();
//     void EnableColoredRectangles();
//     void DisableColoredRectangles();
//     void EnableDepartmentLogo();
//     void DisableDepartmentLogo();    
    
//     void changeLayoutTo2DImagesUp();
// 
//     void changeLayoutTo2DImagesLeft();
// 
//     void changeLayoutToDefault();
// 
//     void changeLayoutToBig3D();
// 
//     void changeLayoutToWidget1();
// 
//     void changeLayoutToWidget2();
// 
//     void changeLayoutToWidget3();
// 
//     void changeLayoutToRowWidget3And4();
// 
//     void changeLayoutToColumnWidget3And4();
// 
//     void changeLayoutToRowWidgetSmall3andBig4();
// 
//     void changeLayoutToSmallUpperWidget2Big3and4();
// 
//     void changeLayoutTo2x2Dand3DWidget();
// 
//     void changeLayoutToLeft2Dand3DRight2D();
// 
//     void changeLayoutTo2DUpAnd3DDown();
// 
//     void changeLayoutToLeftACandRightBC();
//     
//     void changeLayoutToUpACandDownBC();
//     
//     void EnableGradientBackground();
//     void DisableGradientBackground();
    
    void EnsureDisplayContainsPoint(mitk::BaseRenderer *renderer, const mitk::Point3D&p);

//     void MouseModeSelected( mitk::MouseModeSwitcher::MouseMode mouseMode );
//     void SetWidgetPlanesLocked(bool locked);
    
    void SetWidgetPlaneVisibility(const char* widgetName, bool visible, mitk::BaseRenderer *renderer=NULL);
    void SetWidgetPlanesVisibility(bool visible, mitk::BaseRenderer *renderer=NULL); 
    
    QmitkMultiWidgetLayoutManager* GetMultiWidgetLayoutManager() const override;
    
    virtual void wheelEvent( QWheelEvent * e ) override;
    virtual void mousePressEvent(QMouseEvent * e) override;
    virtual void moveEvent( QMoveEvent* e ) override;
//     void mouseDoubleClickEvent( QMouseEvent *event ) override;     
    
protected:
    void changeSegmentationColor(int widgetNum);
    
protected slots:
    void onNodeAdded(const mitk::DataNode* node);
    void onNodeRemoved(const mitk::DataNode* node);
//     void setLevelWindowFixed(bool levelWindowFixed);       
    
signals:
//     void LeftMouseClicked(mitk::Point3D pointValue);
    void WheelMoved(QWheelEvent*); 
    void Moved();
    void sendSegmentationNodeAdded(const mitk::DataNode*);
    void sendSegmentationNodeRemoved(const mitk::DataNode*);

// public:	    
//     enum {AXIAL, SAGITTAL, CORONAL,THREE_D};
    
protected:
   virtual void SetLayoutImpl() override;
   virtual void SetInteractionSchemeImpl() override { }
 
//    void CreateAxialRenderWindowWidgets();    
//    void CreateSagittalRenderWindowWidgets();
//    void CreateCoronalRenderWindowWidgets();

protected:
    
  /**
   * @brief m_PlaneNode1 the 3 helper objects which contain the plane geometry.
   */    
    mitk::DataNode::Pointer m_PlaneNodeAC;
    mitk::DataNode::Pointer m_PlaneNodeBC;
    mitk::DataNode::Pointer m_PlaneNodePatient;
    
    /**
     * @brief JCAM. New planes
     */
    mitk::DataNode::Pointer m_PlaneNodeCoronal;
    mitk::DataNode::Pointer m_PlaneNodeSagittal;

    /**
    * @brief m_ParentNodeForGeometryPlanes This helper object is added to the datastorage
    * and contains the 3 planes for displaying the image geometry (crosshair and 3D planes).
    */
    mitk::DataNode::Pointer m_ParentNodeForGeometryPlanes;    
    mitk::Color m_DecorationColorWidget4;
    
    mitk::Color m_SegmentationColor[2];
    
    QList<const mitk::DataNode*> segmentationNodes; 
    
    mitk::NodePredicateOr::Pointer m_IsASegmentationImagePredicate;
    
    MyQmitkMultiWidgetLayoutManager* m_LayoutManager;
    
//     QBoxLayout* layout;
//     int m_Layout;
//     
//     QWidget *renderWindowACContainer;
//     QWidget *renderWindowBCContainer;
//     QWidget *renderWindowPatientContainer;
//     QWidget *renderWindow3DContainer;
// //     QWidget *emptyContainer;
    
//     MyQmitkRenderWindow* renderWindowAC;
//     MyQmitkRenderWindow* renderWindowBC;
//     MyQmitkRenderWindow* renderWindowPatient;
//     MyQmitkRenderWindow* renderWindow3D;
//         
//     vtkSmartPointer<vtkCornerAnnotation> m_CornerAnnotations[4];
//     vtkSmartPointer<vtkMitkRectangleProp> m_RectangleProps[4];    
//     
//     QHBoxLayout* layoutUp;
//     QHBoxLayout* layoutDown;
//     QSplitter* splitter;
//     QSplitter* splitterVert;
//     QSplitter* splitterUp;
//     QSplitter* splitterDown;
        
    
// //     mitk::DataStorage::Pointer dsAC;
// //     mitk::DataStorage::Pointer dsBC;
//     mitk::DataStorage::Pointer ds;
//     mitk::RenderingManager* m_RenderingManager;
// //     mitk::PositionTracker::Pointer m_PositionTracker;
// //     mitk::DataNode::Pointer m_PositionTrackerNode;
    
//  //    MyMitkLevelWindowWidget* levelWindowWidget;
// //     MyMitkLevelWindowWidget* levelWindowWidgetBC;
//     QmitkLevelWindowWidget* levelWindowWidget;
// //    QmitkSliderNavigatorWidget* navigator;
// //     mitk::SliceNavigationController* sliceNaviController;
//     mitk::MouseModeSwitcher::Pointer m_MouseModeSwitcher;
// //     QmitkStepperAdapter* stepperAdapter;
//     QLabel* labelNavigator;
//     mitk::LevelWindow m_LevelWindow;
// 
//     bool m_GradientBackgroundFlag;
  /**
   * @brief m_GradientBackgroundColors Contains the colors of the gradient background.
   *
   */    
//     std::pair<mitk::Color, mitk::Color> m_GradientBackgroundColors[4];
    
  /**
   * @brief m_DecorationColorWidget4 color for annotation and rectangle of widget 4.
   *
   * For other widgets1-2, the color is a property of the respective data node.
   * There is no node for widget 4, hence, we need an extra member.
   */
//  //     mitk::Color m_DecorationColorWidget3;

//     mitk::CoordinateSupplier::Pointer m_LastLeftClickPositionSupplier;
//     mitk::LogoOverlay::Pointer m_LogoRendering;
//     
//     mitk::NodePredicateOr::Pointer m_IsOfTypeImagePredicate;
//     mitk::NodePredicateProperty::Pointer m_IsBinaryPredicate;    
//     mitk::NodePredicateAnd::Pointer m_IsABinaryImagePredicate;
//     mitk::NodePredicateAnd::Pointer m_IsNotABinaryImagePredicate;
//     mitk::NodePredicateNot::Pointer m_IsNotBinaryPredicate;
//     
//     mitk::NodePredicateOr::Pointer m_IsASegmentationImagePredicate;
//     mitk::NodePredicateAnd::Pointer m_IsAPatientImagePredicate;    
//     
//     

};

#endif // AMOSWIDGET_H
