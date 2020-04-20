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

#include "BlastDBSelectorWidgetController.h"

#include <QDirIterator>
#include <QMessageBox>

#include <U2Core/L10n.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>

namespace U2 {

BlastDBSelectorWidgetController::BlastDBSelectorWidgetController(QWidget *parent) : QWidget(parent), isNuclDB(false), inputDataValid(false) {
    setupUi(this);
    connect(selectDatabasePushButton, SIGNAL(clicked()), SLOT(sl_onBrowseDatabasePath()));
    connect(databasePathLineEdit, SIGNAL(textChanged(QString)), SLOT(sl_lineEditChanged()));
    connect(baseNameLineEdit, SIGNAL(textChanged(QString)), SLOT(sl_lineEditChanged()));
}

void BlastDBSelectorWidgetController::sl_lineEditChanged() {
    bool pathWarning = databasePathLineEdit->text().contains(' ');
    QString pathTooltip = pathWarning ? tr("Database path contains space characters.") : "";
    GUIUtils::setWidgetWarning(databasePathLineEdit, pathWarning);
    databasePathLineEdit->setToolTip(pathTooltip);

    bool nameWarning = baseNameLineEdit->text().contains(' ');
    QString nameTooltip = nameWarning ? tr("Database name contains space characters.") : "";
    GUIUtils::setWidgetWarning(baseNameLineEdit, nameWarning);
    baseNameLineEdit->setToolTip(nameTooltip);

    bool isFilledDatabasePathLineEdit = !databasePathLineEdit->text().isEmpty();
    bool isFilledBaseNameLineEdit = !baseNameLineEdit->text().isEmpty();
    bool hasSpacesInDBPath = pathWarning || nameWarning;
    inputDataValid = isFilledBaseNameLineEdit && isFilledDatabasePathLineEdit && !hasSpacesInDBPath;
    emit si_dbChanged();
}

bool BlastDBSelectorWidgetController::isNuclDatabase() const {
    return isNuclDB;
}

bool BlastDBSelectorWidgetController::isInputDataValid() const {
    return inputDataValid;
}

QString BlastDBSelectorWidgetController::getDatabasePath()const {
    return databasePathLineEdit->text() + "/" + baseNameLineEdit->text();
}

void BlastDBSelectorWidgetController::sl_onBrowseDatabasePath() {
    LastUsedDirHelper lod("Database Folder");

    QFileDialog::Options options = 0;
    QString name = U2FileDialog::getOpenFileName(NULL, tr("Select a database file"), lod.dir, "", NULL, options);
    if (!name.isEmpty()) {
        QFileInfo fileInfo(name);
        if (!fileInfo.suffix().isEmpty()) {
            isNuclDB = (fileInfo.suffix().at(0) == 'n');
        }
        QRegExp toReplace("(\\.\\d+)?(((formatDB|makeBlastDB)\\.log)|(\\.(phr|pin|psq|phd|pnd|pog|ppi|psi|phi|pni|ppd|psd|psq|pal|nal|nhr|nin|nsq)))?$", Qt::CaseInsensitive);
        baseNameLineEdit->setText(fileInfo.fileName().replace(toReplace, QString()));
        databasePathLineEdit->setText(fileInfo.dir().path());
        lod.url = name;
    }
}

bool BlastDBSelectorWidgetController::validateDatabaseDir() {
    QStringList extList;
    if (isNuclDB) {
        extList << "nal" << "nin";
    } else {
        extList << "pal" << "pin";
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

}
