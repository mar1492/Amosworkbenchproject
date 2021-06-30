#ifndef MYQMITKMULTIWIDGETLAYOUTMANAGER_H
#define MYQMITKMULTIWIDGETLAYOUTMANAGER_H

#include <QmitkMultiWidgetLayoutManager.h>
class AmosWidget;

class QmitkAbstractMultiWidget;

/**
 * @todo write docs
 */
class MyQmitkMultiWidgetLayoutManager : public QmitkMultiWidgetLayoutManager
{
//     Q_OBJECT
    
public:
    /**
     * Constructor
     *
     * @param multiwidget TODO
     */
    MyQmitkMultiWidgetLayoutManager(QmitkAbstractMultiWidget* multiwidget);

    /**
     * @todo write docs
     *
     * @param layoutDesign TODO
     * @return TODO
     */
    virtual void SetLayoutDesign(QmitkMultiWidgetLayoutManager::LayoutDesign layoutDesign);
    
    void SetLeftACandRightBC();
    void SetACUpandBCDown();
    
protected:
    AmosWidget* amosWidget;

};

#endif // MYQMITKMULTIWIDGETLAYOUTMANAGER_H
