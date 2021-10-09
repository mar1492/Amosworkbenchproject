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
            //amosWidget->CreateAxialRenderWindowWidgets();
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
         case LayoutDesign::TWO_SAGITTAL_WITH_SEGMENTATION:
            //amosWidget->CreateSagittalRenderWindowWidgets();
            TwoSagittalsWithSegments();
            //QMessageBox::information(NULL,"SetLayoutDesign","AXIAL_3D_ONLY: To do");
            break;
        case LayoutDesign::TWO_CORONNAL_WITH_SEGMENTATION:
            TwoCoronalsWithSegments();
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

void MyQmitkMultiWidgetLayoutManager::TwoCoronalsWithSegments()
{
  MITK_INFO << "TWO_CORONNAL_WITH_SEGMENTATION layout. 6 windows CORONAL" << std::endl;

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
    for (int row = CORONAL_ROW_0; row <= CORONAL_ROW_1 /*m_MultiWidget->GetRowCount()*/; ++row)
    {
        splitterSizeRow.push_back(1000);

        QList<int> splitterSizeColumn;
        auto splitter = new QSplitter(mainSplit);
        for (int column = 0; column < m_MultiWidget->GetColumnCount(); ++column)
        {
            // 3D View is only created in Axial view
            if (row == CORONAL_ROW_1 && column == 2)
            {
                splitterSizeColumn.push_back(1000);
                auto renderWindowWidget = m_MultiWidget->GetRenderWindowWidget(AXIAL_ROW_1, column);
                splitter->addWidget(renderWindowWidget.get());
                 renderWindowWidget->show();
           }
            else
            {
                splitterSizeColumn.push_back(1000);
                auto renderWindowWidget = m_MultiWidget->GetRenderWindowWidget(row, column);
                splitter->addWidget(renderWindowWidget.get());
                 renderWindowWidget->show();
            }
            /// JCAM 19092021
            // if (row == CORONAL_ROW_1 && column == 2)
            //    mitk::BaseRenderer::GetInstance(renderWindowWidget->GetRenderWindow()->renderWindow())->SetMapperID(mitk::BaseRenderer::Standard3D);
            ///
           
        }
        splitter->setSizes(splitterSizeColumn);
    }

    mainSplit->setSizes(splitterSizeRow);

    m_MultiWidget->ActivateMenuWidget(true);

    auto allRenderWindows = m_MultiWidget->GetRenderWindows();
    for (auto& renderWindow : allRenderWindows)
    {
        renderWindow->UpdateLayoutDesignList(LayoutDesign::TWO_CORONNAL_WITH_SEGMENTATION);
    }

}

