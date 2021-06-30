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

#include <QObject>

#include "utils.hpp"

#include <mitkBaseData.h>

#include <mitkProperties.h>
#include <mitkSegTool2D.h>
#include <mitkStatusBar.h>

#include <mitkILinkedRenderWindowPart.h>
#include <QmitkNewSegmentationDialog.h>
#include <QmitkRenderWindow.h>

#include <QMessageBox>

#include <berryIWorkbenchPage.h>

#include "QmitkSegmentationView.h"
// #include "QmitkSegmentationOrganNamesHandling.cpp"

#include <mitkSurfaceToImageFilter.h>
#include <myqmitkrenderwindow.h>

// #include <mitkVtkResliceInterpolationProperty.h>

#include <mitkApplicationCursor.h>
#include <mitkSegmentationObjectFactory.h>
#include "mitkPluginActivator.h"

#include <usModuleResource.h>
#include <usModuleResourceStream.h>

#include <mitkIRenderWindowPart.h>
#include <mitkIRenderWindowPartListener.h>
#include <mitkIZombieViewPart.h>

//micro service to get the ToolManager instance
#include <mitkToolManagerProvider.h>

#include <mitkLabelSetImage.h>
#include <mitkCameraController.h>

const std::string QmitkSegmentationView::VIEW_ID = "org.mitk.views.segmentation";
// public methods

QmitkSegmentationView::QmitkSegmentationView()
:m_MouseCursorSet(false)
,m_Parent(NULL)
,m_Controls(NULL)
,m_DataSelectionChanged(false)
//    ,m_ActiveZombieView(nullptr)
//    ,m_ActiveRenderWindowPart(nullptr)
//    ,m_VisibleRenderWindowPart(nullptr)
,m_Active(false)
{
    mitk::NodePredicateDataType::Pointer isDwi = mitk::NodePredicateDataType::New("DiffusionImage");
    mitk::NodePredicateDataType::Pointer isDti = mitk::NodePredicateDataType::New("TensorImage");
    mitk::NodePredicateDataType::Pointer isQbi = mitk::NodePredicateDataType::New("QBallImage");
    mitk::NodePredicateOr::Pointer isDiffusionImage = mitk::NodePredicateOr::New(isDwi, isDti);
    isDiffusionImage = mitk::NodePredicateOr::New(isDiffusionImage, isQbi);
    m_IsOfTypeImagePredicate = mitk::NodePredicateOr::New(isDiffusionImage, mitk::TNodePredicateDataType<mitk::Image>::New());
    
    m_IsBinaryPredicate = mitk::NodePredicateProperty::New("binary", mitk::BoolProperty::New(true));
    m_IsNotBinaryPredicate = mitk::NodePredicateNot::New( m_IsBinaryPredicate );
    
    m_IsNotABinaryImagePredicate = mitk::NodePredicateAnd::New( m_IsOfTypeImagePredicate, m_IsNotBinaryPredicate );
    m_IsABinaryImagePredicate = mitk::NodePredicateAnd::New( m_IsOfTypeImagePredicate, m_IsBinaryPredicate);
    
    m_IsASegmentationImagePredicate = mitk::NodePredicateOr::New(m_IsABinaryImagePredicate, mitk::TNodePredicateDataType<mitk::LabelSetImage>::New());
    m_IsAPatientImagePredicate = mitk::NodePredicateAnd::New(m_IsNotABinaryImagePredicate, mitk::NodePredicateNot::New(mitk::TNodePredicateDataType<mitk::LabelSetImage>::New()));   
    
    m_IsACPredicate = mitk::NodePredicateProperty::New("SegmentationType", mitk::StringProperty::New("Amos_AC"));
    m_IsBCPredicate = mitk::NodePredicateProperty::New("SegmentationType", mitk::StringProperty::New("Amos_BC"));
    m_IsACSegmentationPredicate = mitk::NodePredicateAnd::New(m_IsASegmentationImagePredicate, m_IsACPredicate);
    m_IsBCSegmentationPredicate = mitk::NodePredicateAnd::New(m_IsASegmentationImagePredicate, m_IsBCPredicate);   
}

QmitkSegmentationView::~QmitkSegmentationView()
{
    delete m_Controls;
    //    this->GetSite()->GetPage()->RemovePartListener(this);
    
}

void QmitkSegmentationView::NewNodesGenerated()
{
    MITK_WARN<<"Use of deprecated function: NewNodesGenerated!! This function is empty and will be removed in the next time!";
}

void QmitkSegmentationView::NewNodeObjectsGenerated(mitk::ToolManager::DataVectorType* nodes)
{
    if (!nodes) return;
    
    mitk::ToolManager* toolManager = mitk::ToolManagerProvider::GetInstance()->GetToolManager();
    if (!toolManager) return;
    for (mitk::ToolManager::DataVectorType::iterator iter = nodes->begin(); iter != nodes->end(); ++iter)
    {
        this->FireNodeSelected( *iter );
        // only last iteration meaningful, multiple generated objects are not taken into account here
    }
}

void QmitkSegmentationView::Visible()
{
    // should be moved to ::BecomesVisible() or similar
    m_Active = true;
    if( m_Controls )
    {
        m_Controls->m_ManualToolSelectionBox2D->setEnabled( true );
        //       m_Controls->m_ManualToolSelectionBox3D->setEnabled( true );
        
        //    m_Controls->m_OrganToolSelectionBox->setEnabled( true );
        //    m_Controls->m_LesionToolSelectionBox->setEnabled( true );
        
        //    m_Controls->m_SlicesInterpolator->Enable3DInterpolation( m_Controls->widgetStack->currentWidget() == m_Controls->pageManual );
        
        //       ClassifyACBC();
        
        mitk::DataStorage::SetOfObjects::ConstPointer image = this->GetDataStorage()->GetSubset( m_IsAPatientImagePredicate );
        if (!image->empty()) {
            OnSelectionChanged(*image->begin());
        }
        
        mitk::DataStorage::SetOfObjects::ConstPointer segmentations = this->GetDataStorage()->GetSubset( m_IsASegmentationImagePredicate );
        for ( mitk::DataStorage::SetOfObjects::const_iterator iter = segmentations->begin();
             iter != segmentations->end();
        ++iter)
             {
                 mitk::DataNode* node = *iter;
                 Utils::Segmentation_Type segType = Utils::AmosSegmentationType(node);
                 int index =-1;
                 if(segType == Utils::Amos_AC) {
                     ApplyDisplayOptions(node);
                     index = m_Controls->segImageSelector->Find(node);
                     if(index > -1)
                         m_Controls->segImageSelector->setCurrentIndex(index);
                     else
                         m_Controls->segImageSelector->AddNode(node);
                 }
                 else if(segType == Utils::Amos_BC) {
                     ApplyDisplayOptions(node);
                     index = m_Controls->segImageBCSelector->Find(node);
                     if(index > -1)
                         m_Controls->segImageBCSelector->setCurrentIndex(index);
                     else
                         m_Controls->segImageBCSelector->AddNode(node);
                 }	  
                 
                 itk::SimpleMemberCommand<QmitkSegmentationView>::Pointer command = itk::SimpleMemberCommand<QmitkSegmentationView>::New();
                 command->SetCallbackFunction(this, &QmitkSegmentationView::OnWorkingNodeVisibilityChanged);
                 m_WorkingDataObserverTags.insert( std::pair<mitk::DataNode*, unsigned long>( node, node->GetProperty("visible")->AddObserver( itk::ModifiedEvent(), command ) ) );
                 
                 itk::SimpleMemberCommand<QmitkSegmentationView>::Pointer command2 = itk::SimpleMemberCommand<QmitkSegmentationView>::New();
                 command2->SetCallbackFunction(this, &QmitkSegmentationView::OnBinaryPropertyChanged);
                 m_BinaryPropertyObserverTags.insert( std::pair<mitk::DataNode*, unsigned long>( node, node->GetProperty("binary")->AddObserver( itk::ModifiedEvent(), command2 ) ) );
             }
    }
    
    itk::SimpleMemberCommand<QmitkSegmentationView>::Pointer command3 = itk::SimpleMemberCommand<QmitkSegmentationView>::New();
    command3->SetCallbackFunction( this, &QmitkSegmentationView::RenderingManagerReinitialized );
    m_RenderingManagerObserverTag = mitk::RenderingManager::GetInstance()->AddObserver( mitk::RenderingManagerViewsInitializedEvent(), command3 );
    
    this->SetToolManagerSelection(m_Controls->patImageSelector->GetSelectedNode(), m_Controls->segImageSelector->GetSelectedNode(), m_Controls->segImageBCSelector->GetSelectedNode());    
    
    if (m_DataSelectionChanged)
    {
        std::vector<mitk::DataNode*> dataManagerSelection;
        for (int i = 0; i < this->GetDataManagerSelection().size(); i++) { 
            dataManagerSelection.push_back(this->GetDataManagerSelection().at(i));
        }
        this->OnSelectionChanged(/*(berry::IWorkbenchPart::Pointer) nullptr,*/ dataManagerSelection);
    }
}

void QmitkSegmentationView::Activated()
{
    //    // should be moved to ::BecomesVisible() or similar
    //    m_Active = true;
    //    if( m_Controls )
    //    {
    //       m_Controls->m_ManualToolSelectionBox2D->setEnabled( true );
    // //       m_Controls->m_ManualToolSelectionBox3D->setEnabled( true );
    //       
    //       //    m_Controls->m_OrganToolSelectionBox->setEnabled( true );
    //       //    m_Controls->m_LesionToolSelectionBox->setEnabled( true );
    // 
    //       //    m_Controls->m_SlicesInterpolator->Enable3DInterpolation( m_Controls->widgetStack->currentWidget() == m_Controls->pageManual );
    // 
    // //       ClassifyACBC();
    // 
    //       mitk::DataStorage::SetOfObjects::ConstPointer image = this->GetDataStorage()->GetSubset( m_IsAPatientImagePredicate );
    //       if (!image->empty()) {
    //          OnSelectionChanged(*image->begin());
    //       }
    //       
    //       mitk::DataStorage::SetOfObjects::ConstPointer segmentations = this->GetDataStorage()->GetSubset( m_IsASegmentationImagePredicate );
    //       for ( mitk::DataStorage::SetOfObjects::const_iterator iter = segmentations->begin();
    //          iter != segmentations->end();
    //          ++iter)
    //       {
    // 	mitk::DataNode* node = *iter;
    // 	Utils::Segmentation_Type segType = Utils::AmosSegmentationType(node);
    // 	int index =-1;
    // 	if(segType == Utils::Amos_AC) {
    // 	    ApplyDisplayOptions(node);
    // 	    index = m_Controls->segImageSelector->Find(node);
    // 	    if(index > -1)
    // 		m_Controls->segImageSelector->setCurrentIndex(index);
    // 	    else
    // 		m_Controls->segImageSelector->AddNode(node);
    // 	}
    // 	else if(segType == Utils::Amos_BC) {
    // 	    ApplyDisplayOptions(node);
    // 	    index = m_Controls->segImageBCSelector->Find(node);
    // 	    if(index > -1)
    // 		m_Controls->segImageBCSelector->setCurrentIndex(index);
    // 	    else
    // 		m_Controls->segImageBCSelector->AddNode(node);
    // 	}	  
    // 	           
    //          itk::SimpleMemberCommand<QmitkSegmentationView>::Pointer command = itk::SimpleMemberCommand<QmitkSegmentationView>::New();
    //          command->SetCallbackFunction(this, &QmitkSegmentationView::OnWorkingNodeVisibilityChanged);
    //          m_WorkingDataObserverTags.insert( std::pair<mitk::DataNode*, unsigned long>( node, node->GetProperty("visible")->AddObserver( itk::ModifiedEvent(), command ) ) );
    // 
    //          itk::SimpleMemberCommand<QmitkSegmentationView>::Pointer command2 = itk::SimpleMemberCommand<QmitkSegmentationView>::New();
    //          command2->SetCallbackFunction(this, &QmitkSegmentationView::OnBinaryPropertyChanged);
    //          m_BinaryPropertyObserverTags.insert( std::pair<mitk::DataNode*, unsigned long>( node, node->GetProperty("binary")->AddObserver( itk::ModifiedEvent(), command2 ) ) );
    //       }
    //    }
    // 
    //    itk::SimpleMemberCommand<QmitkSegmentationView>::Pointer command3 = itk::SimpleMemberCommand<QmitkSegmentationView>::New();
    //    command3->SetCallbackFunction( this, &QmitkSegmentationView::RenderingManagerReinitialized );
    //    m_RenderingManagerObserverTag = mitk::RenderingManager::GetInstance()->AddObserver( mitk::RenderingManagerViewsInitializedEvent(), command3 );
    // 
    //    this->SetToolManagerSelection(m_Controls->patImageSelector->GetSelectedNode(), m_Controls->segImageSelector->GetSelectedNode(), m_Controls->segImageBCSelector->GetSelectedNode());
    
    //    m_Active = true;
}

