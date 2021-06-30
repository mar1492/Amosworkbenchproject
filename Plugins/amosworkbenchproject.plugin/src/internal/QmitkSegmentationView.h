/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#ifndef QmitkSegmentationView_h
#define QmitkSegmentationView_h

// #include "QmitkFunctionality.h"
#include <mitkILinkedRenderWindowPart.h>
#include <QmitkAbstractView.h>
#include <mitkIZombieViewPart.h>

#include <berryIPartListener.h>
#include <berryIWindowListener.h>

#include <berryIBerryPreferences.h>

#include "ui_QmitkSegmentationControls.h"

class QmitkRenderWindow;

namespace mitk {

    struct IRenderWindowPart;
    struct IRenderWindowPartListener;
    struct IZombieViewPart;
    struct ILifecycleAwarePart;

}

/**
* \ingroup ToolManagerEtAl
* \ingroup org_mitk_gui_qt_segmentation_internal
* \warning Implementation of this class is split up into two .cpp files to make things more compact. Check both this file and QmitkSegmentationOrganNamesHandling.cpp.cpp
*/
class QmitkSegmentationView :public QmitkAbstractView, public mitk::ILifecycleAwarePart/*, public mitk::ILinkedRenderWindowPart*//*, public mitk::IZombieViewPart,public berry::IPartListener*//*, private berry::IWindowListener*/
{
   Q_OBJECT

public:

   QmitkSegmentationView();

   virtual ~QmitkSegmentationView();

   typedef std::map<mitk::DataNode*, unsigned long> NodeTagMapType;
   
//    enum Segmentation_Type {Non_Amos, Amos_AC, Amos_BC};

   /*!
   \brief Invoked when the DataManager selection changed
   */
   virtual void OnSelectionChanged(mitk::DataNode* node);
   virtual void OnSelectionChanged(/*berry::IWorkbenchPart::Pointer source, */std::vector<mitk::DataNode*> nodes) ;
   virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer source, const QList<mitk::DataNode::Pointer> &nodes) override;

   // reaction to new segmentations being created by segmentation tools
   void NewNodesGenerated();
   void NewNodeObjectsGenerated(mitk::ToolManager::DataVectorType*);

   // ILifecycleAwarePart (old QmitkFunctionality's) activate/deactivate
   virtual void Activated() override;
   virtual void Deactivated()  override {};
   virtual void Visible()  override;
   virtual void Hidden()  override;
   virtual bool IsActivated();
   
//    void PartActivated (const berry::IWorkbenchPartReference::Pointer& partRef) override;
//    void PartDeactivated(const berry::IWorkbenchPartReference::Pointer& /*partRef*/) override;
//    void PartVisible(const berry::IWorkbenchPartReference::Pointer& /*partRef*/) override;
//    berry::IPartListener::Events::Types GetPartEventTypes() const override;

   // QmitkFunctionality's changes regarding THE QmitkStdMultiWidget
//    virtual void StdMultiWidgetAvailable(mitk::ILinkedRenderWindowPart& stdMultiWidget) ;
//    virtual void StdMultiWidgetNotAvailable() ;
//    virtual void StdMultiWidgetClosed(mitk::ILinkedRenderWindowPart& stdMultiWidget) ;

   // BlueBerry's notification about preference changes (e.g. from a dialog)
   virtual void OnPreferencesChanged(const berry::IBerryPreferences* prefs) ;

   // observer to mitk::RenderingManager's RenderingManagerViewsInitializedEvent event
   void RenderingManagerReinitialized();

   // observer to mitk::SliceController's SliceRotation event
   void SliceRotation(const itk::EventObject&);

   static const std::string VIEW_ID;

   protected slots:

      void OnPatientComboBoxSelectionChanged(const mitk::DataNode* node);
      void OnSegmentationComboBoxSelectionChanged(const mitk::DataNode* node);
      void OnBCSegmentationComboBoxSelectionChanged(const mitk::DataNode* node);

      // reaction to the button "New segmentation"
      void CreateNewSegmentation();

      void OnOnlyInACTool2DSelected(int id);

      void OnWorkingNodeVisibilityChanged();

      // called if a node's binary property has changed
      void OnBinaryPropertyChanged();

      void OnShowMarkerNodes(bool);

//       void OnTabWidgetChanged(int);

