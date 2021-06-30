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

#include "myMitkSetRegionTool.h"
#include "diffslicelistoperation.h"
#include "diffslicelistoperationapplier.h"
#include "../imagefilters.hpp"

#include <mitkToolManager.h>

#include <mitkBaseRenderer.h>
#include <mitkImageDataItem.h>
#include <mitkMousePressEvent.h>

#include <mitkITKImageImport.h>
#include <mitkImagePixelReadAccessor.h>
#include <mitkLabelSetImage.h>

#include <itkBinaryFillholeImageFilter.h>
#include <itkConnectedThresholdImageFilter.h>

#include <itkMatrix.h>

//includes for resling and overwriting
#include <mitkExtractSliceFilter.h>
#include <mitkVtkImageOverwrite.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>

#include <mitkDiffSliceOperationApplier.h>
#include <mitkOperationEvent.h>
#include <mitkUndoController.h>


mitk::MySetRegionTool::MySetRegionTool(int paintingPixelValue, int dataNodeInIndex, int dataNodeOutIndex)
:SegTool2D("PressMoveReleaseBC2AC"),
m_PaintingPixelValue(paintingPixelValue),
dataNodeInIndex(dataNodeInIndex),
dataNodeOutIndex(dataNodeOutIndex),
isValidPoint(false), perform3D(false)
{
      //create new node for picked region
  m_ResultNode = mitk::DataNode::New();
  // set some properties
  m_ResultNode->SetProperty("name", mitk::StringProperty::New("result"));
  m_ResultNode->SetProperty("helper object", mitk::BoolProperty::New(true));
  m_ResultNode->SetProperty("color", mitk::ColorProperty::New(1, 1, 0));
  m_ResultNode->SetProperty("layer", mitk::IntProperty::New(1));
  m_ResultNode->SetProperty("opacity", mitk::FloatProperty::New(0.33f));
}

mitk::MySetRegionTool::~MySetRegionTool()
{
}

void mitk::MySetRegionTool::ConnectActionsAndFunctions()
{
    CONNECT_FUNCTION( "PrimaryButtonPressed", OnMousePressed);
    CONNECT_FUNCTION( "CtrlPrimaryButtonPressed", OnMousePressed);
    CONNECT_FUNCTION( "Release", OnMouseReleased);
    CONNECT_FUNCTION( "CtrlRelease", OnMouseReleased);
    //     CONNECT_FUNCTION( "Move", OnMouseMoved);
    //   CONNECT_FUNCTION( "InvertLogic", OnInvertLogic);
}

void mitk::MySetRegionTool::Activated()
{
    Superclass::Activated();
    m_ToolManager->GetDataStorage()->Add(m_ResultNode, m_ToolManager->GetWorkingData(dataNodeInIndex));
 
    
//     DataNode* workingNodeIn( m_ToolManager->GetWorkingData(dataNodeInIndex) ); 
//     Image* workingImageIn = dynamic_cast<Image*>(workingNodeIn->GetData());
//        
//     mitk::LabelSetImage::Pointer resultLabelSetImage = mitk::ConvertImageToLabelSetImage(workingImageIn);
// 
//     arma::Cube<unsigned short> cubeImageIn = Utils::createCubeFromMITKImage<unsigned short>((mitk::Image::Pointer)resultLabelSetImage, false);
//     cubeImageIn.save("LabeledImage.mat", arma::arma_ascii);
}

void mitk::MySetRegionTool::Deactivated()
{
    m_ToolManager->GetDataStorage()->Remove(m_ResultNode);
    Superclass::Deactivated();
}

