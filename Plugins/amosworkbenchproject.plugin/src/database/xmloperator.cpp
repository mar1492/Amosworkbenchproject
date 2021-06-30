/***************************************************************************
                          xmloperator.cpp  -  description
                             -------------------
    begin                : Sat Jun 4 2005
    copyright            : (C) 2005 by Jose M. Cuadra Troncoso
    email                : jmcuadra@dia.uned.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and\or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "xmloperator.h"
#include "filedialog.h"

#include <iostream>
#include <QMessageBox>

XMLOperator* XMLOperator::xml_operator = 0;

XMLOperator* XMLOperator::instance(void)
{

    if(xml_operator == 0)
        xml_operator = new XMLOperator;
    return xml_operator;

}

XMLOperator::XMLOperator(){

    QList<int> indents;
    indents << 0 << 1 << 2 << 3 << 4 << 5 << 6 << 10;
    for(int i = 0; i < indents.size(); i++) {
        QString indent = " class=";
        indent += '"';
        indent += "indented" + QString::number(indents.at(i));
        indent += '"';
        indentations.insert(indents.at(i), indent);
        indent.remove(indent.size() - 1, 1);
        indent += " anchor";
        indent += '"';
        indentationsAnchors.insert(indents.at(i), indent);
    }

    htmlInit = "";
}
XMLOperator::~XMLOperator(){
}

QDomElement XMLOperator::findTag(const QString& tag_name,
                                 const QDomElement& elem, bool direct_child)
{

    QDomElement e_tag, ret_tag;
    QDomNode n;
    if(!direct_child) {
        QDomNodeList n_tag = elem.elementsByTagName(tag_name);
        n = n_tag.item(0);
        if(!n.isNull())
            ret_tag = n.toElement();
    }
    else {
        n = elem.firstChild();
        while(!n.isNull()) {
            e_tag = n.toElement();
            QString s_e = e_tag.tagName();
            if(e_tag.tagName() == tag_name) {
                ret_tag = e_tag;
                break;
            }
            n = n.nextSibling();
        }
    }
    return ret_tag;
}

QDomElement XMLOperator::findTagIdent(const QString& tag_name,
                                      const QDomElement& elem, const QString& tag_id,
                                      bool exact)
{

    QDomNodeList n_tag = elem.elementsByTagName(tag_name);
    QDomElement e_tag;
    QDomElement ret_tag;
    int cnt_tags = n_tag.count();
    for(int i = 0; i < cnt_tags; ++i) {
        QDomNode n = n_tag.item(i);
        e_tag = n.toElement();
        if(exact) {
            if(e_tag.text() == tag_id) {
                ret_tag = e_tag;
                break;
            }
        }
        else {
            if(e_tag.text().compare(tag_id) > 0) {
                ret_tag = e_tag;
                break;
            }
        }
    }
    return ret_tag;
}

QDomElement XMLOperator::findTagIdent(const QString& tag_name,
                                      const QDomElement& elem, const int& tag_id,
                                      bool exact)
{
    bool ok;
    QDomNodeList n_tag = elem.elementsByTagName(tag_name);
    QDomElement e_tag;
    QString e_t = elem.tagName();
    QDomElement ret_tag;
    int cnt_tags = n_tag.count();
    for(int i = 0; i < cnt_tags; ++i) {
        QDomNode n = n_tag.item(i);
        e_tag = n.toElement();
        if(exact) {
            if(e_tag.text().toInt(&ok) == tag_id) {
                ret_tag = e_tag;
                break;
            }
        }
        else {
            if(e_tag.text().toInt(&ok) > tag_id) {
                ret_tag = e_tag;
                break;
            }
        }
    }
    return ret_tag;
}

QDomElement XMLOperator::findParentTag(const QString& tag_name,
                                       const QDomElement& elem, const int& tag_id, bool exact)
{

    QDomElement e_id;
    QDomElement e_parent;
    e_id = findTagIdent(tag_name, elem, tag_id, exact);
    if(!e_id.isNull())
        e_parent = e_id.parentNode().toElement();
    return e_parent;

}

QDomElement XMLOperator::findParentTag(const QString& tag_name,
                                       const QDomElement& elem, const QString& tag_id, bool exact)
{

    QDomElement e_id;
    QDomElement e_parent;
    e_id = findTagIdent(tag_name, elem, tag_id, exact);
    if(!e_id.isNull())
        e_parent = e_id.parentNode().toElement();
    return e_parent;

}


QDomElement XMLOperator::findElementById (const QString& tag_name,
                                          const QDomElement& elem, const QString& id, const QString& id_name)
{

    QDomNodeList n_tag = elem.elementsByTagName(tag_name);
    QDomElement e_tag;
    QDomElement ret_tag;
    int cnt_tags = n_tag.count();
    for(int i = 0; i < cnt_tags; ++i) {
        QDomNode n = n_tag.item(i);
        e_tag = n.toElement();

        if(e_tag.attribute(id_name) == id) {
            ret_tag = e_tag;
            break;
        }
    }
    return ret_tag;
}


void XMLOperator::removeChildren(QDomNode& n_parent, int first,
                                 int last)
{

    QDomNodeList nodes = n_parent.childNodes();
    last = last < 0 ? nodes.count() : last;
    for(int i = first; i < last; i++)
        n_parent.removeChild(nodes.item(first));

}

void XMLOperator::deleteElementsByTagName(const QString& tagname,
                                          QDomElement& e)
{

    QDomNodeList l_elems = e.elementsByTagName(tagname);
    while(l_elems.count()) {
        QDomNode n_parent = l_elems.item(0).parentNode();
        n_parent.removeChild(l_elems.item(0));
    }

}

void XMLOperator::deleteParentsByTagIdent(const QString& tagname,
                                          QDomElement& e, const QString& ident)
{

    QDomNodeList l_oper_mons = e.elementsByTagName(tagname);
    for(int i = 0; i < int(l_oper_mons.count()); i++) {
        if(l_oper_mons.item(i).toElement().text() == ident) {
            QDomNode n_parent = l_oper_mons.item(i).parentNode();
            QDomNode g_parent = n_parent.parentNode();
            g_parent.removeChild(n_parent);
            --i;
        }
    }

}

void XMLOperator::deleteParentsByTagIdent(const QString& tagname,
                                          QDomElement& e, const int& ident)
{

    QDomNodeList l_oper_mons = e.elementsByTagName(tagname);
    for(int i = 0; i < int(l_oper_mons.count()); i++) {
        if(l_oper_mons.item(i).toElement().text().toInt() == ident) {
            QDomNode n_parent = l_oper_mons.item(i).parentNode();
            QDomNode g_parent = n_parent.parentNode();
            g_parent.removeChild(n_parent);
            --i;
        }
    }

}

void XMLOperator::insertChildByTagName(const QString& tagname,
                                       QDomElement& e_parent, QDomElement& e,
                                       bool tagname_no_child)
{

    bool ok;
    QDomElement e_post = findTag(tagname, e_parent,
                                 tagname_no_child);
    QString str_ident;
    if(!tagname_no_child)
        str_ident = findTag(tagname, e).text();
    else
        str_ident = e.text();
    int num_ident = str_ident.toInt(&ok);
    e_parent.appendChild(e);

    if(!e_post.isNull()) {
        if(ok) {
            if(!tagname_no_child)
                e_post = findParentTag(tagname,
                                       e_parent, num_ident, false);
            else
                e_post = findTagIdent(tagname,
                                      e_parent, num_ident, false);
        }
        else {
            if(!tagname_no_child)
                e_post = findParentTag(tagname,
                                       e_parent, str_ident, false);
            else
                e_post = findTagIdent(tagname,
                                      e_parent, str_ident, false);
        }
        if(!e_post.isNull())
            e_parent.insertBefore(e, e_post);
    }

}

QDomElement XMLOperator::createTextElement(QDomDocument& doc,
                                           const QString& tag_name, const QString& tag_text)
{
    QDomElement tag = doc.createElement(tag_name);
    QDomText text = doc.createTextNode(tag_text);
    tag.appendChild(text);
    return tag;
}

QDomElement XMLOperator::createMultiLineTextElement(QDomDocument &doc, const QString &tag_name, const QString &tag_text)
{
    QDomElement tag = doc.createElement(tag_name);
    QStringList lines = tag_text.split("\n");

    for(int i = 0; i < lines.size(); i++)
        tag.appendChild(createTextElement(doc, "line", lines.at(i)));
    return tag;
}

bool XMLOperator::insertDocContent(QDomElement& e_parent,
                                   QString& file_name, const QString& doctype_name,
                                   const QString& sender)
{
    bool ret = false;
    QDomDocument doc = loadXML(file_name, doctype_name, sender);
    if(doc.isNull())
        return ret;
    QDomNode n = doc.firstChild().firstChild();
    if(!n.isNull())
        ret = true;
    while(!n.isNull()) {
        e_parent.appendChild(n.cloneNode());
        n = n.nextSibling();
    }

    return ret;
}

void XMLOperator::changeTextInElement(QDomElement& elem, const QString& new_text)
{

    QDomNode e_parent = elem.parentNode();
    QDomDocument doc = elem.ownerDocument();
    QString tag_name = elem.tagName();
    e_parent.replaceChild(xml_operator->createTextElement(doc, tag_name, new_text), elem);

}

const QString XMLOperator::readMultiLineTextElement(const QDomElement &elem)
{
    QString text;
    QStringList lines;
    QDomNodeList n_lines = elem.childNodes();
    int cnt = n_lines.count();
    QDomElement e_line;
    for(int i = 0; i < cnt; ++i) {
        QDomNode n = n_lines.item(i);
        e_line = n.toElement();
        if(!e_line.isNull())
            lines.append(e_line.text());
    }
    text = lines.join("\n");
    return text;
}

QDomDocument XMLOperator::loadXML(QString& file_name, const QString& doctype_name, const QString& sender, QString type_file, QString extension)
{

    bool to_continue = true;
    QDomDocument doc(doctype_name);
    if(type_file.isEmpty())
        type_file = doctype_name;
    if(!needChangeName(file_name)) {
        to_continue = load_xml(file_name, doc, doctype_name, sender);
        if(!doc.isNull())
            to_continue = false;
    }
    while(to_continue) {
        to_continue = getFileDialog(file_name, sender, QFileDialog::AcceptOpen, QFileDialog::ExistingFile, type_file, extension);
        if(to_continue) {
            to_continue = load_xml(file_name, doc, doctype_name, sender);
            if(!doc.isNull())
                to_continue = false;
        }
        else {
            to_continue = false;
            doc.clear();
        }
    }

    return doc;
}

bool XMLOperator::testXML(void)
{

    QString file_xml_name;
    QDomDocument test_doc;
    QFileDialog fd;
    bool ret = false;
    fd.setDirectory(QDir::current());
    fd.setFileMode(QFileDialog::ExistingFile);

    if ( fd.exec() == QDialog::Accepted ) {
        file_xml_name = fd.selectedFiles().at(0);
        test_doc = loadXML(file_xml_name , "", "Test XML");
        ret = !test_doc.isNull();
        if(ret)
            QMessageBox::information(0,tr("Test XML"),
                                     tr("XML file is well structured"), tr("&Ok"));
    }
    return ret;

}

bool XMLOperator::load_xml(QString& file_name,
      QDomDocument& doc, const QString& doctype_name, const QString& sender)
{

  bool to_continue = false;
  QString errorMsg;
  int errorLine = 0;
  int errorCol = 0;
  QFile file(file_name);
  if( file.open(QIODevice::ReadOnly)) {
    QTextStream ts( &file );
    QString xml_text = "";
    xml_text = ts.readAll();
    if(!doc.setContent(xml_text, &errorMsg, &errorLine, &errorCol)) {
      to_continue = bool(QMessageBox::warning(0, sender, errorMsg + "\n" +
                   tr("Line:") + " " + QString::number(errorLine) + " " +
                   tr("Column:") + " " + QString::number(errorCol) + "\n" +
                   tr("Try again ?") ,
                   tr("&No"), tr("&Yes")));
      doc.clear();
      return to_continue;
    }
    QDomDocumentType doc_type = doc.doctype();
    if(doc_type.isNull()) {
      to_continue = bool(QMessageBox::warning(0, sender,
                          tr("File is not in XML format") + "\n" +
                              tr("Try again ?") ,
                              tr("&No"), tr("&Yes")));
      doc.clear();
      return to_continue;
    }
    if(!doctype_name.isEmpty()) {
      if(doc_type.name() != doctype_name) {
        to_continue = bool(QMessageBox::warning(0, sender,
                            tr("XML file format is not") + " " + doctype_name + "\n" +
                   tr("Try again ?") ,
                   tr("&No"), tr("&Yes")));
        doc.clear();
        return to_continue;
      }
    }
    return to_continue;
  }
  else {
    to_continue = noFileDialog(sender);
    doc.clear();
    return to_continue;
  }

}

bool XMLOperator::saveDocument(QString& file_name, QString content,
                               const QString& type_file, const QString& extension,
                               const QString& sender, bool change_name)
{
    QFile file;
    QTextStream ts;
    bool ret = true;
    if(change_name)
        ret = getFileDialog(file_name, sender, QFileDialog::AcceptSave, QFileDialog::AnyFile, type_file, extension);
    if(ret) {
        file.setFileName(file_name);
        if(file.open(QIODevice::WriteOnly))
            ts.setDevice(&file);
        else {
            QMessageBox::critical(0, tr("Save document"),
                tr("Couldn't open file for saving"), tr("&Cancel"));
            ret = false;
        }
    }
    if(ret) {
        ts << content;
        file.close();
    }
    return ret;
}

bool XMLOperator::noFileDialog(const QString& sender)
{

  bool ret = bool(QMessageBox::warning(0, sender, "\t\t" + tr("Cannot open file.") + "   " + tr("Try again ?") + "\t\t" , tr("&Yes"), tr("&No")));
  return !ret;
}

bool XMLOperator::needChangeName(const QString& file_name)
{
    bool ret = file_name.trimmed().isEmpty() || file_name.contains("Untitled") || file_name.contains(tr("Untitled"));
    return ret;
}

bool XMLOperator::getFileDialog(QString& file_name, const QString& sender,
                QFileDialog::AcceptMode acceptMode, QFileDialog::FileMode filemode,
                QString type_file, QString extension, QFileDialog::Options options, QString topDir)
{
    bool ret = false;
    QFileInfo file(file_name);
//     if(extension.isEmpty())
//         extension = file.completeSuffix();
    if(type_file.isEmpty())
        type_file = extension;

    FileDialog fd(0, sender);
    fd.setFileMode(filemode);
    if(options != 0x00000000)
        fd.setOptions(options);
    QString fileDir = "";
    if(file.exists()) {
        if(file.isDir())
            fileDir = file_name;
        else
            fileDir = file.path();
    }
    if(!fileDir.isEmpty())
        fd.setDirectory(fileDir);
    fd.selectFile(file.fileName());
    
    if(!topDir.isEmpty()) {
        fd.setTopDir(topDir);
        fd.setDirectory(topDir);
    }

    fd.setAcceptMode(acceptMode);
    if(!extension.isEmpty())
        fd.setNameFilter(type_file + " (*." + extension + ");;" + tr("All") + " (*.*)");
    if ( fd.exec() == QDialog::Accepted ) {
        file_name = fd.selectedFiles().at(0);
        ret = true;
    }

    return ret;
}

void XMLOperator::initHtml(QString resource_base_html, QString title, bool /*force*/)
{
    QFile file(resource_base_html);
    file.open(QIODevice::ReadOnly);
    QTextStream ts( &file );
    QString start = ts.readAll();
    int pos = start.indexOf("<nav>");
    htmlInit = start.left(pos);
    start.remove(0, pos);
    pos = htmlInit.indexOf("</title>");
    htmlInit.insert(pos, title);   
    pos = start.indexOf("</nav>") + 6;
    QString navText = start.left(pos);
    htmlNav = navText.split("\n");
}

