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

#include <QApplication>
#include <QClipboard>
#include <QDir>
#include <QFileInfo>
#include <QMimeData>
#include <QUrl>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GHints.h>
#include <U2Core/GUrl.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/StringAdapter.h>
#include <U2Core/Task.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/OpenViewTask.h>

namespace U2 {

static QString parseUrl(const QString &url) {
    const QString fileString = "file://";
    return url.startsWith(fileString) ? url.mid(fileString.length()) : url;
}

static QString joinDirs(const QStringList &dirs, const QString &separator) {
    static const int maxDirsNumber = 4;
    QStringList result = dirs.mid(0, maxDirsNumber);
    if (dirs.size() > maxDirsNumber) {
        result << "...";
    }
    return result.join(separator);
}

static QSet<QString> pasteTaskExcludeUrlSet;

static QString generateClipboardUrl(const QStringList &extensions) {
    QString result = AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath() + "/clipboard";
    if (!extensions.isEmpty()) {
        result += "." + extensions.first();
    }
    return GUrlUtils::rollFileName(result, "_", DocumentUtils::getNewDocFileNameExcludesHint().unite(pasteTaskExcludeUrlSet));
}

////////////////////
///PasteTaskImpl
PasteTaskImpl::PasteTaskImpl(bool addToProject)
    : PasteTask(TaskFlags_NR_FOSCOE), addToProject(addToProject) {
}

QList<Task *> PasteTaskImpl::onSubTaskFinished(Task *task) {
    QList<Task *> res;
    if (task->isCanceled() || task->hasError()) {
        return res;
    }
    DocumentProviderTask *loadTask = qobject_cast<DocumentProviderTask *>(task);
    if (loadTask != nullptr) {
        Document *doc = loadTask->takeDocument();
        processDocument(doc);
        documents.append(doc);
        if (addToProject) {
            res << new AddDocumentAndOpenViewTask(doc);
        }
    }
    return res;
}

///////////////////
///PasteFactoryImpl
PasteFactoryImpl::PasteFactoryImpl(QObject *parent)
    : PasteFactory(parent) {
}

PasteTask *PasteFactoryImpl::createPasteTask(bool isAddToProject) {
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    if (mimeData->hasUrls()) {
        return new PasteUrlsTask(mimeData->urls(), isAddToProject);
    }
    QString clipboardText;
    try {
        clipboardText = clipboard->text();
    } catch (...) {
        coreLog.error(PasteFactory::tr("Data in clipboard is too large."));
        return nullptr;
    }
    if (clipboardText.isEmpty()) {
        coreLog.error("UGENE can not recognize current clipboard content as one of the supported formats.");
        return nullptr;
    }
    return new PasteTextTask(clipboardText, isAddToProject);
}

///////////////////////
///PasteTextTask
PasteUrlsTask::PasteUrlsTask(const QList<QUrl> &toPasteUrls, bool isAddToProject)
    : PasteTaskImpl(isAddToProject) {
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
    foreach (const GUrl &url, urls) {
        DocumentProviderTask *loadDocTask = LoadDocumentTask::getCommonLoadDocTask(url);
        if (loadDocTask) {
            addSubTask(loadDocTask);
        }
    }
}

///////////////////////
///PasteTextTask
PasteTextTask::PasteTextTask(const QString &clipboardText, bool isAddToProject)
    : PasteTaskImpl(isAddToProject) {
    StringAdapterFactoryWithStringData ioAdapterFactory(clipboardText);
    QScopedPointer<IOAdapter> ioAdapter(ioAdapterFactory.createIOAdapter());
    SAFE_POINT(ioAdapter->isOpen(), L10N::internalError("IOAdapter is not opened"), );

    QList<FormatDetectionResult> documentFormatList = DocumentUtils::detectFormat(ioAdapter.data());
    if (documentFormatList.isEmpty()) {
        setError(tr("Failed to detect pasted data format."));
        return;
    }
    DocumentFormat *df = documentFormatList.first().format;

    clipboardUrl = generateClipboardUrl(df->getSupportedDocumentFileExtensions());
    pasteTaskExcludeUrlSet << clipboardUrl;
    if (!IOAdapterUtils::writeTextFile(clipboardUrl, clipboardText)) {
        setError(tr("Failed to create tmp file for clipboard content: %1.").arg(clipboardUrl));
        return;
    }

    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(clipboardUrl));
    CHECK(iof != nullptr, );

    if (isAddToProject) {
        auto openWithProjectTask = AppContext::getProjectLoader()->openWithProjectTask(clipboardUrl);
        if (openWithProjectTask != nullptr) {
            addSubTask(openWithProjectTask);
        }
    } else {
        QVariantMap hints;
        hints[DocumentReadingMode_SequenceAsSeparateHint] = true;
        auto loadDocumentTask = new LoadDocumentTask(df->getFormatId(), GUrl(clipboardUrl), iof, hints);
        addSubTask(loadDocumentTask);
    }
}

Task::ReportResult PasteTextTask::report() {
    pasteTaskExcludeUrlSet.remove(clipboardUrl);
    return PasteTaskImpl::report();
}

}    // namespace U2
