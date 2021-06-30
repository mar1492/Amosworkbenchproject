set(SRC_CPP_FILES
    QmitkSegmentationPreferencePage.cpp 
)

set(DATABASE_CPP_FILES
    xmloperator.cpp
    subject.cpp
    scans.cpp
    reconstruction.cpp
    amosimage.cpp
    project.cpp
    xmlwritable.cpp
    newprojectdialog.cpp
    projectdialog.cpp
    scansdialog.cpp
    subjectsdialog.cpp
    savemenutextedit.cpp
    allamossegmentationsdialog.cpp
    allreconstructionsdialog.cpp
    rightclickmenu.cpp
    annotatedimagedialog.cpp
    imagedialog.cpp
    reconstructiondialog.cpp
    chooseinputimagesdialog.cpp
    reconstructioninputinfo.cpp
    reconstructioninputlist.cpp
    projectpage.cpp
    segmentationdialog.cpp
    imageslistdialog.cpp
    externimagesdialog.cpp
    twoimagesdialog.cpp
    htmlviewer.cpp
    zoomwebview.cpp
    filedialog.cpp
    graphvizdialog.cpp
    QGraphicsViewEc.cpp
    nodegraph.cpp
    edgegraph.cpp
    newreconstructiondialog.cpp
)

set(INTERNAL_CPP_FILES
  mitkPluginActivator.cpp
  QmitkAmosWorkbenchView.cpp
  QmitkSegmentationView.cpp
  MyQmitkToolSelectionBox.cpp
  QmitkCreatePolygonModelAction.cpp
  messagestouser.cpp
)

set(UI_FILES
  src/internal/QmitkAmosWorkbenchViewControls.ui
  src/internal/QmitkSegmentationControls.ui
  src/internal/messagestouser.ui
  src/database/newprojectdialog.ui
  src/database/projectdialog.ui
  src/database/subjectsdialog.ui
  src/database/allreconstructionsdialog.ui
  src/database/imagedialog.ui
  src/database/reconstructiondialog.ui
  src/database/chooseinputimagesdialog.ui
  src/database/imageslistdialog.ui
  src/database/externimagesdialog.ui
  src/database/twoimagesdialog.ui
  src/database/htmlviewer.ui
  src/database/graphvizdialog.ui
  src/database/newreconstructiondialog.ui
)

set(MOC_H_FILES
    src/internal/mitkPluginActivator.h
    src/internal/QmitkAmosWorkbenchView.h
    src/QmitkSegmentationPreferencePage.h
    src/internal/QmitkSegmentationView.h
    src/internal/MyQmitkToolSelectionBox.h
    src/internal/QmitkCreatePolygonModelAction.h
    src/internal/messagestouser.h
    src/database/newprojectdialog.h
    src/database/projectdialog.h
    src/database/scansdialog.h
    src/database/subjectsdialog.h
    src/database/savemenutextedit.h
    src/database/allamossegmentationsdialog.h
    src/database/allreconstructionsdialog.h
    src/database/annotatedimagedialog.h
    src/database/imagedialog.h
    src/database/reconstructiondialog.h
    src/database/chooseinputimagesdialog.h
    src/database/projectpage.h
    src/database/segmentationdialog.h
    src/database/imageslistdialog.h
    src/database/externimagesdialog.h
    src/database/twoimagesdialog.h
    src/database/htmlviewer.h
    src/database/zoomwebview.h
    src/database/filedialog.h
    src/database/graphvizdialog.h
    src/database/QGraphicsViewEc.h
    src/database/newreconstructiondialog.h
)

# list of resource files which can be used by the plug-in
# system without loading the plug-ins shared library,
# for example the icon used in the menu and tabs for the
# plug-in views in the workbench
set(CACHED_RESOURCE_FILES
  resources/icon.xpm
  resources/segmentation.png
  resources/btnNew.png
  resources/iconminus.png
  resources/iconplus.png
  resources/iconcheck.png
  resources/iconuncheck.png
  resources/reprocess.png
  resources/base.html  
  resources/processdata.apd
  resources/svmtest2D.model
  resources/svmtest2D.scale
  resources/svmtest3D.model
  resources/svmtest3D.scale
  plugin.xml
)

# list of Qt .qrc files which contain additional resources
# specific to this plugin
set(QRC_FILES
  resources/plugin-resources.qrc
)

set(CPP_FILES )

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})

foreach(file ${DATABASE_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/database/${file})
endforeach(file ${DATABASE_CPP_FILES})

