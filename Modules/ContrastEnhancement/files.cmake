set(CPP_FILES
    edison/edge/BgEdge.cpp
    edison/edge/BgEdgeDetect.cpp
    edison/edge/BgEdgeList.cpp
    edison/edge/BgGlobalFc.cpp
    edison/edge/BgImage.cpp
    edison/prompt/edison.cpp
    edison/prompt/globalFnc.cpp
    edison/prompt/libppm.cpp
    edison/prompt/parser.cpp
    edison/segm/ms.cpp
    edison/segm/msImageProcessor.cpp
    edison/segm/msSysPrompt.cpp
    edison/segm/RAList.cpp
    edison/segm/rlist.cpp
    contrastprocesscase.cpp
    contrastprocesscaseparallel.cpp
    contrastprocesscasethreaded.cpp
    contrastprocessdirectory.cpp
)

#usFunctionGenerateModuleInit(CPP_FILES)
# 
set(MOC_H_FILES
    contrastprocesscase.h
    contrastprocesscaseparallel.h
    contrastprocesscasethreaded.h
    contrastprocessdirectory.h
)

# set(RESOURCE_FILES
#     edge_detection.eds    
# )
