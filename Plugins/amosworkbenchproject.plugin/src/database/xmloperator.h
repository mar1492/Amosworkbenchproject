/***************************************************************************
                          xmloperator.h  -  description
                             -------------------
    begin                : Sat Jun 4 2005
    copyright            : (C) 2005 by Jose M. Cuadra Troncoso
    email                : jmcuadra@dia.uned.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef XMLOPERATOR_H
#define XMLOPERATOR_H


/**
  *@author Jose M. Cuadra Troncoso
  */

#include <QtXml>
#include <QFileDialog>

class XMLOperator {

    Q_DECLARE_TR_FUNCTIONS(XMLOperator)

protected:
    XMLOperator();

public:
    virtual ~XMLOperator();

    static XMLOperator* instance(void);
    virtual QDomElement findTag(const QString& tag_name,
                                const QDomElement& elem,
                                bool direct_child = true);
    virtual QDomElement findTagIdent(const QString& tag_name,
                                     const QDomElement& elem, const QString& tag_id,
                                     bool exact = true);
    virtual QDomElement findTagIdent(const QString& tag_name,
                                     const QDomElement& elem, const int& tag_id,
                                     bool exact = true);
    virtual QDomElement findParentTag(const QString& tag_name,
                                      const QDomElement& elem, const int& tag_id,
                                      bool exact = true);
    virtual QDomElement findParentTag(const QString& tag_name,
                                      const QDomElement& elem,
                                      const QString& tag_id, bool exact = true);

    virtual QDomElement findElementById (const QString& tag_name,
                                         const QDomElement& elem, const QString& id, const QString& id_name = "id");

    virtual void removeChildren(QDomNode& n_parent, int first = 0,
                                int last = -1);
    virtual void deleteElementsByTagName(const QString& tagname,
                                         QDomElement& e);
    virtual void deleteParentsByTagIdent(const QString& tagname,
                                         QDomElement& e, const QString& ident);
    virtual void deleteParentsByTagIdent(const QString& tagname,
                                         QDomElement& e, const int& ident);
    virtual void insertChildByTagName(const QString& tagname,
                                      QDomElement& e_parent, QDomElement& e,
                                      bool tagname_no_child = true);
    virtual QDomElement createTextElement(QDomDocument& doc,
                                          const QString& tag_name, const QString& tag_text);
    virtual QDomElement createMultiLineTextElement(QDomDocument& doc,
                                          const QString& tag_name, const QString& tag_text);
    virtual bool insertDocContent(QDomElement& e_parent,
                                  QString& file_name, const QString& doc_name,
                                  const QString& sender);
    virtual void changeTextInElement(QDomElement& elem, const QString& new_text);

    const QString readMultiLineTextElement(const QDomElement& elem);

    QDomDocument loadXML(QString& file_name, const QString& doc_name, const QString& sender, QString type_file = "", QString extension = "");
    bool saveDocument(QString& file_name, QString content, const QString& type_file,
                      const QString& extension, const QString& sender,
                      bool change_name = false);
    bool testXML(void);
    bool needChangeName(const QString& file_name);
    bool getFileDialog(QString& file_name, const QString& sender,
                       QFileDialog::AcceptMode acceptMode, QFileDialog::FileMode filemode = QFileDialog::ExistingFile,
                       QString type_file = "", QString extension = "",
                       QFileDialog::Options options = QFlags<QFileDialog::Option>(), QString topDir = "");

    void initHtml(QString resource_base_html, QString title, bool force = false);
    QString initContentDiv();
    QString endHtml();
    QString toParagraph(const QString &text, int indent = 10, const QString &title = "");
    QString toHeading(const QString &text, int level, QString id ="");
    QString emptyLine();
    QString addRule();
    QString initTable(QStringList headers, int indent);
    QString tableRow(QStringList data, int indent);
    QString closeTable();
    QString addList(const QStringList &list, int indent, bool unordered = true);
    QString superScript(const QString& text);
    QString wrapWithAnchor(const QString& text);

    QString getHtmlInit() const;
    QString getHtmlNav() const;
    void insertReconstructionInNav(const QString& id);
    void insertSegmentationInNav(const QString& id);

    QString insertImage(QString svgContent);

protected:
    bool load_xml(QString& file_name, QDomDocument& doc, const QString& doc_name, const QString& sender);
    bool noFileDialog(const QString& sender);
    void insertItemInNav(const QString &id, const QString & parent);

protected:
    static XMLOperator* xml_operator;
    QHash<int, QString> indentations;
    QHash<int, QString> indentationsAnchors;
    QString htmlInit;
    QStringList htmlNav;
};

#endif