void QmitkSegmentationView::Hidden()
{
    if( m_Controls )
    {
        this->SetToolSelectionBoxesEnabled( false );
        //deactivate all tools
        mitk::ToolManagerProvider::GetInstance()->GetToolManager()->ActivateTool(-1);
        
        //Removing all observers
        for ( NodeTagMapType::iterator dataIter = m_WorkingDataObserverTags.begin(); dataIter != m_WorkingDataObserverTags.end(); ++dataIter )
        {
            (*dataIter).first->GetProperty("visible")->RemoveObserver( (*dataIter).second );
        }
        m_WorkingDataObserverTags.clear();
        
        for ( NodeTagMapType::iterator dataIter = m_BinaryPropertyObserverTags.begin(); dataIter != m_BinaryPropertyObserverTags.end(); ++dataIter )
        {
            (*dataIter).first->GetProperty("binary")->RemoveObserver( (*dataIter).second );
        }
        m_BinaryPropertyObserverTags.clear();
        
        mitk::RenderingManager::GetInstance()->RemoveObserver(m_RenderingManagerObserverTag);
        
        ctkPluginContext* context = mitk::PluginActivator::getContext();
        ctkServiceReference ppmRef = context->getServiceReference<mitk::PlanePositionManagerService>();
        mitk::PlanePositionManagerService* service = context->getService<mitk::PlanePositionManagerService>(ppmRef);
        service->RemoveAllPlanePositions();
        context->ungetService(ppmRef);
        this->SetToolManagerSelection(0,0,0);
    }
    m_Active = false;
}

// void QmitkSegmentationView::StdMultiWidgetAvailable( mitk::ILinkedRenderWindowPart& stdMultiWidget )
// {
//    SetMultiWidget(&stdMultiWidget);
// }
// 
// void QmitkSegmentationView::StdMultiWidgetNotAvailable()
// {
//    SetMultiWidget(NULL);
// }
// 
// void QmitkSegmentationView::StdMultiWidgetClosed( mitk::ILinkedRenderWindowPart& /*stdMultiWidget*/ )
// {
//    SetMultiWidget(NULL);
// }

// void QmitkSegmentationView::SetMultiWidget(mitk::ILinkedRenderWindowPart* multiWidget)
// {
//    // save the current multiwidget as the working widget
//    m_MultiWidget = multiWidget;
// 
//    if (m_Parent)
//    {
//       m_Parent->setEnabled(m_MultiWidget->GetMultiWidgetAsQWidget());
//    }
// 
//    // tell the interpolation about toolmanager and multiwidget (and data storage)
//    if (m_Controls && m_MultiWidget)
//    {
// 	mitk::ToolManager* toolManager = mitk::ToolManagerProvider::GetInstance()->GetToolManager();
// 	m_Controls->m_SlicesInterpolator->SetDataStorage( this->GetDataStorage());
// 	QList<mitk::SliceNavigationController*> controllers;
//       
// 	QHash<QString, QmitkRenderWindow*> renderWindows =  this->GetQmitkRenderWindows();   
// 	QHash<QString, int>::const_iterator it = renderWindows.constBegin();
// 	while (it != renderWindows.constEnd()) {
// 	    controllers.push_back(it.value()->GetSliceNavigationController());       
// 	}      
// 	m_Controls->m_SlicesInterpolator->Initialize( toolManager, controllers );
//    }
// }

void QmitkSegmentationView::OnPreferencesChanged(const berry::IBerryPreferences* prefs)
{
    if (m_Controls != NULL)
    {
        bool slimView = prefs->GetBool("slim view", false);
        m_Controls->m_ManualToolSelectionBox2D->SetShowNames(!slimView);
        //       m_Controls->m_ManualToolSelectionBox3D->SetShowNames(!slimView);
    }
    
    m_AutoSelectionEnabled = prefs->GetBool("auto selection", false);
    this->ForceDisplayPreferencesUponAllImages();
}

void QmitkSegmentationView::CreateNewSegmentation()
{
    mitk::DataNode::Pointer node = mitk::ToolManagerProvider::GetInstance()->GetToolManager()->GetReferenceData(0);
    if (node.IsNotNull())
    {
        mitk::Image::Pointer image = dynamic_cast<mitk::Image*>( node->GetData() );
        if (image.IsNotNull())
        {
            if (image->GetDimension()>1)
            {
                // ask about the name and organ type of the new segmentation
                QmitkNewSegmentationDialog* dialog = new QmitkNewSegmentationDialog( m_Parent ); // needs a QWidget as parent, "this" is not QWidget
                
                //             QString storedList = this->GetPreferences()->Get("Organ-Color-List","");
                //             QStringList organColors;
                //             if (storedList.isEmpty())
                //             {
                //                organColors = GetDefaultOrganColorString();
                //             }
                //             else
                //             {
                //                /*
                //                a couple of examples of how organ names are stored:
                // 
                //                a simple item is built up like 'name#AABBCC' where #AABBCC is the hexadecimal notation of a color as known from HTML
                // 
                //                items are stored separated by ';'
                //                this makes it necessary to escape occurrences of ';' in name.
                //                otherwise the string "hugo;ypsilon#AABBCC;eugen#AABBCC" could not be parsed as two organs
                //                but we would get "hugo" and "ypsilon#AABBCC" and "eugen#AABBCC"
                // 
                //                so the organ name "hugo;ypsilon" is stored as "hugo\;ypsilon"
                //                and must be unescaped after loading
                // 
                //                the following lines could be one split with Perl's negative lookbehind
                //                */
                // 
                //                // recover string list from BlueBerry view's preferences
                //                QString storedString = this->GetPreferences()->Get("Organ-Color-List","");
                //                MITK_DEBUG << "storedString: " << storedString.toStdString();
                //                // match a string consisting of any number of repetitions of either "anything but ;" or "\;". This matches everything until the next unescaped ';'
                //                QRegExp onePart("(?:[^;]|\\\\;)*");
                //                MITK_DEBUG << "matching " << onePart.pattern().toStdString();
                //                int count = 0;
                //                int pos = 0;
                //                while( (pos = onePart.indexIn( storedString, pos )) != -1 )
                //                {
                //                   ++count;
                //                   int length = onePart.matchedLength();
                //                   if (length == 0) break;
                //                   QString matchedString = storedString.mid(pos, length);
                //                   MITK_DEBUG << "   Captured length " << length << ": " << matchedString.toStdString();
                //                   pos += length + 1; // skip separating ';'
                // 
                //                   // unescape possible occurrences of '\;' in the string
                //                   matchedString.replace("\\;", ";");
                // 
                //                   // add matched string part to output list
                //                   organColors << matchedString;
                //                }
                //                MITK_DEBUG << "Captured " << count << " organ name/colors";
                //             }
                // 
                //             dialog->SetSuggestionList( organColors );
                
                int dialogReturnValue = dialog->exec();
                
                if ( dialogReturnValue == QDialog::Rejected ) return; // user clicked cancel or pressed Esc or something similar
                
                // ask the user about an organ type and name, add this information to the image's (!) propertylist
                // create a new image of the same dimensions and smallest possible pixel type
                mitk::ToolManager* toolManager = mitk::ToolManagerProvider::GetInstance()->GetToolManager();
                mitk::Tool* firstTool = toolManager->GetToolById(0);
                if (firstTool)
                {
                    try
                    {
                        std::string newNodeName = dialog->GetSegmentationName().toStdString();
                        if(newNodeName.empty())
                            newNodeName = "no_name";
                        
                        mitk::DataNode::Pointer emptySegmentation =
                        firstTool->CreateEmptySegmentationNode( image, newNodeName, dialog->GetColor() );
                        
                        // initialize showVolume to false to prevent recalculating the volume while working on the segmentation
                        emptySegmentation->SetProperty( "showVolume", mitk::BoolProperty::New( false ) );
                        
                        if (!emptySegmentation) return; // could be aborted by user
                        
                        //                   UpdateOrganList( organColors, dialog->GetSegmentationName(), dialog->GetColor() );
                        // 
                        //                   /*
                        //                   escape ';' here (replace by '\;'), see longer comment above
                        //                   */
                        //                   QString stringForStorage = organColors.replaceInStrings(";","\\;").join(";");
                        //                   MITK_DEBUG << "Will store: " << stringForStorage;
                        //                   this->GetPreferences()->Put("Organ-Color-List", stringForStorage);
                        //                   this->GetPreferences()->Flush();
                        
                        if(mitk::ToolManagerProvider::GetInstance()->GetToolManager()->GetWorkingData(0))
                        {
                            mitk::ToolManagerProvider::GetInstance()->GetToolManager()->GetWorkingData(0)->SetSelected(false);
                        }
                        emptySegmentation->SetSelected(true);
                        this->GetDataStorage()->Add( emptySegmentation, node ); // add as a child, because the segmentation "derives" from the original
                        
                        this->ApplyDisplayOptions( emptySegmentation );
                        this->FireNodeSelected( emptySegmentation );
                        this->OnSelectionChanged( emptySegmentation );
                        
                        m_Controls->segImageSelector->SetSelectedNode(emptySegmentation);
                        mitk::RenderingManager::GetInstance()->InitializeViews(emptySegmentation->GetData()->GetTimeGeometry(), mitk::RenderingManager::REQUEST_UPDATE_ALL, true );
                    }
                    catch (std::bad_alloc&)
                    {
                        QMessageBox::warning(NULL,"Create new segmentation","Could not allocate memory for new segmentation");
                    }
                }
            }
            else
            {
                QMessageBox::information(NULL,"Segmentation","Segmentation is currently not supported for 2D images");
            }
        }
    }
    else
    {
        MITK_ERROR << "'Create new segmentation' button should never be clickable unless a patient image is selected...";
    }
}

