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

#ifndef MyQmitkRenderWindowMenu_h
#define MyQmitkRenderWindowMenu_h

#if defined(_WIN32) || defined(__APPLE__)
#define QMITK_USE_EXTERNAL_RENDERWINDOW_MENU
#endif

// mitk qtwidgets module
#include <MitkQtWidgetsExports.h>
#include <QmitkMultiWidgetLayoutManager.h>

// mitk core
#include <mitkBaseRenderer.h>
// qt
#include <QAction>
#include <QEvent>
#include <QLabel>
#include <QMenuBar>
#include <QPushButton>
#include <QTimer>
#include <QToolButton>
#include <QWidget>

// class QmitkStdMultiWidget;

/**
 * \ingroup QmitkModule
 * \brief The QmitkRenderWindowMenu is a popup Widget which shows
 * up when the mouse curser enter a QmitkRenderWindow.
 * The Menu Widget is located in the right top corner of each
 * RenderWindow. It includes different settings. For example
 * the layout design can be changed with the setting button. Switching
 * between full-screen mode and layout design can be done
 * with the full-screen button. Splitting the Widget horizontal or
 * vertical as well closing the Widget is not implemented yet.
 * The popup Widget can be deactivated with ActivateMenuWidget(false) in
 * QmitkRenderWindow.
 *
 * \sa QmitkRenderWindow
 *
 */
// class AMOSWORKBENCHPROJECT_RENDERWINDOWEDITOR MyQmitkRenderWindowMenu : public QWidget
class MyQmitkRenderWindowMenu : public QWidget
{
    Q_OBJECT

public:
    // #if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    //   MyQmitkRenderWindowMenu( QWidget* parent = 0, Qt::WindowFlags f = 0, mitk::BaseRenderer * b = 0/*, QmitkStdMultiWidget* mw = 0*/ );
    // #else
    //   MyQmitkRenderWindowMenu( QWidget* parent = 0, Qt::WFlags f = 0, mitk::BaseRenderer * b = 0/*, QmitkStdMultiWidget* mw = 0*/ );
    // #endif
    using LayoutIndex = mitk::BaseRenderer::ViewDirection;
    using LayoutDesign = QmitkMultiWidgetLayoutManager::LayoutDesign;
    
//     enum class LayoutDesign
//     {
//         DEFAULT = 0,
//         ALL_2D_TOP_3D_BOTTOM,
//         ALL_2D_LEFT_3D_RIGHT,
//         ONE_BIG,
//         ONLY_2D_HORIZONTAL,
//         ONLY_2D_VERTICAL,
//         ONE_TOP_3D_BOTTOM,
//         ONE_LEFT_3D_RIGHT,
//         ALL_HORIZONTAL,
//         ALL_VERTICAL,
//         REMOVE_ONE,
//         NONE,
//         LEFT_AC_AND_RIGHT_BC,
//         AC_UP_AND_BC_DOWN     
//     };
    
    
    MyQmitkRenderWindowMenu(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags(), mitk::BaseRenderer *baseRenderer = nullptr);    
    virtual ~MyQmitkRenderWindowMenu() override;
    
    /*! Return visibility of settings menu. The menu is connected with m_SettingsButton and includes
     *  layout direction (axial AC, axial patient .. ) and layout design (standard layout, 2D images top,
     *  3D bottom ... ). */
    //   bool GetSettingsMenuVisibilty()
    //   {
    //     if( m_Settings == NULL)
    //       return false;
    //     else
    //       return m_Settings->isVisible();
    //   }
    
    bool GetSettingsMenuVisibilty()
    {
        if (m_LayoutActionsMenu == nullptr)
            return false;
        else
            return m_LayoutActionsMenu->isVisible();
    }
    
    /*! Set layout index. Defines layout direction (axial AC, axial patient, axial BC or threeD) of the parent. */
    void SetLayoutIndex(LayoutIndex layoutIndex);
    
    /*! Return layout direction of parent (axial AC, axial patient, axial BC or threeD) */
    LayoutIndex GetLayoutIndex()
    {  return m_Layout;  }
    
    /*! Update list of layout design (standard layout, 2D images top, 3D bottom ..). Set action of current layout design
     *  to disable and all other to enable. */
    void UpdateLayoutDesignList(LayoutDesign layoutDesign );
    
