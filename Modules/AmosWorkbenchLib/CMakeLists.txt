MITK_CREATE_MODULE(AmosWorkbenchLib
  INCLUDE_DIRS Interactions libsvm
  DEPENDS MitkCore MitkSegmentation MitkQtWidgets
    # MITK 2021
    PACKAGE_DEPENDS PUBLIC ITK|IONIFTI+LabelMap
)

#set(module_src amosModuleActivator.cpp)
#usFunctionGenerateModuleInit(module_src)

# target_link_libraries (${PROJECT_NAME} LINK_PUBLIC libsvm)
# target_link_libraries (${PROJECT_NAME} LINK_PUBLIC libsvm LINK_PUBLIC /home/jose/mitk/source/MITK-superbuild/ep/lib/libITKniftiio-4.7d.so.1)