void QmitkSegmentationView::OnWorkingNodeVisibilityChanged()
{
    mitk::DataNode* selectedNode = m_Controls->segImageSelector->GetSelectedNode();
    mitk::DataNode* selectedBCNode = m_Controls->segImageBCSelector->GetSelectedNode();
    if ( !selectedNode || !selectedBCNode)
    {
        this->SetToolSelectionBoxesEnabled(false);
        return;
    }
    
/*    bool selectedNodeIsVisible = selectedNode->IsVisible(mitk::BaseRenderer::GetInstance(
        mitk::BaseRenderer::GetRenderWindowByName("AmosMultiWidgetAC")));
    bool selectedBCNodeIsVisible = selectedBCNode->IsVisible(mitk::BaseRenderer::GetInstance(
        mitk::BaseRenderer::GetRenderWindowByName("AmosMultiWidgetBC"))); */  
    bool selectedNodeIsVisible = selectedNode->IsVisible(mitk::BaseRenderer::GetInstance(
        mitk::BaseRenderer::GetRenderWindowByName("stdmulti.widget0")));
    bool selectedBCNodeIsVisible = selectedBCNode->IsVisible(mitk::BaseRenderer::GetInstance(
        mitk::BaseRenderer::GetRenderWindowByName("stdmulti.widget1"))); 
    
    if (!selectedNodeIsVisible && !selectedBCNodeIsVisible)
    {
        this->SetToolSelectionBoxesEnabled(false);
        this->UpdateWarningLabel("The selected AC and BC segmentations are currently not visible!");
    }
    else if (!selectedNodeIsVisible && selectedBCNodeIsVisible)
    {
        this->SetToolSelectionBoxesEnabled(false);
        this->UpdateWarningLabel("The selected AC segmentation is currently not visible!");
    }
    else if (selectedNodeIsVisible && !selectedBCNodeIsVisible)
    {
        this->SetToolSelectionBoxesEnabled(false);
        this->UpdateWarningLabel("The selected BC segmentation is currently not visible!");
    }   
    else
    {
        this->SetToolSelectionBoxesEnabled(true);
        this->UpdateWarningLabel("");
    }
}

void QmitkSegmentationView::OnBinaryPropertyChanged()
{
    mitk::DataStorage::SetOfObjects::ConstPointer patImages = m_Controls->patImageSelector->GetNodes();
    
    bool isBinary(false);
    
    for (mitk::DataStorage::SetOfObjects::ConstIterator it = patImages->Begin(); it != patImages->End(); ++it)
    {
        const mitk::DataNode::Pointer node = it->Value();
        node->GetBoolProperty("binary", isBinary);
        mitk::LabelSetImage::Pointer labelSetImage = dynamic_cast<mitk::LabelSetImage*>(node->GetData());
        isBinary = isBinary || labelSetImage.IsNotNull();      
        
        if(isBinary)
        {
            m_Controls->patImageSelector->RemoveNode(node);
            Utils::Segmentation_Type segType = Utils::AmosSegmentationType(node);
            if(segType == Utils::Amos_AC)
                m_Controls->segImageSelector->AddNode(node);
            else if(segType == Utils::Amos_BC)
                m_Controls->segImageBCSelector->AddNode(node);
            this->SetToolManagerSelection(NULL,NULL,NULL);
            return;
        }
    }
    
    isBinary = true;
    mitk::DataStorage::SetOfObjects::ConstPointer segImages = GetDataStorage()->GetSubset(m_IsASegmentationImagePredicate);
    for (mitk::DataStorage::SetOfObjects::ConstIterator it = segImages->Begin(); it != segImages->End(); ++it)
    {
        const mitk::DataNode* node = it->Value();
        node->GetBoolProperty("binary", isBinary);
        mitk::LabelSetImage::Pointer labelSetImage = dynamic_cast<mitk::LabelSetImage*>(node->GetData());
        isBinary = isBinary || labelSetImage.IsNotNull();      
        
        if(!isBinary)
        {
            m_Controls->patImageSelector->AddNode(node);	 
            Utils::Segmentation_Type segType = Utils::AmosSegmentationType(node);
            if(segType == Utils::Amos_AC)
                m_Controls->segImageSelector->RemoveNode(node);
            else if(segType == Utils::Amos_BC)
                m_Controls->segImageBCSelector->RemoveNode(node);
            
            mitk::ToolManager::DataVectorType segData = mitk::ToolManagerProvider::GetInstance()->GetToolManager()->GetWorkingData();
            
            unsigned int cnt = 0;
            for (mitk::ToolManager::DataVectorType::iterator dataIter = segData.begin(); dataIter != segData.end(); ++dataIter, cnt++ ) {
                if(*dataIter == node)
                    break;	 
            }
            if(cnt < segData.size()) {
                segData.erase(segData.begin() + cnt);
                mitk::ToolManagerProvider::GetInstance()->GetToolManager()->SetWorkingData(segData);
            }
            
            //          if (mitk::ToolManagerProvider::GetInstance()->GetToolManager()->GetWorkingData(0) == node)
            //             mitk::ToolManagerProvider::GetInstance()->GetToolManager()->SetWorkingData(NULL);
            return;
        }
    }
}

void QmitkSegmentationView::NodeAdded(const mitk::DataNode *node)
{
    bool isBinary (false);
    bool isHelperObject (false);
    node->GetBoolProperty("binary", isBinary);
    mitk::LabelSetImage::Pointer labelSetImage = dynamic_cast<mitk::LabelSetImage*>(node->GetData());
    isBinary = isBinary || labelSetImage.IsNotNull();   
    node->GetBoolProperty("helper object", isHelperObject);
    m_Controls->patImageSelector->AddNode(node);
    
    if (m_AutoSelectionEnabled)
    {
        
        if (!isBinary && dynamic_cast<mitk::Image*>(node->GetData()))
        {
            FireNodeSelected(const_cast<mitk::DataNode*>(node));
            // 	 mitk::BaseData* data = node->GetData();
            // 	 cout << "Descrip " << data->GetProperty("meta.descrip")->GetValueAsString () << endl;
        }
    }
    if (isBinary && !isHelperObject)
    {
        // 	ClassifyACBC();
        
        
        itk::SimpleMemberCommand<QmitkSegmentationView>::Pointer command = itk::SimpleMemberCommand<QmitkSegmentationView>::New();
        command->SetCallbackFunction(this, &QmitkSegmentationView::OnWorkingNodeVisibilityChanged);
        m_WorkingDataObserverTags.insert( std::pair<mitk::DataNode*, unsigned long>( const_cast<mitk::DataNode*>(node), node->GetProperty("visible")->AddObserver( itk::ModifiedEvent(), command ) ) );
        
        itk::SimpleMemberCommand<QmitkSegmentationView>::Pointer command2 = itk::SimpleMemberCommand<QmitkSegmentationView>::New();
        command2->SetCallbackFunction(this, &QmitkSegmentationView::OnBinaryPropertyChanged);
        m_BinaryPropertyObserverTags.insert( std::pair<mitk::DataNode*, unsigned long>( const_cast<mitk::DataNode*>(node), node->GetProperty("binary")->AddObserver( itk::ModifiedEvent(), command2 ) ) );
        
        this->ApplyDisplayOptions(  const_cast<mitk::DataNode*>(node) );
        
        Utils::Segmentation_Type segType = Utils::AmosSegmentationType(node);
        int index =-1;
        if(segType == Utils::Amos_AC) {	  
            index = m_Controls->segImageSelector->Find(node);
            if(index > -1)
                m_Controls->segImageSelector->setCurrentIndex(index);
            else
                m_Controls->segImageSelector->AddNode(node);
        }
        else if(segType == Utils::Amos_BC) {
            index = m_Controls->segImageBCSelector->Find(node);
            if(index > -1)
                m_Controls->segImageBCSelector->setCurrentIndex(index);
            else
                m_Controls->segImageBCSelector->AddNode(node);
        }
    }
}

void QmitkSegmentationView::NodeRemoved(const mitk::DataNode* node)
{
    bool isSeg(false);
    bool isHelperObject(false);
    node->GetBoolProperty("helper object", isHelperObject);
    node->GetBoolProperty("binary", isSeg);
    mitk::LabelSetImage::Pointer labelSetImage = dynamic_cast<mitk::LabelSetImage*>(node->GetData());
    isSeg = isSeg || labelSetImage.IsNotNull();   
    
    mitk::Image* image = dynamic_cast<mitk::Image*>(node->GetData());
    if(isSeg && !isHelperObject && image)
    {
        //First of all remove all possible contour markers of the segmentation
        mitk::DataStorage::SetOfObjects::ConstPointer allContourMarkers = this->GetDataStorage()->GetDerivations(node, mitk::NodePredicateProperty::New("isContourMarker", mitk::BoolProperty::New(true)));
        
        ctkPluginContext* context = mitk::PluginActivator::getContext();
        ctkServiceReference ppmRef = context->getServiceReference<mitk::PlanePositionManagerService>();
        mitk::PlanePositionManagerService* service = context->getService<mitk::PlanePositionManagerService>(ppmRef);
        
        for (mitk::DataStorage::SetOfObjects::ConstIterator it = allContourMarkers->Begin(); it != allContourMarkers->End(); ++it)
        {
            std::string nodeName = node->GetName();
            unsigned int t = nodeName.find_last_of(" ");
            unsigned int id = atof(nodeName.substr(t+1).c_str())-1;
            
            service->RemovePlanePosition(id);
            
            this->GetDataStorage()->Remove(it->Value());
        }
        
        context->ungetService(ppmRef);
        service = NULL;
        
        mitk::DataNode* data0 = mitk::ToolManagerProvider::GetInstance()->GetToolManager()->GetWorkingData(0);
        mitk::DataNode* data1 = mitk::ToolManagerProvider::GetInstance()->GetToolManager()->GetWorkingData(1);
        
        if (m_Controls->patImageSelector->GetSelectedNode().IsNotNull()) {
            Utils::Segmentation_Type segType = Utils::AmosSegmentationType(node);
            if(data0 == node) {
                if(segType == Utils::Amos_AC) {
                    this->SetToolManagerSelection(mitk::ToolManagerProvider::GetInstance()->GetToolManager()->GetReferenceData(0), NULL, data1);
                    this->UpdateWarningLabel("Select or create an AC segmentation");
                }
                else if(segType == Utils::Amos_BC) {
                    this->SetToolManagerSelection(mitk::ToolManagerProvider::GetInstance()->GetToolManager()->GetReferenceData(0), data1, NULL);
                    this->UpdateWarningLabel("Select or create a BC segmentation");		  
                }
                if(data1 == NULL) {
                    this->UpdateWarningLabel("Select or create an AC and BC segmentations");
                }
                this->SetToolSelectionBoxesEnabled( false );
            }
            else if(data1 == node) {
                if(segType == Utils::Amos_AC) {
                    this->SetToolManagerSelection(mitk::ToolManagerProvider::GetInstance()->GetToolManager()->GetReferenceData(0), NULL, data0);
                    this->UpdateWarningLabel("Select or create an AC segmentation");
                }
                else if(segType == Utils::Amos_BC) {
                    this->SetToolManagerSelection(mitk::ToolManagerProvider::GetInstance()->GetToolManager()->GetReferenceData(0), data0, NULL);
                    this->UpdateWarningLabel("Select or create a BC segmentation");		  
                }
                if(data0 == NULL)
                    this->UpdateWarningLabel("Select or create an AC and BC segmentations");
                this->SetToolSelectionBoxesEnabled( false );
            }
        }
        
        //       if ((mitk::ToolManagerProvider::GetInstance()->GetToolManager()->GetWorkingData(0) == node) && m_Controls->patImageSelector->GetSelectedNode().IsNotNull())
        //       {
        // 	 if(Utils::AmosSegmentationType(node) == Amos_AC) {
        //          this->SetToolManagerSelection(mitk::ToolManagerProvider::GetInstance()->GetToolManager()->GetReferenceData(0), NULL, mitk::ToolManagerProvider::GetInstance()->GetToolManager()->GetWorkingData(1));
        //          this->UpdateWarningLabel("Select or create an AC segmentation");
        //       }
        //       else if ((mitk::ToolManagerProvider::GetInstance()->GetToolManager()->GetWorkingData(1) == node) && m_Controls->patImageSelector->GetSelectedNode().IsNotNull())
        //       {
        //          this->SetToolManagerSelection(mitk::ToolManagerProvider::GetInstance()->GetToolManager()->GetReferenceData(0), mitk::ToolManagerProvider::GetInstance()->GetToolManager()->GetWorkingData(0), NULL);
        //          this->UpdateWarningLabel("Select or create a BC segmentation");
        //       }
        
        //       mitk::SurfaceInterpolationController::GetInstance()->RemoveInterpolationSession(image);
    }
    mitk::DataNode* tempNode = const_cast<mitk::DataNode*>(node);
    //Since the binary property could be changed during runtime by the user
    if (image && !isHelperObject)
    {
        node->GetProperty("visible")->RemoveObserver( m_WorkingDataObserverTags[tempNode] );
        m_WorkingDataObserverTags.erase(tempNode);
        node->GetProperty("binary")->RemoveObserver( m_BinaryPropertyObserverTags[tempNode] );
        m_BinaryPropertyObserverTags.erase(tempNode);
    }
    
    if((mitk::ToolManagerProvider::GetInstance()->GetToolManager()->GetReferenceData(0) == node))
    {
        //as we don't know which node was actually removed e.g. our reference node, disable 'New Segmentation' button.
        //consider the case that there is no more image in the datastorage
        this->SetToolManagerSelection(NULL, NULL, NULL);
        this->SetToolSelectionBoxesEnabled( false );
    }
}

