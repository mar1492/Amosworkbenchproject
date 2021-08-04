#include "myqmitkmultiwidgetlayoutmanager.h"
#include <QmitkAbstractMultiWidget.h>
#include <QHBoxLayout>
#include <QSplitter>
#include <QmitkRenderWindowWidget.h>
#include <QMessageBox>
#include "amoswidget.h"

MyQmitkMultiWidgetLayoutManager::MyQmitkMultiWidgetLayoutManager(QmitkAbstractMultiWidget* multiwidget) : QmitkMultiWidgetLayoutManager(multiwidget)
{
    amosWidget = dynamic_cast<AmosWidget*> (multiwidget);
}

void MyQmitkMultiWidgetLayoutManager::SetLayoutDesign(QmitkMultiWidgetLayoutManager::LayoutDesign layoutDesign)
{
    //QmitkMultiWidgetLayoutManager::SetLayoutDesign(layoutDesign);
    switch (layoutDesign)
    {
        case LayoutDesign::DEFAULT:
            SetDefaultLayout();
            break;
        case LayoutDesign::LEFT_AC_AND_RIGHT_BC:
            SetLeftACandRightBC();
            break;
        case LayoutDesign::AC_UP_AND_BC_DOWN:
            SetACUpandBCDown();
            break;
        case LayoutDesign::TWO_AXIAL_WITH_SEGMENTATION:
            TwoAxialWithSegmentationLayout();
            break;
         case LayoutDesign::AXIAL_3D_ONLY:
             Axial3DOnlyLayout();
            //QMessageBox::information(NULL,"SetLayoutDesign","AXIAL_3D_ONLY: To do");
            break;
        case LayoutDesign::SAGITTAL_UP_CORONAL_DOWN:
            SagittalUpCoronalDownLayout();
            //QMessageBox::information(NULL,"SetLayoutDesign","AXIAL_UP_SAGITTAL_DOWN: To do");
            break;
        default:
            QmitkMultiWidgetLayoutManager::SetLayoutDesign(layoutDesign);
            break;
    }
    /*
    if (layoutDesign == LayoutDesign::DEFAULT)
        SetDefaultLayout();
    else if(layoutDesign == LayoutDesign::LEFT_AC_AND_RIGHT_BC)
        SetLeftACandRightBC();
    else if(layoutDesign == LayoutDesign::AC_UP_AND_BC_DOWN)
        SetACUpandBCDown();
    /// JCAM
    else if (layoutDesign == LayoutDesign::SAGITTAL_UP_CORONAL_DOWN)
        SetSagittalUpCoronalDownLayout();
    else if (layoutDesign == LayoutDesign::AXIAL_UP_CORONAL_DOWN)
        QMessageBox::information(NULL,"SetLayoutDesign","AXIAL_UP_CORONAL_DOWN: To do");
    else if (layoutDesign == LayoutDesign::AXIAL_UP_SAGITTAL_DOWN)
        QMessageBox::information(NULL,"SetLayoutDesign","AXIAL_UP_SAGITTAL_DOWN: To do");
    else
        QmitkMultiWidgetLayoutManager::SetLayoutDesign(layoutDesign);
    */
    ///    
}

void MyQmitkMultiWidgetLayoutManager::SagittalUpCoronalDownLayout()
{
  MITK_INFO << "SAGITTAL_UP_CORONAL_DOWN layout" << std::endl;

  QMessageBox::information(NULL,"SetLayoutDesign","SAGITTAL_UP_CORONAL_DOWN: To do");

    m_MultiWidget->ActivateMenuWidget(true);

    auto allRenderWindows = m_MultiWidget->GetRenderWindows();
    for (auto& renderWindow : allRenderWindows)
    {
        renderWindow->UpdateLayoutDesignList(LayoutDesign::SAGITTAL_UP_CORONAL_DOWN);
    }
}

