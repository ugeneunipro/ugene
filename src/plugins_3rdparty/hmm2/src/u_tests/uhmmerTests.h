#ifndef _U2_UHMMER_TESTS_H_
#define _U2_UHMMER_TESTS_H_

#include <QDomElement>

#include <U2Core/GObject.h>

#include <U2Test/XMLTestUtils.h>

namespace U2 {

class Document;
class GTest_LoadDocument;
class HMMCalibrateToFileTask;
class HMMBuildToFileTask;
class CreateAnnotationModel;
struct plan7_s;

class GTest_uHMMERSearch : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_uHMMERSearch, "uhmmer-search");

    ~GTest_uHMMERSearch();
    void cleanup() override;
    ReportResult report() override;
    void prepare() override;

private:
    Task* searchTask = nullptr;
    Task* saveTask = nullptr;
    QString hmmFileName;
    QString seqDocCtxName;
    QString resultDocName;
    QString resultDocContextName;
    bool expertOptions = false;
    int number_of_seq = 0;
    int hmmSearchChunk = 0;
    bool customHmmSearchChunk = false;
    bool parallel_flag = false;
    float evalueCutoff = 0;
    float domEvalueCutoff = 0;
    float minScoreCutoff = 0;
    Document* aDoc = nullptr;

protected:
    QList<Task*> onSubTaskFinished(Task* subTask) override;
};

class GTest_uHMMERBuild : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_uHMMERBuild, "uhmmer-build");
    ReportResult report() override;
    void cleanup() override;

private:
    QString outFile;
    HMMBuildToFileTask* buildTask = nullptr;
    bool deleteTempFile = false;
};

class GTest_hmmCompare : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_hmmCompare, "hmm-compare");
    ReportResult report() override;

private:
    QString file1Name;
    QString file2Name;
};

class GTest_uHMMERCalibrate : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_uHMMERCalibrate, "uhmmer-calibrate");
    ReportResult report() override;
    void cleanup() override;

private:
    class GTest_uHMMERCalibrateSubtask : public Task {
    public:
        GTest_uHMMERCalibrateSubtask(HMMCalibrateToFileTask** calibrateTask, int n);
    };
    HMMCalibrateToFileTask** calibrateTask = nullptr;
    float mu = 0;
    float lambda = 0;
    int nCalibrates = 0;
};

class UHMMERTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};

}  // namespace U2
#endif
