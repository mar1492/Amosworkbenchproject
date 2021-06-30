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

#ifndef QmitkAmosWorkbenchRenderWindowEditor_H_
#define QmitkAmosWorkbenchRenderWindowEditor_H_

#include <QMetaType>

#include <mitkWorkbenchUtil.h>
#include <QmitkAbstractMultiWidgetEditor.h>
#include <mitkILinkedRenderWindowPart.h>
#include <mitkIRenderingManager.h>
#include <QmitkLevelWindowWidget.h>
#include <amosworkbenchproject_renderwindoweditor_Export.h>

// c++
 #include <memory>

class AmosWidget;
class QmitkRenderWindow;
// class MyQmitkRenderWindow;
class QmitkMouseModeSwitcher;
class QmitkAmosWorkbenchRenderWindowEditorPrivate;

// class SlicesRotator;
// class SlicesSwiveller;

/**
 * \ingroup amosworkbenchproject_renderwindoweditor
 */
class AMOSWORKBENCHPROJECT_RENDERWINDOWEDITOR QmitkAmosWorkbenchRenderWindowEditor : public QmitkAbstractMultiWidgetEditor, public mitk::ILinkedRenderWindowPart
{
    Q_OBJECT
    
public:
    
//     berryObjectMacro(QmitkAmosWorkbenchRenderWindowEditor)
    
    static const QString EDITOR_ID;
    
//     enum IRenderWindowPartStrategy {
//         
//         /** Do nothing. */
//         NONE           = 0x00000000,
//         /** Bring the most recently activated mitk::IRenderWindowPart instance to the front. */
//         BRING_TO_FRONT = 0x00000001,
//         /** Activate a mitk::IRenderWindowPart part (implies bringing it to the front). */
//         ACTIVATE       = 0x00000002,
//         /** Create a mitk::IRenderWindowPart if none is alredy opened. */
//         OPEN           = 0x00000004
//     };  
    
//     Q_DECLARE_FLAGS(IRenderWindowPartStrategies, IRenderWindowPartStrategy)
    
    QmitkAmosWorkbenchRenderWindowEditor();
    virtual ~QmitkAmosWorkbenchRenderWindowEditor() override;
    
//     AmosWidget* GetAmosWidget();
    
    
    // -------------------  mitk::IRenderWindowPart  ----------------------
    
    /**
     * \see mitk::IRenderWindowPart::GetActiveQmitkRenderWindow()
     */
//     QmitkRenderWindow* GetActiveQmitkRenderWindow() const override;
    
    /**
     * \see mitk::IRenderWindowPart::GetQmitkRenderWindows()
     */
//     QHash<QString, QmitkRenderWindow*> GetQmitkRenderWindows() const override;
    
    /**
     * \see mitk::IRenderWindowPart::GetQmitkRenderWindow(QString)
     */
//     QmitkRenderWindow* GetQmitkRenderWindow(const QString& id) const override;
    
    /**
     * \see mitk::IRenderWindowPart::GetSelectionPosition()
     */
    mitk::Point3D GetSelectedPosition(const QString& id = QString()) const override;
    
    /**
     * \see mitk::IRenderWindowPart::SetSelectedPosition()
     */
    void SetSelectedPosition(const mitk::Point3D &pos, const QString& id = QString()) override;
    
    /**
     * \see mitk::IRenderWindowPart::EnableDecorations()
     */
//     void EnableDecorations(bool enable, const QStringList& decorations = QStringList()) override;
    
    /**
     * \see mitk::IRenderWindowPart::IsDecorationEnabled()
     */
//     bool IsDecorationEnabled(const QString& decoration) const override;
    
    /**
     * \see mitk::IRenderWindowPart::GetDecorations()
     */
//     QStringList GetDecorations() const override;
    
    //   mitk::SlicesRotator* GetSlicesRotator() const;
    //   mitk::SlicesSwiveller* GetSlicesSwiveller() const;
    
