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

#include <QApplication>
#include <QClipboard>
#include <QMimeData>

#include <U2Algorithm/MsaColorScheme.h>
#include <U2Algorithm/MsaHighlightingScheme.h>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GHints.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/LocalFileAdapter.h>
#include <U2Core/Log.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/Settings.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include "SubalignmentToClipboardTask.h"
#include "ov_msa/MaCollapseModel.h"
#include "ov_msa/MSAEditorSequenceArea.h"

namespace U2{


////////////////////////////////////////////////////////////////////////////////
PrepareMsaClipboardDataTask::PrepareMsaClipboardDataTask(const U2Region &window, const QStringList &names)
: Task (tr("Copy formatted alignment to the clipboard"), TaskFlags_FOSE_COSC), window(window), names(names)
{
}

QString PrepareMsaClipboardDataTask::getResult() const {
    return result;
}

PrepareMsaClipboardDataTask * MsaClipboardDataTaskFactory::getInstance(MSAEditor *context, const QRect &selection, const DocumentFormatId &formatId) {
    U2Region window = getWindowBySelection(selection);
    QStringList names = getNamesBySelection(context, selection);
    if ("RTF" == formatId) {
        return new RichTextMsaClipboardTask(context, window, names);
    } else {
        return new FormatsMsaClipboardTask(context->getMaObject(), window, names, formatId);
    }
}

U2Region MsaClipboardDataTaskFactory::getWindowBySelection(const QRect &selection){
    return U2Region (selection.x(), selection.width());;
}

QStringList MsaClipboardDataTaskFactory::getNamesBySelection(MaEditor *context, const QRect &selection){
    MaCollapseModel* m = context->getUI()->getCollapseModel();
    int startMaRowIndex = m->getMaRowIndexByViewRowIndex(selection.y());
    int endMaRowIndex = m->getMaRowIndexByViewRowIndex(selection.y() + selection.height() - 1);
    const MultipleAlignment &ma = context->getMaObject()->getMultipleAlignment();
    QStringList names;
    for (int maRowIndex = startMaRowIndex; maRowIndex <= endMaRowIndex; ++maRowIndex) {
        if (m->getViewRowIndexByMaRowIndex(maRowIndex, true) >= 0) {
            names.append(ma->getRow(maRowIndex)->getName());
        }
    }
    return names;
}

FormatsMsaClipboardTask::FormatsMsaClipboardTask(MultipleSequenceAlignmentObject *msaObj, const U2Region &window, const QStringList &names, const DocumentFormatId &formatId)
    :PrepareMsaClipboardDataTask(window, names), createSubalignmentTask(NULL), msaObj(msaObj), formatId(formatId){

}

void FormatsMsaClipboardTask::prepare(){
    CreateSubalignmentSettings settings = defineSettings(names, window, formatId, stateInfo);
    CHECK_OP(stateInfo, )

    createSubalignmentTask = new CreateSubalignmentTask(msaObj, settings);
    addSubTask(createSubalignmentTask);
}

void FormatsMsaClipboardTask::run(){
}

#define READ_BUF_SIZE 4096
QList<Task*> FormatsMsaClipboardTask::onSubTaskFinished(Task *subTask) {
    QList<Task*> res;
    QList<Task*> subTasks;
    if (subTask->hasError() || isCanceled()) {
        return subTasks;
    }

    if(subTask == createSubalignmentTask){
        Document* doc = createSubalignmentTask->getDocument();
        SAFE_POINT_EXT(doc != NULL, setError(tr("No temporary document.")), subTasks);
        QScopedPointer<LocalFileAdapterFactory> factory( new LocalFileAdapterFactory());
        QScopedPointer<IOAdapter> io(factory->createIOAdapter());
        if(!io->open(doc->getURL(), IOAdapterMode_Read)){
            setError(tr("Cannot read the temporary file."));
            return subTasks;
        }

        QByteArray buf;
        while(!io->isEof()){
            buf.resize(READ_BUF_SIZE);
            buf.fill(0);
            bool terminatorFound = false;
            int read = io->readLine(buf.data(), READ_BUF_SIZE, &terminatorFound);
            buf.resize(read);
            result.append(buf);
            if (terminatorFound){
                result.append('\n');
            }
        }
    }
    return res;
}

CreateSubalignmentSettings FormatsMsaClipboardTask::defineSettings(const QStringList& names, const U2Region &window, const DocumentFormatId &formatId, U2OpStatus& os){
    //Create temporal document for the workflow run task
    const AppSettings* appSettings = AppContext::getAppSettings();
    SAFE_POINT_EXT(NULL != appSettings, os.setError(tr("Invalid applications settings detected")), CreateSubalignmentSettings());

    UserAppsSettings* usersSettings = appSettings->getUserAppsSettings();
    SAFE_POINT_EXT(NULL != usersSettings, os.setError(tr("Invalid users applications settings detected")), CreateSubalignmentSettings());
    const QString tmpDirPath = usersSettings->getCurrentProcessTemporaryDirPath();
    GUrl path = GUrlUtils::prepareTmpFileLocation(tmpDirPath, "clipboard", "tmp", os);

    return CreateSubalignmentSettings(window, names, path, true, false, formatId);
}

RichTextMsaClipboardTask::RichTextMsaClipboardTask(MaEditor *context, const U2Region &window, const QStringList &names)
    :PrepareMsaClipboardDataTask(window, names), context(context){

}

void RichTextMsaClipboardTask::run(){
    MultipleAlignmentObject* obj = context->getMaObject();
    const DNAAlphabet* al = obj->getAlphabet();
    if (!al){
        return;
    }
    Settings* s = AppContext::getSettings();
    SAFE_POINT(NULL != s, "RTFMSA entry storing: NULL settings object", );

    DNAAlphabetType atype = al->getType();
    MsaColorSchemeRegistry* csr = AppContext::getMsaColorSchemeRegistry();
        QString csid = atype == DNAAlphabet_AMINO ?
            s->getValue(MSAE_SETTINGS_ROOT + MOBJECT_SETTINGS_COLOR_AMINO, MsaColorScheme::UGENE_AMINO).toString()
          : s->getValue(MSAE_SETTINGS_ROOT + MOBJECT_SETTINGS_COLOR_NUCL, MsaColorScheme::UGENE_NUCL).toString();

    MsaColorSchemeFactory* csf = csr->getSchemeFactoryById(csid);
    if (csf == NULL) {
        csf = csr->getSchemeFactoryById(atype == DNAAlphabet_AMINO ? MsaColorScheme::UGENE_AMINO : MsaColorScheme::UGENE_NUCL);
    }
    SAFE_POINT(csf!=NULL, "RTFMSA entry storing: NULL MsaColorSchemeFactory object", );
    MsaColorScheme* colorScheme = csf->create(this, obj);

    QString fontFamily = s->getValue(MSAE_SETTINGS_ROOT + MOBJECT_SETTINGS_FONT_FAMILY, MOBJECT_DEFAULT_FONT_FAMILY).toString();
    int pointSize = s->getValue(MSAE_SETTINGS_ROOT + MOBJECT_SETTINGS_FONT_SIZE, MOBJECT_DEFAULT_FONT_SIZE).toInt();

    MsaHighlightingScheme* highlightingScheme = context->getUI()->getSequenceArea()->getCurrentHighlightingScheme();
    SAFE_POINT(highlightingScheme!=NULL, "RTFMSA entry storing: NULL highlightingScheme object", );

    QString schemeName = highlightingScheme->metaObject()->className();
    bool isGapsScheme = schemeName == "U2::MSAHighlightingSchemeGaps";

    const MultipleAlignment msa = obj->getMultipleAlignment();

    U2OpStatusImpl os;
    const int refSeq = msa->getRowIndexByRowId(context->getReferenceRowId(), os);

    result.append(QString("<span style=\"font-size:%1pt; font-family:%2;\">\n").arg(pointSize).arg(fontFamily).toLatin1());
    int numRows = msa->getNumRows();
    for (int seq = 0; seq < numRows; seq++){
        QString res;
        const MultipleAlignmentRow row = msa->getRow(seq);
        if (!names.contains(row->getName())){
            continue;
        }

        result.append("<p>");
        for (int pos = window.startPos; pos < window.endPos(); pos++){
            char c = row->charAt(pos);
            bool highlight = false;
            QColor color = colorScheme->getBackgroundColor(seq, pos, c);
            if (isGapsScheme || highlightingScheme->getFactory()->isRefFree()) { //schemes which applied without reference
                const char refChar = '\n';
                highlightingScheme->process(refChar, c, color, highlight, pos, seq);
            } else if (seq == refSeq || U2MsaRow::INVALID_ROW_ID == refSeq) {
                highlight = true;
            } else {
                SAFE_POINT_EXT(NULL != row, setError("MSA row is NULL"), );
                const char refChar = row->charAt(pos);
                highlightingScheme->process(refChar, c, color, highlight, pos, seq);
            }

            if (color.isValid() && highlight) {
                res.append(QString("<span style=\"background-color:%1;\">%2</span>").arg(color.name()).arg(c));
            } else {
                res.append(QString("%1").arg(c));
            }
        }

        result.append(res.toLatin1());
        result.append("</p>\n");
    }
    result.append("</span>");

    delete colorScheme;
}

SubalignmentToClipboardTask::SubalignmentToClipboardTask(MSAEditor *context, const QRect &selection, const DocumentFormatId &formatId)
: Task(tr("Copy formatted alignment to the clipboard"), TaskFlags_NR_FOSE_COSC), formatId(formatId)
{
    prepareDataTask = MsaClipboardDataTaskFactory::getInstance(context, selection, formatId);
    addSubTask(prepareDataTask);
}

QList<Task*> SubalignmentToClipboardTask::onSubTaskFinished(Task *subTask) {
    QList<Task*> result;
    CHECK(subTask == prepareDataTask, result);
    CHECK(!prepareDataTask->getStateInfo().isCoR(), result);
    QString clipboardData = prepareDataTask->getResult();
    if ("RTF" == formatId) {
        QMimeData *reportRichTextMime = new QMimeData();
        reportRichTextMime->setHtml(clipboardData);
        reportRichTextMime->setText(clipboardData);
        QApplication::clipboard()->setMimeData(reportRichTextMime);
    } else {
        QApplication::clipboard()->setText(clipboardData);
    }
    return result;
}
}