    /*! Move menu widget to correct position (right upper corner). E.g. it is necessary when the full-screen mode
     *  is activated.*/
    #ifdef QMITK_USE_EXTERNAL_RENDERWINDOW_MENU
    void MoveWidgetToCorrectPos(float opacity);
    #else
    void MoveWidgetToCorrectPos(float /*opacity*/);
    #endif
    
    void ShowMenu();
    void HideMenu();
    
    //   void ChangeFullScreenMode( bool state );
    // 
    //   void NotifyNewWidgetPlanesMode( int mode );
    
protected:
    
    /*! Create menu widget. The menu contains five QPushButtons (hori-split, verti-split, full-screen, settings and close button)
     *  and their signal/slot connection for handling.  */
    void CreateMenuWidget();
    
    
    /*! Create settings menu which contains layout direction and the different layout designs. */
    void CreateSettingsWidget();
    
    /*! Reimplemented from QWidget. The paint event is a request to repaint all or part of a widget.*/
    void paintEvent(QPaintEvent *event) override;
    
    /*! Update list of layout direction (axial AC, axial patient, axial BC or threeD). Set action of currect layout direction
     *  to disable and all other to enable. Normaly the user can switch here between the different layout direction, but
     *  this is not supported yet. */
    //   void UpdateLayoutList();
    
    /*! Change Icon of full-screen button depending on full-screen mode. */
    void ChangeFullScreenIcon();
    
    
    //   int currentCrosshairRotationMode;
    
//     public slots:    
//         void CrosshairVisibility( bool state ) ;
    
    
Q_SIGNALS:
    
    void ResetView(); // == "global reinit"
    
    // \brief int parameters are enum from QmitkStdMultiWidget
    //   void ChangeCrosshairRotationMode(int);
    
    /*! emit signal, when layout design changed by the setting menu.*/
    void LayoutDesignChanged(LayoutDesign layoutDesign);
    
public Q_SLOTS:
    
    void DeferredHideMenu( );
    void DeferredShowMenu( );
    void smoothHide( );
    
protected Q_SLOTS:
    
    ///
    /// this function is continously called by a timer
    /// to do the auto rotation
    ///
    void AutoRotateNextStep();
    ///
    /// this function is invoked when the auto-rotate action
    /// is clicked
    ///
    void OnAutoRotationActionTriggered();
    
    void enterEvent( QEvent* /*e*/ ) override;
    void leaveEvent( QEvent* /*e*/ ) override;
    void OnTSNumChanged(int);
    
    //   void OnCrosshairRotationModeSelected(QAction*);
    
    /*! slot for activating/deactivating the full-screen mode. The slot is connected to the clicked() event of m_FullScreenButton.
     *  Activating the full-screen maximize the current widget, deactivating restore If layout design changed by the settings menu,
     *  the full-Screen mode is automatically switch to false. */
    void OnFullScreenButton( bool checked );
    
    /*! Slot for opening setting menu. The slot is connected to the clicked() event of m_SettingsButton.
     *  The settings menu includes differen layout directions (axial AC, axial patient, axial BC and 3D) as well all layout design
     *  (standard layout, 2D images top, 3D bottom ..)*/
    //   void OnSettingsButton( bool checked );
    void OnLayoutDesignButton( bool checked );
    
    void OnSetLayout(LayoutDesign layoutDesign);
    
