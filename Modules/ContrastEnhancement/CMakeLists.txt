MITK_CREATE_MODULE(ContrastEnhancement
  INCLUDE_DIRS edison
  DEPENDS MitkCore MitkSegmentation MitkQtWidgets AmosWorkbenchLib
  # MITK 2021 removed, not necessary
  # PACKAGE_DEPENDS PUBLIC ITK|IONIFTI
)

#set(module_src amosModuleActivator.cpp)
#usFunctionGenerateModuleInit(module_src)

# target_link_libraries (${PROJECT_NAME} LINK_PUBLIC libsvm)
# target_link_libraries (${PROJECT_NAME} LINK_PUBLIC libsvm LINK_PUBLIC /home/jose/mitk/source/MITK-superbuild/ep/lib/libITKniftiio-4.7d.so.1)
