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

#include "myMitkPickingTool.h"

#include <mitkToolManager.h>
#include <mitkProperties.h>
#include <mitkShowSegmentationAsSurface.h>
#include <mitkProgressBar.h>
#include <mitkStatusBar.h>
#include "Pick.xpm"

// us
#include <usModule.h>
#include <usModuleResource.h>
#include <usGetModuleContext.h>
#include <usModuleContext.h>
namespace mitk {
    MITK_TOOL_MACRO(AmosWorkbenchLib_EXPORT, MyPickingTool, "MyPickingTool")
}

mitk::MyPickingTool::MyPickingTool() : MySetRegionTool(1, 1, 0)
{

}

mitk::MyPickingTool::~MyPickingTool()
{
}

void mitk::MyPickingTool::ConnectActionsAndFunctions()
{
//     CONNECT_FUNCTION( "PrimaryButtonPressed", OnMousePressed);
    CONNECT_FUNCTION( "CtrlPrimaryButtonPressed", OnMousePressed);
//     CONNECT_FUNCTION( "Release", OnMouseReleased);
    CONNECT_FUNCTION( "CtrlRelease", OnMouseReleased);
    //     CONNECT_FUNCTION( "Move", OnMouseMoved);
    //   CONNECT_FUNCTION( "InvertLogic", OnInvertLogic);
}

const char** mitk::MyPickingTool::GetXPM() const
{
    return Pick_xpm;
}

us::ModuleResource mitk::MyPickingTool::GetCursorIconResource() const
{
    us::Module* module = us::GetModuleContext()->GetModule();
    us::ModuleResource resource = module->GetResource("Pick_cursor_32x32.png");
    return resource;
}
const char* mitk::MyPickingTool::GetName() const
{
    return "3DPick";
}

us::ModuleResource mitk::MyPickingTool::GetIconResource() const
{
    us::Module* module = us::GetModuleContext()->GetModule();
    us::ModuleResource resource = module->GetResource("Pick_48x48.png");
    return resource;
}

void mitk::MyPickingTool::OnMousePressed ( StateMachineAction*, InteractionEvent* interactionEvent)
{
    m_LastEventSender = interactionEvent->GetSender();
    QString name(m_LastEventSender->GetName());
    m_ToolManager->GetDataStorage()->Remove(m_ResultNode);
//     if(name == "AmosMultiWidgetAC") {
    if(name == "stdmulti.widget0") {
    /// JCAM
    //if(name == "stdmulti.widget3") {
        dataNodeOutIndex = 0;
        dataNodeInIndex = 0;
    }
//     else if(name == "AmosMultiWidgetBC") {    
//    else if(name == "stdmulti.widget1") {
    /// JCAM
    else if(name == "stdmulti.widget3") {
        dataNodeOutIndex = 1;
        dataNodeInIndex = 1;        
    }
    m_ToolManager->GetDataStorage()->Add(m_ResultNode, m_ToolManager->GetWorkingData(dataNodeInIndex));
    Superclass::OnMousePressed(0, interactionEvent);
}

void mitk::MyPickingTool::performOperation2D(Image::Pointer /*image*/, Image::Pointer /*slice*/, const PlaneGeometry* /*planeGeometry*/, int /*timeStep*/)
{
}

void mitk::MyPickingTool::performOperation3D(Image::Pointer /*slice*/, const SlicedGeometry3D* /*slicedGeometry*/, int /*timeStep*/)
{
    mitk::DataNode::Pointer segmentationNode = m_ToolManager->GetWorkingData(dataNodeInIndex);    
    mitk::Image::Pointer blob3D = dynamic_cast <mitk::Image*> (m_ResultNode->GetData());
    try {
    
        ShowSegmentationAsSurface::Pointer surfaceFilter = ShowSegmentationAsSurface::New();

        // Activate callback functions
        itk::SimpleMemberCommand<MyPickingTool>::Pointer successCommand = itk::SimpleMemberCommand<MyPickingTool>::New();
        successCommand->SetCallbackFunction(this, &MyPickingTool::OnSurfaceCalculationDone);
        surfaceFilter->AddObserver(ResultAvailable(), successCommand);

        itk::SimpleMemberCommand<MyPickingTool>::Pointer errorCommand = itk::SimpleMemberCommand<MyPickingTool>::New();
        errorCommand->SetCallbackFunction(this, &MyPickingTool::OnSurfaceCalculationDone);
        surfaceFilter->AddObserver(ProcessingError(), errorCommand);

        // set filter parameter
        surfaceFilter->SetDataStorage(*(m_ToolManager->GetDataStorage()));
        surfaceFilter->SetPointerParameter("Input", blob3D);
        surfaceFilter->SetPointerParameter("Group node", segmentationNode);
        
        surfaceFilter->SetParameter("Show result", true);
        surfaceFilter->SetParameter("Sync visibility", false);
        surfaceFilter->SetParameter("Median kernel size", 3u);
        surfaceFilter->SetParameter("Decimate mesh", false);

        surfaceFilter->SetParameter("Apply median", false);
        surfaceFilter->SetParameter("Smooth", false);
        StatusBar::GetInstance()->DisplayText("Surface creation started in background...");
        
        surfaceFilter->StartAlgorithm();
    }
    catch(...) {
        MITK_ERROR << "Surface creation failed!";
    }

    m_ResultNode->SetData(NULL);

    mitk::RenderingManager::GetInstance()->RequestUpdateAll();    
}

void MyPickingTool::OnSurfaceCalculationDone()
{
    StatusBar::GetInstance()->Clear();
    mitk::DataNode::Pointer segmentationNode = m_ToolManager->GetWorkingData(dataNodeInIndex);
    mitk::DataNode* blobNode = m_ToolManager->GetDataStorage()->GetNamedDerivedNode(segmentationNode->GetName().c_str(), segmentationNode, true);
    if(blobNode) {
        DataStorage::SetOfObjects::ConstPointer children = m_ToolManager->GetDataStorage()->GetDerivations(segmentationNode);
        int number = children->size() - 1;
        blobNode->SetProperty("name", mitk::StringProperty::New(segmentationNode->GetName() + "_blob3D_" + std::to_string(number)));
    }
}
