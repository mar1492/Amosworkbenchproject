project(amosworkbenchproject_plugin)

include_directories(${CTK_INCLUDE_DIRS} /usr/include/qgv /usr/include/qew5)

find_library(QEW_LIBRARY
    NAMES qewextensibledialogs5 libqewextensibledialogs5
    HINTS "/usr/lib/x86_64-linux-gnu/qt5/plugins/designer"
)

MESSAGE( STATUS "QEW_LIBRARY:         " ${QEW_LIBRARY} )

find_library(QGV_LIBRARY
    NAMES QGVCore libQGVCore
    HINTS "/usr/lib"
)

# for make package to conatian the library
# find_library(QGV_LIBRARY
#     NAMES QGVCore libQGVCore
#     PATHS "/home/jose/mitk/qtprojects/build-QGraphViz-Desktop5-Release/lib"
#     NO_CMAKE_PATH
#     NO_CMAKE_ENVIRONMENT_PATH
#     NO_SYSTEM_ENVIRONMENT_PATH
#     NO_CMAKE_SYSTEM_PATH    
# )
# find_library(QGV_LIBRARY libQGVCore.so "/home/jose/mitk/qtprojects/build-QGraphViz-Desktop5-Release/lib"   
# )
MESSAGE( STATUS "QGV_LIBRARY:         " ${QGV_LIBRARY} )
set(LIBS ${LIBS} ${QEW_LIBRARY} ${QGV_LIBRARY})

mitk_create_plugin(
  EXPORT_DIRECTIVE AMOSWORKBENCH_EXPORTS
  EXPORTED_INCLUDE_SUFFIXES src src/internal src/database
  MODULE_DEPENDS MitkQtWidgetsExt AmosWorkbenchLib ExternalProcess ContrastEnhancement MitkSegmentationUI MitkMultilabel amosworkbenchproject_renderwindoweditor 
  PACKAGE_DEPENDS Qt5|WebEngineWidgets  
)
MESSAGE( STATUS "PLUGIN_TARGET:         " ${PLUGIN_TARGET} )
target_link_libraries(${PLUGIN_TARGET} LINK_PRIVATE ${LIBS})
