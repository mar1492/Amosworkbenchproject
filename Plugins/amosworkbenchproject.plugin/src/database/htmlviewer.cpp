#include "htmlviewer.h"
#include "ui_htmlviewer.h"
#include "xmloperator.h"

#include <QUrl>
// #include <QPrinter>
// #include <QPrintDialog>
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QAction>
#include <QPageLayout>
#include <QMessageBox>
#include <qobjectdefs.h>

HtmlViewer::HtmlViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HtmlViewer)
{
    ui->setupUi(this);
    webview = new ZoomWebView(this);

    QWebEngineSettings* settings = webview->settings();
    settings->setFontFamily(QWebEngineSettings::StandardFont, "Helvetica");

    QLayout* mainLayout = layout();
    if(mainLayout->inherits("QBoxLayout")) {
        QBoxLayout* boxLayout = (QBoxLayout*)mainLayout;
        boxLayout->insertWidget(0, webview);
    }
    connect(ui->closePushButton, SIGNAL(released()), this, SLOT(hide()));
    connect(ui->savePushButton, SIGNAL(released()), this, SLOT(save()));
    connect(ui->printPushButton, SIGNAL(released()), this, SLOT(print()));

    connect(webview->pageAction(QWebEnginePage::Reload), SIGNAL(triggered(bool)),
            this, SLOT(reload()));
//    reloadAction->setVisible(false);

}

HtmlViewer::~HtmlViewer()
{
    delete ui;
    delete webview;
}

void HtmlViewer::setHtml(const QString &html)
{    
    webview->load(QUrl::fromLocalFile(html));
}

void HtmlViewer::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

// void HtmlViewer::print()
// {
//     QPrinter printer(QPrinter::ScreenResolution);
//     printer.setOutputFormat(QPrinter::PdfFormat);
//     printer.setPaperSize(QPrinter::A4);
//     QFileInfo fileInfo(QDir::home(), webview->title() + ".pdf");
//     QString filename = fileInfo.absoluteFilePath();
//     printer.setOutputFileName(filename);
// 
//     QPrintDialog printDialog(&printer);
//     if (printDialog.exec() == QDialog::Accepted) {
//         QEventLoop loop;
//         bool result;
//         auto printPreview = [&](bool success) { result = success; loop.quit(); };
//         webview->page()->print(&printer, std::move(printPreview));
// //         webview->print(&printer);
//     }
// }

void HtmlViewer::print()
{    
    XMLOperator* xml_operator = XMLOperator::instance();
    QString sender = QObject::tr("Print project to PDF");
    QFileInfo fileInfo(QDir::home(), webview->title() + ".pdf");
    QString filename = fileInfo.absoluteFilePath();
    QString type_file = "";
    QString extensions = "pdf";
    bool change_name = true;
    bool ret = true;
    QFile file;
    
    if(change_name)
        ret = xml_operator->getFileDialog(filename, sender, QFileDialog::AcceptSave, QFileDialog::AnyFile, type_file, extensions);
    if(ret) {
        file.setFileName(filename);
        if(file.open(QIODevice::WriteOnly))
           webview->page()->printToPdf(filename);
        else 
            QMessageBox::critical(0, tr("Save PDF"),
                tr("Couldn't open file for saving"), tr("&Cancel"));
    }    
}

void HtmlViewer::save()
{
//     XMLOperator* xml_operator = XMLOperator::instance();
//     QString sender = QObject::tr("Save project html");
//     QFileInfo fileInfo(QDir::home(), webview->title() + ".html");
//     QString filename = fileInfo.absoluteFilePath();
//     QString type_file = "";
//     QString extensions = "html";
//     bool change_name = true;
//     QString html = webview->page()->mainFrame()->toHtml(); 
    connect(this, SIGNAL(getHtml(QString)), this, SLOT(handleHtml(QString)));
    webview->page()->toHtml([this](const QString& result) mutable {emit getHtml(result);});  
//     xml_operator->saveDocument(filename, html, type_file, extensions, sender, change_name);
}

void HtmlViewer::handleHtml(QString html)
{
    disconnect(this, SIGNAL(getHtml(QString)), this, SLOT(handleHtml(QString)));
    XMLOperator* xml_operator = XMLOperator::instance();
    QString sender = QObject::tr("Save project html");
    QFileInfo fileInfo(QDir::home(), webview->title() + ".html");
    QString filename = fileInfo.absoluteFilePath();
    QString type_file = "";
    QString extensions = "html";
    bool change_name = true;    
    xml_operator->saveDocument(filename, html, type_file, extensions, sender, change_name);    
}

void HtmlViewer::reload()
{
    emit reloadHtml();
    QTimer::singleShot(200, webview, SLOT(reload()));
}

