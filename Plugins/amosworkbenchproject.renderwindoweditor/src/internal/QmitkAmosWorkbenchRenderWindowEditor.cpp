/*=========================================================================
 * 
 * Program:   Medical Imaging & Interaction Toolkit
 * Language:  C++
 * Date:      $Date$
 * Version:   $Revision$
 * 
 * Copyright (c) German Cancer Research Center, Division of Medical and
 * Biological Informatics. All rights reserved.
 * See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.
 * 
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the above copyright notices for more information.
 * 
 * =========================================================================*/

#include "QmitkAmosWorkbenchRenderWindowEditor.h"
#include "amoswidget.h"

#include <mitkPlaneGeometry.h>
#include <mitkInteractionConst.h>

#include <berryUIException.h>
#include <berryIWorkbenchPage.h>
#include <berryIPreferencesService.h>
// #include <berryIPartListener.h>
#include <berryIPreferences.h>

#include <mitkColorProperty.h>
// // #include <mitkGlobalInteraction.h>
#include <mitkNodePredicateNot.h>
#include <mitkNodePredicateProperty.h>

#include <mitkDataStorageEditorInput.h>
#include <mitkIDataStorageService.h>

 // mitk qt widgets module
#include <QmitkInteractionSchemeToolBar.h>
#include <QmitkLevelWindowWidget.h>
#include <QmitkRenderWindowWidget.h>
//   #include <QmitkStdMultiWidget.h>

// mitk gui qt common plugin
#include <QmitkMultiWidgetDecorationManager.h>
 
// #include <QmitkMouseModeSwitcher.h>
// #include <mitkWorkbenchUtil.h>

// // #include <mbilogo.h>

// class QmitkAmosWorkbenchRenderWindowEditorPrivate
// {
// public:
//     
//     QmitkAmosWorkbenchRenderWindowEditorPrivate();
//     ~QmitkAmosWorkbenchRenderWindowEditorPrivate();
//     
//     AmosWidget* m_amosWidget;
//     QmitkMouseModeSwitcher* m_MouseModeToolbar;
//     /**
//      * @brief Members for the MultiWidget decorations.
//      */
//     QString m_WidgetBackgroundColor1[4];
//     QString m_WidgetBackgroundColor2[4];
//     QString m_WidgetDecorationColor[4];
//     QString m_WidgetAnnotation[4];
//     QString m_SegmentationColor[2];
//     bool m_MenuWidgetsEnabled;
//     QScopedPointer<berry::IPartListener> m_PartListener;
//     
//     QHash<QString, QmitkRenderWindow*> m_RenderWindows;
//     
// };

// struct QmitkAmosWidgetPartListener : public berry::IPartListener
// {
//     QmitkAmosWidgetPartListener(QmitkAmosWorkbenchRenderWindowEditorPrivate* dd)
//     : d(dd)
//     {}
//     
//     Events::Types GetPartEventTypes() const override
//     {
//         return Events::CLOSED | Events::HIDDEN | Events::VISIBLE;
//     }
//     
//     void PartClosed(const berry::IWorkbenchPartReference::Pointer& partRef) override
//     {
//         if (partRef->GetId() == QmitkAmosWorkbenchRenderWindowEditor::EDITOR_ID)
//         {
//             QmitkAmosWorkbenchRenderWindowEditor::Pointer amosWidgetEditor = partRef->GetPart(false).Cast<QmitkAmosWorkbenchRenderWindowEditor>();
//             
//             if (d->m_amosWidget == amosWidgetEditor->GetMultiWidget())
//             {
//                 d->m_amosWidget->RemovePlanesFromDataStorage();
//                 amosWidgetEditor->RequestActivateMenuWidget(false);
//             }
//         }
//     }
//     
//     void PartHidden(const berry::IWorkbenchPartReference::Pointer& partRef) override
//     {
//         if (partRef->GetId() == QmitkAmosWorkbenchRenderWindowEditor::EDITOR_ID)
//         {
//             QmitkAmosWorkbenchRenderWindowEditor::Pointer amosWidgetEditor = partRef->GetPart(false).Cast<QmitkAmosWorkbenchRenderWindowEditor>();
//             
//             if (d->m_amosWidget == amosWidgetEditor->GetMultiWidget())
//             {
//                 d->m_amosWidget->RemovePlanesFromDataStorage();
//                 amosWidgetEditor->RequestActivateMenuWidget(false);
//             }
//         }
//     }
//     
//     void PartVisible(const berry::IWorkbenchPartReference::Pointer& partRef) override
//     {
//         if (partRef->GetId() == QmitkAmosWorkbenchRenderWindowEditor::EDITOR_ID)
//         {
//             QmitkAmosWorkbenchRenderWindowEditor::Pointer amosWidgetEditor = partRef->GetPart(false).Cast<QmitkAmosWorkbenchRenderWindowEditor>();
//             
//             if (d->m_amosWidget == amosWidgetEditor->GetMultiWidget())
//             {
//                 d->m_amosWidget->AddPlanesToDataStorage();
//                 amosWidgetEditor->RequestActivateMenuWidget(true);
//             }
//         }
//     }
//     
// private:
//     
//     QmitkAmosWorkbenchRenderWindowEditorPrivate* const d;
//     
// };

// QmitkAmosWorkbenchRenderWindowEditorPrivate::QmitkAmosWorkbenchRenderWindowEditorPrivate()
// : m_amosWidget(0), m_MouseModeToolbar(0)
// , m_MenuWidgetsEnabled(false)
// , m_PartListener(new QmitkAmosWidgetPartListener(this))
// {}

// QmitkAmosWorkbenchRenderWindowEditorPrivate::~QmitkAmosWorkbenchRenderWindowEditorPrivate()
// {    
// }

const QString QmitkAmosWorkbenchRenderWindowEditor::EDITOR_ID = "amosworkbenchproject.editors.renderwindow";

struct QmitkAmosWorkbenchRenderWindowEditor::Impl final
{
    Impl();
    ~Impl() = default;
    
    QmitkInteractionSchemeToolBar* m_InteractionSchemeToolBar;
    QmitkLevelWindowWidget* m_LevelWindowWidget;
    