void mitk::MySetRegionTool::OnMousePressed ( StateMachineAction*, InteractionEvent* interactionEvent )
{
    mitk::InteractionPositionEvent* positionEvent = dynamic_cast<mitk::InteractionPositionEvent*>( interactionEvent );
    if (!positionEvent) return;
    
    mitk::MousePressEvent* pressEvent = dynamic_cast<mitk::MousePressEvent*>( interactionEvent );    
    
    if(pressEvent->GetModifiers() == InteractionEvent::ControlKey) 
        perform3D = true;
    else
        perform3D = false;
    
    m_LastEventSender = positionEvent->GetSender();
    m_LastEventSlice = m_LastEventSender->GetSlice();
    
    DataNode* workingNodeOut = 0;
    Image* workingImageOut = 0;
    if(dataNodeOutIndex != dataNodeInIndex) {
        workingNodeOut = m_ToolManager->GetWorkingData(dataNodeOutIndex); // in SetRegionTool is GetWorkingData(0)
        if ( !workingNodeOut ) return;
        workingImageOut = dynamic_cast<Image*>(workingNodeOut->GetData());
        if ( !workingImageOut ) return; 
    }
    
    DataNode* workingNodeIn( m_ToolManager->GetWorkingData(dataNodeInIndex) ); // in SetRegionTool is GetWorkingData(0)
    if ( !workingNodeIn ) return;
    Image* workingImageIn = dynamic_cast<Image*>(workingNodeIn->GetData());
    if ( !workingImageIn ) return;
    
    // these two line necessary for feedback shown in 3D operation
    // in addition they make feedback appear in all 2D windows
    m_ToolManager->GetDataStorage()->Remove(m_ResultNode);
    m_ToolManager->GetDataStorage()->Add(m_ResultNode, workingNodeIn);     
    
    // 1. Get the working image
    Image::Pointer workingSliceOut = 0;
    if(dataNodeOutIndex != dataNodeInIndex) {
        workingSliceOut = GetAffectedImageSliceAs2DImage(positionEvent, workingImageOut);
        if ( workingSliceOut.IsNull() ) return; // can't do anything without the segmentation
    }
    
    Image::Pointer workingSliceIn = GetAffectedImageSliceAs2DImage(positionEvent, workingImageIn);
    if ( workingSliceIn.IsNull() ) return; // can't do anything without the segmentation  
    
    // if click was outside the image, don't continue
    const BaseGeometry* sliceGeometry = workingSliceIn->GetGeometry();
    itk::Index<3> projectedPointIn2D;
    sliceGeometry->WorldToIndex( positionEvent->GetPositionInWorld(), projectedPointIn2D );
//     cout << "projectedPointIn2D " << projectedPointIn2D << endl;
    if ( !sliceGeometry->IsIndexInside( projectedPointIn2D ) )
    {
        MITK_ERROR << "point apparently not inside segmentation slice" << std::endl;
        return; // can't use that as a seed point
    }
    
    //     typedef itk::Image<DefaultSegmentationDataType, 2> InputImageType;
    typedef itk::Image<unsigned char, 2> InputSliceType;
    typedef InputSliceType::IndexType IndexType;
    // convert world coordinates to image indices
    IndexType seedIndex;
    sliceGeometry->WorldToIndex( positionEvent->GetPositionInWorld(), seedIndex);
    
    InputSliceType::Pointer itkImageOut = 0;
    if(dataNodeOutIndex != dataNodeInIndex) {
        itkImageOut = InputSliceType::New();
        CastToItkImage(workingSliceOut, itkImageOut);
    }
    InputSliceType::Pointer itkImageIn = InputSliceType::New();
    CastToItkImage(workingSliceIn, itkImageIn); 
    
    InputSliceType::PixelType boundOut = 0;
    if(dataNodeOutIndex != dataNodeInIndex)
        boundOut = itkImageOut->GetPixel(seedIndex);
    
    InputSliceType::PixelType boundIn = itkImageIn->GetPixel(seedIndex); 
    
    if(dataNodeOutIndex != dataNodeInIndex)
        isValidPoint = !boundOut && boundIn;
    else
        isValidPoint = boundIn;
    if(!isValidPoint) 
        return; 
    
    // depending on sign changes in diagonal of matrices
    // blobIndexes corresponding coordinates will be inverted in OnMouseReleased()    
    itk::Matrix<ScalarType> imageMatrix;
    mitk::AffineTransform3D* transformImage = workingImageIn->GetGeometry()->GetIndexToWorldTransform();
    imageMatrix = transformImage->GetMatrix();
    cout << imageMatrix << endl;
    
    itk::Matrix<ScalarType> sliceMatrix;
    mitk::AffineTransform3D* transformSlice = workingSliceIn->GetGeometry()->GetIndexToWorldTransform();
    sliceMatrix = transformSlice->GetMatrix();
    cout << sliceMatrix << endl;
    
    changeRows = imageMatrix(0, 0) / sliceMatrix(0, 0) < 0 ? true : false;
    changeCols = imageMatrix(1, 1) / sliceMatrix(1, 1) < 0 ? true : false;
    changeSlices = imageMatrix(2, 2) / sliceMatrix(2, 2) < 0 ? true : false; 
//     changeSlices = imageMatrix(2, 2) / sliceMatrix(2, 2) < 0 ? false : true; // changing eyes looking down    
    
    //perform region growing in desired segmented region
    if(perform3D) {
//         regionGrowing2D(positionEvent, workingSliceIn, false);
        regionGrowing3D(positionEvent, workingImageOut, workingImageIn);
    }
    else
        regionGrowing2D(positionEvent, workingSliceIn, workingImageIn);
    
}

