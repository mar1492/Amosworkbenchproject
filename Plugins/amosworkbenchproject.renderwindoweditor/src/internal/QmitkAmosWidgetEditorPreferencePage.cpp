/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/


#include <ui_QmitkAmosWidgetEditorPreferencePage.h>
#include "QmitkAmosWidgetEditorPreferencePage.h"
#include <QmitkAmosWorkbenchRenderWindowEditor.h>

#include <berryIPreferencesService.h>
#include <berryPlatform.h>

#include <QColorDialog>

QmitkAmosWidgetEditorPreferencePage::QmitkAmosWidgetEditorPreferencePage()
  : m_Preferences(nullptr),
    m_Ui(new Ui::QmitkAmosWidgetEditorPreferencePage),
    m_Control(nullptr)
{
}

QmitkAmosWidgetEditorPreferencePage::~QmitkAmosWidgetEditorPreferencePage()
{
}

void QmitkAmosWidgetEditorPreferencePage::CreateQtControl(QWidget* parent)
{
  m_Control = new QWidget(parent);

  m_Ui->setupUi(m_Control);

  berry::IPreferencesService* prefService = berry::Platform::GetPreferencesService();
  Q_ASSERT(prefService);

  m_Preferences = prefService->GetSystemPreferences()->Node(QmitkAmosWorkbenchRenderWindowEditor::EDITOR_ID);

  QObject::connect( m_Ui->m_ColorButton1, SIGNAL( clicked() )
                    , this, SLOT( ColorChooserButtonClicked() ) );

  QObject::connect( m_Ui->m_ColorButton2, SIGNAL( clicked() )
                    , this, SLOT( ColorChooserButtonClicked() ) );

  QObject::connect( m_Ui->m_ResetButton, SIGNAL( clicked() )
                    , this, SLOT( ResetPreferencesAndGUI() ) );

  QObject::connect( m_Ui->m_RenderingMode, SIGNAL(activated(int) )
                    , this, SLOT( ChangeRenderingMode(int) ) );

  QObject::connect( m_Ui->m_RenderWindowDecorationColor, SIGNAL( clicked() )
                    , this, SLOT( ColorChooserButtonClicked() ) );

  QObject::connect( m_Ui->m_RenderWindowChooser, SIGNAL(activated(int) )
                    , this, SLOT( OnWidgetComboBoxChanged(int) ) );
  QObject::connect( m_Ui->m_RenderWindowDecorationText, SIGNAL(textChanged(QString) )
                    , this, SLOT( AnnotationTextChanged(QString) ) );
  
  QObject::connect( m_Ui->m_SegmentationColorButton, SIGNAL( clicked() )
                    , this, SLOT( ColorChooserButtonClicked() ) );  


  this->Update();
}

QWidget* QmitkAmosWidgetEditorPreferencePage::GetQtControl() const
{
  return m_Control;
}

void QmitkAmosWidgetEditorPreferencePage::Init(berry::IWorkbench::Pointer)
{
}

void QmitkAmosWidgetEditorPreferencePage::PerformCancel()
{
}

