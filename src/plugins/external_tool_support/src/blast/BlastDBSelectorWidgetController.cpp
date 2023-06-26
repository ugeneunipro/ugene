/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "BlastDBSelectorWidgetController.h"

#include <QDirIterator>
#include <QMessageBox>

#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/L10n.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/LastUsedDirHelper.h>

namespace U2 {

BlastDBSelectorWidgetController::BlastDBSelectorWidgetController(QWidget* parent)
    : QWidget(parent), isNuclDB(false), inputDataValid(false) {
    setupUi(this);
    connect(selectDatabasePushButton, SIGNAL(clicked()), SLOT(sl_onBrowseDatabasePath()));
    connect(databasePathLineEdit, SIGNAL(textChanged(QString)), SLOT(sl_lineEditChanged()));
    connect(baseNameLineEdit, SIGNAL(textChanged(QString)), SLOT(sl_lineEditChanged()));
}

void BlastDBSelectorWidgetController::sl_lineEditChanged() {
    static QString pathTooltip = tr("Database path contains spaces or/and not Latin characters.");
    static QString nameTooltip = tr("Database name contains spaces or/and not Latin characters.");

    bool pathWarning = !checkValidPathLE(databasePathLineEdit, pathTooltip);
    bool nameWarning = !checkValidPathLE(baseNameLineEdit, nameTooltip);

    bool isFilledDatabasePathLineEdit = !databasePathLineEdit->text().isEmpty();
    bool isFilledBaseNameLineEdit = !baseNameLineEdit->text().isEmpty();
    bool hasProblemsInDBPath = pathWarning || nameWarning;

    inputDataValid = isFilledBaseNameLineEdit && isFilledDatabasePathLineEdit && !hasProblemsInDBPath;
    emit si_dbChanged();
}

bool BlastDBSelectorWidgetController::checkValidPathLE(QLineEdit *le, const QString& errorTooltip) {
    bool isOkay = ExternalToolSupportUtils::checkArgumentPathLatinSymbols({le->text()}).isEmpty() &&
                  ExternalToolSupportUtils::checkArgumentPathSpaces({le->text()}).isEmpty();

    GUIUtils::setWidgetWarningStyle(le, !isOkay);
    le->setToolTip(isOkay ? "" : errorTooltip);
    return isOkay;
}

bool BlastDBSelectorWidgetController::isNuclDatabase() const {
    return isNuclDB;
}

bool BlastDBSelectorWidgetController::isInputDataValid() const {
    return inputDataValid;
}

QString BlastDBSelectorWidgetController::getDatabasePath() const {
    return databasePathLineEdit->text() + "/" + baseNameLineEdit->text();
}

void BlastDBSelectorWidgetController::sl_onBrowseDatabasePath() {
    LastUsedDirHelper lod("Database Folder");

    QFileDialog::Options options = 0;
    QString name = U2FileDialog::getOpenFileName(nullptr, tr("Select a database file"), lod.dir, "", nullptr, options);
    if (!name.isEmpty()) {
        QFileInfo fileInfo(name);
        isNuclDB = !fileInfo.suffix().isEmpty() && fileInfo.suffix().at(0) == 'n';

        // Build list of known file suffixes produced by 'makeblastdb' command.
        QStringList subSuffixes = QString("al|db|hr|in|sq|hd|nd|og|ot|pi|si|hi|ni|pd|sd|sq|tf|to").split("|");
        QStringList blastDbFileSuffixes;
        for (const QString& subSuffix : qAsConst(subSuffixes)) {
            blastDbFileSuffixes << (".n" + subSuffix) << (".p" + subSuffix);  // nucleic and protein variants.
        }
        blastDbFileSuffixes << "formatDB.log";
        blastDbFileSuffixes << "MakeBLASTDB.log";
        blastDbFileSuffixes << "MakeBLASTDB.perf";

        // Guess the database name: a part of the file name with no suffix.
        QString databaseName = fileInfo.fileName();
        for (const QString& suffix : qAsConst(blastDbFileSuffixes)) {
            if (databaseName.endsWith(suffix, Qt::CaseInsensitive)) {
                databaseName = databaseName.left(databaseName.length() - suffix.length());
                break;
            }
        }
        // Remove counter-like suffix from the database file name: database-name.01.
        databaseName = databaseName.replace(QRegExp("(\\.\\d+)$"), "");

        baseNameLineEdit->setText(databaseName);
        databasePathLineEdit->setText(fileInfo.dir().path());
        lod.url = name;
    }
}

bool BlastDBSelectorWidgetController::validateDatabaseDir() {
    QStringList extList;
    if (isNuclDB) {
        extList << "nal"
                << "nin";
    } else {
        extList << "pal"
                << "pin";
    }
    QDirIterator dirIt(databasePathLineEdit->text(), QDirIterator::Subdirectories);
    while (dirIt.hasNext()) {
        dirIt.next();
        if (QFileInfo(dirIt.filePath()).isFile()) {
            if (QFileInfo(dirIt.filePath()) == databasePathLineEdit->text() + QDir::separator() + baseNameLineEdit->text() + "." + extList[1]) {
                return true;
            } else if (QFileInfo(dirIt.filePath()) == databasePathLineEdit->text() + QDir::separator() + baseNameLineEdit->text() + "." + extList[0]) {
                return true;
            }
        }
    }

    QMessageBox::warning(this, L10N::warningTitle(), tr("No alias or index file found for selected database."));
    return false;
}

}  // namespace U2