//void QmitkSegmentationView::CreateSegmentationFromSurface()
//{
//  mitk::DataNode::Pointer surfaceNode =
//      m_Controls->MaskSurfaces->GetSelectedNode();
//  mitk::Surface::Pointer surface(0);
//  if(surfaceNode.IsNotNull())
//    surface = dynamic_cast<mitk::Surface*> ( surfaceNode->GetData() );
//  if(surface.IsNull())
//  {
//    this->HandleException( "No surface selected.", m_Parent, true);
//    return;
//  }

//  mitk::DataNode::Pointer imageNode
//      = mitk::ToolManagerProvider::GetInstance()->GetToolManager()->GetReferenceData(0);
//  mitk::Image::Pointer image(0);
//  if (imageNode.IsNotNull())
//    image = dynamic_cast<mitk::Image*>( imageNode->GetData() );
//  if(image.IsNull())
//  {
//    this->HandleException( "No image selected.", m_Parent, true);
//    return;
//  }

//  mitk::SurfaceToImageFilter::Pointer s2iFilter
//      = mitk::SurfaceToImageFilter::New();

//  s2iFilter->MakeOutputBinaryOn();
//  s2iFilter->SetInput(surface);
//  s2iFilter->SetImage(image);
//  s2iFilter->Update();

//  mitk::DataNode::Pointer resultNode = mitk::DataNode::New();
//  std::string nameOfResultImage = imageNode->GetName();
//  nameOfResultImage.append(surfaceNode->GetName());
//  resultNode->SetProperty("name", mitk::StringProperty::New(nameOfResultImage) );
//  resultNode->SetProperty("binary", mitk::BoolProperty::New(true) );
//  resultNode->SetData( s2iFilter->GetOutput() );

//  this->GetDataStorage()->Add(resultNode, imageNode);

//}

//void QmitkSegmentationView::ToolboxStackPageChanged(int id)
//{
//  // interpolation only with manual tools visible
//  m_Controls->m_SlicesInterpolator->EnableInterpolation( id == 0 );

//  if( id == 0 )
//  {
//    mitk::DataNode::Pointer workingData =   mitk::ToolManagerProvider::GetInstance()->GetToolManager()->GetWorkingData(0);
//    if( workingData.IsNotNull() )
//    {
//      m_Controls->segImageSelector->setCurrentIndex( m_Controls->segImageSelector->Find(workingData) );
//    }
//  }

//  // this is just a workaround, should be removed when all tools support 3D+t
//  if (id==2) // lesions
//  {
//    mitk::DataNode::Pointer node = mitk::ToolManagerProvider::GetInstance()->GetToolManager()->GetReferenceData(0);
//    if (node.IsNotNull())
//    {
//      mitk::Image::Pointer image = dynamic_cast<mitk::Image*>( node->GetData() );
//      if (image.IsNotNull())
//      {
//        if (image->GetDimension()>3)
//        {
//          m_Controls->widgetStack->setCurrentIndex(0);
//          QMessageBox::information(NULL,"Segmentation","Lesion segmentation is currently not supported for 4D images");
//        }
//      }
//    }
//  }
//}

// protected

void QmitkSegmentationView::OnPatientComboBoxSelectionChanged( const mitk::DataNode* node )
{
    //mitk::DataNode* selectedNode = const_cast<mitk::DataNode*>(node);
    if( node != NULL )
    {
        this->UpdateWarningLabel("");
        mitk::DataNode* segNode = m_Controls->segImageSelector->GetSelectedNode();
        mitk::DataNode* segBCNode = m_Controls->segImageBCSelector->GetSelectedNode();
        if (segNode)
        {
            mitk::DataStorage::SetOfObjects::ConstPointer possibleParents = this->GetDataStorage()->GetSources( segNode, m_IsAPatientImagePredicate );
            bool isSourceNode(false);
            
            for (mitk::DataStorage::SetOfObjects::ConstIterator it = possibleParents->Begin(); it != possibleParents->End(); it++)
            {
                if (it.Value() == node)
                    isSourceNode = true;
            }
            
            if ( !isSourceNode && (!this->CheckForSameGeometry(segNode, node) || possibleParents->Size() > 0 ))
            {
                this->SetToolManagerSelection(node, NULL, segBCNode);
                this->SetToolSelectionBoxesEnabled( false );
                this->UpdateWarningLabel("The selected patient image does not match with the selected AC segmentation!");
            }
            else if ((!isSourceNode && this->CheckForSameGeometry(segNode, node)) || isSourceNode )
            {
                this->SetToolManagerSelection(node, segNode, segBCNode);
                //Doing this we can assure that the segmenation is always visible if the segmentation and the patient image are
                //loaded separately
                int layer(10);
                node->GetIntProperty("layer", layer);
                layer++;
                segNode->SetProperty("layer", mitk::IntProperty::New(layer));
                //this->UpdateWarningLabel("");
                RenderingManagerReinitialized();
            }
        }
        else
        {
            this->SetToolManagerSelection(node, NULL, segBCNode);
            this->SetToolSelectionBoxesEnabled( false );
            this->UpdateWarningLabel("Select or create an AC segmentation");
        }
        if (segBCNode)
        {
            mitk::DataStorage::SetOfObjects::ConstPointer possibleParents = this->GetDataStorage()->GetSources( segBCNode, m_IsAPatientImagePredicate );
            bool isSourceNode(false);
            
            for (mitk::DataStorage::SetOfObjects::ConstIterator it = possibleParents->Begin(); it != possibleParents->End(); it++)
            {
                if (it.Value() == node)
                    isSourceNode = true;
            }
            
            if ( !isSourceNode && (!this->CheckForSameGeometry(segBCNode, node) || possibleParents->Size() > 0 ))
            {
                this->SetToolManagerSelection(node, NULL, segBCNode);
                this->SetToolSelectionBoxesEnabled( false );
                this->UpdateWarningLabel("The selected patient image does not match with the selected BC segmentation!");
            }
            else if ((!isSourceNode && this->CheckForSameGeometry(segBCNode, node)) || isSourceNode )
            {
                this->SetToolManagerSelection(node, segNode, segBCNode);
                //Doing this we can assure that the segmenation is always visible if the segmentation and the patient image are
                //loaded separately
                int layer(10);
                node->GetIntProperty("layer", layer);
                layer++;
                segBCNode->SetProperty("layer", mitk::IntProperty::New(layer));
                //this->UpdateWarningLabel("");
                RenderingManagerReinitialized();
            }
        }
        else
        {
            this->SetToolManagerSelection(node, segNode, NULL);
            this->SetToolSelectionBoxesEnabled( false );
            this->UpdateWarningLabel("Select or create a BC segmentation");
        } 
        if(!segNode && !segBCNode)
            this->UpdateWarningLabel("Select or create an AC and BC segmentations");
    }
    else
    {
        this->UpdateWarningLabel("Please load an image!");
        this->SetToolSelectionBoxesEnabled( false );
    }
}

void QmitkSegmentationView::OnSegmentationComboBoxSelectionChanged(const mitk::DataNode *node)
{
    if (node == NULL)
    {
        this->UpdateWarningLabel("Select or create an AC segmentation");
        this->SetToolSelectionBoxesEnabled( false );
        return;
    }
    
    mitk::DataNode* refNode = m_Controls->patImageSelector->GetSelectedNode();
    mitk::DataNode* segBCNode = m_Controls->segImageBCSelector->GetSelectedNode();
    
    RenderingManagerReinitialized();
    //    if ( m_Controls->lblSegmentationWarnings->isVisible()) // "RenderingManagerReinitialized()" caused a warning. we do not need to go any further
    //       return;
    
    if (m_AutoSelectionEnabled)
    {
        this->OnSelectionChanged(const_cast<mitk::DataNode*>(node));
    }
    else
    {
        mitk::DataStorage::SetOfObjects::ConstPointer possibleParents = this->GetDataStorage()->GetSources( node, m_IsAPatientImagePredicate );
        
        if ( possibleParents->Size() == 1 )
        {
            mitk::DataNode* parentNode = possibleParents->ElementAt(0);
            
            if (parentNode != refNode)
            {
                this->UpdateWarningLabel("The selected AC segmentation does not match with the selected patient image!");
                this->SetToolSelectionBoxesEnabled( false );
                this->SetToolManagerSelection(NULL, node, segBCNode);
            }
            else
            {
                this->UpdateWarningLabel("");
                this->SetToolManagerSelection(refNode, node, segBCNode);
            }
        }
        else if (refNode && this->CheckForSameGeometry(node, refNode))
        {
            this->UpdateWarningLabel("");
            mitk::DataNode* segBCNode = m_Controls->segImageBCSelector->GetSelectedNode();
            this->SetToolManagerSelection(refNode, node, segBCNode);
        }
        else if (!refNode || !this->CheckForSameGeometry(node, refNode))
        {
            this->UpdateWarningLabel("Please select or load the according patient image!");
        }
    }
    /*   if (!node->IsVisible(mitk::BaseRenderer::GetInstance( mitk::BaseRenderer::GetRenderWindowByName("stdmulti.widget1"))))*/
    
//     if (!node->IsVisible(mitk::BaseRenderer::GetInstance( mitk::BaseRenderer::GetRenderWindowByName("AmosMultiWidgetAC"))))
    if (!node->IsVisible(mitk::BaseRenderer::GetInstance( mitk::BaseRenderer::GetRenderWindowByName("stdmulti.widget0"))))
    {
        this->UpdateWarningLabel("The AC selected segmentation is currently not visible!");
        this->SetToolSelectionBoxesEnabled( false );
    }
}