QString XMLOperator::initContentDiv()
{
    return "\n\t\t<div class=""content"">";
}

QString XMLOperator::getHtmlInit() const
{
    return htmlInit;
}

QString XMLOperator::getHtmlNav() const
{
    return htmlNav.join("\n");
}

void XMLOperator::insertReconstructionInNav(const QString &id)
{
    insertItemInNav(id, "#recons");
}

void XMLOperator::insertSegmentationInNav(const QString &id)
{
    insertItemInNav(id, "#segments");
}

QString XMLOperator::insertImage(QString svgContent)
{
    int pos = svgContent.indexOf("<svg");
    svgContent.remove(0, pos);
    svgContent = "\n\t\t<div" + indentations.value(5) + ">\n\t\t" + svgContent + "\n\t\t</div>";
    return svgContent;
}

void XMLOperator::insertItemInNav(const QString & id, const QString &parent)
{
    int index;
    for(index = 0; index < htmlNav.size(); index++) {
        if(htmlNav.at(index).contains(parent))
            break;
    }
    index++;
    for(; index < htmlNav.size(); index++) {
        if(htmlNav.at(index).contains("</ul>"))
            break;
    }
    QString refId = id;
    refId = "#" + refId;
    refId = '"' + refId + '"';
    QString text = "                <li><a href=" + refId + ">" + id + "</a></li>";
    htmlNav.insert(index, text);
}