void mitk::MySetRegionTool::regionGrowing2D(mitk::InteractionPositionEvent* positionEvent, Image::Pointer workingSliceIn, Image* workingImageIn)
{
    typedef itk::Image<unsigned char, 2> InputImageType;
    typedef InputImageType::IndexType IndexType;
    
    typedef itk::Image<unsigned char, 3> InputImageType3D;
    typedef InputImageType3D::IndexType IndexType3D;    
    // convert world coordinates to image indices
    IndexType seedIndex;
    IndexType3D seedIndex3D;
    
    InputImageType::Pointer itkImageIn = InputImageType::New();
    CastToItkImage(workingSliceIn, itkImageIn);     
    const BaseGeometry* sliceGeometry = workingSliceIn->GetGeometry();
    sliceGeometry->WorldToIndex( positionEvent->GetPositionInWorld(), seedIndex);
    InputImageType::PixelType boundIn = itkImageIn->GetPixel(seedIndex); 
    
    //     cout << "seedIndex 2D " << seedIndex << endl;
    //     itk::Matrix<ScalarType> qformANOT;
    //     mitk::AffineTransform3D* transform = workingSliceIn->GetGeometry()->GetIndexToWorldTransform();
    //     qformANOT = transform->GetMatrix();
    //     cout << qformANOT << endl;    
    
//     auto begin = chrono::high_resolution_clock::now();
    
    typedef itk::ConnectedThresholdImageFilter<InputImageType, InputImageType> RegionGrowingFilterType;
    RegionGrowingFilterType::Pointer regionGrowerIn = RegionGrowingFilterType::New();
    regionGrowerIn->SetConnectivity(RegionGrowingFilterType::FullConnectivity);
    regionGrowerIn->SetInput( itkImageIn);
    regionGrowerIn->AddSeed( seedIndex );
    
    regionGrowerIn->SetLower( boundIn );
    regionGrowerIn->SetUpper( boundIn );
    regionGrowerIn->SetReplaceValue(1);
    
    try {
        regionGrowerIn->Update();
    }
    catch(const itk::ExceptionObject&) {
        return; // can't work
    }
    catch( ... ) {
        return;
    }  

    mitk::Image::Pointer resultImage = mitk::GrabItkImageMemory(regionGrowerIn->GetOutput());
    resultImage->SetGeometry(workingSliceIn->GetGeometry()); 
    
    ImageReadAccessor accessor(resultImage); 
    unsigned char *mPointer = (unsigned char*)(accessor.GetData());
    arma::Mat<unsigned char> matSlice(mPointer, resultImage->GetDimension(0), resultImage->GetDimension(1), false, true);   
    blobIndexes = find(matSlice);
    //     matSlice.save("regionGrower2D.mat", arma::arma_ascii);
    //     arma::umat t = ind2sub(arma::size(matSlice), blobIndexes);
    //     t.save("blobIndexes2D.mat", arma::arma_ascii);            
    
    arma::Cube<unsigned char> cubeImageResult;
    cubeImageResult.zeros(workingImageIn->GetDimension(0), workingImageIn->GetDimension(1), workingImageIn->GetDimension(2));
    
    const BaseGeometry* imageInGeometry = workingImageIn->GetGeometry();
    imageInGeometry->WorldToIndex( positionEvent->GetPositionInWorld(), seedIndex3D);
    
    int nRows = workingImageIn->GetDimension(0);
    int nCols = workingImageIn->GetDimension(1);
    arma::umat t = arma::ind2sub(arma::size(nRows, nCols), blobIndexes);
    for(uint i = 0; i < blobIndexes.n_elem; i++) {
        if(changeRows)
            t(0, i) = nRows - t(0, i) - 1;
        if(changeCols)
            t(1, i) = nCols - t(1, i) - 1;        
    }
    
    arma::uvec feedBackIndexes = arma::sub2ind(arma::size(nRows, nCols), t);
    
    cubeImageResult.elem(feedBackIndexes + workingImageIn->GetDimension(0) * workingImageIn->GetDimension(1) * seedIndex3D[2]) += 1;
//         cubeImageResult.save("cubeImageResult.mat", arma::arma_ascii);
    
    mitk::Image::Pointer resultImage3D = Utils::cubeToMITKImage(&cubeImageResult);
    resultImage3D->SetGeometry(workingImageIn->GetGeometry());
/*        auto end = chrono::high_resolution_clock::now();
    cout << "PreLabelSetImage " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl; */        
    
//         begin = chrono::high_resolution_clock::now();
//         mitk::LabelSetImage::Pointer resultLabelSetImage = mitk::LabelSetImage::New();
//                 end = chrono::high_resolution_clock::now();
//         cout << "Create LabelSetImage " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;        
// 
//         begin = chrono::high_resolution_clock::now();
//         resultLabelSetImage->InitializeByLabeledImage(resultImage3D);
//         m_ResultNode->SetData(resultLabelSetImage);
//         end = chrono::high_resolution_clock::now();
//         cout << "Initialize LabelSetImage " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;
//         m_ResultNode->SetData(resultLabelSetImage);
    m_ResultNode->SetData(resultImage3D);
    
    mitk::RenderingManager::GetInstance()->RequestUpdate(positionEvent->GetSender()->GetRenderWindow());    
}

