/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "URLLineEdit.h"

#include <QFocusEvent>
#include <QMainWindow>
#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Gui/MainWindow.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/SuggestCompleter.h>
#include <U2Gui/U2FileDialog.h>

#include "PropertyWidget.h"
#include "WorkflowGUIUtils.h"

namespace U2 {

class FilenameCompletionFiller : public CompletionFiller {
public:
    FilenameCompletionFiller(URLWidget* _widget)
        : CompletionFiller(), widget(_widget) {
    }

    QStringList getSuggestions(const QString& str) override {
        QString fileName = str;
        if (fileName.endsWith(".")) {
            fileName = fileName.left(fileName.size() - 1);
        }

        QStringList choices;
        const QFileInfo f(fileName);
        const QString completeFileName = f.fileName();
        choices << completeFileName;

        const QStringList presetExtensions = DelegateTags::getStringList(widget->tags(), "extensions");
        if (presetExtensions.isEmpty()) {
            bool ok = fillChoicesWithFormatExtensions(fileName, choices);
            CHECK(ok, QStringList());
        } else {
            fillChoicesWithPresetExtensions(fileName, presetExtensions, choices);
        }

        return choices;
    }

    bool fillChoicesWithFormatExtensions(const QString& fileName, QStringList& choices) {
        const QFileInfo f(fileName);
        const QString curExt = f.suffix();
        const QString baseName = f.completeBaseName();
        const QString completeFileName = f.fileName();

        const QString fileFormat = DelegateTags::getString(widget->tags(), "format");
        DocumentFormat* format = AppContext::getDocumentFormatRegistry()->getFormatById(fileFormat);
        CHECK(format != nullptr, false);

        QStringList formats = format->getSupportedDocumentFileExtensions();
        CHECK(!formats.empty(), false);
        formats.append("gz");

        foreach (const QString& ext, formats) {
            if (!curExt.isEmpty()) {
                if (ext.startsWith(curExt, Qt::CaseInsensitive)) {
                    choices << baseName + "." + ext;
                    if (ext != "gz") {
                        choices << baseName + "." + ext + ".gz";
                    }
                }
            }
        }

        if (choices.size() == 1) {
            foreach (const QString& ext, formats) {
                choices << completeFileName + "." + ext;
                if (ext != "gz") {
                    choices << completeFileName + "." + ext + ".gz";
                }
            }
        }
        return true;
    }

    static void fillChoicesWithPresetExtensions(const QString& fileName, const QStringList& presetExtensions, QStringList& choices) {
        const QFileInfo f(fileName);
        const QString baseName = f.completeBaseName();

        foreach (const QString& extension, presetExtensions) {
            choices << baseName + "." + extension;
        }
    }

