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

#ifndef _U2_MULTIPLE_SEQUENCE_FILES_SELECTOR_H_
#define _U2_MULTIPLE_SEQUENCE_FILES_SELECTOR_H_

#include <QDialog>
#include <QList>
#include <QVariant>

#include <U2Core/DocumentUtils.h>
#include <U2Core/GUrl.h>

#include "ui_MultipleSequenceFilesReadingMode.h"

namespace U2 {

class SaveDocumentController;

class MultipleDocumentsReadingModeSelectorController {
public:	
    static bool adjustReadingMode(QVariantMap& , QList<GUrl>& urls,const QMap<QString, qint64>& headerSequenceLengths);
    static bool mergeDocumentOption(const FormatDetectionResult& formatResult, QMap<QString, qint64>* headerSequenceLengths);
private:
    MultipleDocumentsReadingModeSelectorController();
};

class MultipleDocumentsReadingModeDialog : public QDialog, public Ui_MultipleDocumentsReadingModeSelectorController{
    Q_OBJECT
public:
    MultipleDocumentsReadingModeDialog(const QList<GUrl>& urls, QWidget* parent = 0);

    bool setupGUI(QList<GUrl>& urls, QVariantMap& hintsDocuments, const QMap<QString, qint64>& headerSequenceLengths);

private slots:
    void sl_onMoveUp();
    void sl_onMoveDown();
    void sl_optionChanged();

private:
    void initSequenceSaveController();
    void initMsaSaveController();
    QString setupNewUrl();
    QString deleteNumPrefix(QString);
    void deleteAllNumPrefix();
    void changeNumPrefix();
    QString findUrlByFileName(const QString& fileName);

    SaveDocumentController *saveController;
    QList<GUrl> urls;
};

}

#endif
