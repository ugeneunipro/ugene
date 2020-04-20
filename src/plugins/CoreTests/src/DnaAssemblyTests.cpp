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

#include <QDir>
#include <QDomElement>

#include <U2Algorithm/DnaAssemblyMultiTask.h>
#include <U2Algorithm/DnaAssemblyTask.h>

#include <U2Core/AppContext.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>

#include <U2Formats/BAMUtils.h>
#include <U2Formats/SAMFormat.h>

#include <U2View/DnaAssemblyUtils.h>

#include "DnaAssemblyTests.h"

namespace U2 {

#define FILE1_ATTR "file1"
#define FILE2_ATTR "file2"
#define REF_SEQ_ATTR "ref-seq"
#define INDEX_FILE_ATTR "index-file"
#define METHOD_NAME_ATTR "assembly-method"
#define SHORT_READS_ATTR "shortreads"
#define RES_OBJ_NAME "res-index"
#define RES_FILE_NAME "res-file"
#define CUSTOM_ATTR "custom-options"
#define PAIRED_READS_ATTR "paired-reads"
#define IS_BAM_ATTR "isbam"

void GTest_DnaAssemblyToReferenceTask::init(XMLTestFormat*, const QDomElement& el)  {

    refSeqUrl = el.attribute(REF_SEQ_ATTR);
    if (refSeqUrl.isEmpty()) {
        failMissingValue(REF_SEQ_ATTR);
        return;
    }
    refSeqUrl.prepend(env->getVar("COMMON_DATA_DIR") + "/");

    indexFileName = el.attribute(INDEX_FILE_ATTR);
    if (!indexFileName.isEmpty()) {
        indexFileName.prepend(env->getVar("COMMON_DATA_DIR") + "/");
    }

    algName = el.attribute(METHOD_NAME_ATTR);
    if (algName.isEmpty()) {
        failMissingValue(METHOD_NAME_ATTR);
        return;
    }

    QString shortReads = el.attribute(SHORT_READS_ATTR);
    if (shortReads.isEmpty()) {
        failMissingValue(SHORT_READS_ATTR);
        return;
    }

    objName = el.attribute(RES_OBJ_NAME);
    QString pr = el.attribute(PAIRED_READS_ATTR);
    pairedReads = !pr.isEmpty();

    QString resName = el.attribute(RES_FILE_NAME);
    if (!resName.isEmpty()) {
        resName.prepend(env->getVar("TEMP_DATA_DIR") + "/");
        resultFileName = resName;
    }
    resultFileName = resName;

    QStringList shortReadList = shortReads.split(";");
    if (shortReadList.isEmpty()) {
        setError("No short reads urls are found in test");
        return;
    }


   QString buf = el.attribute(CUSTOM_ATTR);
   QStringList customAttrs = buf.split(",");
   foreach (const QString& attr, customAttrs) {
       QStringList keyValPair = attr.split('=');
       if (keyValPair.size() == 2) {
           QString optName = keyValPair[0];
           QString optVal = keyValPair[1];
           customOptions.insert(optName, optVal);
       }
   }

    foreach (const QString& url, shortReadList) {
        shortReadUrls.append(GUrl(env->getVar("COMMON_DATA_DIR") + "/" + url));
    }
    loadResultTask = NULL;
    assemblyMultiTask = NULL;
}

void GTest_DnaAssemblyToReferenceTask::prepare()
{
    QString dir(env->getVar("TEMP_DATA_DIR"));
    if (!QDir(dir).exists()) {
        bool ok = QDir::root().mkpath(dir);
        if (!ok) {
            return;
        }
    }
    QString id = QString::number(rand());
    if (resultFileName.isEmpty()) {
        resultFileName = GUrlUtils::rollFileName(dir+"/"+GUrl(refSeqUrl).baseFileName() + "_" + id + "_aligned.sam",DocumentUtils::getNewDocFileNameExcludesHint());
    }
    DnaAssemblyToRefTaskSettings settings;
    if (indexFileName.isEmpty()) {
        indexFileName = GUrlUtils::rollFileName(dir+"/"+GUrl(refSeqUrl).baseFileName() + "_index_" + id,DocumentUtils::getNewDocFileNameExcludesHint());
        settings.prebuiltIndex = false;
    } else {
        settings.prebuiltIndex = true;
    }

    settings.openView = false;
    settings.algName = algName;
    settings.refSeqUrl = refSeqUrl;
    settings.indexFileName = indexFileName;
    settings.resultFileName = resultFileName;
    settings.pairedReads = pairedReads;

    foreach (const GUrl& url, shortReadUrls) {
        settings.shortReadSets.append( url );
    }
    foreach (const QString& optionName, customOptions.keys()) {
        settings.setCustomValue(optionName, customOptions.value(optionName));
    }

    assemblyMultiTask = new DnaAssemblyMultiTask( settings,  false);
    addSubTask(assemblyMultiTask);
}

Task::ReportResult GTest_DnaAssemblyToReferenceTask::report() {
    if (hasError() || isCanceled()) {
        return ReportResult_Finished;
    }

    return ReportResult_Finished;
}

void GTest_DnaAssemblyToReferenceTask::cleanup()
{
    // cleanup temporary files

    if (!XMLTestUtils::parentTasksHaveError(this)) {
        QDir dir(env->getVar("TEMP_DATA_DIR"));
        QStringList tmpFiles = dir.entryList(QStringList() << "*.sarr" << "*.idx" << "*.ref", QDir::Files);

        foreach (const QString& f, tmpFiles) {
            QFile::remove(dir.absoluteFilePath(f));
        }
    }

     XmlTest::cleanup();
}

//----------------------------------------------------------
void GTest_AssemblycompareTwoSAMbyLength::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    file1Url = el.attribute(FILE1_ATTR);
    if (file1Url.isEmpty()) {
        failMissingValue(FILE1_ATTR);
        return;
    }
    file1Url = env->getVar("TEMP_DATA_DIR")+"/" + file1Url;

    file2Url = el.attribute(FILE2_ATTR);
    if (file2Url.isEmpty()) {
        failMissingValue(FILE2_ATTR);
        return;
    }
    file2Url = env->getVar("COMMON_DATA_DIR")+"/" + file2Url;

    QString isBamAtr = el.attribute(IS_BAM_ATTR);
    if (!isBamAtr.isEmpty()) {
        isBam = true;
    }else{
        isBam = false;
    }
}

Task::ReportResult GTest_AssemblycompareTwoSAMbyLength::report() {
    BAMUtils::isEqualByLength(file1Url, file2Url, stateInfo, isBam);

    return ReportResult_Finished;
}


///////////////////////////////////////////////////////////////////////////////////////////

QList<XMLTestFactory*> DnaAssemblyTests::createTestFactories()
{
    QList<XMLTestFactory*> res;
    res.append(GTest_DnaAssemblyToReferenceTask::createFactory());
    res.append(GTest_AssemblycompareTwoSAMbyLength::createFactory());

    return res;
}

} // U2