    std::unique_ptr<QmitkMultiWidgetDecorationManager> m_MultiWidgetDecorationManager;
};

QmitkAmosWorkbenchRenderWindowEditor::Impl::Impl() : m_InteractionSchemeToolBar(nullptr)
, m_LevelWindowWidget(nullptr)
{
    // nothing here
}

// QmitkStdMultiWidgetEditor
QmitkAmosWorkbenchRenderWindowEditor::QmitkAmosWorkbenchRenderWindowEditor() : QmitkAbstractMultiWidgetEditor()
, m_Impl(std::make_unique<Impl>())
{
    // nothing here
}

// QmitkAmosWorkbenchRenderWindowEditor::QmitkAmosWorkbenchRenderWindowEditor() : d(new QmitkAmosWorkbenchRenderWindowEditorPrivate)
// {
// }

// QmitkAmosWorkbenchRenderWindowEditor::~QmitkAmosWorkbenchRenderWindowEditor()
// {
//     this->GetSite()->GetPage()->RemovePartListener(d->m_PartListener.data());
// }

QmitkAmosWorkbenchRenderWindowEditor::~QmitkAmosWorkbenchRenderWindowEditor()
{
    GetSite()->GetPage()->RemovePartListener(this);
}

// AmosWidget* QmitkAmosWorkbenchRenderWindowEditor::GetAmosWidget()
// {
//     return d->m_amosWidget;
// }

// QmitkRenderWindow *QmitkAmosWorkbenchRenderWindowEditor::GetActiveQmitkRenderWindow() const
// {
//     if (d->m_amosWidget) return d->m_amosWidget->GetRenderWindowAC();
//     return 0;
// }

berry::IPartListener::Events::Types QmitkAmosWorkbenchRenderWindowEditor::GetPartEventTypes() const
{
    return Events::CLOSED | Events::OPENED | Events::HIDDEN | Events::VISIBLE;
}

void QmitkAmosWorkbenchRenderWindowEditor::PartClosed(const berry::IWorkbenchPartReference::Pointer& partRef) 
{
    if (partRef->GetId() == QmitkAmosWorkbenchRenderWindowEditor::EDITOR_ID)
    {
        const auto& amosWidget = dynamic_cast<AmosWidget*>(GetMultiWidget());
        if (nullptr != amosWidget)
        {
            amosWidget->RemovePlanesFromDataStorage();
            amosWidget->ActivateMenuWidget(false);
        }
    }    
}

void QmitkAmosWorkbenchRenderWindowEditor::PartOpened(const berry::IWorkbenchPartReference::Pointer& partRef)
{
    if (partRef->GetId() == QmitkAmosWorkbenchRenderWindowEditor::EDITOR_ID)
    {
        const auto& amosWidget = dynamic_cast<AmosWidget*>(GetMultiWidget());
        if (nullptr != amosWidget)
        {
            amosWidget->AddPlanesToDataStorage();
            amosWidget->ActivateMenuWidget(true);
        }
    }
}

void QmitkAmosWorkbenchRenderWindowEditor::PartHidden(const berry::IWorkbenchPartReference::Pointer& partRef)
{
    if (partRef->GetId() == QmitkAmosWorkbenchRenderWindowEditor::EDITOR_ID)
    {
        const auto& amosWidget = dynamic_cast<AmosWidget*>(GetMultiWidget());
        if (nullptr != amosWidget)
        {
            amosWidget->ActivateMenuWidget(false);
        }
    }
}

void QmitkAmosWorkbenchRenderWindowEditor::PartVisible(const berry::IWorkbenchPartReference::Pointer& partRef)
{
    if (partRef->GetId() == QmitkAmosWorkbenchRenderWindowEditor::EDITOR_ID)
    {
        const auto& amosWidget = dynamic_cast<AmosWidget*>(GetMultiWidget());
        if (nullptr != amosWidget)
        {
            amosWidget->ActivateMenuWidget(true);
        }
    }
}

void QmitkAmosWorkbenchRenderWindowEditor::EnableSlicingPlanes(bool /*enable*/) 
{}

bool QmitkAmosWorkbenchRenderWindowEditor::IsSlicingPlanesEnabled() const
{ return false; }

void QmitkAmosWorkbenchRenderWindowEditor::ShowLevelWindowWidget(bool show)
{
    if (show)
    {
        m_Impl->m_LevelWindowWidget->disconnect(this);
        m_Impl->m_LevelWindowWidget->SetDataStorage(GetDataStorage());
        m_Impl->m_LevelWindowWidget->show();
    }
    else
    {
        m_Impl->m_LevelWindowWidget->disconnect(this);
        m_Impl->m_LevelWindowWidget->hide();
    }
}

QmitkLevelWindowWidget* QmitkAmosWorkbenchRenderWindowEditor::GetLevelWindowWidget() const
{
//     return GetMultiWidget()->GetLevelWindowWidget();
    return m_Impl->m_LevelWindowWidget;
}

// QHash<QString, QmitkRenderWindow *> QmitkAmosWorkbenchRenderWindowEditor::GetQmitkRenderWindows() const
// {
//     return d->m_RenderWindows;
// }

// QmitkRenderWindow *QmitkAmosWorkbenchRenderWindowEditor::GetQmitkRenderWindow(const QString &id) const
// {
//     if (d->m_RenderWindows.contains(id))
//         return d->m_RenderWindows[id];
//     
//     return 0;
// }

// For revision
mitk::Point3D QmitkAmosWorkbenchRenderWindowEditor::GetSelectedPosition(const QString & /*id*/) const
{
    const mitk::PlaneGeometry* pg =
    GetActiveQmitkRenderWindow()->GetSliceNavigationController()->GetCurrentPlaneGeometry();
    if (pg)
    {
        return pg->GetCenter();
    }
    else
    {
        return mitk::Point3D();
    }
}

// For revision
void QmitkAmosWorkbenchRenderWindowEditor::SetSelectedPosition(const mitk::Point3D &pos, const QString &/*id*/)
{
    GetActiveQmitkRenderWindow()->GetSliceNavigationController()->SelectSliceByPoint(pos);
}

