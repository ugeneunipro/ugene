/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#ifndef _U2_DOCUMENT_MODEL_TESTS_H_
#define _U2_DOCUMENT_MODEL_TESTS_H_

#include <U2Test/XMLTestUtils.h>

#include <U2Core/IOAdapter.h>

#include <QDomElement>

namespace U2 {

class Document;
class GObject;
class LoadDocumentTask;
class SaveDocumentTask;
class DocumentProviderTask;

class GTest_LoadDocument : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_LoadDocument, "load-document");

    ReportResult report();
    void prepare();

    virtual void cleanup();


private:
    QString             docContextName;
    LoadDocumentTask*   loadTask;
    bool                contextAdded;
    bool                tempFile;
    QString             url;
    GTestLogHelper      logHelper;

    QString expectedLogMessage;
    QString expectedLogMessage2;
    QString unexpectedLogMessage;

    bool                needVerifyLog;
};

class GTest_SaveDocument : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_SaveDocument, "save-document");
    void prepare();

private:
     QString             url;
     DocumentFormatId    formatId;
     IOAdapterFactory*   iof;


    QString             docContextName;
    SaveDocumentTask*   saveTask;
};

class GTest_LoadBrokenDocument : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_LoadBrokenDocument, "load-broken-document", TaskFlags(TaskFlag_NoRun)| TaskFlag_FailOnSubtaskCancel);

    Document* getDocument() const;
    ReportResult report();
    void cleanup();

private:
    LoadDocumentTask*   loadTask;
    QString url;
    bool tempFile;
    QString message;
};

class GTest_ImportDocument : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_ImportDocument, "import-document");

    ReportResult report();
    void prepare();

    virtual void cleanup();


private:
    QString                 docContextName;
    DocumentProviderTask*   importTask;
    bool                    contextAdded;
    bool                    tempFile;
    QString                 url;
    QString                 destUrl;
    GTestLogHelper          logHelper;

    QString expectedLogMessage;
    QString expectedLogMessage2;
    QString unexpectedLogMessage;

    bool                needVerifyLog;
};

class GTest_ImportBrokenDocument : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_ImportBrokenDocument, "import-broken-document", TaskFlags(TaskFlag_NoRun)| TaskFlag_FailOnSubtaskCancel);

    Document* getDocument() const;
    ReportResult report();
    void cleanup();

private:
    DocumentProviderTask*   importTask;
    QString url;
    QString destUrl;
    bool tempFile;
    QString message;
};

class GTest_DocumentNumObjects : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DocumentNumObjects, "check-num-objects");

    ReportResult report();
    QString docContextName;
    int numObjs;
};

class GTest_DocumentFormat : public XmlTest {
    Q_OBJECT
        SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DocumentFormat, "check-document-format");

    ReportResult report();

    QString docUrl;
    QString docFormat;
};

class GTest_DocumentObjectNames : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DocumentObjectNames, "check-document-object-names");

    ReportResult report();

    QString docContextName;
    QStringList names;
};

class GTest_DocumentObjectTypes : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DocumentObjectTypes, "check-document-object-types");

    ReportResult report();

    QString docContextName;
    QList<GObjectType> types;
};

class DocumentModelTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};


class GTest_FindGObjectByName : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_FindGObjectByName, "find-object-by-name");

    ReportResult report();

    void cleanup();

private:
    QString     docContextName;
    QString     objContextName;
    QString     objName;
    GObjectType type;
    GObject*    result;

};

class GTest_CompareFiles : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_CompareFiles, "compare-docs");

    ReportResult report();

private:
    void compareMixed();
    QByteArray getLine(IOAdapter* io);
    IOAdapter* createIoAdapter(const QString& filePath);


    QString doc1Path;
    QString doc2Path;
    bool byLines;
    QStringList commentsStartWith;
    bool line_num_only;
    bool mixed_lines;
    qint64 forceBufferSize;

    int first_n_lines;

};

class GTest_Compare_VCF_Files : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_Compare_VCF_Files, "compare-vcf-docs");

    ReportResult report();

private:
    IOAdapter* createIoAdapter(const QString& filePath);
    QString getLine(IOAdapter* io);

    QString doc1Path;
    QString doc2Path;

    static const QByteArray COMMENT_MARKER;
};

class GTest_Compare_PDF_Files : public XmlTest {
    Q_OBJECT
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_Compare_PDF_Files, "compare-pdf-docs");

    ReportResult report();

private:
    QString doc1Path;
    QString doc2Path;
    bool byLines;

};

}//namespace
#endif