void MyQmitkMultiWidgetLayoutManager::Axial3DOnlyLayout()
{
  MITK_INFO << "AXIAL_3D_ONLY layout" << std::endl;

  QMessageBox::information(NULL,"SetLayoutDesign","AXIAL_3D_ONLY: To do");

    m_MultiWidget->ActivateMenuWidget(true);

    auto allRenderWindows = m_MultiWidget->GetRenderWindows();
    for (auto& renderWindow : allRenderWindows)
    {
        renderWindow->UpdateLayoutDesignList(LayoutDesign::AXIAL_3D_ONLY);
    }    
}

void MyQmitkMultiWidgetLayoutManager::TwoAxialWithSegmentationLayout()
{
  MITK_INFO << "Two axial with segmentation layout" << std::endl;

    //Hide all Menu Widgets
    m_MultiWidget->ActivateMenuWidget(false);

    delete m_MultiWidget->layout();

    auto hBoxLayout = new QHBoxLayout(m_MultiWidget);
    hBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_MultiWidget->setLayout(hBoxLayout);
    hBoxLayout->setMargin(0);

    auto mainSplit = new QSplitter(Qt::Vertical, m_MultiWidget);
    hBoxLayout->addWidget(mainSplit);

    QList<int> splitterSizeRow;
    for (int row = 0; row < m_MultiWidget->GetRowCount(); ++row)
    {
        splitterSizeRow.push_back(1000);

        QList<int> splitterSizeColumn;
        auto splitter = new QSplitter(mainSplit);
        for (int column = 0; column < m_MultiWidget->GetColumnCount(); ++column)
        {
            splitterSizeColumn.push_back(1000);
            auto renderWindowWidget = m_MultiWidget->GetRenderWindowWidget(row, column);
            splitter->addWidget(renderWindowWidget.get());
            renderWindowWidget->show();
        }
        splitter->setSizes(splitterSizeColumn);
    }

    mainSplit->setSizes(splitterSizeRow);

    m_MultiWidget->ActivateMenuWidget(true);

    auto allRenderWindows = m_MultiWidget->GetRenderWindows();
    for (auto& renderWindow : allRenderWindows)
    {
        renderWindow->UpdateLayoutDesignList(LayoutDesign::TWO_AXIAL_WITH_SEGMENTATION);
    }
}

void MyQmitkMultiWidgetLayoutManager::SetLeftACandRightBC()
{
    MITK_INFO << "changing layout to left AC and right BC" << std::endl;

    //Hide all Menu Widgets
    amosWidget->ActivateMenuWidget(false);

    delete amosWidget->layout();

    //create Main Layout
    auto hBoxLayout = new QHBoxLayout(amosWidget);
    hBoxLayout->setContentsMargins(0, 0, 0, 0);
    amosWidget->setLayout(hBoxLayout);
    hBoxLayout->setMargin(0);

    //create main splitter
    auto splitter = new QSplitter(Qt::Horizontal, amosWidget);
    hBoxLayout->addWidget(splitter);
    //create splitterVert  and add to the mainSplit
    auto splitterUp = new QSplitter(splitter);
    splitter->addWidget( splitterUp );

    //add LevelWindow Widget to mainSplitter
//     splitter->addWidget( levelWindowWidget );

    //create splitterUp and splitterDown
//     splitterUp = new QSplitter( splitter );
//     splitterDown = new QSplitter( splitterVert );

    //insert Widget Container into splitter top
    
    splitterUp->addWidget(amosWidget->GetRenderWindowWidget(amosWidget->GetRenderWindowAC()).get());   

    //set SplitterSize for splitter top
    QList<int> splitterSize;
    //splitterVert Widget Container into splitter bottom
    splitterUp->addWidget(amosWidget->GetRenderWindowWidget(amosWidget->GetRenderWindowBC()).get());
    //set SplitterSize for splitter splitterVert
    splitterSize.clear();
    splitterSize.push_back(1000);
    splitterSize.push_back(1000);
    splitterUp->setSizes( splitterSize );

    m_MultiWidget->ActivateMenuWidget(true);

    auto allRenderWindows = m_MultiWidget->GetRenderWindows();
    for (auto& renderWindow : allRenderWindows)
    {
        //MITK 2021
        //renderWindow->LayoutDesignListChanged(LayoutDesign::LEFT_AC_AND_RIGHT_BC);
        renderWindow->UpdateLayoutDesignList(LayoutDesign::LEFT_AC_AND_RIGHT_BC);
    }       
}