// void QmitkAmosWorkbenchRenderWindowEditor::EnableDecorations(bool enable, const QStringList &decorations)
// {
//     if (decorations.isEmpty() || decorations.contains(DECORATION_MENU))
//     {
//         if (decorations.isEmpty() || decorations.contains(DECORATION_BORDER))
//         {
//             enable ? d->m_amosWidget->EnableColoredRectangles()
//             : d->m_amosWidget->DisableColoredRectangles();
//         }
//         if (decorations.isEmpty() || decorations.contains(DECORATION_LOGO))
//         {
//             enable ? d->m_amosWidget->EnableDepartmentLogo()
//             : d->m_amosWidget->DisableDepartmentLogo();
//         }
//         if (decorations.isEmpty() || decorations.contains(DECORATION_MENU))
//         {
//             d->m_amosWidget->ActivateMenuWidget(enable);
//         }
//         if (decorations.isEmpty() || decorations.contains(DECORATION_BACKGROUND))
//         {
//             enable ? d->m_amosWidget->EnableGradientBackground()
//             : d->m_amosWidget->DisableGradientBackground();
//         }
//     }
// }

// bool QmitkAmosWorkbenchRenderWindowEditor::IsDecorationEnabled(const QString &decoration) const
// {
//     if (decoration == DECORATION_BORDER)
//     {
//         return d->m_amosWidget->IsColoredRectanglesEnabled();
//     }
//     else if (decoration == DECORATION_LOGO)
//     {
//         return d->m_amosWidget->IsColoredRectanglesEnabled();
//     }
//     else if (decoration == DECORATION_MENU)
//     {
//         return d->m_amosWidget->IsMenuWidgetEnabled();
//     }
//     else if (decoration == DECORATION_BACKGROUND)
//     {
//         return d->m_amosWidget->GetGradientBackgroundFlag();
//     }
//     return false;
//     
// }

// QStringList QmitkAmosWorkbenchRenderWindowEditor::GetDecorations() const
// {
//     QStringList decorations;
//     decorations << DECORATION_BORDER << DECORATION_LOGO << DECORATION_MENU << DECORATION_BACKGROUND;
//     return decorations;
// }

// mitk::SlicesRotator* QmitkAmosWorkbenchRenderWindowEditor::GetSlicesRotator() const
// { return 0; }
// 
// mitk::SlicesSwiveller* QmitkAmosWorkbenchRenderWindowEditor::GetSlicesSwiveller() const
// { return 0; }

// void QmitkAmosWorkbenchRenderWindowEditor::EnableLinkedNavigation(bool /*enable*/)
// {}
// bool QmitkAmosWorkbenchRenderWindowEditor::IsLinkedNavigationEnabled() const
// { return false; } 

void QmitkAmosWorkbenchRenderWindowEditor::SetFocus()
{
//     if (d->m_amosWidget != 0)
//         d->m_amosWidget->setFocus();
   const auto& amosWidget = GetMultiWidget();
   if (nullptr != amosWidget)
   {
     amosWidget->setFocus();
   }    
}

// void QmitkAmosWorkbenchRenderWindowEditor::CreateQtPartControl(QWidget* parent)
// {
//     if (d->m_amosWidget == 0)
//     {
//         
//         QHBoxLayout* layout = new QHBoxLayout(parent);
//         layout->setContentsMargins(0,0,0,0);
//         
//         if (d->m_MouseModeToolbar == NULL)
//         {
//             d->m_MouseModeToolbar = new QmitkMouseModeSwitcher(parent); // delete by Qt via parent
//             layout->addWidget(d->m_MouseModeToolbar);
//         }
//         
//         berry::IPreferences::Pointer prefs = this->GetPreferences();
//         
//         mitk::BaseRenderer::RenderingMode::Type renderingMode = static_cast<mitk::BaseRenderer::RenderingMode::Type>(prefs->GetInt( "Rendering Mode" , 0 ));
//         
//         d->m_amosWidget = new AmosWidget(parent,0,0,renderingMode);
//         d->m_RenderWindows.insert("axial", d->m_amosWidget->GetRenderWindowAC());
//         d->m_RenderWindows.insert("axial2", d->m_amosWidget->GetRenderWindowBC());
//         d->m_RenderWindows.insert("axial3", d->m_amosWidget->GetRenderWindowPatient());
//         d->m_RenderWindows.insert("3d", d->m_amosWidget->GetRenderWindow3D());
//         
//         d->m_MouseModeToolbar->setMouseModeSwitcher( d->m_amosWidget->GetMouseModeSwitcher() );
//         //     connect( d->m_MouseModeToolbar, SIGNAL( MouseModeSelected(mitk::MouseModeSwitcher::MouseMode) ),
//         //       d->m_amosWidget, SLOT( MouseModeSelected(mitk::MouseModeSwitcher::MouseMode) ) );
//         
//         layout->addWidget(d->m_amosWidget);    
//         
//         qRegisterMetaType<mitk::DataNode*>();
//         connect(d->m_amosWidget, SIGNAL(sendSegmentationNodeAdded(const mitk::DataNode*)), this, SLOT(segmentationNodeAdded(const mitk::DataNode*)));
//         connect(d->m_amosWidget, SIGNAL(sendSegmentationNodeRemoved(const mitk::DataNode*)), this, SLOT(segmentationNodeRemoved(const mitk::DataNode*)));
//         
//         mitk::DataStorage::Pointer ds = this->GetDataStorage();
//         
//         // Tell the multiWidget which (part of) the tree to render
//         d->m_amosWidget->SetDataStorage(ds);
//         
//         // Initialize views as axial, sagittal, coronar to all data objects in DataStorage
//         // (from top-left to bottom)
//         mitk::TimeGeometry::Pointer geo = ds->ComputeBoundingGeometry3D(ds->GetAll());
//         mitk::RenderingManager::GetInstance()->InitializeViews(geo);
//         
//         // Initialize bottom-right view as 3D view
//         d->m_amosWidget->GetRenderWindow3D()->GetRenderer()->SetMapperID(mitk::BaseRenderer::Standard3D );
//         
//         // Enable standard handler for levelwindow-slider
//         d->m_amosWidget->EnableStandardLevelWindow();
//         
//         // Add the displayed views to the tree to see their positions
//         // in 2D and 3D
//         d->m_amosWidget->AddDisplayPlaneSubTree();
//         
//         // Enabling this a problem happen when left clicking in AC or BC windows 
//         // slice change from this_slice to last_slice - this_slice
//         // Solved
//         //     d->m_amosWidget->EnableNavigationControllerEventListening();
//         
//         // Store the initial visibility status of the menu widget.
//         d->m_MenuWidgetsEnabled = d->m_amosWidget->IsMenuWidgetEnabled();
//         
//         this->GetSite()->GetPage()->AddPartListener(d->m_PartListener.data());
//         
//         berry::IBerryPreferences* berryprefs = dynamic_cast<berry::IBerryPreferences*>(prefs.GetPointer());
//         InitializePreferences(berryprefs);
//         this->OnPreferencesChanged(berryprefs);
//         
//         this->RequestUpdate();
//     }
// }

