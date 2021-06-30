#ifndef RECONSTRUCTIONDIALOG_H
#define RECONSTRUCTIONDIALOG_H

#include <qewtabdialog.h>
#include "reconstructioninputlist.h"
#include "reconstruction.h"
#include "rightclickmenu.h"
#include "amosimage.h"
#include "xmlwritable.h"

class ImagesListDialog;
class QewSimpleDialog;
class QTableWidgetItem;
class QRadioButton;
class QSpinBox;

namespace Ui {
class ReconstructionDialog;
}

class ReconstructionDialog : public QewTabDialog, public XMLWritable
{
    Q_OBJECT

protected:
    struct ParameterData {
        QString name;
        QString prefix;
        QString type;
        QString defaultValue;
        QString condition1;
        QString condition2;
        QString toolTip;
    };

public:        
    explicit ReconstructionDialog(QWidget *parent = 0);
    ~ReconstructionDialog();

    bool setUpDialog();
    void setSelectableImages(ReconstructionInputList* selectableImages);
    void removeAllChildren();

    Reconstruction *getReconstruction() const;
    void setReconstruction(Reconstruction *reconstruction);

    bool dialogActivated(bool activated, QewDialogInactivate from = FromOutside,
                         bool noSaveChildren = false);
    void askReconstructionState();

    /*virtual */void setMenuActions(const QMap<RightCLickMenu::ActionTypes, QAction*> &menuActions);
    virtual QList<QAction*> setMenuActionTexts();
    bool getDataChangedThis() const;
    
    QString saveXML();
    void writeToXml(QDomDocument& doc, QDomElement& e);
    void readFromXML(QDomElement& e);
    QString generateHtml(int /*prevIndent*/) const {return "";}

    void setTypesToRemove(QList<AmosImage::ImageType> types);

    void setRootOutputPath(const QString &rootOutputPath);
    
    void setMaximumThreads(int maximumThreads);
    void setProgressBarVisible(bool visible);
    void setProcessProgressBarValue(int progress);
    void setProgressProcessType(QString progressType);
    void setCaseName(QString caseName);
    void setCaseProgressRange(int numCases);
    void setCaseProgress(int caseNum);
    void setMaximumBatchProgressBar(int numSlices);
    void displayTime(QString time);
    void processFinished(QStringList fullyProcessedSubjects);
    
    int getNumberOfThreads();
    int getNumberOfSubjects();
    
    bool getSubjectsAsMainDirs();
    void setSubjectsAsMainDirs(bool subjectsAsMainDirs);
    QStringList getSelectedSubjects();
    void updateSubjects();
    
    void conditionalEnableRun(bool enable);
    void showPrefixParamColumn(bool show);
    
    void programDataChangedFromMenu();
    
    void enableMaxSubjects(bool enable);
    void setMaxSubjects(int maximumSubjects);
        

public slots:
    void getExternVal(const QString& var_name, const QVariant& var_val);

protected:
    void changeEvent(QEvent *e);
    void saveThis( void );
    bool validateThis( void );
    void cleanThis( void );    
    void restoreThis();
    void setDataChangedThis(bool dataChanged);
    void fillUi();

    void fillSubjectsTable();
    void fillSubjectRow(int row, bool selected, QString id, bool processed);
    void fillParametersTable();
    void addParameter(const QString& name, const Reconstruction::Parameter &parameter);
    void setCellWidgetValue(ParameterData paramData, QTableWidgetItem* itemValue);
    void setCellWidgetValue(QWidget* widget, QString value);

    virtual QString getTitle() const;
    void selectSubjects(bool select);
    void selectParamImage(QList<ReconstructionInputInfo> list, QString dialogTitle);    

    void loadSubjectsFile(bool openDialog = true);
    
    void disableProcessedSubjects(QStringList fullyProcessedSubjects);
    bool reprocessMessage(bool plural);
    void subjectForReprocessing(int row, int col);
    
    void readProgramData();
    void setProgramData();
    
    void write_constructor(QDomElement& /*e*/) {}

protected slots:
    void addParameter();
    void removeParameters();
    void enableParametersEdition(bool enable);

    void onDescriptionModified();

    void selectSubjects();
    void unselectSubjects();
    void reprocessSubjects();
    void onSubjectItemChecked(QTableWidgetItem *item);    
    void loadSubjectsFileFromList(int index);
    void saveSubjectsFile();     
    
    QString XMLDocName();
    
    void chooseProgramPath();
    void setProgramPath();
    void editingFinished();

    void parameterChanged(int row, int col = -1);
    void parameterToBeChanged(int row, int col);
    void boolParamChanged();
    void doubleParamChanged();
    void enumParamChanged();
    void intParamChanged();
    void selectParameterPath();
    void selectInputImage();
    void selectOutputImage();        
    
    void showActionsMenu(const QPoint & pos);

    bool changeImageNameInParams(QString oldName, QString newName, bool changeInReconstruction);
    bool removeImageFromParams(QString name, bool changeInReconstruction);

    void imageRemovedFromList(QString uuid, QString name);
    void imageNameChangedInList(QString uuid, QString name);

    void stateButtonClicked(int index);
    
    void performProcess();
    void delayedEmitDialogSaved();
    
    void setNumberOfThreads(int numThreads);
    void setNumberOfSubjects(int numSubjects);

signals:
    void sendIdChanged(QTreeWidgetItem*, QString);
    void sendRemoveDialog(ReconstructionDialog* );
    void sendPerformReconstruction(ReconstructionDialog* );

protected:
    QewExtensibleDialog* imagesDialog;
    ImagesListDialog* inputDialog;
    ImagesListDialog* outputDialog;
    QString id;
    QString prevId;

    QString programPath;
     QMap<QString, Reconstruction::Parameter> parameters;
//     QMultiMap<QString, Reconstruction::Parameter> parameters;
    QList<ParameterData> parametersData;
    bool programDataSet;
    int paramRow;
    QString description;
    bool prevRunEnabled;
    QMap<QString, bool> subjects;
    QStringList selectedSubjects;
    QString subjectsFilename;
    QStringList subjectsFileList;
    QStringList processedSubjects;
    int maximumThreads;
    int maximumSubjects;
    int numThreads;
    int numSubjects;    

    ReconstructionInputList* selectableImages;
    QList<ReconstructionInputInfo> inputInfoImages;
    Reconstruction* reconstruction;
    bool reconstructionSet;

    QString rootOutputPath;
    QString outputPath;
    QList<AmosImage*> outputImages;
    QMap<RightCLickMenu::ActionTypes, QAction*> menuActions;
    QTableWidgetItem* selectedParamItem;
    QString prevParameterName;
    QVector<QStringList> removedInputInfos;

    QList<AmosImage::ImageType> typesToRemove;
    Reconstruction::ExecStatus execState;
    QList<QRadioButton*> stateButtons;
    
    bool subjectsAsMainDirs;
    
    bool prevDataChanged;

//private:
    Ui::ReconstructionDialog *ui;
};

#endif // RECONSTRUCTIONDIALOG_H
