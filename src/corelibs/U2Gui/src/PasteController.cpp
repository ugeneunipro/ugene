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

#include "PasteController.h"

#include <U2Core/AddDocumentTask.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/GHints.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/DocumentImport.h>
#include <U2Core/GUrl.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/Task.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/StringAdapter.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/OpenViewTask.h>

#include <QApplication>
#include <QFileInfo>
#include <QMimeData>
#include <QUrl>

namespace U2 {
namespace {
QString parseUrl(const QString &url) {
    const QString fileString = "file://";
    if (url.startsWith(fileString)) {
        return url.mid(fileString.length());
    }
    return url;
}

QString joinDirs(const QStringList &dirs, const QString &separator) {
    static const int maxDirsNumber = 4;
    QStringList result = dirs.mid(0, maxDirsNumber);
    if (dirs.size() > maxDirsNumber) {
        result << "...";
    }
    return result.join(separator);
}

QString fetchClipboardText(const QClipboard *clipboard, U2OpStatus &os) {
    QString result;
    try {
        result = clipboard->text();
    } catch (std::bad_alloc) {
        os.setError(PasteFactory::tr("Unable to handle so huge data in clipboard."));
        return result;
    }
    if (result.isEmpty()) {
        os.setError(PasteFactory::tr("UGENE can not recognize current clipboard content as one of supported formats."));
    }
    return result;
}

QString generateClipboardUrl(const QStringList &extensions, const QSet<QString> &excludedFilenames) {
    QString result = AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath() + "/clipboard";
    if (!extensions.isEmpty()) {
        result += "." + extensions.first();
    }
    return GUrlUtils::rollFileName(result, DocumentUtils::getNewDocFileNameExcludesHint().unite(excludedFilenames));
}

void saveFile(const QString& url, const QString& data){
    QFile outputFile(url);
    outputFile.open(QIODevice::WriteOnly);
    outputFile.write(data.toLatin1());
    outputFile.close();
}

}
////////////////////
///PasteTaskImpl
PasteTaskImpl::PasteTaskImpl(bool addToProject)
:PasteTask(), addToProject(addToProject){
}

void PasteTaskImpl::run(){

}

QList<Task*> PasteTaskImpl::onSubTaskFinished(Task* task){
    QList<Task*> res;
    if (task->isCanceled() || task->hasError()){
        return res;
    }
    DocumentProviderTask* loadTask = qobject_cast<DocumentProviderTask*>(task);
    if (loadTask != NULL){
        Document* doc = loadTask->takeDocument();
        processDocument(doc);
        documents.append(doc);
        if(addToProject){
            res << new AddDocumentAndOpenViewTask(doc);
        }
    }
    return res;
}

///////////////////
///PasteFactoryImpl
PasteFactoryImpl::PasteFactoryImpl(QObject *parent)
    :PasteFactory(parent){
    excludedFilenames = QSet<QString>();
}

PasteTask* PasteFactoryImpl::pasteTask(bool addToProject) {
    PasteTask* res = NULL;
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mdata = clipboard->mimeData();
    if (mdata->hasUrls()) {
        res = new PasteUrlsTask(mdata->urls(), addToProject);
    }else{
        res = new PasteTextTask(clipboard, excludedFilenames, addToProject);
    }

    connectExclude(res);

    return res;
}

void PasteFactoryImpl::connectExclude(PasteTask *task){
    TaskSignalMapper* loadTaskSignalMapper = new TaskSignalMapper(task);
    connect(loadTaskSignalMapper, SIGNAL(si_taskFinished(Task *)), SLOT(sl_excludeBack()));
}

void PasteFactoryImpl::sl_excludeBack() {
    TaskSignalMapper* mapper = qobject_cast<TaskSignalMapper*>(sender());
    SAFE_POINT(mapper != NULL, "Incorrect sender", );

    PasteTask *task =  qobject_cast<PasteTask*>(mapper->getTask());
    SAFE_POINT(task != NULL, "Can not convert to LoadDocumentTask", );
    foreach (const GUrl& gurl, task->getUrls()){
        excludedFilenames.remove(gurl.getURLString());
    }
}

///////////////////////
///PasteTextTask
PasteUrlsTask::PasteUrlsTask(const QList<QUrl> &toPasteUrls, bool addToProject)
    :PasteTaskImpl(addToProject){
    QStringList dirs;
    foreach (const QUrl &url, toPasteUrls) {
        QString parsedUrl = parseUrl(url.toLocalFile());
        if (QFileInfo(parsedUrl).isDir()) {
            dirs << parsedUrl;
            continue;
        }
        urls << GUrl(parsedUrl, GUrl_File);
    }

    if (!dirs.isEmpty()) {
        setError(tr("Pasting of folders is not supported:") + "\n" + joinDirs(dirs, "\n"));
        return;
    }
    CHECK(!urls.isEmpty(), );
    foreach (const GUrl& url, urls){
        DocumentProviderTask* loadDocTask = LoadDocumentTask::getCommonLoadDocTask(url);
        if (loadDocTask){
            addSubTask(loadDocTask);
        }
    }
}

///////////////////////
///PasteTextTask
PasteTextTask::PasteTextTask(const QClipboard* clipboard, QSet<QString>& excludedFilenames, bool addToProject)
    :PasteTaskImpl(addToProject){

    QString clipboardText = fetchClipboardText(clipboard, stateInfo);
    CHECK_OP(stateInfo, );

    QScopedPointer<StringAdapterFactoryWithStringData> siof (new StringAdapterFactoryWithStringData(clipboardText));
    QScopedPointer<IOAdapter> ioa (siof->createIOAdapter());
    SAFE_POINT(ioa->isOpen(), L10N::internalError("IOAdapter is not opened"), );

    QList<FormatDetectionResult> dfs = DocumentUtils::detectFormat(ioa.data());
    if( dfs.isEmpty() ) {
        setError(tr("Cannot detect file format"));
        return ;
    }
    DocumentFormat * df = dfs.first().format;

    QString clipboardUrl = generateClipboardUrl(df->getSupportedDocumentFileExtensions(), excludedFilenames);
    excludedFilenames.insert(clipboardUrl);

    urls.append(GUrl(clipboardUrl));

    saveFile(clipboardUrl, clipboardText);

    IOAdapterFactory * iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById( IOAdapterUtils::url2io( clipboardUrl ) );
    CHECK(iof != NULL, );

    QVariantMap hints;
    hints[ProjectLoaderHint_DoNotAddToRecentDocuments] = true;
    hints[DocumentReadingMode_SequenceAsSeparateHint] = true;
    LoadDocumentTask* loadDocumentTask = new LoadDocumentTask(df->getFormatId(), GUrl(clipboardUrl), iof, hints);
    addSubTask(loadDocumentTask);
}

} // U2