QString XMLOperator::endHtml()
{
    QString end = "\n\t\t</div>\n\t</body>\n</html>\n";
    return end;
}

QString XMLOperator::toParagraph(const QString &text, int indent, const QString &title)
{
    QString paragrah = text;
    QString htmlTitle = "";
    if(!title.isEmpty())
        htmlTitle = "<b>" + title + ": " + "</b>";
    if(indent < 1)
        indent = 1;
    else if(indent > 6)
        indent = 10;
    paragrah = "\n\t\t<p" + indentations.value(indent) + ">" + htmlTitle + paragrah.replace("\n", "<br>") + "</p>";
    return paragrah;
}

QString XMLOperator::toHeading(const QString &text, int level, QString id)
{
    if(level < 1)
        level = 1;
    else if(level > 6)
        level = 6;
    int indent = level - 1;
    QString idTag = "";
    if(!id.isEmpty()) {
        id = '"' + id + '"';
        idTag = " id=" + id;
    }
    QString heading = "\n\t\t<h" + QString::number(level) + idTag;
    if(!id.isEmpty())
        heading += indentationsAnchors.value(indent) + ">" + text + "</h" +
            QString::number(level) + ">";
    else
        heading += indentations.value(indent) + ">" + text + "</h" +
            QString::number(level) + ">";

    return heading;
}