bool QmitkAmosWidgetEditorPreferencePage::PerformOk()
{
  m_Preferences->Put("stdmulti.widget0 corner annotation", m_WidgetAnnotation[0]);
  m_Preferences->Put("stdmulti.widget1 corner annotation", m_WidgetAnnotation[1]);
  m_Preferences->Put("stdmulti.widget2 corner annotation", m_WidgetAnnotation[2]);
  m_Preferences->Put("stdmulti.widget3 corner annotation", m_WidgetAnnotation[3]);
  /// JCAM
  m_Preferences->Put("stdmulti.widget4 corner annotation", m_WidgetAnnotation[4]);
  m_Preferences->Put("stdmulti.widget5 corner annotation", m_WidgetAnnotation[5]);
  m_Preferences->Put("stdmulti.widget6 corner annotation", m_WidgetAnnotation[6]);
  m_Preferences->Put("stdmulti.widget7 corner annotation", m_WidgetAnnotation[7]);
  m_Preferences->Put("stdmulti.widget8 corner annotation", m_WidgetAnnotation[8]);
  m_Preferences->Put("stdmulti.widget9 corner annotation", m_WidgetAnnotation[9]);
  m_Preferences->Put("stdmulti.widget10 corner annotation", m_WidgetAnnotation[10]);
  m_Preferences->Put("stdmulti.widget11 corner annotation", m_WidgetAnnotation[11]);
  m_Preferences->Put("stdmulti.widget12 corner annotation", m_WidgetAnnotation[12]);
  m_Preferences->Put("stdmulti.widget13 corner annotation", m_WidgetAnnotation[13]);
  m_Preferences->Put("stdmulti.widget14 corner annotation", m_WidgetAnnotation[14]);
  m_Preferences->Put("stdmulti.widget15 corner annotation", m_WidgetAnnotation[15]);
  m_Preferences->Put("stdmulti.widget16 corner annotation", m_WidgetAnnotation[16]);
  m_Preferences->Put("stdmulti.widget17 corner annotation", m_WidgetAnnotation[17]);

  m_Preferences->Put("stdmulti.widget0 decoration color", m_WidgetDecorationColor[0]);
  m_Preferences->Put("stdmulti.widget1 decoration color", m_WidgetDecorationColor[1]);
  m_Preferences->Put("stdmulti.widget2 decoration color", m_WidgetDecorationColor[2]);
  m_Preferences->Put("stdmulti.widget3 decoration color", m_WidgetDecorationColor[3]);
  /// JCAM
  m_Preferences->Put("stdmulti.widget4 decoration color", m_WidgetDecorationColor[4]);
  m_Preferences->Put("stdmulti.widget5 decoration color", m_WidgetDecorationColor[5]);
  m_Preferences->Put("stdmulti.widget6 decoration color", m_WidgetDecorationColor[6]);
  m_Preferences->Put("stdmulti.widget7 decoration color", m_WidgetDecorationColor[7]);
  m_Preferences->Put("stdmulti.widget8 decoration color", m_WidgetDecorationColor[8]);
  m_Preferences->Put("stdmulti.widget9 decoration color", m_WidgetDecorationColor[9]);
  m_Preferences->Put("stdmulti.widget10 decoration color", m_WidgetDecorationColor[10]);
  m_Preferences->Put("stdmulti.widget11 decoration color", m_WidgetDecorationColor[11]);
  m_Preferences->Put("stdmulti.widget12 decoration color", m_WidgetDecorationColor[12]);
  m_Preferences->Put("stdmulti.widget13 decoration color", m_WidgetDecorationColor[13]);
  m_Preferences->Put("stdmulti.widget14 decoration color", m_WidgetDecorationColor[14]);
  m_Preferences->Put("stdmulti.widget15 decoration color", m_WidgetDecorationColor[15]);
  m_Preferences->Put("stdmulti.widget16 decoration color", m_WidgetDecorationColor[16]);
  m_Preferences->Put("stdmulti.widget17 decoration color", m_WidgetDecorationColor[17]);

  m_Preferences->Put("stdmulti.widget0 first background color", m_WidgetBackgroundColor1[0]);
  m_Preferences->Put("stdmulti.widget1 first background color", m_WidgetBackgroundColor1[1]);
  m_Preferences->Put("stdmulti.widget2 first background color", m_WidgetBackgroundColor1[2]);
  m_Preferences->Put("stdmulti.widget3 first background color", m_WidgetBackgroundColor1[3]);
  /// JCAM
  m_Preferences->Put("stdmulti.widget4 first background color", m_WidgetBackgroundColor1[4]);
  m_Preferences->Put("stdmulti.widget5 first background color", m_WidgetBackgroundColor1[5]);
  m_Preferences->Put("stdmulti.widget6 first background color", m_WidgetBackgroundColor1[6]);
  m_Preferences->Put("stdmulti.widget7 first background color", m_WidgetBackgroundColor1[7]);
  m_Preferences->Put("stdmulti.widget8 first background color", m_WidgetBackgroundColor1[8]);
  m_Preferences->Put("stdmulti.widget9 first background color", m_WidgetBackgroundColor1[9]);
  m_Preferences->Put("stdmulti.widget10 first background color", m_WidgetBackgroundColor1[10]);
  m_Preferences->Put("stdmulti.widget11 first background color", m_WidgetBackgroundColor1[11]);
  m_Preferences->Put("stdmulti.widget12 first background color", m_WidgetBackgroundColor1[12]);
  m_Preferences->Put("stdmulti.widget13 first background color", m_WidgetBackgroundColor1[13]);
  m_Preferences->Put("stdmulti.widget14 first background color", m_WidgetBackgroundColor1[14]);
  m_Preferences->Put("stdmulti.widget15 first background color", m_WidgetBackgroundColor1[15]);
  m_Preferences->Put("stdmulti.widget16 first background color", m_WidgetBackgroundColor1[16]);
  m_Preferences->Put("stdmulti.widget17 first background color", m_WidgetBackgroundColor1[17]);
  
  m_Preferences->Put("stdmulti.widget0 second background color", m_WidgetBackgroundColor2[0]);
  m_Preferences->Put("stdmulti.widget1 second background color", m_WidgetBackgroundColor2[1]);
  m_Preferences->Put("stdmulti.widget2 second background color", m_WidgetBackgroundColor2[2]);
  m_Preferences->Put("stdmulti.widget3 second background color", m_WidgetBackgroundColor2[3]);
  /// JCAM
  m_Preferences->Put("stdmulti.widget4 second background color", m_WidgetBackgroundColor2[4]);
  m_Preferences->Put("stdmulti.widget5 second background color", m_WidgetBackgroundColor2[5]);
  m_Preferences->Put("stdmulti.widget6 second background color", m_WidgetBackgroundColor2[6]);
  m_Preferences->Put("stdmulti.widget7 second background color", m_WidgetBackgroundColor2[7]);
  m_Preferences->Put("stdmulti.widget8 second background color", m_WidgetBackgroundColor2[8]);
  m_Preferences->Put("stdmulti.widget9 second background color", m_WidgetBackgroundColor2[9]);
  m_Preferences->Put("stdmulti.widget10 second background color", m_WidgetBackgroundColor2[10]);
  m_Preferences->Put("stdmulti.widget11 second background color", m_WidgetBackgroundColor2[11]);
  m_Preferences->Put("stdmulti.widget12 second background color", m_WidgetBackgroundColor2[12]);
  m_Preferences->Put("stdmulti.widget13 second background color", m_WidgetBackgroundColor2[13]);
  m_Preferences->Put("stdmulti.widget14 second background color", m_WidgetBackgroundColor2[14]);
  m_Preferences->Put("stdmulti.widget15 second background color", m_WidgetBackgroundColor2[15]);
  m_Preferences->Put("stdmulti.widget16 second background color", m_WidgetBackgroundColor2[16]);
  m_Preferences->Put("stdmulti.widget17 second background color", m_WidgetBackgroundColor2[17]);
  
  m_Preferences->Put("AC segmentation color", m_SegmentationColor[0]);
  m_Preferences->Put("BC segmentation color", m_SegmentationColor[1]);
  
//   m_Preferences->PutInt("crosshair gap size", m_Ui->m_CrosshairGapSize->value());

  m_Preferences->PutBool("Use constrained zooming and padding"
                         , m_Ui->m_EnableFlexibleZooming->isChecked());
  m_Preferences->PutBool("Show level/window widget", m_Ui->m_ShowLevelWindowWidget->isChecked());
  m_Preferences->PutBool("PACS like mouse interaction", m_Ui->m_PACSLikeMouseMode->isChecked());
  m_Preferences->PutInt("Rendering Mode", m_Ui->m_RenderingMode->currentIndex());

  return true;
}

