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

#ifndef _U2_IMPORT_ANNOTATIONS_FROM_CSV_DIALOG_H_
#define _U2_IMPORT_ANNOTATIONS_FROM_CSV_DIALOG_H_

#include <ui_ImportAnnotationsFromCSVDialog.h>

#include "CSVColumnConfiguration.h"

class QTreeWidgetItem;

namespace U2 {

class CSVParsingConfig;
class ImportAnnotationsFromCSVTaskConfig;
class SaveDocumentController;

class ImportAnnotationsFromCSVDialog : public QDialog, private Ui_ImportAnnotationsFromCSVDialog {
    Q_OBJECT
public:
    ImportAnnotationsFromCSVDialog(QWidget* w);

    void toTaskConfig(ImportAnnotationsFromCSVTaskConfig& config) const;
    void toParsingConfig(CSVParsingConfig& config) const;

public slots:
    virtual void accept();

private slots:
    void sl_readFileClicked();
    void sl_previewClicked();
    void sl_guessSeparatorClicked();
    void sl_scriptSeparatorClicked();
    void sl_separatorChanged(const QString&);
    void sl_prefixToSkipChanged(const QString&);
    void sl_tableItemClicked(QTableWidgetItem*);
    void sl_tableHeaderClicked(int);
    void sl_separatorRadioToggled(bool);
    void sl_scriptRadioToggled(bool);
    void sl_removeQuotesToggled(bool);
    void sl_linesToSkipChanged(int);

private:
    void initSaveController();

    // returns input file name if no errors found or empty string
    QString checkInputGroup(bool silentFail);
    // returns output file name if no errors found or empty string
    QString checkOutputGroup();

    bool checkSeparators(bool silentFail);

    void prepareColumnsConfig(int numColumnsHint);
    QTableWidgetItem* createHeaderItem(int column) const;
    QString getHeaderItemText(int column) const;
    void    configureColumn(int column);
    QString readFileHeader(const QString& fileName, bool silentFail);

    void guessSeparator(bool silentFail);
    void preview(bool silentFail);

    SaveDocumentController* saveController;
    QList<ColumnConfig>          columnsConfig;

    // script text used to parse separator
    QString parsingScript;
    // header of the script
    QString scriptHeader;
    // last separator value before switch to script mode
    QString lastUsedSeparator;

    static const int SEPARATOR_EDIT_MAX_LENGTH = 20;
};

} // namespace U2

#endif