QString XMLOperator::emptyLine()
{
   return "<br>";
}

QString XMLOperator::addRule()
{
    QString rule = "\n\t\t<hr width=""75%"" align=""left"">";
    return rule;
}

QString XMLOperator::initTable(QStringList headers, int indent)
{
    if(indent < 1)
        indent = 1;
    else if(indent > 6)
        indent = 10;

    QString text = "\n\t\t<table style=""width:100%""" +
            indentations.value(indent) + ">";

    text += "\n\t\t\t<tr>";
    for(int i = 0; i < headers.size(); i++)
        text += "\n\t\t\t\t<th align=""left"">" + headers.at(i) + "</th>";
    text += "\n\t\t\t</tr>";

    return text;
}

QString XMLOperator::tableRow(QStringList data, int /*indent*/)
{
    QString text = "\n\t\t\t<tr>";
    for(int i = 0; i < data.size(); i++)
        text += "\n\t\t\t\t<td>" + data.at(i) + "</td>";
    text += "\n\t\t\t</tr>";

    return text;
}

QString XMLOperator::closeTable()
{
    return "\n\t\t</table>";
}

QString XMLOperator::addList(const QStringList &list, int indent, bool unordered)
{
    if(indent < 1)
        indent = 1;
    else if(indent > 6)
        indent = 10;

    QString text = "";
    if(unordered)
        text += "\n\t\t<ul" + indentations.value(indent) + ">";
    else
        text += "\n\t\t<ol" + indentations.value(indent) + ">";
    for(int i = 0; i < list.size(); i++)
        text += "\n\t\t\t<li>" + list.at(i);

    if(unordered)
        text += "\n\t\t</ul>";
    else
        text += "\n\t\t</ol>";
    return text;
}

QString XMLOperator::superScript(const QString &text)
{
    return "<sup>" + text + "</sup>";
}

QString XMLOperator::wrapWithAnchor(const QString &text)
{
    QString html = "<a href=";
    html += '"';
    html += "#" + text;
    html += '"';
    html += ">" + text + "</a>";

    return html;
}