void MyQmitkMultiWidgetLayoutManager::TwoSagittalsWithSegments()
{
  MITK_INFO << "TWO_SAGITTAL_WITH_SEGMENTATION layout. 6 windows SAGITTAL" << std::endl;

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
    for (int row = SAGITTAL_ROW_0; row <= SAGITTAL_ROW_1 /*m_MultiWidget->GetRowCount()*/; ++row)
    {
        splitterSizeRow.push_back(1000);

        QList<int> splitterSizeColumn;
        auto splitter = new QSplitter(mainSplit);
        for (int column = 0; column < m_MultiWidget->GetColumnCount(); ++column)
        {
             // 3D View is only created in Axial view
            if (row == SAGITTAL_ROW_1 && column == 2)
            {
            splitterSizeColumn.push_back(1000);
            auto renderWindowWidget = m_MultiWidget->GetRenderWindowWidget(AXIAL_ROW_1, 2);
            splitter->addWidget(renderWindowWidget.get());
            renderWindowWidget->show();
            }
            else
            {
            splitterSizeColumn.push_back(1000);
            auto renderWindowWidget = m_MultiWidget->GetRenderWindowWidget(row, column);
            splitter->addWidget(renderWindowWidget.get());
            renderWindowWidget->show();
            }
        }
        splitter->setSizes(splitterSizeColumn);
    }

    mainSplit->setSizes(splitterSizeRow);

    m_MultiWidget->ActivateMenuWidget(true);

    auto allRenderWindows = m_MultiWidget->GetRenderWindows();
    for (auto& renderWindow : allRenderWindows)
    {
        renderWindow->UpdateLayoutDesignList(LayoutDesign::TWO_SAGITTAL_WITH_SEGMENTATION);
    }

/*
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
    
    splitterSizeRow.push_back(1000);
    QList<int> splitterSizeColumn;
    auto splitter = new QSplitter(mainSplit);

    // Sagittal AC
    splitterSizeColumn.push_back(1000);
    auto renderWindowWidget = amosWidget->GetRenderWindowWidget(0,0);
    renderWindowWidget->GetRenderWindow()->GetSliceNavigationController()->SetDefaultViewDirection(mitk::SliceNavigationController::Sagittal);     
    //renderWindowWidget->GetRenderWindow()->SetLayoutIndex(QmitkMxNMultiWidget::ViewDirection::SAGITTAL);
    splitter->addWidget(renderWindowWidget.get());   
    renderWindowWidget->show();
    
    // Axial AC
    splitterSizeColumn.push_back(1000);
    renderWindowWidget = amosWidget->GetRenderWindowWidget(0,1);
    renderWindowWidget->GetRenderWindow()->GetSliceNavigationController()->SetDefaultViewDirection(mitk::SliceNavigationController::Axial); 
    //renderWindowWidget->GetRenderWindow()->SetLayoutIndex(QmitkMxNMultiWidget::ViewDirection::AXIAL);
    splitter->addWidget(renderWindowWidget.get());   
    renderWindowWidget->show();
   
    // Sagittal Patient
    splitterSizeColumn.push_back(1000);
    renderWindowWidget = amosWidget->GetRenderWindowWidget(0,2);
    renderWindowWidget->GetRenderWindow()->GetSliceNavigationController()->SetDefaultViewDirection(mitk::SliceNavigationController::Axial); 
    //renderWindowWidget->GetRenderWindow()->SetLayoutIndex(QmitkMxNMultiWidget::ViewDirection::SAGITTAL);
    splitter->addWidget(renderWindowWidget.get());   
    renderWindowWidget->show();

    splitterSizeRow.push_back(1000);
    auto splitter2 = new QSplitter(mainSplit);
    
    // Sagittal BC
    splitterSizeColumn.push_back(1000);
    renderWindowWidget = amosWidget->GetRenderWindowWidget(1,0);
    renderWindowWidget->GetRenderWindow()->GetSliceNavigationController()->SetDefaultViewDirection(mitk::SliceNavigationController::Sagittal); 
    //renderWindowWidget->GetRenderWindow()->SetLayoutIndex(QmitkMxNMultiWidget::ViewDirection::SAGITTAL);
    splitter2->addWidget(renderWindowWidget.get());   
    renderWindowWidget->show();

    // Coronal
    splitterSizeColumn.push_back(1000);
    renderWindowWidget = amosWidget->GetRenderWindowWidget(1,1);
    renderWindowWidget->GetRenderWindow()->GetSliceNavigationController()->SetDefaultViewDirection(mitk::SliceNavigationController::Frontal); 
    //renderWindowWidget->GetRenderWindow()->SetLayoutIndex(QmitkMxNMultiWidget::ViewDirection::CORONAL);
    splitter2->addWidget(renderWindowWidget.get());   
    renderWindowWidget->show();

    // 3D
    splitterSizeColumn.push_back(1000);
    renderWindowWidget = amosWidget->GetRenderWindowWidget(1,2);
    splitter2->addWidget(renderWindowWidget.get());   
    renderWindowWidget->show();

    mainSplit->setSizes(splitterSizeRow);

    m_MultiWidget->ActivateMenuWidget(true);

    auto allRenderWindows = m_MultiWidget->GetRenderWindows();
    for (auto& renderWindow : allRenderWindows)
    {
        renderWindow->UpdateLayoutDesignList(LayoutDesign::TWO_SAGITTAL_WITH_SEGMENTATION);
    }  
*/
}

void MyQmitkMultiWidgetLayoutManager::TwoAxialWithSegmentationLayout()
{
  MITK_INFO << "Two axial with segmentation layout. 6 windows AXIAL" << std::endl;

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
    for (int row = AXIAL_ROW_0; row <= AXIAL_ROW_1 /*m_MultiWidget->GetRowCount()*/; ++row)
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
  MITK_INFO << "Set my default layout: 4 windows Axial" << std::endl;

  m_MultiWidget->ActivateMenuWidget(false);

  delete m_MultiWidget->layout();

  auto hBoxLayout = new QHBoxLayout(m_MultiWidget);
  hBoxLayout->setContentsMargins(0, 0, 0, 0);
  m_MultiWidget->setLayout(hBoxLayout);
  hBoxLayout->setMargin(0);

  auto mainSplit = new QSplitter(Qt::Vertical, m_MultiWidget);
  hBoxLayout->addWidget(mainSplit);

  QList<int> splitterSizeRow;
  for (int row = AXIAL_ROW_0; row <= AXIAL_ROW_1 /*m_MultiWidget->GetRowCount()*/; ++row)
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
