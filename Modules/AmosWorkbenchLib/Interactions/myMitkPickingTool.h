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

#ifndef myMitkPickingTool_h_Included
#define myMitkPickingTool_h_Included

#include "myMitkSetRegionTool.h"
#include <AmosWorkbenchLibExports.h>

namespace us {
    class ModuleResource;
}

namespace mitk
{
    
    /**
     *  \brief Extracts a single region from a segmentation image and creates a new image with same geometry of the input image.
     * 
     *  The region is extracted in 3D space. This is done by performing region growing within the desired region.
     *  Use shift click to add the seed point.
     * 
     *  \ingroup ToolManagerEtAl
     *  \sa mitk::Tool
     *  \sa QmitkInteractiveSegmentation
     * 
     */
    class AmosWorkbenchLib_EXPORT MyPickingTool : public MySetRegionTool
    {
    public:
        
        mitkClassMacro(MyPickingTool, MySetRegionTool)
        itkFactorylessNewMacro(Self)
        itkCloneMacro(Self)
        
        virtual const char** GetXPM() const override;
        virtual us::ModuleResource GetCursorIconResource() const override;
        us::ModuleResource GetIconResource() const override;
        
        virtual const char* GetName() const override;
        void OnSurfaceCalculationDone();
        
    protected:
        
        MyPickingTool(); // purposely hidden
        virtual ~MyPickingTool();
        
        void ConnectActionsAndFunctions() override;
        void OnMousePressed ( StateMachineAction*, InteractionEvent* interactionEvent) override;
        virtual void performOperation2D(Image::Pointer /*image*/, Image::Pointer /*slice*/, const PlaneGeometry* /*planeGeometry*/, int /*timeStep = 0*/) override;
        virtual void performOperation3D(Image::Pointer /*image*/, const SlicedGeometry3D* /*slicedGeometry*/, int /*timeStep = 0*/) override;             

    };
    
} // namespace

#endif