void mitk::MySetRegionTool::regionGrowing3D(mitk::InteractionPositionEvent* positionEvent, Image* workingImageOut, Image* workingImageIn)
{
    
    typedef itk::Image<unsigned char, 3> InputImageType;
    typedef InputImageType::IndexType IndexType;
    // convert world coordinates to image indices
    
    InputImageType::Pointer itkImageIn = InputImageType::New();
    CastToItkImage(workingImageIn, itkImageIn);    
    IndexType seedIndex;
    const BaseGeometry* imageGeometry = workingImageIn->GetGeometry();
    imageGeometry->WorldToIndex( positionEvent->GetPositionInWorld(), seedIndex);
    InputImageType::PixelType boundIn = itkImageIn->GetPixel(seedIndex); 
    
    //     cout << "seedIndex 3D " << seedIndex << endl;    
    
    typedef itk::ConnectedThresholdImageFilter<InputImageType, InputImageType> RegionGrowingFilterType;
    RegionGrowingFilterType::Pointer regionGrowerIn = RegionGrowingFilterType::New();
    regionGrowerIn->SetConnectivity(RegionGrowingFilterType::FullConnectivity);
    regionGrowerIn->SetInput( itkImageIn);
    regionGrowerIn->AddSeed( seedIndex );
    
    regionGrowerIn->SetLower( boundIn );
    regionGrowerIn->SetUpper( boundIn );
    regionGrowerIn->SetReplaceValue(1);
    
    try {
        regionGrowerIn->Update();
    }
    catch(const itk::ExceptionObject&) {
        return; // can't work
    }
    catch( ... ) {
        return;
    }    
    
    mitk::Image::Pointer resultImage = mitk::GrabItkImageMemory(regionGrowerIn->GetOutput());
    resultImage->SetGeometry(workingImageIn->GetGeometry());
    
//     mitk::LabelSetImage::Pointer resultLabelSetImage = mitk::LabelSetImage::New();
//     resultLabelSetImage->InitializeByLabeledImage(resultImage);
//     m_ResultNode->SetData(resultLabelSetImage);
    m_ResultNode->SetData(resultImage);
    
    arma::Cube<unsigned char> cubeImageIn = Utils::createCubeFromMITKImage<unsigned char>(resultImage, false);        
    arma::uvec indexesIn = find(cubeImageIn);
    
    if(workingImageOut) {
        arma::Cube<unsigned char> cubeImageOut = Utils::createCubeFromMITKImage<unsigned char>(workingImageOut, false);    
        
        // remove pixels existing in AC
        arma::Col<unsigned char> indexesOut = cubeImageOut.elem(indexesIn);
        blobIndexes.set_size(size(find(indexesOut == 0)));
        int cnt = 0;
        for(uint i = 0; i < indexesOut.n_elem; i++) {
            if(indexesOut(i) == 0) {
                blobIndexes(cnt) = indexesIn(i);
                cnt++;
            }
        }
    }
    else
        blobIndexes = indexesIn;
    //     arma::umat t = ind2sub(arma::size(cubeImageIn), blobIndexes);
    //     for(uint i = 0; i < blobIndexes.n_elem; i++) {
    //         if(changeRows)
    //             t(0, i) = cubeImageOut.n_rows - t(0, i) - 1;
    //         if(changeCols)
    //             t(1, i) = cubeImageOut.n_cols - t(1, i) - 1;
    //         if(changeSlices)
    //             t(2, i) = cubeImageOut.n_slices - t(2, i) - 1;        
    //     }     
    //     t.save("blobIndexes3D.mat", arma::arma_ascii);
}

