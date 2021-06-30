#include "subjectsdialog.h"
#include "ui_subjectsdialog.h"
#include "scansdialog.h"
#include "subject.h"
#include "savemenutextedit.h"
#include "qvariantptr.h"
#include "utils.hpp"

#include <QInputDialog>
#include <QMessageBox>

#include <iostream>
#include <algorithm>
using namespace std;

SubjectsDialog::SubjectsDialog(QWidget *parent) :
    QewExtensibleDialog(parent),
    ui(new Ui::SubjectsDialog)
{
    ui->setupUi(this);
    connect(ui->subjectsTableWidget, SIGNAL(cellDoubleClicked(int, int)) ,
     this, SLOT(cellSelected(int, int)));
    ui->subjectsTableWidget->horizontalHeader()->setStretchLastSection(true);

    selectedRow = -1;
    currentSubject = 0;
    subjectsAsMainDirs = false;
    scansDialog = 0;
    setAcceptMeansClose(false);
}

SubjectsDialog::~SubjectsDialog()
{
    delete ui;
    qDebug("Deleting SubjectsDialog");
}

void SubjectsDialog::removeSubjects()
{
    QList<QTableWidgetItem*> items = ui->subjectsTableWidget->selectedItems();
    QList<int> rows = Utils::getRowsForItems(items);

    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Deleting subjects"));
    int cnt = rows.size();
    if(cnt == 0) {
        msgBox.setText(tr("You have to select subjects in the subjects list"));
        msgBox.exec();
        return;
    }

    for(int i = 0; i < cnt; i++) {
        if(!rows.contains(items.at(i)->row()))
            rows.append(items.at(i)->row());
    }

    cnt = rows.size();

    msgBox.setText(tr("Deleting ") + QString::number(cnt) + tr(" subjects"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);

    int ret = msgBox.exec();
    if(ret == QMessageBox::Cancel)
        return;

    sort(rows.begin(), rows.end());
    for(int i = cnt - 1; i >= 0; i--) {
        QString id = ui->subjectsTableWidget->item(rows.at(i), 0)->text();
        Subject* subject = subjects->take(id);
        delete subject;
        ui->subjectsTableWidget->removeRow(rows.at(i));
    }

}

void SubjectsDialog::removeSubjects(QStringList subjectsId)
{
    int cnt = subjectsId.size();
    if(!cnt)
        return;

    QList<int> rows;
    for(int i = 0; i < cnt; i++) {
        QList<QTableWidgetItem*> items = ui->subjectsTableWidget->findItems(subjectsId.at(i), Qt::MatchExactly);
        if(items.size() > 0) {
            QTableWidgetItem* item = items.at(0);
            rows.append(item->row());
        }

    }
    cnt = rows.size();
    sort(rows.begin(), rows.end());
    for(int i = cnt - 1; i >= 0; i--) {
        QString id = ui->subjectsTableWidget->item(rows.at(i), 0)->text();
        Subject* subject = subjects->take(id);
        delete subject;
        ui->subjectsTableWidget->removeRow(rows.at(i));
    }
}

void SubjectsDialog::getExternVal(const QString &var_name, const QVariant &var_val)
{
    if(this != QVariantPtr<QewExtensibleDialog>::asPtr(var_val))
        return;

    if(var_name == RightCLickMenu::getMenuActionText(RightCLickMenu::Add))
        generateSubjects();
    else if(var_name == RightCLickMenu::getMenuActionText(RightCLickMenu::Remove))
        removeSubjects();
    else if(var_name == RightCLickMenu::getMenuActionText(RightCLickMenu::Save))
        emitUpdateSubjects();
    else if(var_name == RightCLickMenu::getMenuActionText(RightCLickMenu::Restore))
        emitRestoreSubjects();
}

void SubjectsDialog::changeEvent(QEvent *e)
{
    QewExtensibleDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void SubjectsDialog::cellSelected(int row, int col)
{
    if(selectedRow > -1 && (selectedRow != row || col != 1)) {
        if(currentSubject && getDataChangedThis()) {
            QMessageBox msgBox;
            msgBox.setText(currentSubject->getId() + tr( " description not saved.\nDo you want to continue?"));
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setDefaultButton(QMessageBox::Cancel);
            int ret = msgBox.exec();

            if(ret == QMessageBox::Cancel) {
                ui->subjectsTableWidget->setCurrentCell(selectedRow, 1);
                ui->subjectsTableWidget->item(selectedRow, 1)->setText(currentSubject->getDescription());
                return;
            }
            else {
                setDataChangedThis(false);
            }
        }

        SaveMenuTextEdit* te = (SaveMenuTextEdit*) (ui->subjectsTableWidget->cellWidget(selectedRow, 1));
        ui->subjectsTableWidget->item(selectedRow, 1)->setText(te->toPlainText());
        ui->subjectsTableWidget->setCellWidget(selectedRow, 1, 0);
        ui->subjectsTableWidget->setRowHeight(selectedRow, rowHeight);
    }

    if(row > -1 && selectedRow != row && col == 1) {
        currentSubject = subjects->value(ui->subjectsTableWidget->item(row, 0)->text());
        rowHeight = ui->subjectsTableWidget->rowHeight(row);
        ui->subjectsTableWidget->setRowHeight(row, 3*rowHeight);
        ui->subjectsTableWidget->setCellWidget(row, 1, new SaveMenuTextEdit);
        SaveMenuTextEdit* te2 = (SaveMenuTextEdit*) (ui->subjectsTableWidget->cellWidget(row, 1));
        te2->setPlainText(currentSubject->getDescription());
        connect(te2, SIGNAL(saveText(const QString &)), this, SLOT(saveText(const QString &)));
        connect(te2, SIGNAL(textChanged()), this, SLOT(onDescriptionModified()));
    }

    if(col != 1)
        row = -1;
    selectedRow = row;
}

void SubjectsDialog::saveText(const QString &text)
{
    setDataChangedThis(false);
    if(currentSubject)
       currentSubject->setDescription(text);
}

void SubjectsDialog::onDescriptionModified()
{
    setDataChangedThis(true);
}

void SubjectsDialog::setMenuActions(const QMap<RightCLickMenu::ActionTypes, QAction *> &menuActions)
{
    this->menuActions = menuActions;
}

QList<QAction *> SubjectsDialog::setMenuActionTexts()
{
    QList<QAction*> actions;
    QMapIterator<RightCLickMenu::ActionTypes, QAction *> it(menuActions);
    while(it.hasNext()) {
        it.next();
        if(it.key() == RightCLickMenu::Add) {
            if(subjects->size() > 0) {
                it.value()->setText(tr("Recreate subjects"));
                it.value()->setToolTip(tr("Recreate valid subjects from scans directory"));
            }
            else {
                it.value()->setText(tr("Add subjects"));
                it.value()->setToolTip(tr("Add valid subjects from scans directory"));
            }
        }
        else if(it.key() == RightCLickMenu::Remove) {
            it.value()->setText(tr("Remove subjects"));
            it.value()->setToolTip(tr("Remove selected subjects"));
        }
        else if(it.key() == RightCLickMenu::Save) {
            it.value()->setText(tr("Update subjects"));
            it.value()->setToolTip(tr("Update subjects list"));
        }
        else if(it.key() == RightCLickMenu::Restore) {
            it.value()->setText(tr("Restore subjects"));
            it.value()->setToolTip(tr("Restore subjects list\nto previously saved"));
        }
        actions.append(it.value());
    }
    return actions;
}

void SubjectsDialog::setSubjects(QMap<QString, Subject *> *subjects)
{
    this->subjects = subjects;
    populateSubjectsDialog();
    if(scansDialog) {
        if(subjects->size() > 0)
            scansDialog->setOneValidSubjectId(subjects->firstKey());
        else
            scansDialog->setOneValidSubjectId("");
    }
}

void SubjectsDialog::setScansDialog(const ScansDialog *scansDialog)
{
    this->scansDialog = scansDialog;
}

void SubjectsDialog::generateSubjects(bool askToUser)
{

    QString scansPath;
    if(subjectsAsMainDirs)
        scansPath = rootPath;
    else
        scansPath = scansDialog->getPath();
    QFileInfo dirInfo(scansPath);
    QMessageBox msgBox;
    if(askToUser) {
        if(!dirInfo.exists() && dirInfo.isDir()) {
            msgBox.setText(tr("Scans path does not exist or is not a directory"));
            msgBox.exec();
            return;
        }
    }

    QString flairName = scansDialog->getFlairName();
    QString T1Name = scansDialog->getT1Name();

    if(askToUser) {
        if(flairName.isEmpty() || T1Name.isEmpty()) {
            msgBox.setText(tr("Scan image names are empty"));
            msgBox.exec();
            return;
        }
    }

    bool noSubjects = (subjects->size() == 0);

    if(askToUser) {
        QString text;
        if(noSubjects)
            text = tr("Generating subjects from ");
        else
            text = tr("Updating subjects from ");
        msgBox.setText(text + scansPath);
        msgBox.setInformativeText(tr("Changes in database will be made when you click Save button"));
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();

        if(ret == QMessageBox::Cancel)
          return;
    }

    QDir subjectsDirectory = QDir(scansPath, QString(), QDir::SortFlags(QDir::Name | QDir::IgnoreCase ), QDir::Dirs);
    QFileInfoList subdirsSubjects;
    QFileInfoList subdirsScans;

    subdirsSubjects = subjectsDirectory.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
/*    subdirsSubjects.removeFirst(); // .
    subdirsSubjects.removeFirst(); // ..*/   
    int numValidCases = subdirsSubjects.size();
    
    if(subjectsAsMainDirs) {
        for(int i = 0; i < numValidCases; i++) {
            subdirsScans.append(QFileInfo(QDir(subdirsSubjects[i].filePath()), scansDialog->getPath()));
            cout << subdirsScans[i].filePath().toStdString() << endl;
        }
    }
    else {
        for(int i = 0; i < numValidCases; i++) {
            subdirsScans.append(subdirsSubjects.at(i));
            cout << subdirsScans[i].filePath().toStdString() << endl;
        }        
    }

    QStringList invalidSubdirs;
    QStringList validSubdirs;
    for (int i = 0; i < numValidCases; ++i) {
        QDir subdir(subdirsScans.at(i).filePath(), QString(), QDir::SortFlags(QDir::Name | QDir::IgnoreCase ), QDir::Files);
        QString id = subdirsSubjects.at(i).fileName();
        if(!subdir.entryList().contains(flairName) || !subdir.entryList().contains(T1Name)) {
            invalidSubdirs.append(id);
            subdirsScans.removeAt(i);
            subdirsSubjects.removeAt(i);
            i--;
            numValidCases--;
        }
        else if(!subjects->contains(id)) {
            Subject* subject = new Subject();
            subject->setId(subdirsSubjects.at(i).fileName());
//             subject->setDescription(subject->getId()+".\n2.\n3.\n4.");
            subjects->insert(id, subject);
            insertRow(subject, i);
            validSubdirs.append(id);
        }
        else
            validSubdirs.append(id);
    }
    if(invalidSubdirs.size() > 0) {
        QStringList text;
        text.append(tr("Subjects without valid scan image names"));
        text.append("");
        for(int i = 0; i < invalidSubdirs.size(); i++)
            text.append(invalidSubdirs.at(i));
        emit emitExternVal(RightCLickMenu::getMenuActionText(RightCLickMenu::WarningBox), QVariant(text));
    }

    QStringList subjectIds;
    QStringList rowsToRemoved;
    QMapIterator<QString, Subject*> it(*subjects);
    while(it.hasNext()) {
        it.next();
        subjectIds.append(it.key());
        if(!noSubjects) {
            if(!validSubdirs.contains(it.key())) {
                rowsToRemoved.append(it.key());
                subjects->remove(it.key());
            }
        }
    }
    removeSubjects(rowsToRemoved);
    if(subjects->size() > 0)
        scansDialog->setOneValidSubjectId(subjects->firstKey());
    else
        scansDialog->setOneValidSubjectId("");    

    emit emitSubjectIds(subjectIds);

}

void SubjectsDialog::populateSubjectsDialog()
{
    for(int i = ui->subjectsTableWidget->rowCount() - 1; i >= 0; i--)
        ui->subjectsTableWidget->removeRow(i);

    QMapIterator<QString, Subject *> it(*subjects);
    int row = 0;
    while(it.hasNext()) {
        it.next();
        insertRow(it.value(), row);
        row++;
    }
}

void SubjectsDialog::insertRow(Subject *subject, int row)
{
    ui->subjectsTableWidget->insertRow(row);
    subjects->insert(subject->getId(), subject);
    QTableWidgetItem* itemId = new QTableWidgetItem(subject->getId());
    itemId->setFlags(itemId->flags() &  ~Qt::ItemIsEditable);
    QString descript = subject->getDescription();
    QTableWidgetItem* itemDescript = new QTableWidgetItem(descript);
    itemDescript->setFlags(itemId->flags() &  ~Qt::ItemIsEditable);
    ui->subjectsTableWidget->setItem(row, 0, itemId);
    ui->subjectsTableWidget->setItem(row, 1, itemDescript);
}

void SubjectsDialog::setSubjectsAsMainDirs(bool subjectsAsMainDirs)
{
    this->subjectsAsMainDirs = subjectsAsMainDirs;
}

void SubjectsDialog::setRootPath(const QString& rootPath)
{
    this->rootPath = rootPath;
}