protected:

   // a type for handling lists of DataNodes
   typedef std::vector<mitk::DataNode*> NodeList;

   // set available multiwidget
   void SetMultiWidget(mitk::ILinkedRenderWindowPart* multiWidget);

   // actively query the current selection of data manager
   //void PullCurrentDataManagerSelection();

   // reactions to selection events from data manager (and potential other senders)
   //void BlueBerrySelectionChanged(berry::IWorkbenchPart::Pointer sourcepart, berry::ISelection::ConstPointer selection);
   mitk::DataNode::Pointer FindFirstRegularImage( std::vector<mitk::DataNode*> nodes );
   mitk::DataNode::Pointer FindFirstSegmentation( std::vector<mitk::DataNode*> nodes );

   // propagate BlueBerry selection to ToolManager for manual segmentation
   void SetToolManagerSelection(const mitk::DataNode* referenceData, const mitk::DataNode* workingData, const mitk::DataNode* workingBCData);

   // checks if given render window aligns with the slices of given image
   bool IsRenderWindowAligned(QmitkRenderWindow* renderWindow, mitk::Image* image);

   // make sure all images/segmentations look as selected by the users in this view's preferences
   void ForceDisplayPreferencesUponAllImages();

   // decorates a DataNode according to the user preference settings
   void ApplyDisplayOptions(mitk::DataNode* node);

   // GUI setup
   void CreateQtPartControl(QWidget* parent) ;

   void ResetMouseCursor( );

   void SetMouseCursor(const us::ModuleResource&, int hotspotX, int hotspotY );

   void SetToolSelectionBoxesEnabled(bool);

   bool m_MouseCursorSet;

   // handling of a list of known (organ name, organ color) combination
   // ATTENTION these methods are defined in QmitkSegmentationOrganNamesHandling.cpp
   QStringList GetDefaultOrganColorString();
   void UpdateOrganList(QStringList& organColors, const QString& organname, mitk::Color colorname);
   void AppendToOrganList(QStringList& organColors, const QString& organname, int r, int g, int b);

   // If a contourmarker is selected, the plane in the related widget will be reoriented according to the marker`s geometry
   void OnContourMarkerSelected (const mitk::DataNode* node);

   void NodeRemoved(const mitk::DataNode* node) ;

   void NodeAdded(const mitk::DataNode *node) ;

   bool CheckForSameGeometry(const mitk::DataNode*, const mitk::DataNode*) const;

   void UpdateWarningLabel(QString text/*, bool overwriteExistingText = true*/);
   
   void SetFocus() override;
   void ClassifyACBC();
   
   void DataStorageModified();
   
//    Segmentation_Type AmosSegmentationType(const mitk::DataNode* node);

   // the Qt parent of our GUI (NOT of this object)
   QWidget* m_Parent;

   // our GUI
   Ui::QmitkSegmentationControls * m_Controls;

   // THE currently existing QmitkStdMultiWidget
//    mitk::ILinkedRenderWindowPart * m_MultiWidget;

   unsigned long m_VisibilityChangedObserverTag;

   bool m_DataSelectionChanged;

   NodeTagMapType  m_WorkingDataObserverTags;

   NodeTagMapType  m_BinaryPropertyObserverTags;

   unsigned int m_RenderingManagerObserverTag;

   bool m_AutoSelectionEnabled;

   mitk::NodePredicateOr::Pointer m_IsOfTypeImagePredicate;
   mitk::NodePredicateProperty::Pointer m_IsBinaryPredicate;
   mitk::NodePredicateNot::Pointer m_IsNotBinaryPredicate;
   mitk::NodePredicateAnd::Pointer m_IsNotABinaryImagePredicate;
   mitk::NodePredicateAnd::Pointer m_IsABinaryImagePredicate;
   
   mitk::NodePredicateOr::Pointer m_IsASegmentationImagePredicate;
   mitk::NodePredicateAnd::Pointer m_IsAPatientImagePredicate;   

   mitk::NodePredicateProperty::Pointer m_IsACPredicate;
   mitk::NodePredicateProperty::Pointer m_IsBCPredicate;   
   mitk::NodePredicateAnd::Pointer m_IsACSegmentationPredicate;
   mitk::NodePredicateAnd::Pointer m_IsBCSegmentationPredicate;   
   
   bool m_Active;
   
   QStringList ACandBCToolsList;
    
private:
    mitk::IRenderWindowPart* m_ActiveRenderWindowPart;

/*  void RenderWindowPartActivated(mitk::IRenderWindowPart* renderPart);
  void RenderWindowPartDeactivated(mitk::IRenderWindowPart* renderPart);  

    mitk::IZombieViewPart* m_ActiveZombieView;
    mitk::IRenderWindowPart* m_ActiveRenderWindowPart;
    mitk::IRenderWindowPart* m_VisibleRenderWindowPart;
    QSet<mitk::IRenderWindowPartListener*> m_RenderWindowListeners; */ 
};

#endif /*QMITKsegmentationVIEW_H_*/