void mitk::MySetRegionTool::OnMouseReleased( StateMachineAction*, InteractionEvent* interactionEvent )
{
    if(!isValidPoint || blobIndexes.n_elem == 0) {
        return;
    }    
    mitk::InteractionPositionEvent* positionEvent = dynamic_cast<mitk::InteractionPositionEvent*>( interactionEvent );
    if (!positionEvent) return;
    
    //     Point3D point3D = positionEvent->GetPositionInWorld();
    //     Point2D point2D = positionEvent->GetPointerPositionOnScreen();
    //     cout << "positionEvent.GetPointerPositionOnScreen " << point2D << endl;
    //     cout << "positionEvent.GetPositionInWorld " << point3D << endl;
    //     typedef itk::Image<unsigned char, 3> InputImageType;
    //     typedef InputImageType::IndexType IndexType;
    // convert world coordinates to image indices
    //     IndexType seedIndex;
    // 
    //     positionEvent->GetSender()->GetCurrentWorldGeometry()->WorldToIndex( point3D, seedIndex);
    //     cout << "seedIndex released " << seedIndex << endl;   
    
    assert( positionEvent->GetSender()->GetRenderWindow() );
    mitk::RenderingManager::GetInstance()->RequestUpdate(positionEvent->GetSender()->GetRenderWindow());
    
    int timeStep = positionEvent->GetSender()->GetTimeStep();
    
    DataNode* workingNodeOut = 0;
    if(dataNodeOutIndex != dataNodeInIndex) {
        workingNodeOut = m_ToolManager->GetWorkingData(dataNodeOutIndex); // SetRegionTool only works with GetWorkingData(0)
    }
    DataNode* workingNodeIn( m_ToolManager->GetWorkingData(dataNodeInIndex) );
    
    bool notFollow = dataNodeOutIndex != dataNodeInIndex ? !workingNodeOut || !workingNodeIn : !workingNodeIn;
    if (notFollow) return;
    
    // MITK 2021
    Image* image = 0;
    
    Image* imageOut = 0;
    if(dataNodeOutIndex != dataNodeInIndex)
        imageOut = dynamic_cast<Image*>(workingNodeOut->GetData());   
    Image* imageIn = dynamic_cast<Image*>(workingNodeIn->GetData());

    Image::Pointer sliceIn = GetAffectedImageSliceAs2DImage( positionEvent, imageIn );
    if (sliceIn.IsNull()) {
        MITK_ERROR << "Unable to extract slice." << std::endl;
        return;
    } 
    
    typedef itk::Image<unsigned char, 2> InputSliceType;
    InputSliceType::Pointer itkImageIn = InputSliceType::New();
    CastToItkImage(sliceIn, itkImageIn); 
    
    arma::uvec foundInd;

    if(!perform3D) {
        itk::Index<2> seedIndex;
        const BaseGeometry* sliceGeometry = sliceIn->GetGeometry();
        sliceGeometry->WorldToIndex( positionEvent->GetPositionInWorld(), seedIndex);
        int nRows = sliceIn->GetDimension(0);
        int nCols = sliceIn->GetDimension(1);
        arma::uword seedIndexes = arma::sub2ind(arma::size(nRows, nCols), seedIndex[0], seedIndex[1]);
        foundInd = find(blobIndexes == seedIndexes);
    }
    else {
        itk::Index<3> seedIndex;
        const BaseGeometry* imageGeometry = imageIn->GetGeometry();
        imageGeometry->WorldToIndex( positionEvent->GetPositionInWorld(), seedIndex);
        int nRows = imageIn->GetDimension(0);
        int nCols = imageIn->GetDimension(1);
        int nSlices = imageIn->GetDimension(2);
        arma::uword seedIndexes = arma::sub2ind(arma::size(nRows, nCols, nSlices), seedIndex[0], seedIndex[1], seedIndex[2]);
        foundInd = find(blobIndexes == seedIndexes);
    }
    if(foundInd.n_elem == 0) {
        m_ResultNode->SetData(NULL);
        mitk::RenderingManager::GetInstance()->RequestUpdateAll();     
        return;
    }
    itkImageIn = 0; // to unlock for ImageWriteAccessor accessor(slice);
    if(!perform3D) {
        const PlaneGeometry* planeGeometry( dynamic_cast<const PlaneGeometry*> (positionEvent->GetSender()->GetCurrentWorldPlaneGeometry() ) );
        
        Image::Pointer slice = 0;
        if(dataNodeOutIndex != dataNodeInIndex) {
            if ( !imageOut || !imageIn || !planeGeometry ) return;
            
            Image::Pointer sliceOut = GetAffectedImageSliceAs2DImage( positionEvent, imageOut );
        
            if (sliceOut.IsNull()) {
                MITK_ERROR << "Unable to extract slice." << std::endl;
                return;
            }            
            slice = sliceOut;
            // MITK 2021
            image = imageOut;
        }
        else {
            if ( !imageIn || !planeGeometry ) return;
            slice = sliceIn;
            // MITK 2021
            image = imageIn;            
        }
        
        performOperation2D(image, slice, planeGeometry, timeStep);
        
    }
    else {
        const SlicedGeometry3D* slicedGeometry( dynamic_cast<const SlicedGeometry3D*> (positionEvent->GetSender()->GetCurrentWorldGeometry() ) );
        
        // Image::Pointer image = 0;
        if(dataNodeOutIndex != dataNodeInIndex) {
            if ( !imageOut || !imageIn || !slicedGeometry ) return;
            image = imageOut;
        }
        else {
            if ( !imageIn || !slicedGeometry ) return;
            image = imageIn;
        }
        
        performOperation3D(image, slicedGeometry, timeStep);                
    }
//     m_ResultNode->SetData(NULL);
    mitk::RenderingManager::GetInstance()->RequestUpdateAll();  

/*    m_FeedbackContour->Clear();
    SetFeedbackContourVisible(false); */   
    
}