void QmitkSegmentationView::OnBCSegmentationComboBoxSelectionChanged(const mitk::DataNode *node)
{
    if (node == NULL)
    {
        this->UpdateWarningLabel("Select or create a BC segmentation");
        this->SetToolSelectionBoxesEnabled( false );
        return;
    }
    
    mitk::DataNode* refNode = m_Controls->patImageSelector->GetSelectedNode();
    mitk::DataNode* segNode = m_Controls->segImageSelector->GetSelectedNode();
    
    RenderingManagerReinitialized();
    //    if ( m_Controls->lblSegmentationWarnings->isVisible()) // "RenderingManagerReinitialized()" caused a warning. we do not need to go any further
    //       return;
    
    if (m_AutoSelectionEnabled)
    {
        this->OnSelectionChanged(const_cast<mitk::DataNode*>(node));
    }
    else
    {
        mitk::DataStorage::SetOfObjects::ConstPointer possibleParents = this->GetDataStorage()->GetSources( node, m_IsAPatientImagePredicate );
        
        if ( possibleParents->Size() == 1 )
        {
            mitk::DataNode* parentNode = possibleParents->ElementAt(0);
            
            if (parentNode != refNode)
            {
                this->UpdateWarningLabel("The selected BC segmentation does not match with the selected patient image!");
                this->SetToolSelectionBoxesEnabled( false );
                this->SetToolManagerSelection(NULL, segNode, node);
            }
            else
            {
                this->UpdateWarningLabel("");
                mitk::DataNode* segNode = m_Controls->segImageSelector->GetSelectedNode();
                this->SetToolManagerSelection(refNode, segNode, node);
            }
        }
        else if (refNode && this->CheckForSameGeometry(node, refNode))
        {
            this->UpdateWarningLabel("");
            this->SetToolManagerSelection(refNode, segNode, node);
        }
        else if (!refNode || !this->CheckForSameGeometry(node, refNode))
        {
            this->UpdateWarningLabel("Please select or load the according patient image!");
        }
    }
    /*   if (!node->IsVisible(mitk::BaseRenderer::GetInstance( mitk::BaseRenderer::GetRenderWindowByName("stdmulti.widget1"))))*/
    
//     if (!node->IsVisible(mitk::BaseRenderer::GetInstance( mitk::BaseRenderer::GetRenderWindowByName("AmosMultiWidgetBC"))))
    if (!node->IsVisible(mitk::BaseRenderer::GetInstance( mitk::BaseRenderer::GetRenderWindowByName("stdmulti.widget1"))))
    {
        this->UpdateWarningLabel("The selected BC segmentation is currently not visible!");
        this->SetToolSelectionBoxesEnabled( false );
    }
}

void QmitkSegmentationView::OnShowMarkerNodes (bool state)
{
    mitk::SegTool2D::Pointer manualSegmentationTool;
    
    unsigned int numberOfExistingTools = mitk::ToolManagerProvider::GetInstance()->GetToolManager()->GetTools().size();
    
    for(unsigned int i = 0; i < numberOfExistingTools; i++)
    {
        manualSegmentationTool = dynamic_cast<mitk::SegTool2D*>(mitk::ToolManagerProvider::GetInstance()->GetToolManager()->GetToolById(i));
        
        if (manualSegmentationTool)
        {
            if(state == true)
            {
                manualSegmentationTool->SetShowMarkerNodes( true );
            }
            else
            {
                manualSegmentationTool->SetShowMarkerNodes( false );
            }
        }
    }
}

void QmitkSegmentationView::OnSelectionChanged(mitk::DataNode* node)
{
    std::vector<mitk::DataNode*> nodes;
    //    QList<mitk::DataNode::Pointer> nodes;
    nodes.push_back( node );
    this->OnSelectionChanged(/*(berry::IWorkbenchPart::Pointer) nullptr,*/ nodes);
}

//Review jose
void QmitkSegmentationView::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/, const QList<mitk::DataNode::Pointer>& nodes)
{
    std::vector<mitk::DataNode*> nodesVec;
    for(int i = 0; i < nodes.size(); i++)
        nodesVec.push_back(nodes.at(i));
    OnSelectionChanged(nodesVec);
}

void QmitkSegmentationView::OnSelectionChanged(std::vector<mitk::DataNode*> nodes)
{
    if (nodes.size() != 0)
    {
        std::string markerName = "Position";
        unsigned int numberOfNodes = nodes.size();
        std::string nodeName = nodes.at( 0 )->GetName();
        if ( ( numberOfNodes == 1 ) && ( nodeName.find( markerName ) == 0) )
        {
            this->OnContourMarkerSelected( nodes.at( 0 ) );
            return;
        }
    }
    if (m_AutoSelectionEnabled && this->IsActivated())
    {
        if (nodes.size() == 0 && m_Controls->patImageSelector->GetSelectedNode().IsNull())
        {
            SetToolManagerSelection(NULL,NULL,NULL);
        }
        else if (nodes.size() >= 1)
        {
            mitk::DataNode::Pointer selectedNode = nodes.at(nodes.size() - 1);
            if(selectedNode.IsNull())
            {
                return;
            }
            
            mitk::Image::Pointer selectedImage = dynamic_cast<mitk::Image*>(selectedNode->GetData());
            if (selectedImage.IsNull())
            {
                SetToolManagerSelection(NULL,NULL,NULL);
                return;
            }
            else
            {
                bool isASegmentation(false);
                selectedNode->GetBoolProperty("binary", isASegmentation);
                mitk::LabelSetImage::Pointer labelSetImage = dynamic_cast<mitk::LabelSetImage*>(selectedNode->GetData());
                isASegmentation = isASegmentation || labelSetImage.IsNotNull();	    
                
                if (isASegmentation)
                {
                    //If a segmentation is selected find a possible reference image:
                    mitk::DataStorage::SetOfObjects::ConstPointer sources = this->GetDataStorage()->GetSources(selectedNode, m_IsAPatientImagePredicate);
                    mitk::DataNode::Pointer refNode;
                    if (sources->Size() != 0)
                    {
                        refNode = sources->ElementAt(0);
                        
                        refNode->SetVisibility(true);
                        selectedNode->SetVisibility(true);
                        
                        Utils::Segmentation_Type segType = Utils::AmosSegmentationType(selectedNode);
                        if(segType == Utils::Amos_AC)
                            SetToolManagerSelection(refNode,selectedNode,NULL);
                        else if(segType == Utils::Amos_BC)
                            SetToolManagerSelection(refNode, NULL, selectedNode);
                        
                        mitk::DataStorage::SetOfObjects::ConstPointer otherACBCSegmentations = 0;
                        if(segType == Utils::Amos_AC)
                            otherACBCSegmentations = this->GetDataStorage()->GetSubset(m_IsACSegmentationPredicate);
                        else if(segType == Utils::Amos_BC)
                            otherACBCSegmentations = this->GetDataStorage()->GetSubset(m_IsBCSegmentationPredicate);
                        if(!otherACBCSegmentations)
                            return;
                        
                        for(mitk::DataStorage::SetOfObjects::const_iterator iter = otherACBCSegmentations->begin(); iter != otherACBCSegmentations->end(); ++iter)
                        {
                            mitk::DataNode* node = *iter;
                            if (dynamic_cast<mitk::Image*>(node->GetData()) != selectedImage.GetPointer())
                                node->SetVisibility(false);
                        }
                        
                        mitk::DataStorage::SetOfObjects::ConstPointer otherPatientImages = this->GetDataStorage()->GetSubset(m_IsAPatientImagePredicate);
                        for(mitk::DataStorage::SetOfObjects::const_iterator iter = otherPatientImages->begin(); iter != otherPatientImages->end(); ++iter)
                        {
                            mitk::DataNode* node = *iter;
                            if (dynamic_cast<mitk::Image*>(node->GetData()) != dynamic_cast<mitk::Image*>(refNode->GetData()))
                                node->SetVisibility(false);
                        }
                    }
                    else
                    {
                        mitk::DataStorage::SetOfObjects::ConstPointer possiblePatientImages = this->GetDataStorage()->GetSubset(m_IsAPatientImagePredicate);
                        
                        for (mitk::DataStorage::SetOfObjects::ConstIterator it = possiblePatientImages->Begin(); it != possiblePatientImages->End(); it++)
                        {
                            refNode = it->Value();
                            
                            if (this->CheckForSameGeometry(selectedNode, it->Value()))
                            {
                                refNode->SetVisibility(true);
                                selectedNode->SetVisibility(true);
                                
                                mitk::DataStorage::SetOfObjects::ConstPointer otherACBCSegmentations = 0;
                                Utils::Segmentation_Type segType = Utils::AmosSegmentationType(selectedNode);
                                if(segType == Utils::Amos_AC)
                                    otherACBCSegmentations = this->GetDataStorage()->GetSubset(m_IsACSegmentationPredicate);
                                else if(segType == Utils::Amos_BC)
                                    otherACBCSegmentations = this->GetDataStorage()->GetSubset(m_IsBCSegmentationPredicate);
                                
                                if(!otherACBCSegmentations)
                                    return;			
                                
                                for(mitk::DataStorage::SetOfObjects::const_iterator iter = otherACBCSegmentations->begin(); iter != otherACBCSegmentations->end(); ++iter)
                                {
                                    mitk::DataNode* node = *iter;
                                    if (dynamic_cast<mitk::Image*>(node->GetData()) != selectedImage.GetPointer())
                                        node->SetVisibility(false);
                                }
                                
                                mitk::DataStorage::SetOfObjects::ConstPointer otherPatientImages = this->GetDataStorage()->GetSubset(m_IsAPatientImagePredicate);
                                for(mitk::DataStorage::SetOfObjects::const_iterator iter = otherPatientImages->begin(); iter != otherPatientImages->end(); ++iter)
                                {
                                    mitk::DataNode* node = *iter;
                                    if (dynamic_cast<mitk::Image*>(node->GetData()) != dynamic_cast<mitk::Image*>(refNode->GetData()))
                                        node->SetVisibility(false);
                                }
                                
                                if(segType == Utils::Amos_AC)
                                    SetToolManagerSelection(refNode,selectedNode,NULL);
                                else if(segType == Utils::Amos_BC)
                                    SetToolManagerSelection(refNode, NULL, selectedNode);
                                
                                //Doing this we can assure that the segmenation is always visible if the segmentation and the patient image are at the
                                //same level in the datamanager
                                int layer(10);
                                refNode->GetIntProperty("layer", layer);
                                layer++;
                                selectedNode->SetProperty("layer", mitk::IntProperty::New(layer));
                                if ( m_Controls->lblSegmentationWarnings->isVisible()) // "RenderingManagerReinitialized()" caused a warning. we do not need to go any further
                                    return;
                                RenderingManagerReinitialized();			
                                //                         return;
                            }
                        }
                        Utils::Segmentation_Type segType = Utils::AmosSegmentationType(selectedNode);
                        if(segType == Utils::Amos_AC)
                            SetToolManagerSelection(NULL,selectedNode,NULL);
                        else if(segType == Utils::Amos_BC)
                            SetToolManagerSelection(NULL, NULL, selectedNode);                  
                    }
                }
                else
                {
                    if (mitk::ToolManagerProvider::GetInstance()->GetToolManager()->GetReferenceData(0) != selectedNode)
                    {
                        SetToolManagerSelection(selectedNode, NULL,NULL);
                        //May be a bug in the selection services. A node which is deselected will be passed as selected node to the OnSelectionChanged function
//                         if (!selectedNode->IsVisible(mitk::BaseRenderer::GetInstance( mitk::BaseRenderer::GetRenderWindowByName("AmosMultiWidgetAC"))))
                        if (!selectedNode->IsVisible(mitk::BaseRenderer::GetInstance( mitk::BaseRenderer::GetRenderWindowByName("stdmulti.widget0"))))                            
                            selectedNode->SetVisibility(true);
                        this->UpdateWarningLabel("The selected patient image does not\nmatch with the selected segmentation!");
                        this->SetToolSelectionBoxesEnabled( false );
                    }
                }
            }
        }
        mitk::RenderingManager::GetInstance()->RequestUpdateAll();
        if ( m_Controls->lblSegmentationWarnings->isVisible()) // "RenderingManagerReinitialized()" caused a warning. we do not need to go any further
            return;
        RenderingManagerReinitialized();
    }
}