void QmitkAmosWorkbenchRenderWindowEditor::CreateQtPartControl(QWidget* parent)
{
    QHBoxLayout* layout = new QHBoxLayout(parent);
    layout->setContentsMargins(0, 0, 0, 0);
    
    berry::IBerryPreferences* preferences = dynamic_cast<berry::IBerryPreferences*>(GetPreferences().GetPointer());
    
    auto amosWidget = GetMultiWidget();
    if (nullptr == amosWidget)
    {
        amosWidget = new AmosWidget(parent, Qt::WindowFlags());
        
        // create left toolbar: interaction scheme toolbar to switch how the render window navigation behaves (in PACS mode)
        if (nullptr == m_Impl->m_InteractionSchemeToolBar)
        {
            m_Impl->m_InteractionSchemeToolBar = new QmitkInteractionSchemeToolBar(parent);
            layout->addWidget(m_Impl->m_InteractionSchemeToolBar);
        }
        m_Impl->m_InteractionSchemeToolBar->SetInteractionEventHandler(amosWidget->GetInteractionEventHandler());
        
        // show / hide PACS mouse mode interaction scheme toolbar
        bool PACSInteractionScheme = preferences->GetBool("PACS like mouse interaction", false);
        m_Impl->m_InteractionSchemeToolBar->setVisible(PACSInteractionScheme);        
        
        amosWidget->SetDataStorage(GetDataStorage());
        amosWidget->InitializeMultiWidget();
        SetMultiWidget(amosWidget);
    }
    
    layout->addWidget(amosWidget);
    
    // create level window slider on the right side
    if (nullptr == m_Impl->m_LevelWindowWidget)
    {
        m_Impl->m_LevelWindowWidget = new QmitkLevelWindowWidget(parent, Qt::WindowFlags());
        m_Impl->m_LevelWindowWidget->setObjectName(QString::fromUtf8("levelWindowWidget"));
        
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(m_Impl->m_LevelWindowWidget->sizePolicy().hasHeightForWidth());
        m_Impl->m_LevelWindowWidget->setSizePolicy(sizePolicy);
        m_Impl->m_LevelWindowWidget->setMaximumWidth(50);
    }
    
    layout->addWidget(m_Impl->m_LevelWindowWidget);
    
    m_Impl->m_MultiWidgetDecorationManager = std::make_unique<QmitkMultiWidgetDecorationManager>(amosWidget);
    
    GetSite()->GetPage()->AddPartListener(this);
    
    InitializePreferences(preferences);
    OnPreferencesChanged(preferences);
    
    qRegisterMetaType<mitk::DataNode*>();
    connect(amosWidget, SIGNAL(sendSegmentationNodeAdded(const mitk::DataNode*)), this, SLOT(segmentationNodeAdded(const mitk::DataNode*)));
    connect(amosWidget, SIGNAL(sendSegmentationNodeRemoved(const mitk::DataNode*)), this, SLOT(segmentationNodeRemoved(const mitk::DataNode*)));    
}