   // -------------------  mitk::ILinkedRenderWindowPart  ----------------------    
    
    virtual void EnableSlicingPlanes(bool enable) override;
    virtual bool IsSlicingPlanesEnabled() const override;
    virtual berry::IPartListener::Events::Types GetPartEventTypes() const override;
    virtual void PartClosed(const berry::IWorkbenchPartReference::Pointer& partRef) override;
    virtual void PartOpened(const berry::IWorkbenchPartReference::Pointer& partRef) override;
    virtual void PartHidden(const berry::IWorkbenchPartReference::Pointer& partRef) override;
    virtual void PartVisible(const berry::IWorkbenchPartReference::Pointer& partRef) override;    
    void ShowLevelWindowWidget(bool show);
//     virtual void EnableLinkedNavigation(bool enable);
//     virtual bool IsLinkedNavigationEnabled() const;
    
    virtual QmitkLevelWindowWidget* GetLevelWindowWidget() const override;
    
//     void RequestActivateMenuWidget(bool on);
    
protected slots:
    void segmentationNodeAdded(const mitk::DataNode* node);
    void segmentationNodeRemoved(const mitk::DataNode* node);
    void delayedReinit();
    
protected:
    
    /**
     * @brief FillMembersWithCurrentDecorations Helper method to fill internal members with
     * current values of the std multi widget.
     */
//     void FillMembersWithCurrentDecorations();
    
    /**
     * @brief GetPreferenceDecorations Getter to fill internal members with values of preferences.
     * @param preferences The berry preferences.
     *
     * If a preference is set, the value will overwrite the current value. If it does not exist,
     * the value will not change.
     */
    void GetPreferenceDecorations(const berry::IBerryPreferences *preferences); 
    
    void SetFocus() override;
    
    void OnPreferencesChanged(const berry::IBerryPreferences* prefs) override;
    
    void CreateQtPartControl(QWidget* parent) override;
    
    /**
     * @brief GetColorForWidget helper method to convert a saved color string to mitk::Color.
     * @param widgetColorInHex color in hex format (#12356) where each diget is in the form (0-F).
     * @return the color in mitk format.
     */
    mitk::Color HexColorToMitkColor(const QString& widgetColorInHex);
    /**
     * @brief MitkColorToHex Convert an mitk::Color to hex string.
     * @param color mitk format.
     * @return String in hex (#RRGGBB).
     */
    QString MitkColorToHex(const mitk::Color& color);
    
    /**
     * @brief InitializePreferences Internal helper method to set default preferences.
     * This method is used to show the current preferences in the first call of
     * the preference page (the GUI).
     *
     * @param preferences berry preferences.
     */
    void InitializePreferences(berry::IBerryPreferences *preferences);
    
//     mitk::IRenderWindowPart* GetRenderWindowPart(IRenderWindowPartStrategies strategies = NONE) const;
//     mitk::IRenderWindowPart *OpenRenderWindowPart(bool activatedEditor = true); 
    mitk::IRenderWindowPart* GetRenderWindowPart(mitk::WorkbenchUtil::IRenderWindowPartStrategies strategies = mitk::WorkbenchUtil::IRenderWindowPartStrategy::NONE) const;
    mitk::IRenderWindowPart *OpenRenderWindowPart(bool activatedEditor = true); 
        
    QList<const mitk::DataNode*> segmentationNodesForReinit;
    
 private:
//     
//     const QScopedPointer<QmitkAmosWorkbenchRenderWindowEditorPrivate> d;
   struct Impl;
   std::unique_ptr<Impl> m_Impl;     
    
};
// Q_DECLARE_OPERATORS_FOR_FLAGS(QmitkAmosWorkbenchRenderWindowEditor::IRenderWindowPartStrategies)
Q_DECLARE_METATYPE(mitk::DataNode*)

#endif /*QmitkAmosWorkbenchRenderWindowEditor_H_*/
