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

#ifndef mitkFromBCToACTool_h_Included
#define mitkFromBCToACTool_h_Included

#include "myMitkSetRegionTool.h"
#include <AmosWorkbenchLibExports.h>

namespace us {
    class ModuleResource;
}

namespace mitk
{
    
    /**
     *  \brief Copy a segmentation blob form BC to AC
     * 
     *  \sa MySetRegionTool
     * 
     *  \ingroup Interaction
     *  \ingroup ToolManagerEtAl
     * 
     *  Finds the outer contour of a shape in 2D and 3D (possibly including holes) and copy 
     *  the segmentation blob form BC to AC
     * 
     *  \warning Only to be instantiated by mitk::ToolManager.
     * 
     *  $Author$
     */
    class AmosWorkbenchLib_EXPORT FromBCToACTool : public MySetRegionTool
    {
    public:
        
        mitkClassMacro(FromBCToACTool, MySetRegionTool)
        itkFactorylessNewMacro(Self)
        itkCloneMacro(Self)
        
        virtual const char** GetXPM() const override;
        virtual us::ModuleResource GetCursorIconResource() const override;
        us::ModuleResource GetIconResource() const override;
        
        virtual const char* GetName() const override;
        
    protected:
        
        FromBCToACTool(); // purposely hidden
        virtual ~FromBCToACTool();
        
    };
    
} // namespace

#endif


