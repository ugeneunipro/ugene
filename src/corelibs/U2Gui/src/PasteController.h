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

#ifndef _U2_PASTE_CONTROLLER_H_
#define _U2_PASTE_CONTROLLER_H_

#include <U2Core/ClipboardController.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/Task.h>
#include <U2Core/U2OpStatus.h>

namespace U2 {

class U2GUI_EXPORT PasteTaskImpl : public PasteTask {
    Q_OBJECT
public:
    PasteTaskImpl(bool addToProject);

    QList<GUrl> getUrls() const {
        return urls;
    }
    QList<Document *> getDocuments() const {
        return documents;
    }

protected:
    QList<GUrl> urls;
    bool addToProject;
    QList<Document *> documents;

protected:
    QList<Task *> onSubTaskFinished(Task *task);
};

class U2GUI_EXPORT PasteFactoryImpl : public PasteFactory {
    Q_OBJECT
public:
    PasteFactoryImpl(QObject *parent = nullptr);

    PasteTask *createPasteTask(bool isAddToProject) override;
};

class PasteUrlsTask : public PasteTaskImpl {
    Q_OBJECT
public:
    PasteUrlsTask(const QList<QUrl> &urls, bool isAddToProject);
};

class PasteTextTask : public PasteTaskImpl {
    Q_OBJECT
public:
    PasteTextTask(const QString &clipboardText, bool isAddToProject);
    Task::ReportResult report() override;

private:
    QString clipboardUrl;
};

}    // namespace U2

#endif    // _U2_PASTE_CONTROLLER_H_