void QmitkSegmentationView::OnContourMarkerSelected(const mitk::DataNode *node)
{
    QmitkRenderWindow* selectedRenderWindow = 0;
    
    QHash<QString, QmitkRenderWindow*> renderWindows =  GetRenderWindowPart()->GetQmitkRenderWindows();   
    bool PlanarFigureInitializedWindow = false;
    // find initialized renderwindow
    //  cuold be better to use GetActiveQmitkRenderWindow()?
    selectedRenderWindow = GetRenderWindowPart()->GetActiveQmitkRenderWindow();
    QHash<QString, QmitkRenderWindow*>::const_iterator it = renderWindows.constBegin();
    while (it != renderWindows.constEnd() && !selectedRenderWindow) {
        if (node->GetBoolProperty("PlanarFigureInitializedWindow",
            PlanarFigureInitializedWindow, it.value()->GetRenderer())) {
            selectedRenderWindow = it.value();
            }       
    }   
    
    // make node visible
    if (selectedRenderWindow)
    {
        std::string nodeName = node->GetName();
        unsigned int t = nodeName.find_last_of(" ");
        unsigned int id = atof(nodeName.substr(t+1).c_str())-1;
        
        {
            ctkPluginContext* context = mitk::PluginActivator::getContext();
            ctkServiceReference ppmRef = context->getServiceReference<mitk::PlanePositionManagerService>();
            mitk::PlanePositionManagerService* service = context->getService<mitk::PlanePositionManagerService>(ppmRef);
            selectedRenderWindow->GetSliceNavigationController()->ExecuteOperation(service->GetPlanePosition(id));
            context->ungetService(ppmRef);
        }
        
        selectedRenderWindow->GetRenderer()->GetCameraController()->Fit();
        mitk::RenderingManager::GetInstance()->RequestUpdateAll();
    }
}

// void QmitkSegmentationView::OnTabWidgetChanged(int id)
// {
//    //always disable tools on tab changed
//    mitk::ToolManagerProvider::GetInstance()->GetToolManager()->ActivateTool(-1);
// 
//    //2D Tab ID = 0
//    //3D Tab ID = 1
//    if (id == 0)
//    {
//       //Hide 3D selection box, show 2D selection box
//       m_Controls->m_ManualToolSelectionBox3D->hide();
//       m_Controls->m_ManualToolSelectionBox2D->show();
//       //Deactivate possible active tool
// 
//       //TODO Remove possible visible interpolations -> Maybe changes in SlicesInterpolator
//    }
//    else
//    {
//       //Hide 3D selection box, show 2D selection box
//       m_Controls->m_ManualToolSelectionBox2D->hide();
//       m_Controls->m_ManualToolSelectionBox3D->show();
//       //Deactivate possible active tool
//    }
// }

void QmitkSegmentationView::SetToolManagerSelection(const mitk::DataNode* referenceData, const mitk::DataNode* workingData, const mitk::DataNode* workingBCData)
{
    // called as a result of new BlueBerry selections
    //   tells the ToolManager for manual segmentation about new selections
    //   updates GUI information about what the user should select
    mitk::ToolManager* toolManager = mitk::ToolManagerProvider::GetInstance()->GetToolManager();
    toolManager->SetReferenceData(const_cast<mitk::DataNode*>(referenceData));
    mitk::ToolManager::DataVectorType segData;
    if(workingData)
        segData.push_back(const_cast<mitk::DataNode*>(workingData));
    if(workingBCData)
        segData.push_back(const_cast<mitk::DataNode*>(workingBCData));
    toolManager->SetWorkingData(segData);
    //    toolManager->SetWorkingData(  const_cast<mitk::DataNode*>(workingData));
    
    // check original image
    //    m_Controls->btnNewSegmentation->setEnabled(referenceData != NULL);
    if (referenceData)
    {
        this->UpdateWarningLabel("");
        disconnect( m_Controls->patImageSelector, SIGNAL( OnSelectionChanged( const mitk::DataNode* ) ),
                    this, SLOT( OnPatientComboBoxSelectionChanged( const mitk::DataNode* ) ) );
        
        m_Controls->patImageSelector->setCurrentIndex( m_Controls->patImageSelector->Find(referenceData) );
        
        connect( m_Controls->patImageSelector, SIGNAL( OnSelectionChanged( const mitk::DataNode* ) ),
                 this, SLOT( OnPatientComboBoxSelectionChanged( const mitk::DataNode* ) ) );
    }
    
    // check segmentation
    if (referenceData)
    {
        if (workingData)
        {
            this->FireNodeSelected(const_cast<mitk::DataNode*>(workingData));
            
            //      if( m_Controls->widgetStack->currentIndex() == 0 )
            //      {
            disconnect( m_Controls->segImageSelector, SIGNAL( OnSelectionChanged( const mitk::DataNode* ) ),
                        this, SLOT( OnSegmentationComboBoxSelectionChanged( const mitk::DataNode* ) ) );
            
            m_Controls->segImageSelector->setCurrentIndex(m_Controls->segImageSelector->Find(workingData));
            
            connect( m_Controls->segImageSelector, SIGNAL( OnSelectionChanged( const mitk::DataNode* ) ),
                     this, SLOT( OnSegmentationComboBoxSelectionChanged(const mitk::DataNode*)) );
            
            //      }
        }
        if (workingBCData)
        {
            this->FireNodeSelected(const_cast<mitk::DataNode*>(workingBCData));
            
            //      if( m_Controls->widgetStack->currentIndex() == 0 )
            //      {
            disconnect( m_Controls->segImageBCSelector, SIGNAL( OnSelectionChanged( const mitk::DataNode* ) ),
                        this, SLOT( OnBCSegmentationComboBoxSelectionChanged( const mitk::DataNode* ) ) );
            
            m_Controls->segImageBCSelector->setCurrentIndex(m_Controls->segImageBCSelector->Find(workingBCData));
            
            connect( m_Controls->segImageBCSelector, SIGNAL( OnSelectionChanged( const mitk::DataNode* ) ),
                     this, SLOT( OnBCSegmentationComboBoxSelectionChanged(const mitk::DataNode*)) );
            
            //      }
        }      
    }
}

