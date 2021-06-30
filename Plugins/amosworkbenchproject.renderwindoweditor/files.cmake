SET(SRC_CPP_FILES 
)

SET(INTERNAL_CPP_FILES
  amosworkbenchproject_renderwindoweditor_Activator.cpp
  QmitkAmosWorkbenchRenderWindowEditor.cpp
  amoswidget.cpp
  myqmitkrenderwindow.cpp
  MyQmitkRenderWindowWidget.cpp
  MyQmitkRenderWindowMenu.cpp
  QmitkAmosWidgetEditorPreferencePage.cpp
  myqmitkmultiwidgetlayoutmanager.cpp
)

SET(MOC_H_FILES
  src/internal/amosworkbenchproject_renderwindoweditor_Activator.h 
  src/internal/QmitkAmosWorkbenchRenderWindowEditor.h
  src/internal/amoswidget.h
  src/internal/myqmitkrenderwindow.h
  src/internal/MyQmitkRenderWindowWidget.h
  src/internal/MyQmitkRenderWindowMenu.h
  src/internal/QmitkAmosWidgetEditorPreferencePage.h
  #scr/internal/myqmitkmultiwidgetlayoutmanager.h
)

SET(UI_FILES
  src/internal/QmitkAmosWidgetEditorPreferencePage.ui
)

SET(CACHED_RESOURCE_FILES
  plugin.xml
  resources/iconFullScreen.xpm
  resources/iconLeaveFullScreen.xpm
  resources/iconSettings.xpm
  resources/StdMultiWidgetEditor.png
)

SET(QRC_FILES
)

SET(CPP_FILES )

foreach(file ${SRC_CPP_FILES})
  SET(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  SET(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})