void QmitkAmosWidgetEditorPreferencePage::Update()
{
  //Note: there should be default preferences already defined in the
  //QmitkAmosWorkbenchRenderWindowEditor::InitializePreferences(). Therefore,
  //all default values here are not relevant.
  //gradient background colors
  m_WidgetBackgroundColor1[0] = m_Preferences->Get("stdmulti.widget0 first background color", "#000000");
  m_WidgetBackgroundColor2[0] = m_Preferences->Get("stdmulti.widget0 second background color", "#000000");
  m_WidgetBackgroundColor1[1] = m_Preferences->Get("stdmulti.widget1 first background color", "#000000");
  m_WidgetBackgroundColor2[1] = m_Preferences->Get("stdmulti.widget1 second background color", "#000000");
  m_WidgetBackgroundColor1[2] = m_Preferences->Get("stdmulti.widget2 first background color", "#000000");
  m_WidgetBackgroundColor2[2] = m_Preferences->Get("stdmulti.widget2 second background color", "#000000");
  m_WidgetBackgroundColor1[3] = m_Preferences->Get("stdmulti.widget3 first background color", "#191919");
  m_WidgetBackgroundColor2[3] = m_Preferences->Get("stdmulti.widget3 second background color", "#7F7F7F");
    /// JCAM
  m_WidgetBackgroundColor1[4] = m_Preferences->Get("stdmulti.widget4 first background color", "#191919");
  m_WidgetBackgroundColor2[4] = m_Preferences->Get("stdmulti.widget4 second background color", "#7F7F7F");
  m_WidgetBackgroundColor1[5] = m_Preferences->Get("stdmulti.widget5 first background color", "#191919");
  m_WidgetBackgroundColor2[5] = m_Preferences->Get("stdmulti.widget5 second background color", "#7F7F7F");
  
  m_WidgetBackgroundColor1[6] = m_Preferences->Get("stdmulti.widget6 first background color", "#000000");
  m_WidgetBackgroundColor2[6] = m_Preferences->Get("stdmulti.widget6 second background color", "#000000");
  m_WidgetBackgroundColor1[7] = m_Preferences->Get("stdmulti.widget7 first background color", "#000000");
  m_WidgetBackgroundColor2[7] = m_Preferences->Get("stdmulti.widget7 second background color", "#000000");
  m_WidgetBackgroundColor1[8] = m_Preferences->Get("stdmulti.widget8 first background color", "#000000");
  m_WidgetBackgroundColor2[8] = m_Preferences->Get("stdmulti.widget8 second background color", "#000000");
  m_WidgetBackgroundColor1[9] = m_Preferences->Get("stdmulti.widget9 first background color", "#191919");
  m_WidgetBackgroundColor2[9] = m_Preferences->Get("stdmulti.widget9 second background color", "#7F7F7F");
    /// JCAM
  m_WidgetBackgroundColor1[10] = m_Preferences->Get("stdmulti.widget10 first background color", "#191919");
  m_WidgetBackgroundColor2[10] = m_Preferences->Get("stdmulti.widget10 second background color", "#7F7F7F");
  m_WidgetBackgroundColor1[11] = m_Preferences->Get("stdmulti.widget11 first background color", "#191919");
  m_WidgetBackgroundColor2[11] = m_Preferences->Get("stdmulti.widget11 second background color", "#7F7F7F");
  
  m_WidgetBackgroundColor1[12] = m_Preferences->Get("stdmulti.widget12 first background color", "#000000");
  m_WidgetBackgroundColor2[12] = m_Preferences->Get("stdmulti.widget12 second background color", "#000000");
  m_WidgetBackgroundColor1[13] = m_Preferences->Get("stdmulti.widget13 first background color", "#000000");
  m_WidgetBackgroundColor2[13] = m_Preferences->Get("stdmulti.widget13 second background color", "#000000");
  m_WidgetBackgroundColor1[14] = m_Preferences->Get("stdmulti.widget14 first background color", "#000000");
  m_WidgetBackgroundColor2[14] = m_Preferences->Get("stdmulti.widget14 second background color", "#000000");
  m_WidgetBackgroundColor1[15] = m_Preferences->Get("stdmulti.widget15 first background color", "#191919");
  m_WidgetBackgroundColor2[15] = m_Preferences->Get("stdmulti.widget15 second background color", "#7F7F7F");
    /// JCAM
  m_WidgetBackgroundColor1[16] = m_Preferences->Get("stdmulti.widget16 first background color", "#191919");
  m_WidgetBackgroundColor2[16] = m_Preferences->Get("stdmulti.widget16 second background color", "#7F7F7F");
  m_WidgetBackgroundColor1[17] = m_Preferences->Get("stdmulti.widget17 first background color", "#191919");
  m_WidgetBackgroundColor2[17] = m_Preferences->Get("stdmulti.widget17 second background color", "#7F7F7F");
  
  //decoration colors
  m_WidgetDecorationColor[0] = m_Preferences->Get("stdmulti.widget0 decoration color", "#FF0000");
  m_WidgetDecorationColor[1] = m_Preferences->Get("stdmulti.widget1 decoration color", "#00FF00");
  m_WidgetDecorationColor[2] = m_Preferences->Get("stdmulti.widget2 decoration color", "#0000FF");
  m_WidgetDecorationColor[3] = m_Preferences->Get("stdmulti.widget3 decoration color", "#FFFF00");
  m_WidgetDecorationColor[4] = m_Preferences->Get("stdmulti.widget4 decoration color", "#0000FF");
  m_WidgetDecorationColor[5] = m_Preferences->Get("stdmulti.widget5 decoration color", "#FFFF00");
  
  m_WidgetDecorationColor[6] = m_Preferences->Get("stdmulti.widget6 decoration color", "#FF0000");
  m_WidgetDecorationColor[7] = m_Preferences->Get("stdmulti.widget7 decoration color", "#00FF00");
  m_WidgetDecorationColor[8] = m_Preferences->Get("stdmulti.widget8 decoration color", "#0000FF");
  m_WidgetDecorationColor[9] = m_Preferences->Get("stdmulti.widget9 decoration color", "#FFFF00");
  m_WidgetDecorationColor[10] = m_Preferences->Get("stdmulti.widget10 decoration color", "#0000FF");
  m_WidgetDecorationColor[11] = m_Preferences->Get("stdmulti.widget11 decoration color", "#FFFF00");
  
  m_WidgetDecorationColor[12] = m_Preferences->Get("stdmulti.widget12 decoration color", "#FF0000");
  m_WidgetDecorationColor[13] = m_Preferences->Get("stdmulti.widget13 decoration color", "#00FF00");
  m_WidgetDecorationColor[14] = m_Preferences->Get("stdmulti.widget14 decoration color", "#0000FF");
  m_WidgetDecorationColor[15] = m_Preferences->Get("stdmulti.widget15 decoration color", "#FFFF00");
  m_WidgetDecorationColor[16] = m_Preferences->Get("stdmulti.widget16 decoration color", "#0000FF");
  m_WidgetDecorationColor[17] = m_Preferences->Get("stdmulti.widget17 decoration color", "#FFFF00");
  
  //segmentation colors
  m_SegmentationColor[0] = m_Preferences->Get("AC segmentation color", "#FF0000");
  m_SegmentationColor[1] = m_Preferences->Get("BC segmentation color", "#00FF00");  

  //annotation text
  m_WidgetAnnotation[0] = m_Preferences->Get("stdmulti.widget0 corner annotation", "Axial");
  m_WidgetAnnotation[1] = m_Preferences->Get("stdmulti.widget1 corner annotation", "Sagittal");
  m_WidgetAnnotation[2] = m_Preferences->Get("stdmulti.widget2 corner annotation", "Patient");
  m_WidgetAnnotation[3] = m_Preferences->Get("stdmulti.widget3 corner annotation", "3D");
  m_WidgetAnnotation[4] = m_Preferences->Get("stdmulti.widget4 corner annotation", "Coronal");
  m_WidgetAnnotation[5] = m_Preferences->Get("stdmulti.widget5 corner annotation", "Sagittal");

  m_WidgetAnnotation[6] = m_Preferences->Get("stdmulti.widget6 corner annotation", "Axial");
  m_WidgetAnnotation[7] = m_Preferences->Get("stdmulti.widget7 corner annotation", "Sagittal");
  m_WidgetAnnotation[8] = m_Preferences->Get("stdmulti.widget8 corner annotation", "Patient");
  m_WidgetAnnotation[9] = m_Preferences->Get("stdmulti.widget9 corner annotation", "3D");
  m_WidgetAnnotation[10] = m_Preferences->Get("stdmulti.widget10 corner annotation", "Coronal");
  m_WidgetAnnotation[11] = m_Preferences->Get("stdmulti.widget11 corner annotation", "Sagittal");

  m_WidgetAnnotation[12] = m_Preferences->Get("stdmulti.widget12 corner annotation", "Axial");
  m_WidgetAnnotation[13] = m_Preferences->Get("stdmulti.widget13 corner annotation", "Sagittal");
  m_WidgetAnnotation[14] = m_Preferences->Get("stdmulti.widget14 corner annotation", "Patient");
  m_WidgetAnnotation[15] = m_Preferences->Get("stdmulti.widget15 corner annotation", "3D");
  m_WidgetAnnotation[16] = m_Preferences->Get("stdmulti.widget16 corner annotation", "Coronal");
  m_WidgetAnnotation[17] = m_Preferences->Get("stdmulti.widget17 corner annotation", "Sagittal");


  //Ui stuff
  int index = m_Ui->m_RenderWindowChooser->currentIndex();
  QColor firstBackgroundColor(m_WidgetBackgroundColor1[index]);
  QColor secondBackgroundColor(m_WidgetBackgroundColor2[index]);
  QColor widgetColor(m_WidgetDecorationColor[index]);
  QColor segmentationColor(m_SegmentationColor[index]);

  this->SetStyleSheetToColorChooserButton(widgetColor, m_Ui->m_RenderWindowDecorationColor);
  this->SetStyleSheetToColorChooserButton(firstBackgroundColor, m_Ui->m_ColorButton1);
  this->SetStyleSheetToColorChooserButton(secondBackgroundColor, m_Ui->m_ColorButton2);
  this->SetStyleSheetToColorChooserButton(segmentationColor, m_Ui->m_SegmentationColorButton);

  m_Ui->m_RenderWindowDecorationText->setText(m_WidgetAnnotation[index]);

  m_Ui->m_EnableFlexibleZooming->setChecked(m_Preferences->GetBool("Use constrained zooming and padding", true));
  m_Ui->m_ShowLevelWindowWidget->setChecked(m_Preferences->GetBool("Show level/window widget", true));
  m_Ui->m_PACSLikeMouseMode->setChecked(m_Preferences->GetBool("PACS like mouse interaction", false));
  int mode= m_Preferences->GetInt("Rendering Mode",0);
  m_Ui->m_RenderingMode->setCurrentIndex(mode);
//   m_Ui->m_CrosshairGapSize->setValue(m_Preferences->GetInt("crosshair gap size", 32));
}