void MyQmitkMultiWidgetLayoutManager::SetACUpandBCDown()
{
        MITK_INFO << "changing layout to up AC and down BC" << std::endl;

    //Hide all Menu Widgets
    m_MultiWidget->ActivateMenuWidget(false);

    delete m_MultiWidget->layout();

    //create Main Layout
    auto hBoxLayout = new QHBoxLayout(m_MultiWidget);
    hBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_MultiWidget->setLayout(hBoxLayout);
    hBoxLayout->setMargin(0);

    //create main splitter
    auto splitter = new QSplitter(Qt::Vertical, m_MultiWidget);
    hBoxLayout->addWidget(splitter);
    //create splitterVert  and add to the mainSplit
    auto splitterUp = new QSplitter(splitter);
    splitter->addWidget( splitterUp );

    //add LevelWindow Widget to mainSplitter
//     splitter->addWidget( levelWindowWidget );

    //create splitterUp and splitterDown
//     splitterUp = new QSplitter( splitter );
//     splitterDown = new QSplitter( splitterVert );

    //insert Widget Container into splitter top
    
    splitterUp->addWidget(amosWidget->GetRenderWindowWidget(amosWidget->GetRenderWindowAC()).get());   

    //set SplitterSize for splitter top
    QList<int> splitterSize;
    //splitterVert Widget Container into splitter bottom
    splitterUp->addWidget(amosWidget->GetRenderWindowWidget(amosWidget->GetRenderWindowBC()).get());
    //set SplitterSize for splitter splitterVert
    splitterSize.clear();
    splitterSize.push_back(1000);
    splitterSize.push_back(1000);
    splitterUp->setSizes( splitterSize );

    m_MultiWidget->ActivateMenuWidget(true);

    auto allRenderWindows = m_MultiWidget->GetRenderWindows();
    for (auto& renderWindow : allRenderWindows)
    {
        //MITK 2021
        //renderWindow->LayoutDesignListChanged(LayoutDesign::AC_UP_AND_BC_DOWN);
        renderWindow->UpdateLayoutDesignList(LayoutDesign::AC_UP_AND_BC_DOWN);        
    }       
}

void MyQmitkMultiWidgetLayoutManager::SetDefaultLayout()
{
  MITK_INFO << "Set my default layout" << std::endl;

  m_MultiWidget->ActivateMenuWidget(false);

  delete m_MultiWidget->layout();

  auto hBoxLayout = new QHBoxLayout(m_MultiWidget);
  hBoxLayout->setContentsMargins(0, 0, 0, 0);
  m_MultiWidget->setLayout(hBoxLayout);
  hBoxLayout->setMargin(0);

  auto mainSplit = new QSplitter(Qt::Vertical, m_MultiWidget);
  hBoxLayout->addWidget(mainSplit);

  QList<int> splitterSizeRow;
  for (int row = 0; row < m_MultiWidget->GetRowCount(); ++row)
  {
    splitterSizeRow.push_back(1000);

    QList<int> splitterSizeColumn;
    auto splitter = new QSplitter(mainSplit);
    for (int column = 0; column < m_MultiWidget->GetColumnCount(); ++column)
    {
        // It's shown every view but coronal and sagittal views
        if (column == 1)
            continue;
      splitterSizeColumn.push_back(1000);
      auto renderWindowWidget = m_MultiWidget->GetRenderWindowWidget(row, column);
      splitter->addWidget(renderWindowWidget.get());
      renderWindowWidget->show();
    }
    splitter->setSizes(splitterSizeColumn);
  }

  mainSplit->setSizes(splitterSizeRow);

  m_MultiWidget->ActivateMenuWidget(true);

  auto allRenderWindows = m_MultiWidget->GetRenderWindows();
  for (auto& renderWindow : allRenderWindows)
  {
    renderWindow->UpdateLayoutDesignList(LayoutDesign::DEFAULT);
  }
}