    //   /*! Slot for changing layout design to standard layout. The slot is connected to the triggered() signal of m_DefaultLayoutAction. */
    //   void OnChangeLayoutToDefault(bool);
    // 
    //   /*! Slot for changing layout design to 2D images top, 3D bottom layout. The slot is connected to the triggered() signal of m_2DImagesUpLayoutAction. */
    //   void OnChangeLayoutTo2DImagesUp(bool);
    // 
    //   /*! Slot for changing layout design to 2D images left, 3D right layout. The slot is connected to the triggered() signal of m_2DImagesLeftLayoutAction. */
    //   void OnChangeLayoutTo2DImagesLeft(bool);
    // 
    //   /*! Slot for changing layout to Big 3D layout. The slot is connected to the triggered() signal of m_Big3DLayoutAction. */
    //   void OnChangeLayoutToBig3D(bool);
    // 
    //   /*! Slot for changing layout design to Axial AC plane layout. The slot is connected to the triggered() signal of m_Widget1LayoutAction. */
    //   void OnChangeLayoutToWidget1(bool);
    // 
    //   /*! Slot for changing layout design to Axial BC plane layout. The slot is connected to the triggered() signal of m_Widget2LayoutAction. */
    //   void OnChangeLayoutToWidget2(bool);
    // 
    //   /*! Slot for changing layout design to Axial Patient plane layout. The slot is connected to the triggered() signal of m_Widget3LayoutAction. */
    //   void OnChangeLayoutToWidget3(bool);
    // 
    //   /*! Slot for changing layout design to Axial Patient top, 3D bottom layout. The slot is connected to the triggered() signal of m_RowWidget3And4LayoutAction. */
    //   void OnChangeLayoutToRowWidget3And4(bool);
    // 
    //   /*! Slot for changing layout design to Axial Patient left, 3D right layout. The slot is connected to the triggered() signal of m_ColumnWidget3And4LayoutAction. */
    //   void OnChangeLayoutToColumnWidget3And4(bool);
    // 
    //   /*! Slot for changing layout design to Axial BC top, Axial Patient n 3D bottom layout. The slot is connected to the triggered() signal of m_SmallUpperWidget2Big3and4LayoutAction. */
    //   void OnChangeLayoutToSmallUpperWidget2Big3and4(bool);
    // 
    //   /*! Slot for changing layout design to Axial AC n Axial BC left, 3D right layout. The slot is connected to the triggered() signal of m_2x2Dand3DWidgetLayoutAction. */
    //   void OnChangeLayoutTo2x2Dand3DWidget(bool);
    // 
    //   /*! Slot for changing layout design to Axial AC n 3D left, Axial BC right layout. The slot is connected to the triggered() signal of m_Left2Dand3DRight2DLayoutAction. */
    //   void OnChangeLayoutToLeft2Dand3DRight2D(bool);
    //   
    //   void OnChangeLayoutTo2DUp3DDown(bool);
    //   
    //   /*! Slot for changing layout design to Axial AC left, Axial BC right layout. The slot is connected to the triggered() signal of m_Left2Dand3DRight2DLayoutAction. */
    //   void OnChangeLayoutToLeftACandRightBC(bool);  
    //   
    //   /*! Slot for changing layout design to Axial AC up, Axial BC down layout. The slot is connected to the triggered() signal of m_Left2Dand3DRight2DLayoutAction. */
    //   void OnChangeLayoutToUpACandDownBC(bool);  
    
//     void OnCrossHairMenuAboutToShow();
    
    // public:
    
    /*! enum for layout direction*/
    //   enum
    //   {
    //     AXIAL,
    //     SAGITTAL,
    //     CORONAL,
    //     THREE_D
    //   };
    
    /*! enum for layout design */
    //   enum
    //   {
    //     LAYOUT_DEFAULT,
    //     LAYOUT_2DIMAGEUP,
    //     LAYOUT_2DIMAGELEFT,
    //     LAYOUT_BIG3D,
    //     LAYOUT_AXIAL,
    //     LAYOUT_SAGITTAL,
    //     LAYOUT_CORONAL,
    //     LAYOUT_2X2DAND3DWIDGET,
    //     LAYOUT_ROWWIDGET3AND4,
    //     LAYOUT_COLUMNWIDGET3AND4,
    //     LAYOUT_ROWWIDGETSMALL3ANDBIG4, //not in use in this class, but we need it here to synchronize with the SdtMultiWidget.
    //     LAYOUT_SMALLUPPERWIDGET2BIGAND4,
    //     LAYOUT_LEFT2DAND3DRIGHT2D,
    //     LAYOUT_2D_UP_AND_3D_DOWN,
    //     LEFT_AC_AND_RIGHT_BC,
    //     AC_UP_AND_BC_DOWN
    //   };
    
protected:
    
    //   QPushButton*        m_CrosshairModeButton;
    
    //QAction*            m_ShowHideCrosshairVisibilityAction;
    
    /*! QPushButton for activating/deactivating full-screen mode*/
    QToolButton*        m_FullScreenButton;
    
    /*! QPushButton for open the settings menu*/
    //   QToolButton*        m_SettingsButton;
    QToolButton* m_LayoutDesignButton;
    