// MITK 2021
// void mitk::MySetRegionTool::performOperation2D(Image::Pointer slice, const PlaneGeometry* planeGeometry,
//int timeStep)
void mitk::MySetRegionTool::performOperation2D(Image::Pointer image, Image::Pointer slice, const PlaneGeometry* planeGeometry, int timeStep)
{
    ImageWriteAccessor accessor(slice);      
    unsigned char *mPointer = static_cast<unsigned char*>(accessor.GetData());
    arma::Mat<unsigned char> matSlice(mPointer, slice->GetDimension(0), slice->GetDimension(1), false, true);
    matSlice.elem(blobIndexes) = arma::ones<arma::Col<unsigned char>>(blobIndexes.n_elem)* m_PaintingPixelValue;
    blobIndexes.clear();
    //     matSlice.save("AC.mat", arma::arma_ascii);
    
    SliceInformation sliceInfo (slice, const_cast<mitk::PlaneGeometry*>(planeGeometry), timeStep);
    // MITK 2021
//    this->WriteSliceToVolume(sliceInfo);
    bool allowUndo = true;
    this->WriteSliceToVolume(image, sliceInfo, allowUndo);

    m_ResultNode->SetData(NULL);    
}

void mitk::MySetRegionTool::performOperation3D(Image::Pointer image, const SlicedGeometry3D* slicedGeometry, int timeStep)
{
    int nRows = image->GetDimension(0);
    int nCols = image->GetDimension(1);
    int nSlices = image->GetDimension(2);
    arma::umat t = arma::ind2sub(arma::size(nRows, nCols, nSlices), blobIndexes);

    // MITK 2021
//     QList<SliceInformation> listSliceInfo;
    QList<MySliceInformation> listSliceInfo;
    
    for(uint i = 0; i < blobIndexes.n_elem; i++) {
        if(changeRows)
            t(0, i) = nRows - t(0, i) - 1;
        if(changeCols)
            t(1, i) = nCols - t(1, i) - 1;
        if(changeSlices)
            t(2, i) = nSlices - t(2, i) - 1;        
    }        
    uint firstSlice = min(t.row(2));
    uint lastSlice = max(t.row(2));
            
    for(uint i = firstSlice; i <= lastSlice; i++) {
        arma::uvec ind = find(t.row(2) == i); // extract slice
        if(ind.n_elem == 0)
            continue;
        
        const PlaneGeometry* planeGeometry = slicedGeometry->GetPlaneGeometry(i);
        Image::Pointer slice = GetAffectedImageSliceAs2DImage(planeGeometry, image, timeStep);
        ImageWriteAccessor accessor(slice);
        
        unsigned char *mPointer = static_cast<unsigned char*>(accessor.GetData());
        arma::Mat<unsigned char> matSlice(mPointer, nRows, nCols, false, true);                                    
        
        for(arma::uword j = 0; j < ind.n_elem; j++)
            matSlice(t(0, ind(j)), t(1, ind(j))) = m_PaintingPixelValue;
        // MITK 2021
        // SliceInformation sliceInfo (slice, const_cast<mitk::PlaneGeometry*>(planeGeometry), timeStep);
        MySliceInformation sliceInfo (slice, const_cast<mitk::PlaneGeometry*>(planeGeometry), timeStep);
        listSliceInfo.append(sliceInfo);                      
    }
    this->WriteSliceListToVolume(listSliceInfo);
    blobIndexes.clear();
    //     cubeImage.save("AC.mat", arma::arma_ascii);
    m_ResultNode->SetData(NULL);    
}