// void QmitkAmosWorkbenchRenderWindowEditor::OnPreferencesChanged(const berry::IBerryPreferences* prefs)
// {
//     // Enable change of logo. If no DepartmentLogo was set explicitly, MBILogo is used.
//     // Set new department logo by prefs->Set("DepartmentLogo", "PathToImage");
//     
//     // If no logo was set for this plug-in specifically, walk the parent preference nodes
//     // and lookup a logo value there.
//     
//     const berry::IPreferences* currentNode = prefs;
//     
//     while(currentNode)
//     {
//         bool logoFound = false;
//         foreach (const QString& key, currentNode->Keys())
//         {
//             if( key == "DepartmentLogo")
//             {
//                 QString departmentLogoLocation = currentNode->Get("DepartmentLogo", "");
//                 
//                 if (departmentLogoLocation.isEmpty())
//                 {
//                     d->m_amosWidget->DisableDepartmentLogo();
//                 }
//                 else
//                 {
//                     // we need to disable the logo first, otherwise setting a new logo will have
//                     // no effect due to how mitkManufacturerLogo works...
//                     d->m_amosWidget->DisableDepartmentLogo();
//                     d->m_amosWidget->SetDepartmentLogoPath(qPrintable(departmentLogoLocation));
//                     d->m_amosWidget->EnableDepartmentLogo();
//                 }
//                 logoFound = true;
//                 break;
//             }
//         }
//         
//         if (logoFound) break;
//         currentNode = currentNode->Parent().GetPointer();
//     }
//     
//     //Update internal members
//     this->FillMembersWithCurrentDecorations();
//     this->GetPreferenceDecorations(prefs);
//     //Now the members can be used to modify the stdmultiwidget
//     mitk::Color upper = HexColorToMitkColor(d->m_WidgetBackgroundColor1[0]);
//     mitk::Color lower = HexColorToMitkColor(d->m_WidgetBackgroundColor2[0]);
//     d->m_amosWidget->SetGradientBackgroundColorForRenderWindow(upper, lower, 0);
//     upper = HexColorToMitkColor(d->m_WidgetBackgroundColor1[1]);
//     lower = HexColorToMitkColor(d->m_WidgetBackgroundColor2[1]);
//     d->m_amosWidget->SetGradientBackgroundColorForRenderWindow(upper, lower, 1);
//     upper = HexColorToMitkColor(d->m_WidgetBackgroundColor1[2]);
//     lower = HexColorToMitkColor(d->m_WidgetBackgroundColor2[2]);
//     d->m_amosWidget->SetGradientBackgroundColorForRenderWindow(upper, lower, 2);
//     upper = HexColorToMitkColor(d->m_WidgetBackgroundColor1[3]);
//     lower = HexColorToMitkColor(d->m_WidgetBackgroundColor2[3]);
//     d->m_amosWidget->SetGradientBackgroundColorForRenderWindow(upper, lower, 3);
//     d->m_amosWidget->EnableGradientBackground();
//     
//     // preferences for renderWindows
//     mitk::Color colorDecorationWidget1 = HexColorToMitkColor(d->m_WidgetDecorationColor[0]);
//     mitk::Color colorDecorationWidget2 = HexColorToMitkColor(d->m_WidgetDecorationColor[1]);
//     mitk::Color colorDecorationWidget3 = HexColorToMitkColor(d->m_WidgetDecorationColor[2]);
//     mitk::Color colorDecorationWidget4 = HexColorToMitkColor(d->m_WidgetDecorationColor[3]);
//     d->m_amosWidget->SetDecorationColor(0, colorDecorationWidget1);
//     d->m_amosWidget->SetDecorationColor(1, colorDecorationWidget2);
//     d->m_amosWidget->SetDecorationColor(2, colorDecorationWidget3);
//     d->m_amosWidget->SetDecorationColor(3, colorDecorationWidget4);
//     
//     // preferences for renderWindows
//     for(unsigned int i = 0; i < 4; ++i)
//     {
//         d->m_amosWidget->SetDecorationProperties(d->m_WidgetAnnotation[i].toStdString(), HexColorToMitkColor(d->m_WidgetDecorationColor[i]), i);
//     }
//     
//     // preferences for segmentations
//     mitk::Color colorSegmentationAC = HexColorToMitkColor(d->m_SegmentationColor[0]);
//     mitk::Color colorSegmentationBC = HexColorToMitkColor(d->m_SegmentationColor[1]);
//     d->m_amosWidget->SetSegmentationColor(0, colorSegmentationAC);
//     d->m_amosWidget->SetSegmentationColor(1, colorSegmentationBC);    
//     
//     //The crosshair gap
//     //   int crosshairgapsize = prefs->GetInt("crosshair gap size", 32);
//     //   d->m_amosWidget->GetWidgetPlaneAC()->SetIntProperty("Crosshair.Gap Size", crosshairgapsize);
//     //   d->m_amosWidget->GetWidgetPlaneBC()->SetIntProperty("Crosshair.Gap Size", crosshairgapsize);
//     
//     //refresh colors of rectangles
//     d->m_amosWidget->EnableColoredRectangles();
//     
//     // Set preferences respecting zooming and padding
//     //   bool constrainedZooming = prefs->GetBool("Use constrained zooming and padding", true);
//     
//     //   mitk::RenderingManager::GetInstance()->SetConstrainedPaddingZooming(constrainedZooming);
//     
//     mitk::RenderingManager::GetInstance()->InitializeViewsByBoundingObjects(this->GetDataStorage());
//     
//     mitk::RenderingManager::GetInstance()->RequestUpdateAll();
//     
//     level window setting
//     bool showLevelWindowWidget = prefs->GetBool("Show level/window widget", true);
//     if (showLevelWindowWidget)
//     {
//         d->m_amosWidget->EnableStandardLevelWindow();
//     }
//     else
//     {
//         d->m_amosWidget->DisableStandardLevelWindow();
//     }
//     
//     // mouse modes toolbar
//     bool newMode = prefs->GetBool("PACS like mouse interaction", false);
//     d->m_MouseModeToolbar->setVisible( newMode );
//     d->m_amosWidget->GetMouseModeSwitcher()->SetInteractionScheme( newMode ? mitk::MouseModeSwitcher::PACS : mitk::MouseModeSwitcher::MITK );
//     
//     QTimer::singleShot(100, this, SLOT(delayedReinit()));
// }  

