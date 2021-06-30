PROJECT(amosworkbenchproject_renderwindoweditor)

mitk_create_plugin(
  EXPORT_DIRECTIVE AMOSWORKBENCHPROJECT_RENDERWINDOWEDITOR
  EXPORTED_INCLUDE_SUFFIXES src src/internal
  MODULE_DEPENDS MitkQtWidgets MitkQtWidgetsExt MitkMultilabel AmosWorkbenchLib
  SUBPROJECTS MITK-CoreUI
  )