    // 
    //   /*! QAction for 2D images up layout design */
    //   QAction*            m_2DImagesUpLayoutAction;
    // 
    //   /*! QAction for 2D images left layout design */
    //   QAction*            m_2DImagesLeftLayoutAction;
    // 
    //   /*! QAction for big 3D layout design */
    //   QAction*            m_Big3DLayoutAction;
    // 
    //   /*! QAction for big axial AC layout design */
    //   QAction*            m_Widget1LayoutAction;
    // 
    //   /*! QAction for big axial BC layout design */
    //   QAction*            m_Widget2LayoutAction;
    // 
    //   /*! QAction for big axial patient layout design */
    //   QAction*            m_Widget3LayoutAction;
    // 
    //   /*! QAction for axial patient top, 3D bottom layout design */
    //   QAction*            m_RowWidget3And4LayoutAction;
    // 
    //   /*! QAction for axial patient left, 3D right layout design */
    //   QAction*            m_ColumnWidget3And4LayoutAction;
    // 
    //   /*! QAction for axial BC top, axial patient n 3D bottom layout design */
    //   QAction*            m_SmallUpperWidget2Big3and4LayoutAction;
    // 
    //   /*! QAction for axial AC n axial BC left, 3D right layout design */
    //   QAction*            m_2x2Dand3DWidgetLayoutAction;
    // 
    //   /*! QAction for axial AC n 3D left, axial BC right layout design*/
    //   QAction*            m_Left2Dand3DRight2DLayoutAction;
    //   
    //   /*! QAction for axial AC n 3D left, axial BC right layout design*/
    //   QAction*            m_2DUp3DDown; 
    //   
    
    /*! QAction for Default layout design */
    QAction* m_DefaultLayoutAction;  
    QAction* m_All2DTop3DBottomLayoutAction;
    QAction* m_All2DLeft3DRightLayoutAction;
    QAction* m_OneBigLayoutAction;
    QAction* m_Only2DHorizontalLayoutAction;
    QAction* m_Only2DVerticalLayoutAction;
    QAction* m_OneTop3DBottomLayoutAction;
    QAction* m_OneLeft3DRightLayoutAction;
    QAction* m_AllHorizontalLayoutAction;
    QAction* m_AllVerticalLayoutAction;
    QAction* m_RemoveOneLayoutAction;  
    
    /*! QAction for axial AC left, axial BC right layout design*/
    QAction* m_LeftACandRightBCLayoutAction;  
    
    /*! QAction for axial AC left, axial BC right layout design*/
    QAction* m_UpACandDownBCLayoutAction;  
    
    
    /*! JCAM. QAction for 2 axials with segments */
    QAction* m_2AxialsWithSegmentLayoutAction;
    /*! JCAM. QAction for 2 axials with segments */
    QAction* m_2SagittalsWithSegmentLayoutAction;
    /*! JCAM. QAction for 2 axials with segments */
    QAction* m_2CoronalsWithSegmentLayoutAction;
    
    //   QLabel *m_TSLabel;
    
    
    /*! QMenu containg all layout direction and layout design settings.*/
    //   QMenu*              m_Settings;
    QMenu* m_LayoutActionsMenu;
    
    //   QMenu*              m_CrosshairMenu;
    
    /*! Index of layout direction. 0: axial AC; 1: axial BC; 2: axial patient; 3: threeD */
    LayoutIndex m_Layout;
    
    /*! Index of layout design. 0: LAYOUT_DEFAULT; 1: LAYOUT_2DIMAGEUP; 2: LAYOUT_2DIMAGELEFT; 3: LAYOUT_BIG3D
     *  4: LAYOUT_AXIAL; 5: LAYOUT_SAGITTAL; 6: LAYOUT_CORONAL; 7: LAYOUT_2X2DAND3DWIDGET; 8: LAYOUT_ROWWIDGET3AND4;
     *  9: LAYOUT_COLUMNWIDGET3AND4; 10: LAYOUT_ROWWIDGETSMALL3ANDBIG4; 11: LAYOUT_SMALLUPPERWIDGET2BIGAND4; 12: LAYOUT_LEFT2DAND3DRIGHT2D; 13: LEFT_AC_AND_RIGHT_BC */
    LayoutDesign m_LayoutDesign;
    
    /*! Store index of old layout design. It is used e.g. for the full-screen mode, when deactivating the mode the former layout design will restore.*/
    LayoutDesign m_OldLayoutDesign;
    
    /*! Flag if full-screen mode is activated or deactivated. */
    bool m_FullScreenMode;
    
    //   bool                m_Entered;
    // 
    //   bool                m_Hidden;
    
private:
    
    mitk::BaseRenderer::Pointer m_Renderer;
    
    //   QmitkStdMultiWidget* m_MultiWidget;
    
    ///
    /// a timer for the auto rotate action
    ///
    //   QTimer m_AutoRotationTimer;
    
    QWidget* m_Parent;
};

#endif // MyQmitkRenderWindowMenu_H