void QmitkAmosWidgetEditorPreferencePage::ColorChooserButtonClicked()
{
  unsigned int widgetIndex = m_Ui->m_RenderWindowChooser->currentIndex();
  /// JCAM
  if(widgetIndex > 17)
  {
    MITK_ERROR << "Selected index for unknown.";
    return;
  }
  QObject *senderObj = sender(); // This will give Sender button
  //find out last used color and set it
  QColor initialColor;
  if( senderObj->objectName() == m_Ui->m_ColorButton1->objectName())
  {
    initialColor = QColor(m_WidgetBackgroundColor1[widgetIndex]);
  }else if( senderObj->objectName() == m_Ui->m_ColorButton2->objectName())
  {
    initialColor = QColor(m_WidgetBackgroundColor2[widgetIndex]);
  }else if( senderObj->objectName() == m_Ui->m_RenderWindowDecorationColor->objectName())
  {
    initialColor = QColor(m_WidgetDecorationColor[widgetIndex]);
  }else if( senderObj->objectName() == m_Ui->m_SegmentationColorButton->objectName())
  {
    if(widgetIndex > 1)
    {
	MITK_WARN << "Selected index not for segemntation";
	return;
    }      
    initialColor = QColor(m_SegmentationColor[widgetIndex]);
  }

  //get the new color
  QColor newcolor = QColorDialog::getColor(initialColor);
  if(!newcolor.isValid())
  {
    newcolor = initialColor;
  }
  this->SetStyleSheetToColorChooserButton(newcolor, static_cast<QPushButton*>(senderObj));

  //convert it to std string and apply it
  if( senderObj->objectName() == m_Ui->m_ColorButton1->objectName())
  {
    m_WidgetBackgroundColor1[widgetIndex] = newcolor.name();
  }
  else if( senderObj->objectName() == m_Ui->m_ColorButton2->objectName())
  {
    m_WidgetBackgroundColor2[widgetIndex] = newcolor.name();
  }
  else if( senderObj->objectName() == m_Ui->m_RenderWindowDecorationColor->objectName())
  {
    m_WidgetDecorationColor[widgetIndex] = newcolor.name();
  }
  else if( senderObj->objectName() == m_Ui->m_SegmentationColorButton->objectName())
  {
    m_SegmentationColor[widgetIndex] = newcolor.name();  
  } 
 
}

