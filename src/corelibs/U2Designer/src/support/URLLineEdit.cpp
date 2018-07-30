/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <QFocusEvent>
#include <QLayout>

#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/SuggestCompleter.h>
#include <U2Gui/U2FileDialog.h>

#include <U2Lang/SchemaConfig.h>

#include "PropertyWidget.h"
#include "URLLineEdit.h"

namespace U2 {

class FilenameCompletionFiller : public CompletionFiller {
public:
    FilenameCompletionFiller(URLWidget *_widget)
        :CompletionFiller(), widget(_widget)
    {

    }

    virtual QStringList getSuggestions(const QString &str) {
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
            bool ok = fillChoisesWithFormatExtensions(fileName, choices);
            CHECK(ok, QStringList());
        } else {
            fillChoisesWithPresetExtensions(fileName, presetExtensions, choices);
        }

        return choices;
    }

    bool fillChoisesWithFormatExtensions(const QString &fileName, QStringList &choices) {
        const QFileInfo f(fileName);
        const QString curExt = f.suffix();
        const QString baseName = f.completeBaseName();
        const QString completeFileName = f.fileName();

        const QString fileFormat = DelegateTags::getString(widget->tags(), "format");
        DocumentFormat *format = AppContext::getDocumentFormatRegistry()->getFormatById(fileFormat);
        CHECK(NULL != format, false);

        QStringList formats = format->getSupportedDocumentFileExtensions();
        CHECK(formats.size() > 0, false);
        formats.append("gz");

        foreach (const QString &ext, formats) {
            if (!curExt.isEmpty()) {
                if (ext.startsWith(curExt, Qt::CaseInsensitive)) {
                    choices << baseName + "." + ext;
                    if (ext != "gz"){
                        choices << baseName + "." + ext + ".gz";
                    }
                }
            }
        }

        if (choices.size() == 1) {
            foreach (const QString &ext, formats) {
                choices << completeFileName + "." + ext;
                if (ext != "gz") {
                    choices << completeFileName + "." + ext + ".gz";
                }
            }
        }
        return true;
    }

    static void fillChoisesWithPresetExtensions(const QString &fileName, const QStringList &presetExtensions, QStringList &choices) {
        const QFileInfo f(fileName);
        const QString baseName = f.completeBaseName();

        foreach (const QString &extenstion, presetExtensions) {
            choices << baseName + "." + extenstion;
        }
    }

    virtual QString finalyze(const QString &editorText, const QString &suggestion) {
        QString path = editorText;
        path.replace("\\", "/");

        int slashPos = path.lastIndexOf("/");
        QString dirPath = path.left(slashPos + 1);

        return dirPath + suggestion;
    }

private:
    URLWidget *widget;
};

URLLineEdit::URLLineEdit(const QString &type,
                         bool multi,
                         bool isPath,
                         bool saveFile,
                         URLWidget *_parent)
    : QLineEdit(_parent),
      schemaConfig(NULL),
      type(type),
      multi(multi),
      isPath(isPath),
      saveFile(saveFile),
      parent(_parent)
{
    if (saveFile && NULL != parent) {
        new BaseCompleter(new FilenameCompletionFiller(parent), this);
    }
    setPlaceholderText(DelegateTags::getString(parent->tags(), DelegateTags::PLACEHOLDER_TEXT));
}

CompletionFiller * URLLineEdit::getCompletionFillerInstance() {
    if (saveFile && NULL != parent) {
        return new FilenameCompletionFiller(parent);
    }
    return NULL;
}

void URLLineEdit::sl_onBrowse() {
    this->browse(false);
}

void URLLineEdit::sl_onBrowseWithAdding() {
    this->browse(true);
}

void URLLineEdit::browse(bool addFiles) {
    QString FileFilter;
    if (NULL != parent) {
        FileFilter = DelegateTags::getString(parent->tags(), DelegateTags::FILTER);
    }
    LastUsedDirHelper lod(type);
    QString lastDir = lod.dir;
    if(!text().isEmpty()) {
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
    if(isPath || multi){
        QStringList lst;
        if (isPath) {
            QString dir = U2FileDialog::getExistingDirectory(NULL, tr("Select a folder"), lastDir, QFileDialog::Options());
            lst << dir;
        } else {
            lst = U2FileDialog::getOpenFileNames(NULL, tr("Select file(s)"), lastDir, FileFilter);
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
        if(saveFile) {
            lod.url = name = U2FileDialog::getSaveFileName(NULL, tr("Select a file"), lastDir, FileFilter, 0, QFileDialog::DontConfirmOverwrite);
            this->checkExtension(name);
        } else {
            #ifdef Q_OS_MAC
                if (qgetenv(ENV_GUI_TEST).toInt() == 1 && qgetenv(ENV_USE_NATIVE_DIALOGS).toInt() == 0) {
                    lod.url = name = U2FileDialog::getOpenFileName(NULL, tr("Select a file"), lastDir, FileFilter, 0, QFileDialog::DontUseNativeDialog );
                } else
            #endif
            lod.url = name = U2FileDialog::getOpenFileName(NULL, tr("Select a file"), lastDir, FileFilter );
        }
    }
    if (!name.isEmpty()) {
        if (name.length() > this->maxLength()) {
            this->setMaxLength(name.length() + this->maxLength());
        }
        setText(name);
    }
    setFocus();
    emit si_finished();
}

void URLLineEdit::focusOutEvent(QFocusEvent *event) {
    sl_completionFinished();
    QLineEdit::focusOutEvent(event);
}

void URLLineEdit::keyPressEvent(QKeyEvent *event) {
    if (Qt::Key_Enter == event->key()) {
        sl_completionFinished();
    }
    QLineEdit::keyPressEvent(event);
}

void URLLineEdit::checkExtension(QString &name) {
    QString fileFormat;
    if (NULL != parent) {
        fileFormat = DelegateTags::getString(parent->tags(), DelegateTags::FORMAT);
    }
    DocumentFormat *format = AppContext::getDocumentFormatRegistry()->getFormatById(fileFormat);
    if (NULL != format && !name.isEmpty()) {
        QString newName(name);
        GUrl url(newName);
        QString lastSuffix = url.lastFileSuffix();
        if ("gz" == lastSuffix) {
            int dotPos = newName.length() - lastSuffix.length() - 1;
            if ((dotPos >= 0) && (QChar('.') == newName[dotPos])) {
                newName = url.getURLString().left(dotPos);
                GUrl tmp(newName);
                lastSuffix = tmp.lastFileSuffix();
            }
        }
        bool foundExt = false;
        foreach (QString supExt, format->getSupportedDocumentFileExtensions()) {
            if (lastSuffix == supExt) {
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

bool URLLineEdit::isMulti() {
    return multi;
}

void URLLineEdit::sl_completionFinished() {
    emit si_finished();
}

} // U2