void mitk::MySetRegionTool::OnMouseMoved ( StateMachineAction*, InteractionEvent* )
{
}

// MITK 2021
// void mitk::MySetRegionTool::WriteSliceListToVolume (QList<SliceInformation> listSliceInfo)
void mitk::MySetRegionTool::WriteSliceListToVolume (QList<MySliceInformation> listSliceInfo)
{
    if(listSliceInfo.size() == 0)
        return;
    
    DataNode* workingNode( m_ToolManager->GetWorkingData(dataNodeOutIndex) );
    Image* image = dynamic_cast<Image*>(workingNode->GetData());
    
    QList<mitk::Operation*> undoOperationList;
    QList<mitk::Operation*> doOperationList;
    
    for(int i = 0; i < listSliceInfo.size(); i++) {
        // MITK 2021
        // SliceInformation sliceInfo = listSliceInfo.at(i);
        MySliceInformation sliceInfo = listSliceInfo.at(i);
        /*============= BEGIN undo/redo feature block ========================*/
        // Create undo operation by caching the not yet modified slices
        mitk::Image::Pointer originalSlice = GetAffectedImageSliceAs2DImage(sliceInfo.plane, image, sliceInfo.timestep);
        DiffSliceOperation* undoOperation = new DiffSliceOperation(const_cast<mitk::Image*>(image), originalSlice, dynamic_cast<SlicedGeometry3D*>(originalSlice->GetGeometry()), sliceInfo.timestep, sliceInfo.plane);
        undoOperationList.append(undoOperation);
        /*============= END undo/redo feature block ========================*/
        
        //Make sure that for reslicing and overwriting the same alogrithm is used. We can specify the mode of the vtk reslicer
        vtkSmartPointer<mitkVtkImageOverwrite> reslice = vtkSmartPointer<mitkVtkImageOverwrite>::New();
        
        //Set the slice as 'input'
        reslice->SetInputSlice(sliceInfo.slice->GetVtkImageData());
        
        //set overwrite mode to true to write back to the image volume
        reslice->SetOverwriteMode(true);
        reslice->Modified();
        
        mitk::ExtractSliceFilter::Pointer extractor =  mitk::ExtractSliceFilter::New(reslice);
        extractor->SetInput( image );
        extractor->SetTimeStep( sliceInfo.timestep );
        extractor->SetWorldGeometry( sliceInfo.plane );
        extractor->SetVtkOutputRequest(false);
        extractor->SetResliceTransformByGeometry( image->GetGeometry( sliceInfo.timestep ) );
        
        extractor->Modified();
        extractor->Update();
        
        //the image was modified within the pipeline, but not marked so
        image->Modified();
        image->GetVtkImageData()->Modified();
        
        /*============= BEGIN undo/redo feature block ========================*/
        //specify the undo operation with the edited slice
        DiffSliceOperation* doOperation = new DiffSliceOperation(image, extractor->GetOutput(),dynamic_cast<SlicedGeometry3D*>(sliceInfo.slice->GetGeometry()), sliceInfo.timestep, sliceInfo.plane);
        doOperationList.append(doOperation);
        
        /*============= END undo/redo feature block ========================*/
    }
    
    DiffSliceListOperation* undoListOperation = new DiffSliceListOperation(undoOperationList);
    DiffSliceListOperation* doListOperation = new DiffSliceListOperation(doOperationList);
    //create an operation event for the undo stack
    OperationEvent* undoStackItem = new OperationEvent( DiffSliceListOperationApplier::GetInstance(), doListOperation, undoListOperation, "Segmentation" );
    
    //add it to the undo controller
    UndoStackItem::IncCurrObjectEventId();
    UndoStackItem::IncCurrGroupEventId();
    UndoController::GetCurrentUndoModel()->SetOperationEvent(undoStackItem);
    
    //clear the pointers as the operation are stored in the undocontroller and also deleted from there
    undoListOperation = NULL;
    doListOperation = NULL;    
}