    QString handleNewUrlInput(const QString& editorText, const QString& suggestion) override {
        QString path = editorText;
        path.replace("\\", "/");

        int slashPos = path.lastIndexOf("/");
        QString dirPath = path.left(slashPos + 1);

        return dirPath + suggestion;
    }

private:
    URLWidget* widget;
};

URLLineEdit::URLLineEdit(const QString& type,
                         bool multi,
                         bool isPath,
                         bool saveFile,
                         URLWidget* _parent)
    : QLineEdit(_parent),
      schemaConfig(nullptr),
      type(type),
      multi(multi),
      isPath(isPath),
      saveFile(saveFile),
      parent(_parent) {
    if (saveFile && parent != nullptr) {
        new BaseCompleter(new FilenameCompletionFiller(parent), this);
    }
    setPlaceholderText(DelegateTags::getString(parent->tags(), DelegateTags::PLACEHOLDER_TEXT));
    connect(this, &QLineEdit::editingFinished, this, &URLLineEdit::sl_editingFinished);
}

CompletionFiller* URLLineEdit::getCompletionFillerInstance() const {
    if (saveFile && parent != nullptr) {
        return new FilenameCompletionFiller(parent);
    }
    return nullptr;
}

void URLLineEdit::sl_onBrowse() {
    this->browse(false);
}

void URLLineEdit::sl_onBrowseWithAdding() {
    this->browse(true);
}

void URLLineEdit::sl_editingFinished() {
    CHECK(!saveFile, );
    auto tearDown = [&]() {
        disconnect(this);
        setText("");
    };
    const QStringList urlsList = text().split(';');
    for (const QString& url : qAsConst(urlsList)) {
        CHECK_CONTINUE(!url.isEmpty())
        QFileInfo fi(url);
        if (!fi.exists()) {
            tearDown();
            QString message = URLLineEdit::tr("File %1 not exists or it path/name contains ';' symbol.\r\n"
                                  "That kind of file path/name can't be correctly handled by this element.\r\n"
                                  "Please rename the file or move it to directory which not contain ';' in it path.").arg(url);
            QMessageBox::critical(qobject_cast<QWidget*>(AppContext::getMainWindow()->getQMainWindow()),
                                  L10N::errorTitle(),
                                  message);
            return;
        }
        QFile testReadAccess(url);
        if (!testReadAccess.open(QIODevice::ReadOnly)) {
            tearDown();
            QMessageBox::critical(qobject_cast<QWidget*>(AppContext::getMainWindow()->getQMainWindow()), 
                                  L10N::errorTitle(), L10N::errorOpeningFileRead(url));
            return;
        }
    }
}

void URLLineEdit::browse(bool addFiles) {
    QString FileFilter;
    if (parent != nullptr) {
        FileFilter = DelegateTags::getString(parent->tags(), DelegateTags::FILTER);
    }
    LastUsedDirHelper lod(type);
    QString lastDir = lod.dir;
    if (!text().isEmpty()) {
        QString curPath(text());
        int slashPos = curPath.lastIndexOf("/");
        slashPos = qMax(slashPos, curPath.lastIndexOf("\\"));
        if (slashPos >= 0) {
            QDir dir(curPath.left(slashPos + 1));
            if (dir.exists()) {
                lastDir = dir.absolutePath();
            }
        }
    }

    QString name;
    bool filePathIsOk = true;
    if (isPath || multi) {
        QStringList lst;
        if (isPath) {
            QString dir = U2FileDialog::getExistingDirectory(nullptr, tr("Select a folder"), lastDir, QFileDialog::Options());
            lst << dir;
        } else {
            lst = U2FileDialog::getOpenFileNames(nullptr, tr("Select file(s)"), lastDir, FileFilter);
        }
        for (const QString& nameStr : qAsConst(lst)) {
            filePathIsOk = checkNameNoSemicolon(nameStr);
            CHECK_BREAK(filePathIsOk);
        }
        if (addFiles) {
            name = this->text();
            if (!lst.isEmpty()) {
                name += ";";
            }
        }
        name += lst.join(";");
        if (!lst.isEmpty()) {
            lod.url = lst.first();
        }
    } else {
        if (saveFile) {
            lod.url = name = U2FileDialog::getSaveFileName(nullptr, tr("Select a file"), lastDir, FileFilter, nullptr, QFileDialog::DontConfirmOverwrite);
            this->checkExtension(name);
        } else {
            lod.url = name = U2FileDialog::getOpenFileName(nullptr, tr("Select a file"), lastDir, FileFilter);
            filePathIsOk = checkNameNoSemicolon(name);
        }
    }
    if (!name.isEmpty() && filePathIsOk) {
        if (name.length() > this->maxLength()) {
            this->setMaxLength(name.length() + this->maxLength());
        }
        setText(name);
    }
    setFocus();
    emit si_finished();
}

bool URLLineEdit::checkNameNoSemicolon(const QString& name) {
    CHECK(name.contains(";"), true);
    QString message = URLLineEdit::tr("File path/name contains ';' symbol.\r\n"
                                  "That kind of file path/name can't be correctly handled by this element.\r\n"
                                  "Please rename the file or move it to directory which not contain ';' in it path.");
    QMessageBox::critical(qobject_cast<QWidget*>(AppContext::getMainWindow()->getQMainWindow()),
                          L10N::errorTitle(),
                          message);
    return false;
}

void URLLineEdit::checkExtension(QString& name) const {
    QString fileFormat;
    if (parent != nullptr) {
        fileFormat = DelegateTags::getString(parent->tags(), DelegateTags::FORMAT);
    }
    DocumentFormat* format = AppContext::getDocumentFormatRegistry()->getFormatById(fileFormat);
    if (format != nullptr && !name.isEmpty()) {
        QString newName(name);
        GUrl url(newName);
        QString lastSuffix = url.lastFileSuffix();
        if (lastSuffix == "gz") {
            int dotPos = newName.length() - lastSuffix.length() - 1;
            if ((dotPos >= 0) && (QChar('.') == newName[dotPos])) {
                newName = url.getURLString().left(dotPos);
                GUrl tmp(newName);
                lastSuffix = tmp.lastFileSuffix();
            }
        }
        bool foundExt = false;
        foreach (QString supExt, format->getSupportedDocumentFileExtensions()) {
            if (supExt == lastSuffix) {
                foundExt = true;
                break;
            }
        }
        if (!foundExt) {
            name = name + "." + format->getSupportedDocumentFileExtensions().first();
        } else {
            int dotPos = newName.length() - lastSuffix.length() - 1;
            if ((dotPos < 0) || (QChar('.') != newName[dotPos])) {
                name = name + "." + format->getSupportedDocumentFileExtensions().first();
            }
        }
    }
}

bool URLLineEdit::isMulti() const {
    return multi;
}

}  // namespace U2