void QmitkAmosWorkbenchRenderWindowEditor::OnPreferencesChanged(const berry::IBerryPreferences* preferences)
{
    const auto& amosWidget = dynamic_cast<AmosWidget*>(GetMultiWidget());
    if (nullptr == amosWidget)
    {
        return;
    }
    
    // change and apply decoration preferences
    GetPreferenceDecorations(preferences);
    m_Impl->m_MultiWidgetDecorationManager->DecorationPreferencesChanged(preferences);
    
    QmitkAbstractMultiWidget::RenderWindowWidgetMap renderWindowWidgets = amosWidget->GetRenderWindowWidgets();
    int i = 0;
    for (const auto& renderWindowWidget : renderWindowWidgets)
    {
        auto decorationColor = renderWindowWidget.second->GetDecorationColor();
        amosWidget->SetDecorationColor(i, decorationColor);
        
        ++i;
    }
    
//     int crosshairgapsize = preferences->GetInt("crosshair gap size", 32);
//     amosWidget->GetWidgetPlane1()->SetIntProperty("Crosshair.Gap Size", crosshairgapsize);
//     amosWidget->GetWidgetPlane2()->SetIntProperty("Crosshair.Gap Size", crosshairgapsize);
//     amosWidget->GetWidgetPlane3()->SetIntProperty("Crosshair.Gap Size", crosshairgapsize);
    
    // zooming and panning preferences
    bool constrainedZooming = preferences->GetBool("Use constrained zooming and panning", true);
    mitk::RenderingManager::GetInstance()->SetConstrainedPanningZooming(constrainedZooming);
    
    // mouse modes switcher toolbar
    bool PACSInteractionScheme = preferences->GetBool("PACS like mouse interaction", false);
    OnInteractionSchemeChanged(PACSInteractionScheme ?
    mitk::InteractionSchemeSwitcher::PACSStandard :
    mitk::InteractionSchemeSwitcher::MITKStandard);
    
    // level window setting
    bool showLevelWindowWidget = preferences->GetBool("Show level/window widget", true);
    ShowLevelWindowWidget(showLevelWindowWidget);
    
    mitk::RenderingManager::GetInstance()->InitializeViewsByBoundingObjects(GetDataStorage());
    mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

// void QmitkAmosWorkbenchRenderWindowEditor::InitializePreferences(berry::IBerryPreferences * preferences)
// {
//     this->FillMembersWithCurrentDecorations(); //fill members
//     this->GetPreferenceDecorations(preferences); //overwrite if preferences are defined
//     
//     //create new preferences
//     preferences->Put("widget1 corner annotation", d->m_WidgetAnnotation[0]);
//     preferences->Put("widget2 corner annotation", d->m_WidgetAnnotation[1]);
//     preferences->Put("widget3 corner annotation", d->m_WidgetAnnotation[2]);
//     preferences->Put("widget4 corner annotation", d->m_WidgetAnnotation[3]);
//     
//     preferences->Put("widget1 decoration color", d->m_WidgetDecorationColor[0]);
//     preferences->Put("widget2 decoration color", d->m_WidgetDecorationColor[1]);
//     preferences->Put("widget3 decoration color", d->m_WidgetDecorationColor[2]);
//     preferences->Put("widget4 decoration color", d->m_WidgetDecorationColor[3]);
//     
//     preferences->Put("widget1 first background color", d->m_WidgetBackgroundColor1[0]);
//     preferences->Put("widget2 first background color", d->m_WidgetBackgroundColor1[1]);
//     preferences->Put("widget3 first background color", d->m_WidgetBackgroundColor1[2]);
//     preferences->Put("widget4 first background color", d->m_WidgetBackgroundColor1[3]);
//     preferences->Put("widget1 second background color", d->m_WidgetBackgroundColor2[0]);
//     preferences->Put("widget2 second background color", d->m_WidgetBackgroundColor2[1]);
//     preferences->Put("widget3 second background color", d->m_WidgetBackgroundColor2[2]);
//     preferences->Put("widget4 second background color", d->m_WidgetBackgroundColor2[3]);
//     
//     preferences->Put("AC segmentation color", d->m_SegmentationColor[0]);
//     preferences->Put("BC segmentation color", d->m_SegmentationColor[1]);  
// }

void QmitkAmosWorkbenchRenderWindowEditor::InitializePreferences(berry::IBerryPreferences * preferences)
{
//     auto amosWidget = this->GetMultiWidget();
    auto amosWidget = dynamic_cast<AmosWidget*>(GetMultiWidget());
    
    if (nullptr == amosWidget)
        return;
    
    this->GetPreferenceDecorations(preferences); // Override if preferences are defined
    
    for (const auto& renderWindowWidget : amosWidget->GetRenderWindowWidgets())
    {
        auto widgetName = renderWindowWidget.second->GetWidgetName();
        
        auto gradientBackgroundColors = renderWindowWidget.second->GetGradientBackgroundColors();
        preferences->Put(widgetName + " first background color", this->MitkColorToHex(gradientBackgroundColors.first));
        preferences->Put(widgetName + " second background color", this->MitkColorToHex(gradientBackgroundColors.second));
        
        auto decorationColor = renderWindowWidget.second->GetDecorationColor();
        preferences->Put(widgetName + " decoration color", this->MitkColorToHex(decorationColor));
        
        auto cornerAnnotation = renderWindowWidget.second->GetCornerAnnotationText();
        preferences->Put(widgetName + " corner annotation", QString::fromStdString(cornerAnnotation));
    }
    
    preferences->Put("AC segmentation color", this->MitkColorToHex(amosWidget->GetSegmentationColor(0)));
    preferences->Put("BC segmentation color", this->MitkColorToHex(amosWidget->GetSegmentationColor(1)));    
}

// void QmitkAmosWorkbenchRenderWindowEditor::GetPreferenceDecorations(const berry::IBerryPreferences * preferences)
// {
//     // overwrite members with values from the preferences, if they the prefrence is defined
//     d->m_WidgetBackgroundColor1[0] = preferences->Get("widget1 first background color", d->m_WidgetBackgroundColor1[0]);
//     d->m_WidgetBackgroundColor2[0] = preferences->Get("widget1 second background color", d->m_WidgetBackgroundColor2[0]);
//     d->m_WidgetBackgroundColor1[1] = preferences->Get("widget2 first background color", d->m_WidgetBackgroundColor1[1]);
//     d->m_WidgetBackgroundColor2[1] = preferences->Get("widget2 second background color", d->m_WidgetBackgroundColor2[1]);
//     d->m_WidgetBackgroundColor1[2] = preferences->Get("widget3 first background color", d->m_WidgetBackgroundColor1[2]);
//     d->m_WidgetBackgroundColor2[2] = preferences->Get("widget3 second background color", d->m_WidgetBackgroundColor2[2]);
//     d->m_WidgetBackgroundColor1[3] = preferences->Get("widget4 first background color", d->m_WidgetBackgroundColor1[3]);
//     d->m_WidgetBackgroundColor2[3] = preferences->Get("widget4 second background color", d->m_WidgetBackgroundColor2[3]);
//     
//     d->m_WidgetDecorationColor[0] = preferences->Get("widget1 decoration color", d->m_WidgetDecorationColor[0]);
//     d->m_WidgetDecorationColor[1] = preferences->Get("widget2 decoration color", d->m_WidgetDecorationColor[1]);
//     d->m_WidgetDecorationColor[2] = preferences->Get("widget3 decoration color", d->m_WidgetDecorationColor[2]);
//     d->m_WidgetDecorationColor[3] = preferences->Get("widget4 decoration color", d->m_WidgetDecorationColor[3]);
//     
//     d->m_SegmentationColor[0] = preferences->Get("AC segmentation color", d->m_SegmentationColor[0]);
//     d->m_SegmentationColor[1] = preferences->Get("BC segmentation color", d->m_SegmentationColor[1]);  
//     
//     d->m_WidgetAnnotation[0] = preferences->Get("widget1 corner annotation", d->m_WidgetAnnotation[0]);
//     d->m_WidgetAnnotation[1] = preferences->Get("widget2 corner annotation", d->m_WidgetAnnotation[1]);
//     d->m_WidgetAnnotation[2] = preferences->Get("widget3 corner annotation", d->m_WidgetAnnotation[2]);
//     d->m_WidgetAnnotation[3] = preferences->Get("widget4 corner annotation", d->m_WidgetAnnotation[3]);
// }

void QmitkAmosWorkbenchRenderWindowEditor::GetPreferenceDecorations(const berry::IBerryPreferences * preferences)
{
    auto amosWidget = dynamic_cast<AmosWidget*>(GetMultiWidget());
    
    if (nullptr == amosWidget)
        return;
    
    auto hexBlack = "#000000";
    auto gradientBlack = "#191919";
    auto gradientGray = "#7F7F7F";
    
    auto renderWindowWidgets = amosWidget->GetRenderWindowWidgets();
    int i = 0;
    for (const auto& renderWindowWidget : renderWindowWidgets)
    {
        auto widgetName = renderWindowWidget.second->GetWidgetName();
        
        if (mitk::BaseRenderer::Standard3D == mitk::BaseRenderer::GetInstance(renderWindowWidget.second->GetRenderWindow()->GetVtkRenderWindow())->GetMapperID())
        {
            auto upper = preferences->Get(widgetName + " first background color", gradientBlack);
            auto lower = preferences->Get(widgetName + " second background color", gradientGray);
            renderWindowWidget.second->SetGradientBackgroundColors(HexColorToMitkColor(upper), HexColorToMitkColor(lower));
        }
        else
        {
            auto upper = preferences->Get(widgetName + " first background color", hexBlack);
            auto lower = preferences->Get(widgetName + " second background color", hexBlack);
            renderWindowWidget.second->SetGradientBackgroundColors(HexColorToMitkColor(upper), HexColorToMitkColor(lower));
        }
        
        auto defaultDecorationColor = amosWidget->GetDecorationColor(i);
        auto decorationColor = preferences->Get(widgetName + " decoration color", MitkColorToHex(defaultDecorationColor));
        renderWindowWidget.second->SetDecorationColor(HexColorToMitkColor(decorationColor));
        
        auto defaultCornerAnnotation = renderWindowWidget.second->GetCornerAnnotationText();
        auto cornerAnnotation = preferences->Get(widgetName + " corner annotation", QString::fromStdString(defaultCornerAnnotation));
        renderWindowWidget.second->SetCornerAnnotationText(cornerAnnotation.toStdString());
        
        ++i;
    }
    
    auto amosSegmentationColor0 = amosWidget->GetSegmentationColor(0);
    auto segmentationColor0 = preferences->Get("AC segmentation color", MitkColorToHex(amosSegmentationColor0));
    amosWidget->SetSegmentationColor(0, HexColorToMitkColor(segmentationColor0));
    auto amosSegmentationColor1 = amosWidget->GetSegmentationColor(1);
    auto segmentationColor1 = preferences->Get("BC segmentation color", MitkColorToHex(amosSegmentationColor1));
    amosWidget->SetSegmentationColor(1, HexColorToMitkColor(segmentationColor1));     
}

mitk::Color QmitkAmosWorkbenchRenderWindowEditor::HexColorToMitkColor(const QString& widgetColorInHex)
{
    QColor qColor(widgetColorInHex);
    mitk::Color returnColor;
    float colorMax = 255.0f;
    if (widgetColorInHex.isEmpty()) // default value
    {
        returnColor[0] = 1.0;
        returnColor[1] = 1.0;
        returnColor[2] = 1.0;
        MITK_ERROR << "Using default color for unknown hex color " << qPrintable(widgetColorInHex);
    }
    else
    {
        returnColor[0] = qColor.red() / colorMax;
        returnColor[1] = qColor.green() / colorMax;
        returnColor[2] = qColor.blue() / colorMax;
    }
    return returnColor;
}

QString QmitkAmosWorkbenchRenderWindowEditor::MitkColorToHex(const mitk::Color& color)
{
    QColor returnColor;
    float colorMax = 255.0f;
//     int col = static_cast<int>(color[0]* colorMax + 0.5);
//     if(col < 0 || col > 255)
//         std::cout << "Bad color\n";
    returnColor.setRed(static_cast<int>(color[0]* colorMax + 0.5));
//     col = static_cast<int>(color[1]* colorMax + 0.5);
//     if(col < 0 || col > 255)
//         std::cout << "Bad color\n";
    returnColor.setGreen(static_cast<int>(color[1]* colorMax + 0.5));
//     col = static_cast<int>(color[2]* colorMax + 0.5);
//     if(col < 0 || col > 255)
//         std::cout << "Bad color\n";
    returnColor.setBlue(static_cast<int>(color[2]* colorMax + 0.5));
    return returnColor.name();
}

// void QmitkAmosWorkbenchRenderWindowEditor::FillMembersWithCurrentDecorations()
// {
//     //fill members with current values (or default values) from the std multi widget
//     for(unsigned int i = 0; i < 4; ++i)
//     {
//         //      if(i == 2)
//         // 	 continue;
//         d->m_WidgetDecorationColor[i] = MitkColorToHex(d->m_amosWidget->GetDecorationColor(i));
//         d->m_WidgetBackgroundColor1[i] = MitkColorToHex(d->m_amosWidget->GetGradientColors(i).first);
//         d->m_WidgetBackgroundColor2[i] = MitkColorToHex(d->m_amosWidget->GetGradientColors(i).second);
//         d->m_WidgetAnnotation[i] = QString::fromStdString(d->m_amosWidget->GetCornerAnnotationText(i));
//     }
//     for(unsigned int i = 0; i < 2; ++i)
//     {
//         d->m_SegmentationColor[i] = MitkColorToHex(d->m_amosWidget->GetSegmentationColor(i));
//     }
// }

// void QmitkAmosWorkbenchRenderWindowEditor::RequestActivateMenuWidget(bool on)
// {
//     if (d->m_amosWidget)
//     {
//         if (on)
//         {
//             d->m_amosWidget->ActivateMenuWidget(d->m_MenuWidgetsEnabled);
//         }
//         else
//         {
//             d->m_MenuWidgetsEnabled = d->m_amosWidget->IsMenuWidgetEnabled();
//             d->m_amosWidget->ActivateMenuWidget(false);
//         }
//     }
//     
// }

// mitk::IRenderWindowPart* QmitkAmosWorkbenchRenderWindowEditor::GetRenderWindowPart( IRenderWindowPartStrategies strategies ) const
// {
//     berry::IWorkbenchPage::Pointer page = this->GetSite()->GetPage();
//     
//     // Return the active editor if it implements mitk::IRenderWindowPart
//     mitk::IRenderWindowPart* renderPart =
//     dynamic_cast<mitk::IRenderWindowPart*>(page->GetActiveEditor().GetPointer());
//     if (renderPart) return renderPart;
//     
//     // No suitable active editor found, check visible editors
//     QList<berry::IEditorReference::Pointer> editors = page->GetEditorReferences();
//     for (QList<berry::IEditorReference::Pointer>::iterator i = editors.begin();
//          i != editors.end(); ++i)
//          {
//              berry::IWorkbenchPart::Pointer part = (*i)->GetPart(false);
//              if (page->IsPartVisible(part))
//              {
//                  renderPart = dynamic_cast<mitk::IRenderWindowPart*>(part.GetPointer());
//                  if (renderPart) return renderPart;
//              }
//          }
//          
//          // No suitable visible editor found, check visible views
//          QList<berry::IViewReference::Pointer> views = page->GetViewReferences();
//          for(QList<berry::IViewReference::Pointer>::iterator i = views.begin();
//              i != views.end(); ++i)
//              {
//                  berry::IWorkbenchPart::Pointer part = (*i)->GetPart(false);
//                  if (page->IsPartVisible(part))
//                  {
//                      renderPart = dynamic_cast<mitk::IRenderWindowPart*>(part.GetPointer());
//                      if (renderPart) return renderPart;
//                  }
//              }
//              
//              // No strategies given
//              if (strategies == NONE) return nullptr;
//              
//              mitk::DataStorageEditorInput::Pointer input(new mitk::DataStorageEditorInput(GetDataStorageReference()));
//              
//              bool activate = false;
//              if(strategies & ACTIVATE)
//              {
//                  activate = true;
//              }
//              
//              berry::IEditorPart::Pointer editorPart;
//              
//              if(strategies & OPEN)
//              {
//                  // This will create a default editor for the given input. If an editor
//                  // with that input is already open, the editor is brought to the front.
//                  try
//                  {
//                      editorPart = mitk::WorkbenchUtil::OpenEditor(page, input, activate);
//                  }
//                  catch (const berry::PartInitException&)
//                  {
//                      // There is no editor registered which can handle the given input.
//                  }
//              }
//              else if (activate || (strategies & BRING_TO_FRONT))
//              {
//                  // check if a suitable editor is already opened
//                  editorPart = page->FindEditor(input);
//                  if (editorPart)
//                  {
//                      if (activate)
//                      {
//                          page->Activate(editorPart);
//                      }
//                      else
//                      {
//                          page->BringToTop(editorPart);
//                      }
//                  }
//              }
//              
//              return dynamic_cast<mitk::IRenderWindowPart*>(editorPart.GetPointer());
// }

 mitk::IRenderWindowPart* QmitkAmosWorkbenchRenderWindowEditor::GetRenderWindowPart(mitk::WorkbenchUtil::IRenderWindowPartStrategies strategies) const
{
   berry::IWorkbenchPage::Pointer page = GetSite()->GetPage();
   return mitk::WorkbenchUtil::GetRenderWindowPart(page, strategies);
 }

mitk::IRenderWindowPart* QmitkAmosWorkbenchRenderWindowEditor::OpenRenderWindowPart(bool activatedEditor)
{
    if (activatedEditor)
    {
        return this->GetRenderWindowPart(mitk::WorkbenchUtil::IRenderWindowPartStrategy::ACTIVATE | mitk::WorkbenchUtil::IRenderWindowPartStrategy::OPEN);
    }
    else
    {
        return this->GetRenderWindowPart(mitk::WorkbenchUtil::IRenderWindowPartStrategy::BRING_TO_FRONT | mitk::WorkbenchUtil::IRenderWindowPartStrategy::OPEN);
    }
}

void QmitkAmosWorkbenchRenderWindowEditor::segmentationNodeAdded(const mitk::DataNode* node)
{
    if(!segmentationNodesForReinit.contains(node))
        segmentationNodesForReinit.append(node);
    if(segmentationNodesForReinit.size() == 2)
        QTimer::singleShot(100, this, SLOT(delayedReinit()));        
}

void QmitkAmosWorkbenchRenderWindowEditor::segmentationNodeRemoved(const mitk::DataNode* node)
{
    if(segmentationNodesForReinit.contains(node))
        segmentationNodesForReinit.removeOne(node);        
}

void QmitkAmosWorkbenchRenderWindowEditor::delayedReinit()
{
    mitk::IRenderWindowPart* renderWindow = this->GetRenderWindowPart();
    
    if (renderWindow == NULL)
        renderWindow = this->OpenRenderWindowPart(false);
    foreach(const mitk::DataNode* node, segmentationNodesForReinit)
    {
        mitk::BaseData::Pointer basedata = node->GetData();
        if ( basedata.IsNotNull() && basedata->GetTimeGeometry()->IsValid() )
        {
            renderWindow->GetRenderingManager()->InitializeViews(basedata->GetTimeGeometry(), mitk::RenderingManager::REQUEST_UPDATE_ALL, true );
            renderWindow->GetRenderingManager()->RequestUpdateAll();
        }
    }    
    cout << "delayedReinit" << endl;
    
    segmentationNodesForReinit.clear();
}