void QmitkAmosWidgetEditorPreferencePage::SetStyleSheetToColorChooserButton(QColor backgroundcolor,
                                                                                QPushButton* button)
{
  button->setAutoFillBackground(true);
  QString styleSheet = "background-color:rgb(";

  styleSheet.append(QString::number(backgroundcolor.red()));
  styleSheet.append(",");
  styleSheet.append(QString::number(backgroundcolor.green()));
  styleSheet.append(",");
  styleSheet.append(QString::number(backgroundcolor.blue()));
  styleSheet.append(")");
  button->setStyleSheet(styleSheet);
  button->clearFocus();
}

void QmitkAmosWidgetEditorPreferencePage::AnnotationTextChanged(QString text)
{
  unsigned int widgetIndex = m_Ui->m_RenderWindowChooser->currentIndex();
  /// JCAM
  if( widgetIndex > 17)
  {
    MITK_INFO << "Selected index for unknown widget.";
    return;
  }
  m_WidgetAnnotation[widgetIndex] = text;
}

void QmitkAmosWidgetEditorPreferencePage::ResetPreferencesAndGUI()
{
  m_Preferences->Clear();
  this->Update();
}

void QmitkAmosWidgetEditorPreferencePage::OnWidgetComboBoxChanged(int i)
{
    /// JCAM
  if( i > 17)
  {
    MITK_ERROR << "Selected unknown widget.";
    return;
  }
  QColor widgetColor(m_WidgetDecorationColor[i]);
  QColor gradientBackground1(m_WidgetBackgroundColor1[i]);
  QColor gradientBackground2(m_WidgetBackgroundColor2[i]);
  QColor segmentationColor;
  
    if(i > 1) {
	m_Ui->m_SegmentationColorButton->setEnabled(false);
	m_Ui->m_SegmentationColorButton->setFlat(true);
	m_Ui->m_SegmentationColorButton->setAutoFillBackground(true);
	segmentationColor = m_Ui->m_SegmentationColorButton->palette().color(QPalette::Button);
    }	
    else {
	m_Ui->m_SegmentationColorButton->setFlat(false);
	m_Ui->m_SegmentationColorButton->setEnabled(true);
	segmentationColor.setNamedColor(m_SegmentationColor[i]);
    }  
  
  this->SetStyleSheetToColorChooserButton(widgetColor, m_Ui->m_RenderWindowDecorationColor);
  this->SetStyleSheetToColorChooserButton(gradientBackground1, m_Ui->m_ColorButton1);
  this->SetStyleSheetToColorChooserButton(gradientBackground2, m_Ui->m_ColorButton2);
  this->SetStyleSheetToColorChooserButton(segmentationColor, m_Ui->m_SegmentationColorButton);
 
  m_Ui->m_RenderWindowDecorationText->setText(m_WidgetAnnotation[i]);
  
}

void QmitkAmosWidgetEditorPreferencePage::ChangeRenderingMode(int i)
{
  if( i == 0 )
  {
    m_CurrentRenderingMode = "Standard";
  }
  else if( i == 1 )
  {
    m_CurrentRenderingMode = "Multisampling";
  }
  else if( i == 2 )
  {
    m_CurrentRenderingMode = "DepthPeeling";
  }
}