void QmitkSegmentationView::ForceDisplayPreferencesUponAllImages()
{
    if (!m_Parent || !m_Parent->isVisible()) return;
    
    // check all images and segmentations in DataStorage:
    // (items in brackets are implicitly done by previous steps)
    // 1.
    //   if  a reference image is selected,
    //     show the reference image
    //     and hide all other images (orignal and segmentation),
    //     (and hide all segmentations of the other original images)
    //     and show all the reference's segmentations
    //   if no reference image is selected, do do nothing
    //
    // 2.
    //   if  a segmentation is selected,
    //     show it
    //     (and hide all all its siblings (childs of the same parent, incl, NULL parent))
    //   if no segmentation is selected, do nothing
    
    if (!m_Controls)
        return; // might happen on initialization (preferences loaded)
        
    mitk::ToolManager* toolManager = mitk::ToolManagerProvider::GetInstance()->GetToolManager();
    mitk::DataNode::Pointer referenceData = toolManager->GetReferenceData(0);
    mitk::DataNode::Pointer workingData =   toolManager->GetWorkingData(0);
    mitk::DataNode::Pointer workingBCData =   toolManager->GetWorkingData(1);
    
    // 1.
    if (referenceData.IsNotNull())
    {
        // iterate all images
        mitk::DataStorage::SetOfObjects::ConstPointer allImages = this->GetDataStorage()->GetSubset( m_IsASegmentationImagePredicate );
        
        for ( mitk::DataStorage::SetOfObjects::const_iterator iter = allImages->begin(); iter != allImages->end(); ++iter)
            
        {
            mitk::DataNode* node = *iter;
            // apply display preferences
            ApplyDisplayOptions(node);
            
            // set visibility
            node->SetVisibility(node == referenceData);
        }
    }
    
    // 2.
    if (workingData.IsNotNull())
        workingData->SetVisibility(true);
    
    if (workingBCData.IsNotNull())
        workingBCData->SetVisibility(true);   
    
    mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void QmitkSegmentationView::ApplyDisplayOptions(mitk::DataNode* node)
{
    if (!node) return;
    
    bool isBinary(false);
    node->GetPropertyValue("binary", isBinary);
    
    if (isBinary)
    {
        node->SetProperty( "outline binary", mitk::BoolProperty::New( this->GetPreferences()->GetBool("draw outline", true)) );
        node->SetProperty( "outline width", mitk::FloatProperty::New( 2.0 ) );
        node->SetProperty( "opacity", mitk::FloatProperty::New( this->GetPreferences()->GetBool("draw outline", true) ? 1.0 : 0.3 ) );
        node->SetProperty( "volumerendering", mitk::BoolProperty::New( this->GetPreferences()->GetBool("volume rendering", false) ) );
        //       if(Utils::AmosSegmentationType(node) == Utils::Amos_BC)
        // 	  node->SetProperty("color", mitk::ColorProperty::New(0.0f, 1.0f, 0.0f));
    }
}

void QmitkSegmentationView::RenderingManagerReinitialized()
{
    m_ActiveRenderWindowPart = GetRenderWindowPart();
    if ( ! m_ActiveRenderWindowPart ) { return; }
    
    if(!m_Controls->patImageSelector->GetSelectedNode()) {
        UpdateWarningLabel("Please load an image!");
        return;
    }
    
    /*
     * Here we check whether the geometry of the selected segmentation image if aligned with the worldgeometry
     * At the moment it is not supported to use a geometry different from the selected image for reslicing.
     * For further information see Bug 16063
     */
    mitk::DataNode* workingACNode = m_Controls->segImageSelector->GetSelectedNode();
    mitk::DataNode* workingBCNode = m_Controls->segImageBCSelector->GetSelectedNode();
    
    if(!workingACNode && !workingBCNode) {
        this->UpdateWarningLabel("Please load AC and BC segmentation images!");
        return;
    }
    if(!workingACNode) {
        this->UpdateWarningLabel("Please load an AC segmentation image!");
        return;
    }       
    if(!workingBCNode) {
        this->UpdateWarningLabel("Please load an BC segmentation image!");
        return;
    }   
    
    const mitk::BaseGeometry* worldGeo = m_ActiveRenderWindowPart->GetQmitkRenderWindow("3d")->GetSliceNavigationController()->GetCurrentGeometry3D();
    
    bool boundingBoxOk = false;
    bool boundingBoxACOk = false;   
    
    if (workingACNode && worldGeo)
    {
        
        const mitk::BaseGeometry* workingNodeGeo = workingACNode->GetData()->GetGeometry();
        //       const mitk::BaseGeometry* worldGeo = m_MultiWidget->GetRenderWindow4()->GetSliceNavigationController()->GetCurrentGeometry3D();
        /*      const mitk::BaseGeometry* worldGeo = m_ActiveRenderWindowPart->GetQmitkRenderWindow("3d")->GetSliceNavigationController()->GetCurrentGeometry3D();*/ 
        bool ret = mitk::Equal(*workingNodeGeo->GetBoundingBox(), *worldGeo->GetBoundingBox(), mitk::eps, true);
        if (ret)
        {
            //          this->SetToolManagerSelection(m_Controls->patImageSelector->GetSelectedNode(), workingNode);
            //          this->SetToolSelectionBoxesEnabled(true);
            //          this->UpdateWarningLabel("");
            boundingBoxOk = true;
            boundingBoxACOk = true;
        }
        else
        {
            this->SetToolManagerSelection(m_Controls->patImageSelector->GetSelectedNode(), NULL, workingBCNode);
            this->SetToolSelectionBoxesEnabled(false);
            this->UpdateWarningLabel("Please perform a reinit on the AC segmentation image!");
        }
    }
    
    if (workingBCNode && worldGeo)
    {
        const mitk::BaseGeometry* workingBCNodeGeo = workingBCNode->GetData()->GetGeometry();
        //       const mitk::BaseGeometry* worldGeo = m_MultiWidget->GetRenderWindow4()->GetSliceNavigationController()->GetCurrentGeometry3D();
        /*      const mitk::BaseGeometry* worldGeo = m_ActiveRenderWindowPart->GetQmitkRenderWindow("3d")->GetSliceNavigationController()->GetCurrentGeometry3D();*/      
        bool ret = mitk::Equal(*workingBCNodeGeo->GetBoundingBox(), *worldGeo->GetBoundingBox(), mitk::eps, true);
        if (ret)
        {
            //          this->SetToolManagerSelection(m_Controls->patImageSelector->GetSelectedNode(), workingNode);
            //          this->SetToolSelectionBoxesEnabled(true);
            //          this->UpdateWarningLabel("");
            
            boundingBoxOk = boundingBoxOk && true;
        }
        else
        {
            this->SetToolManagerSelection(m_Controls->patImageSelector->GetSelectedNode(), workingACNode, NULL);
            this->SetToolSelectionBoxesEnabled(false);
            if(boundingBoxACOk)
                this->UpdateWarningLabel("Please perform a reinit on the BC segmentation image!");
            else
                this->UpdateWarningLabel("Please perform a reinit on the AC and BC segmentation images!");
        }
    } 
    
    if(boundingBoxOk) {
        this->SetToolManagerSelection(m_Controls->patImageSelector->GetSelectedNode(), workingACNode, workingBCNode);
        if(m_Controls->patImageSelector->GetSelectedNode()) {
            this->SetToolSelectionBoxesEnabled(true);
            this->UpdateWarningLabel("");
        }
    }
}

bool QmitkSegmentationView::CheckForSameGeometry(const mitk::DataNode *node1, const mitk::DataNode *node2) const
{
    bool isSameGeometry(true);
    
    mitk::Image* image1 = dynamic_cast<mitk::Image*>(node1->GetData());
    mitk::Image* image2 = dynamic_cast<mitk::Image*>(node2->GetData());
    if (image1 && image2)
    {
        mitk::BaseGeometry* geo1 = image1->GetGeometry();
        mitk::BaseGeometry* geo2 = image2->GetGeometry();
        
        isSameGeometry = isSameGeometry && mitk::Equal(geo1->GetOrigin(), geo2->GetOrigin());
        isSameGeometry = isSameGeometry && mitk::Equal(geo1->GetExtent(0), geo2->GetExtent(0));
        isSameGeometry = isSameGeometry && mitk::Equal(geo1->GetExtent(1), geo2->GetExtent(1));
        isSameGeometry = isSameGeometry && mitk::Equal(geo1->GetExtent(2), geo2->GetExtent(2));
        isSameGeometry = isSameGeometry && mitk::Equal(geo1->GetSpacing(), geo2->GetSpacing());
        isSameGeometry = isSameGeometry && mitk::MatrixEqualElementWise(geo1->GetIndexToWorldTransform()->GetMatrix(), geo2->GetIndexToWorldTransform()->GetMatrix());
        
        return isSameGeometry;
    }
    else
    {
        return false;
    }
}

void QmitkSegmentationView::UpdateWarningLabel(QString text)
{
    if (text.size() == 0)
        m_Controls->lblSegmentationWarnings->hide();
    else
        m_Controls->lblSegmentationWarnings->show();
    m_Controls->lblSegmentationWarnings->setText(text);
}

void QmitkSegmentationView::CreateQtPartControl(QWidget* parent)
{
    // setup the basic GUI of this view
    m_Parent = parent;
    m_ActiveRenderWindowPart = GetRenderWindowPart();
    
    m_Controls = new Ui::QmitkSegmentationControls;
    m_Controls->setupUi(parent);
    
    m_Controls->patImageSelector->SetDataStorage(this->GetDataStorage());
    m_Controls->patImageSelector->SetPredicate(mitk::NodePredicateAnd::New(m_IsAPatientImagePredicate, mitk::NodePredicateNot::New(mitk::NodePredicateProperty::New("helper object"))).GetPointer());
    
    this->UpdateWarningLabel("Please load an image!");
    
    if( m_Controls->patImageSelector->GetSelectedNode().IsNotNull() )
        this->UpdateWarningLabel("Select or create a new segmentation");
    
    mitk::DataStorage::SetOfObjects::ConstPointer setOfObjects;
    setOfObjects = this->GetDataStorage()->GetSubset(m_IsASegmentationImagePredicate);
    
    m_Controls->segImageSelector->SetDataStorage(this->GetDataStorage());
    //     m_Controls->segImageSelector->SetPredicate(mitk::NodePredicateAnd::New(m_IsABinaryImagePredicate, mitk::NodePredicateNot::New(mitk::NodePredicateProperty::New("helper object"))).GetPointer());
    m_Controls->segImageSelector->SetPredicate(mitk::NodePredicateAnd::New(m_IsACSegmentationPredicate, mitk::NodePredicateNot::New(mitk::NodePredicateProperty::New("helper object"))).GetPointer());    
    
    //    if( m_Controls->segImageSelector->GetSelectedNode().IsNotNull() )
    //       this->UpdateWarningLabel("");
    
    m_Controls->segImageBCSelector->SetDataStorage(this->GetDataStorage());
    //     m_Controls->segImageBCSelector->SetPredicate(mitk::NodePredicateAnd::New(m_IsABinaryImagePredicate, mitk::NodePredicateNot::New(mitk::NodePredicateProperty::New("helper object"))).GetPointer());
    m_Controls->segImageBCSelector->SetPredicate(mitk::NodePredicateAnd::New(m_IsBCSegmentationPredicate, mitk::NodePredicateNot::New(mitk::NodePredicateProperty::New("helper object"))).GetPointer());    
    
    for (mitk::DataStorage::SetOfObjects::ConstIterator nodeIt = setOfObjects->Begin()
        ; nodeIt != setOfObjects->End(); ++nodeIt)  // for each _DataNode
    {
        Utils::Segmentation_Type segType = Utils::AmosSegmentationType(nodeIt.Value().GetPointer());
        if(segType == Utils::Amos_AC)
            m_Controls->segImageSelector->SetSelectedNode(nodeIt.Value().GetPointer());
        else if(segType == Utils::Amos_BC)
            m_Controls->segImageBCSelector->SetSelectedNode(nodeIt.Value().GetPointer());      
    }   
    
    if( m_Controls->segImageSelector->GetSelectedNode().IsNotNull() && m_Controls->segImageBCSelector->GetSelectedNode().IsNotNull() )
        this->UpdateWarningLabel("");   
    
    mitk::ToolManager* toolManager = mitk::ToolManagerProvider::GetInstance()->GetToolManager();
    assert ( toolManager );
    
    toolManager->SetDataStorage( *(this->GetDataStorage()) );
    toolManager->InitializeTools();
    
    // all part of open source MITK
    m_Controls->m_ManualToolSelectionBox2D->SetGenerateAccelerators(true);
    m_Controls->m_ManualToolSelectionBox2D->SetToolGUIArea( m_Controls->m_ManualToolGUIContainer2D );
    //    m_Controls->m_ManualToolSelectionBox2D->SetDisplayedToolGroups("Add Subtract Correction Paint Wipe 'Region Growing' Fill Erase 'Live Wire' '2D Fast Marching'");
    m_Controls->m_ManualToolSelectionBox2D->SetDisplayedToolGroups("'BC to AC' Add Subtract Correction Paint Wipe Fill Erase23D 'Live Wire' 3DPick");
    
    // tools that could work on both images
    // although "BC to AC" has no effect on AC image due to
    // myMitkSetRegionTool implementation
    ACandBCToolsList << "BC to AC" << "3DPick";

    m_Controls->m_ManualToolSelectionBox2D->SetLayoutColumns(3);
    m_Controls->m_ManualToolSelectionBox2D->SetEnabledMode( MyQmitkToolSelectionBox::EnabledWithReferenceAndWorkingDataVisible );
    connect( m_Controls->m_ManualToolSelectionBox2D, SIGNAL(ToolSelected(int)), this, SLOT(OnOnlyInACTool2DSelected(int)) );
    
    //setup 3D Tools
    //    m_Controls->m_ManualToolSelectionBox3D->SetGenerateAccelerators(true);
    //    m_Controls->m_ManualToolSelectionBox3D->SetToolGUIArea( m_Controls->m_ManualToolGUIContainer3D );
    //    //specify tools to be added to 3D Tool area
    //    m_Controls->m_ManualToolSelectionBox3D->SetDisplayedToolGroups("Threshold 'UL Threshold' Otsu 'Fast Marching 3D' 'Region Growing 3D' Watershed Picking");
    //    m_Controls->m_ManualToolSelectionBox3D->SetLayoutColumns(3);
    //    m_Controls->m_ManualToolSelectionBox3D->SetEnabledMode( QmitkToolSelectionBox::EnabledWithReferenceAndWorkingDataVisible );
    
    //Hide 3D selection box, show 2D selection box
    //    m_Controls->m_ManualToolSelectionBox3D->hide();
    m_Controls->m_ManualToolSelectionBox2D->show();
    
    toolManager->NewNodesGenerated +=
    mitk::MessageDelegate<QmitkSegmentationView>( this, &QmitkSegmentationView::NewNodesGenerated );      // update the list of segmentations
    toolManager->NewNodeObjectsGenerated +=
    mitk::MessageDelegate1<QmitkSegmentationView, mitk::ToolManager::DataVectorType*>( this, &QmitkSegmentationView::NewNodeObjectsGenerated );      // update the list of segmentations
    
    // create signal/slot connections
    connect( m_Controls->patImageSelector, SIGNAL( OnSelectionChanged( const mitk::DataNode* ) ),
             this, SLOT( OnPatientComboBoxSelectionChanged( const mitk::DataNode* ) ) );
    connect( m_Controls->segImageSelector, SIGNAL( OnSelectionChanged( const mitk::DataNode* ) ),
             this, SLOT( OnSegmentationComboBoxSelectionChanged( const mitk::DataNode* ) ) );
    connect( m_Controls->segImageBCSelector, SIGNAL( OnSelectionChanged( const mitk::DataNode* ) ),
             this, SLOT( OnBCSegmentationComboBoxSelectionChanged( const mitk::DataNode* ) ) );  
    
    //     this->GetSite()->GetPage()->AddPartListener(this);
    
    //  connect( m_Controls->CreateSegmentationFromSurface, SIGNAL(clicked()), this, SLOT(CreateSegmentationFromSurface()) );
    //  connect( m_Controls->widgetStack, SIGNAL(currentChanged(int)), this, SLOT(ToolboxStackPageChanged(int)) );
    
    //    connect( m_Controls->tabWidgetSegmentationTools, SIGNAL(currentChanged(int)), this, SLOT(OnTabWidgetChanged(int)));
    
    //  connect(m_Controls->MaskSurfaces,  SIGNAL( OnSelectionChanged( const mitk::DataNode* ) ),
    //      this, SLOT( OnSurfaceSelectionChanged( ) ) );
    
    //    connect(m_Controls->m_SlicesInterpolator, SIGNAL(SignalShowMarkerNodes(bool)), this, SLOT(OnShowMarkerNodes(bool)));
    
    // tell the interpolation about toolmanager (and data storage)
    //    if (m_Controls && m_MultiWidget)
    //    {
    //     m_Controls->m_SlicesInterpolator->SetDataStorage( this->GetDataStorage());
    
    //     if(!m_ActiveRenderWindowPart)
    // 	return;
    //     QList<mitk::SliceNavigationController*> controllers;
    //     
    //     QHash<QString, QmitkRenderWindow*> renderWindows =  m_ActiveRenderWindowPart->GetQmitkRenderWindows();   
    //     QHash<QString, QmitkRenderWindow*>::const_iterator it = renderWindows.constBegin();
    //     while (it != renderWindows.constEnd()) {
    // 	if(it.key() != "3d")
    // 	    controllers.push_back(it.value()->GetSliceNavigationController());
    // 	it++;
    //     }  
    
    //     m_Controls->m_SlicesInterpolator->Initialize( toolManager, controllers );
    //    }   
    
    //  m_Controls->MaskSurfaces->SetDataStorage(this->GetDataStorage());
    //  m_Controls->MaskSurfaces->SetPredicate(mitk::NodePredicateDataType::New("Surface"));
}

void QmitkSegmentationView::OnOnlyInACTool2DSelected(int id)
{
    std::string name = "";
    if (id >= 0)
    {
        std::string text = "Active Tool: \"";      
        mitk::ToolManager* toolManager = mitk::ToolManagerProvider::GetInstance()->GetToolManager();
        name = toolManager->GetToolById(id)->GetName();
        text += name;
        text += "\"";
        mitk::StatusBar::GetInstance()->DisplayText(text.c_str());
        
        us::ModuleResource resource = toolManager->GetToolById(id)->GetCursorIconResource();
        this->SetMouseCursor(resource, 0, 0);
        
        mitk::IRenderWindowPart* part = GetRenderWindowPart();
        if(!part)
            return;
        
        MyQmitkRenderWindow* renderWindowBC =  (MyQmitkRenderWindow*)part->GetQmitkRenderWindow("axial2");
        if(renderWindowBC) {
            renderWindowBC->setACandBCTools(ACandBCToolsList);
            renderWindowBC->OnOnlyInACTool2DSelected(name);
        }
        
    }
    else
    {
        this->ResetMouseCursor();
        mitk::StatusBar::GetInstance()->DisplayText("");
        
        mitk::IRenderWindowPart* part = GetRenderWindowPart();
        if(!part)
            return;      
        
        MyQmitkRenderWindow* renderWindowBC =  (MyQmitkRenderWindow*)part->GetQmitkRenderWindow("axial2");
        if(renderWindowBC) {
            renderWindowBC->setACandBCTools(ACandBCToolsList);
            renderWindowBC->OnOnlyInACTool2DSelected(name);
        }
    }
}

void QmitkSegmentationView::ResetMouseCursor()
{
    if ( m_MouseCursorSet )
    {
        mitk::ApplicationCursor::GetInstance()->PopCursor();
        m_MouseCursorSet = false;
    }
}

void QmitkSegmentationView::SetMouseCursor( const us::ModuleResource& resource, int hotspotX, int hotspotY )
{
    if (!resource) return;
    
    // Remove previously set mouse cursor
    if ( m_MouseCursorSet )
    {
        mitk::ApplicationCursor::GetInstance()->PopCursor();
    }
    
    us::ModuleResourceStream cursor(resource, std::ios::binary);
    mitk::ApplicationCursor::GetInstance()->PushCursor( cursor, hotspotX, hotspotY );
    m_MouseCursorSet = true;
}

void QmitkSegmentationView::SetToolSelectionBoxesEnabled(bool status)
{
    if (status)
    {
        m_Controls->m_ManualToolSelectionBox2D->RecreateButtons();
        //     m_Controls->m_ManualToolSelectionBox3D->RecreateButtons();
    }
    
    m_Controls->m_ManualToolSelectionBox2D->setEnabled(status);
    //   m_Controls->m_ManualToolSelectionBox3D->setEnabled(status);
    //   m_Controls->m_SlicesInterpolator->setEnabled(status);
}

// void QmitkSegmentationView::PartActivated(const berry::IWorkbenchPartReference::Pointer& partRef )
// {
//   //MITK_INFO << "*** PartActivated (" << partRef->GetPart(false)->GetPartName() << ")";
//   berry::IWorkbenchPart* part = partRef->GetPart(false).GetPointer();
// 
//   // Check for a render window part and inform IRenderWindowPartListener views
//   // that it was activated
//   if ( mitk::IRenderWindowPart* renderPart = dynamic_cast<mitk::IRenderWindowPart*>(part) )
//   {
//     if (m_VisibleRenderWindowPart != renderPart)
//     {
//       RenderWindowPartActivated(renderPart);
//       m_ActiveRenderWindowPart = renderPart;
//       m_VisibleRenderWindowPart = renderPart;
//     }
//   }
// 
//   // Check if the activated part wants to be notified
//   if (mitk::ILifecycleAwarePart* lifecycleAwarePart = dynamic_cast<mitk::ILifecycleAwarePart*>(part))
//   {
//     lifecycleAwarePart->Activated();
//   }
// 
//   // Check if a zombie view has been activated.
//   if (mitk::IZombieViewPart* zombieView = dynamic_cast<mitk::IZombieViewPart*>(part))
//   {
//     if (m_ActiveZombieView && (m_ActiveZombieView != zombieView))
//     {
//       // Another zombie view has been activated. Tell the old one about it.
//       m_ActiveZombieView->ActivatedZombieView(partRef);
//       m_ActiveZombieView = zombieView;
//     }
//   }
// //   Activated();
// }

// void QmitkSegmentationView::PartDeactivated(const berry::IWorkbenchPartReference::Pointer& partRef )
// {
//   //MITK_INFO << "*** PartDeactivated (" << partRef->GetPart(false)->GetPartName() << ")";
//   berry::IWorkbenchPart* part = partRef->GetPart(false).GetPointer();
// 
//   if (mitk::ILifecycleAwarePart* lifecycleAwarePart = dynamic_cast<mitk::ILifecycleAwarePart*>(part))
//   {
//     lifecycleAwarePart->Deactivated();
//   }
// }

// void QmitkSegmentationView::PartVisible( const berry::IWorkbenchPartReference::Pointer& /*partRef*/ )
// {
//     Activated();
//     Visible();
// }

// void QmitkSegmentationView::RenderWindowPartActivated(mitk::IRenderWindowPart* renderPart)
// {
//   foreach (mitk::IRenderWindowPartListener* l, m_RenderWindowListeners)
//   {
//     l->RenderWindowPartActivated(renderPart);
//   }
// }
// 
// void QmitkSegmentationView::RenderWindowPartDeactivated(mitk::IRenderWindowPart* renderPart)
// {
//   foreach (mitk::IRenderWindowPartListener* l, m_RenderWindowListeners)
//   {
//     l->RenderWindowPartDeactivated(renderPart);
//   }
// }

bool QmitkSegmentationView::IsActivated()
{
    //     return m_ActiveZombieView != 0;
    return m_Active;
}

// berry::IPartListener::Events::Types QmitkSegmentationView::GetPartEventTypes() const
// {
//   return berry::IPartListener::Events::ACTIVATED | berry::IPartListener::Events::DEACTIVATED
//     | berry::IPartListener::Events::CLOSED | berry::IPartListener::Events::HIDDEN
//     | berry::IPartListener::Events::VISIBLE | berry::IPartListener::Events::OPENED;
// }

void QmitkSegmentationView::SetFocus()
{
    //     m_Controls.toolBox->setFocus();
}

void QmitkSegmentationView::ClassifyACBC()
{
    mitk::DataStorage::SetOfObjects::ConstPointer binariesImg = GetDataStorage()->GetSubset(m_IsASegmentationImagePredicate);
    for(mitk::DataStorage::SetOfObjects::ConstIterator it = binariesImg->Begin(); it != binariesImg->End();  ++it) {
        mitk::DataNode::Pointer nodeBin = it.Value();
        QString name = QString::fromStdString(nodeBin->GetName());	
        if(name.contains("Amos_AC"))
            nodeBin->SetProperty("SegmentationType", mitk::StringProperty::New("Amos_AC"));
        else if(name.contains("Amos_BC"))	    
            nodeBin->SetProperty("SegmentationType", mitk::StringProperty::New("Amos_BC"));
    }    
}

void QmitkSegmentationView::DataStorageModified()
{
    if(!m_Controls->patImageSelector->GetSelectedNode())
        this->UpdateWarningLabel("Please load an image!");
}
// QmitkSegmentationView::Segmentation_Type QmitkSegmentationView::AmosSegmentationType(const mitk::DataNode* node)
// {
// //     std::string name = node->GetName();
//     std::string segType;
//     node->GetStringProperty("SegmentationType", segType);
//     if(segType == "Amos_AC")
// 	return Amos_AC;
//     else if(segType == "Amos_BC")
// 	return Amos_BC;
//     else
// 	return Non_Amos;    
// //     size_t foundAC = segType.find("Amos_AC");
// //     size_t foundBC = segType.find("Amos_BC");
// //     if(foundAC != std::string::npos)
// // 	return Amos_AC;
// //     else if(foundBC != std::string::npos)
// // 	return Amos_BC;
// //     else
// // 	return Non_Amos;
// }

// ATTENTION some methods for handling the known list of (organ names, colors) are defined in QmitkSegmentationOrganNamesHandling.cpp
