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

#ifndef mitkMySetRegionTool_h_Included
#define mitkMySetRegionTool_h_Included

#include <mitkCommon.h>
#include <AmosWorkbenchLibExports.h>
#include <mitkSegTool2D.h>

#include "../utils.hpp"

namespace mitk
{

class Image;
class StateMachineAction;
class InteractionEvent;
class PlaneGeometry;
class SlicedGeometry3D;
/**
  \brief Fills, erases or copies a 2D and 3D region

  \sa SegTool2D
  \sa ExtractImageFilter
  \sa OverwriteSliceImageFilter

  \ingroup Interaction
  \ingroup ToolManagerEtAl

  Finds the outer contour of a shape in 2D (possibly including holes) and sets all
  the inside pixels to a specified value. This might fill holes or erase segmentations.

  \warning Only to be instantiated by mitk::ToolManager.

  $Author$
*/
class AmosWorkbenchLib_EXPORT MySetRegionTool : public SegTool2D
{
public:

    mitkClassMacro(MySetRegionTool, SegTool2D)

protected:

    MySetRegionTool(int paintingPixelValue, int dataNodeInIndex, int dataNodeOutIndex); // purposely hidden
    virtual ~MySetRegionTool();
    
    //  MITK 2021 mitkSegTool2D change slice and plane to const in SliceInformation
    struct MySliceInformation
    {
      mitk::Image::Pointer slice;
      mitk::PlaneGeometry *plane;
      unsigned int timestep;

      MySliceInformation() {}
      MySliceInformation(mitk::Image *slice, mitk::PlaneGeometry *plane, unsigned int timestep)
      {
        this->slice = slice;
        this->plane = plane;
        this->timestep = timestep;
      }
    };    

    void ConnectActionsAndFunctions() override;

    virtual void Activated() override;
    virtual void Deactivated() override;

    virtual void OnMousePressed ( StateMachineAction*, InteractionEvent* );
    virtual void OnMouseReleased( StateMachineAction*, InteractionEvent* );
    virtual void OnMouseMoved( StateMachineAction*, InteractionEvent* );
    
    virtual void regionGrowing2D(mitk::InteractionPositionEvent* positionEvent, Image::Pointer workingSliceIn, Image* workingImageIn);
    virtual void regionGrowing3D(mitk::InteractionPositionEvent* positionEvent, Image* workingImageOut, Image* workingImageIn);
    
    // MITK 2021
    // void WriteSliceListToVolume (QList<SliceInformation> listSliceInfo);
    void WriteSliceListToVolume (QList<MySliceInformation> listSliceInfo);
    
//     virtual void performOperation2D(Image::Pointer slice, const PlaneGeometry* planeGeometry, int timeStep = 0);
    // MITK 2021
    virtual void performOperation2D(Image::Pointer image, Image::Pointer slice, const PlaneGeometry* planeGeometry, int timeStep = 0);
    virtual void performOperation3D(Image::Pointer image, const SlicedGeometry3D* slicedGeometry, int timeStep = 0);

    int m_PaintingPixelValue;
    int dataNodeInIndex, dataNodeOutIndex;
    bool isValidPoint;
    arma::uvec blobIndexes;
    bool changeRows, changeCols, changeSlices;
    
    bool perform3D;
    
    mitk::DataNode::Pointer m_ResultNode;
};

} // namespace

#endif


