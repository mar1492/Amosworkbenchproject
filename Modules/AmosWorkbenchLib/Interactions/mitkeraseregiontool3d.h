#ifndef MITKERASEREGIONTOOL3D_H
#define MITKERASEREGIONTOOL3D_H

#include "myMitkSetRegionTool.h"
#include <AmosWorkbenchLibExports.h>

namespace us {
    class ModuleResource;
}

namespace mitk
    /**
    \brief Fill the inside of a contour with 1

    \sa SetRegionTool

    \ingroup Interaction
    \ingroup ToolManagerEtAl

    Finds the outer contour of a shape in 2D and 3D (possibly including holes) and sets all
    the inside pixels to 0 (erasing a segmentation).

    \warning Only to be instantiated by mitk::ToolManager.

    $Author$
    */
{
    class EraseRegionTool3D : public MySetRegionTool
    {
    public:
        
        mitkClassMacro(EraseRegionTool3D, MySetRegionTool)
        itkFactorylessNewMacro(Self)
        itkCloneMacro(Self)
        
        virtual const char** GetXPM() const override;
        virtual us::ModuleResource GetCursorIconResource() const override;
        us::ModuleResource GetIconResource() const override;
        
        virtual const char* GetName() const override;
        
    protected:
        
        EraseRegionTool3D(); // purposely hidden
        virtual ~EraseRegionTool3D();        
    };

} // namespace
#endif // MITKERASEREGIONTOOL3D_H
