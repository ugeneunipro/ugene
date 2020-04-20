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

#include <QFileInfo>
#include <QMessageBox>
#include <QPushButton>

#include <U2Core/Annotation.h>
#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Settings.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/SaveDocumentController.h>
#include <U2Gui/ScriptEditorDialog.h>
#include <U2Gui/U2FileDialog.h>

#include "CSVColumnConfigurationDialog.h"
#include "ImportAnnotationsFromCSVDialog.h"
#include "ImportAnnotationsFromCSVTask.h"

//TODO: add complement token configuration
//TODO: autodetect numeric columns, propose using them as start/end/length positions
//TODO: option for negative len -> complement

namespace U2 {

// TODO: unify with plugin settings
#define SETTINGS_ROOT       QString("dna_export/import_annotations_from_csv/")
#define A_NAME              QString("annotation_name")
#define T_SEPARATOR         QString("token_separator")
#define SKIP_LINES_COUNT    QString("skip_lines_count")
#define SKIP_LINES_PREFIX   QString("skip_lines_prefix")

ImportAnnotationsFromCSVDialog::ImportAnnotationsFromCSVDialog(QWidget* w)
    : QDialog (w),
      saveController(NULL) {
    setupUi(this);
    new HelpButton(this, buttonBox, "24748791");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Run"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    connect(readFileButton, SIGNAL(clicked()), SLOT(sl_readFileClicked()));
    connect(previewButton, SIGNAL(clicked()), SLOT(sl_previewClicked()));
    connect(guessButton, SIGNAL(clicked()), SLOT(sl_guessSeparatorClicked()));
    connect(scriptButton, SIGNAL(clicked()), SLOT(sl_scriptSeparatorClicked()));
    connect(separatorEdit, SIGNAL(textChanged(const QString&)), SLOT(sl_separatorChanged(const QString&)));
    connect(prefixToSkipEdit, SIGNAL(textChanged(const QString&)), SLOT(sl_prefixToSkipChanged(const QString&)));
    connect(previewTable, SIGNAL(itemClicked(QTableWidgetItem*)), SLOT(sl_tableItemClicked(QTableWidgetItem*)));
    connect(previewTable->horizontalHeader(), SIGNAL(sectionClicked(int)), SLOT(sl_tableHeaderClicked(int)));
    connect(columnSeparatorRadioButton, SIGNAL(toggled(bool)), SLOT(sl_separatorRadioToggled(bool)));
    connect(scriptRadioButton, SIGNAL(toggled(bool)), SLOT(sl_scriptRadioToggled(bool)));

    initSaveController();

    sl_separatorChanged(separatorEdit->text());
    sl_prefixToSkipChanged(prefixToSkipEdit->text());


    //restore last settings
    QString lastName = AppContext::getSettings()->getValue(SETTINGS_ROOT + A_NAME).toString();
    if (!lastName.isEmpty()) {
        defaultNameEdit->setText(lastName);
    }

    QString separator = AppContext::getSettings()->getValue(SETTINGS_ROOT + T_SEPARATOR).toString();
    if (!separator.isEmpty()) {
        separatorEdit->setText(separator);
    }

    int skipLinesCount = AppContext::getSettings()->getValue(SETTINGS_ROOT + SKIP_LINES_COUNT).toInt();
    linesToSkipBox->setValue(skipLinesCount);

    QString skipPrefix = AppContext::getSettings()->getValue(SETTINGS_ROOT + SKIP_LINES_PREFIX).toString();
    if (!skipPrefix.isEmpty()) {
        prefixToSkipEdit->setText(skipPrefix);
    }

    scriptHeader = tr("//The script parses input line\n // and returns an array of parsed elements as the result\nvar %1; //input line\nvar %2; //parsed line number")
        .arg(ReadCSVAsAnnotationsTask::LINE_VAR).arg(ReadCSVAsAnnotationsTask::LINE_NUM_VAR);

    columnSeparatorRadioButton->setChecked(true);
    sl_scriptRadioToggled(false);

    separatorEdit->setMaxLength(SEPARATOR_EDIT_MAX_LENGTH);

    // connect this slot after initial dialog model is set up
    connect(removeQuotesCheck, SIGNAL(toggled(bool)),SLOT(sl_removeQuotesToggled(bool)));
    connect(linesToSkipBox, SIGNAL(valueChanged(int)), SLOT(sl_linesToSkipChanged(int)));

}

void ImportAnnotationsFromCSVDialog::accept() {
    QString inFile = checkInputGroup(false);
    if (inFile.isEmpty()) {
        return;
    }
    bool qOk = checkSeparators(false);
    if (!qOk) {
        return;
    }
    QString outFile = checkOutputGroup();
    if (outFile.isEmpty()) {
        return;
    }

    //check that position is OK
    int endPos = 0;
    int startPos = 0;
    int length = 0;
    int names = 0;
    foreach (const ColumnConfig& conf, columnsConfig) {
        switch(conf.role) {
            case ColumnRole_EndPos: endPos++; break;
            case ColumnRole_StartPos: startPos++; break;
            case ColumnRole_Length: length++; break;
            case ColumnRole_Name: names++; break;
            case ColumnRole_Ignore: break;
            case ColumnRole_Qualifier: break;
            default:;
        }
    }
    if (endPos + startPos + length < 2  || endPos > 1 || startPos > 1 || length > 1) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("Invalid start position/end position/length configuration!"));
        return;
    }
    if (names > 1 ) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("Multiple columns are used as a name!"));
        return;
    }
    if (!Annotation::isValidAnnotationName(defaultNameEdit->text())) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("Invalid default annotation name!"));
        defaultNameEdit->setFocus();
        return;
    }

    //store last settings
    AppContext::getSettings()->setValue(SETTINGS_ROOT + A_NAME, defaultNameEdit->text());
    AppContext::getSettings()->setValue(SETTINGS_ROOT + T_SEPARATOR, separatorEdit->text());
    AppContext::getSettings()->setValue(SETTINGS_ROOT + SKIP_LINES_COUNT, linesToSkipBox->value());
    AppContext::getSettings()->setValue(SETTINGS_ROOT + SKIP_LINES_PREFIX, prefixToSkipEdit->text());

    return QDialog::accept();
}

void ImportAnnotationsFromCSVDialog::toParsingConfig(CSVParsingConfig& config) const {
    config.columns = columnsConfig;
    config.linesToSkip = linesToSkipBox->value();
    config.prefixToSkip = prefixToSkipEdit->text();
    config.keepEmptyParts = !separatorsModeCheck->isChecked();
    config.defaultAnnotationName = defaultNameEdit->text();
    config.removeQuotes = removeQuotesCheck->isChecked();

    config.parsingScript.clear();
    config.splitToken.clear();
    if( columnSeparatorRadioButton->isChecked() ) {
        config.splitToken = separatorEdit->text();
    } else if( scriptRadioButton->isChecked() ) {
        config.parsingScript = parsingScript;
    } else {
        assert(false);
    }
}

void ImportAnnotationsFromCSVDialog::toTaskConfig(ImportAnnotationsFromCSVTaskConfig& config) const {
    assert(result() == QDialog::Accepted);
    toParsingConfig(config.parsingOptions);
    config.csvFile = readFileName->text();
    config.formatId = saveController->getFormatIdToSave();
    config.dstFile = saveController->getSaveFileName();
    config.addToProject = addToProjectCheck->isChecked();
}

#define PREVIEW_BUFF_SIZE 8196
QString ImportAnnotationsFromCSVDialog::readFileHeader(const QString& fileName, bool silentFail) {
    IOAdapterId ioId = IOAdapterUtils::url2io(fileName);
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(ioId);
    QScopedPointer<IOAdapter> io(iof->createIOAdapter());
    bool ok = io->open(fileName, IOAdapterMode_Read);
    if (!ok) {
        if (!silentFail) {
            QMessageBox::critical(this, L10N::errorTitle(), L10N::errorOpeningFileRead(fileName));
            readFileName->setFocus();
        }
        return QString();
    }

    QByteArray previewBuff;
    previewBuff.resize(PREVIEW_BUFF_SIZE);
    qint64 bytesRead = io->readBlock(previewBuff.data(), PREVIEW_BUFF_SIZE);
    if (bytesRead == -1) {
        if (!silentFail) {
            QMessageBox::critical(this, L10N::errorTitle(), L10N::errorReadingFile(GUrl(fileName)));
            readFileName->setFocus();
        }
        return QString();

    }
    assert(bytesRead >= 0);
    previewBuff.resize(bytesRead);
    return QString::fromLocal8Bit(previewBuff);
}

void ImportAnnotationsFromCSVDialog::sl_removeQuotesToggled(bool) {
    guessSeparator(true);
}

void ImportAnnotationsFromCSVDialog::sl_linesToSkipChanged(int) {
    guessSeparator(true);
}

void ImportAnnotationsFromCSVDialog::initSaveController() {
    SaveDocumentControllerConfig config;
    config.fileDialogButton = saveFileButton;
    config.fileNameEdit = saveFileName;
    config.formatCombo = saveFormatCombo;
    config.parentWidget = this;
    config.saveTitle = tr("Save imported annotations to");

    DocumentFormatConstraints formatConstraints;
    formatConstraints.supportedObjectTypes << GObjectTypes::ANNOTATION_TABLE;
    formatConstraints.addFlagToSupport(DocumentFormatFlag_SupportWriting);
    formatConstraints.formatsToExclude << BaseDocumentFormats::VECTOR_NTI_SEQUENCE;

    saveController = new SaveDocumentController(config, formatConstraints, this);
}

void ImportAnnotationsFromCSVDialog::sl_guessSeparatorClicked() {
    guessSeparator(false);
}

void ImportAnnotationsFromCSVDialog::sl_scriptSeparatorClicked() {
    if (parsingScript.isEmpty()) {
        lastUsedSeparator = separatorEdit->text();
    }
    QObjectScopedPointer<ScriptEditorDialog> d = new ScriptEditorDialog(this, scriptHeader);
    if (!parsingScript.isEmpty()) {
        d->setScriptText(parsingScript);
    } else { //set sample script
        QString l1 = "var firstColumn = ["+ReadCSVAsAnnotationsTask::LINE_NUM_VAR+"];\n";
        QString l2 = "var otherColumns = "+ReadCSVAsAnnotationsTask::LINE_VAR+".split(\" \");\n";
        QString l3 = "result =firstColumn.concat(otherColumns);";
        d->setScriptText(l1 + l2 + l3);
    }

    const int rc = d->exec();
    CHECK(!d.isNull(), );

    if (rc != QDialog::Accepted) {
        return;
    }
    parsingScript = d->getScriptText();
    separatorEdit->setText(lastUsedSeparator);
}

void ImportAnnotationsFromCSVDialog::guessSeparator(bool silent) {
    QString url = checkInputGroup(silent);
    if (url.isEmpty()) {
        return;
    }
    QString text = readFileHeader(url, silent);
    CSVParsingConfig config;
    toParsingConfig(config);
    QString sep = ReadCSVAsAnnotationsTask::guessSeparatorString(text, config);
    if (sep.isEmpty()) {
        if (!silent) {
            QMessageBox::critical(this, L10N::errorTitle(), tr("Failed to guess separator sequence!"));
            readFileName->setFocus();
        }
        return;
    }
    separatorEdit->setText(sep);
    preview(silent);
}

static QString toHex(const QString& text) {
    QString rawHex = text.toLocal8Bit().toHex();
    QString res;
    for (int i=0; i<rawHex.size(); i+=2) {
        QString hexPart = rawHex.mid(i, 2);
        res += (res.length() > 0 ? " ": "") + hexPart;
    }
    return res;
}

void ImportAnnotationsFromCSVDialog::sl_separatorChanged(const QString& v) {
    if (v.isEmpty()) {
        separatorLabel->setText(tr("Column separator"));
    } else {
        separatorLabel->setText(tr("Column separator value: [%1], hex: [%2], length: %3")
            .arg(v).arg(toHex(v)).arg(v.length()));
        if (v.trimmed().length() == 0) {
            separatorsModeCheck->setChecked(true);
        }
    }
    if (!v.isEmpty() && TextUtils::contains(CSVParsingConfig::QUOTES, v.toLocal8Bit().constData(), v.length())) {
        removeQuotesCheck->setChecked(false);
    }
}

void ImportAnnotationsFromCSVDialog::sl_prefixToSkipChanged(const QString& v) {
    skipPrefixLabel->setEnabled(!v.isEmpty());
}

void ImportAnnotationsFromCSVDialog::sl_readFileClicked() {
    // show the dialog
    LastUsedDirHelper lod("CSV");
    QString filter = DialogUtils::prepareFileFilter(tr("CSV Files"), QStringList() << "csv", true, QStringList());
    lod.url = U2FileDialog::getOpenFileName(this, tr("Select CSV file to read"), lod, filter);
    if (lod.url.isEmpty()) {
        return;
    }
    readFileName->setText(lod.url);

    // guess separator & show preview
    guessSeparator(true);
}

QString ImportAnnotationsFromCSVDialog::checkInputGroup(bool silent) {
    QString inputFile = readFileName->text();
    if (inputFile.isEmpty()) {
        if (!silent) {
            QMessageBox::critical(this, L10N::errorTitle(), tr("Enter input CSV file name"));
            readFileName->setFocus();
        }
        return QString();
    }
    QFileInfo csvFile(inputFile);
    if (!csvFile.exists() || !csvFile.isFile()) {
        if (!silent) {
            QMessageBox::critical(this, L10N::errorTitle(), L10N::errorFileNotFound(inputFile));
            readFileName->setFocus();
        }
        return QString();
    }

    if (!csvFile.isReadable()) {
        if (!silent) {
            QMessageBox::critical(this, L10N::errorTitle(), L10N::errorOpeningFileRead(inputFile));
            readFileName->setFocus();
        }
        return QString();
    }
    return csvFile.canonicalFilePath();
}

bool ImportAnnotationsFromCSVDialog::checkSeparators(bool silent) {
    if (columnSeparatorRadioButton->isChecked() && separatorEdit->text().isEmpty() ) {
        if (!silent) {
            QMessageBox::critical(this, L10N::errorTitle(), tr("Separator value is not set"));
            separatorEdit->setFocus();
        }
        return false;
    } else if( scriptRadioButton->isChecked() && parsingScript.isEmpty() ){
        if (!silent) {
            QMessageBox::critical(this, L10N::errorTitle(), tr("Script is empty"));
        }
        return false;
    }
    return true;
}

QString ImportAnnotationsFromCSVDialog::checkOutputGroup() {
    QString outFile = saveController->getSaveFileName();
    if (outFile.isEmpty()) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("Output file name is not set!"));
        saveFileName->setFocus();
        return QString();
    }

    return outFile;
}

void ImportAnnotationsFromCSVDialog::sl_previewClicked() {
    preview(false);
}

void ImportAnnotationsFromCSVDialog::preview(bool silent) {
    QString fileName = checkInputGroup(silent);
    if (fileName.isEmpty()) {
        return;
    }
    QString text = readFileHeader(fileName, silent);

    previewTable->clear();
    rawPreview->clear();

    rawPreview->setPlainText(text);

    if (!checkSeparators(true)) {
        return;
    }
    CSVParsingConfig parseOptions;
    toParsingConfig(parseOptions);
    if (parseOptions.splitToken.isEmpty() && parseOptions.parsingScript.isEmpty()) {
        return;
    }


    int columnCount = 0;
    TaskStateInfo ti;
    QList<QStringList> lines = ReadCSVAsAnnotationsTask::parseLinesIntoTokens(text, parseOptions, columnCount, ti);
    if (ti.hasError()) {
        QMessageBox::critical(this, L10N::errorTitle(), ti.getError());
        return;
    }
    prepareColumnsConfig(columnCount);
    columnCount = qMax(columnCount, columnsConfig.size());

    previewTable->setRowCount(lines.size());
    previewTable->setColumnCount(columnCount);

    for (int column = 0; column < columnCount; column++) {
        QTableWidgetItem* headerItem = createHeaderItem(column);
        previewTable->setHorizontalHeaderItem(column, headerItem);
    }
    for (int row = 0; row < lines.size(); row++) {
        const QStringList& rowData = lines.at(row);
        for (int column = 0; column < rowData.size(); column ++) {
            QString token = rowData.at(column);
            QTableWidgetItem* item = new QTableWidgetItem(token);
            item->setFlags(Qt::ItemIsEnabled);
            previewTable->setItem(row, column, item);
        }
    }
}

void ImportAnnotationsFromCSVDialog::prepareColumnsConfig(int numColumns) {
    while (columnsConfig.size() < numColumns) {
        columnsConfig.append(ColumnConfig());
    }

    while (columnsConfig.size() > numColumns) {
        const ColumnConfig& config = columnsConfig.last();
        if (config.role == ColumnRole_Ignore) {
            columnsConfig.removeLast();
        }
    }
}

QTableWidgetItem* ImportAnnotationsFromCSVDialog::createHeaderItem(int column) const {
    assert(column >=0 && column < columnsConfig.size());
    QString text = getHeaderItemText(column);
    return new QTableWidgetItem(text);
}

QString ImportAnnotationsFromCSVDialog::getHeaderItemText(int column) const {
    const ColumnConfig& config = columnsConfig.at(column);
    QString text = tr("[ignored]");
    switch (config.role) {
        case ColumnRole_Qualifier:
            text = tr("[qualifier %1]").arg(config.qualifierName);
            break;
        case ColumnRole_Name:
            text = tr("[name]");
            break;
        case ColumnRole_StartPos:
            if (config.startPositionOffset!=0) {
                text = tr("[start position with offset %1]").arg(config.startPositionOffset);
            } else {
                text = tr("[start position]");
            }
            break;
        case ColumnRole_EndPos:
            if (config.endPositionIsInclusive) {
                text = tr("[end position (inclusive)]");
            } else {
                text = tr("[end position]");
            }
            break;
        case ColumnRole_Length:
            text = tr("[length]");
            break;
        case ColumnRole_ComplMark:
            if (config.complementMark.isEmpty()) {
                text = tr("[complement]");
            } else {
                text = tr("[complement if '%1']").arg(config.complementMark);
            }
            break;
        case ColumnRole_Group:
            text = tr("[group]");
            break;
        default:
            assert(config.role == ColumnRole_Ignore);
    }
    return text;
}

void ImportAnnotationsFromCSVDialog::sl_tableItemClicked(QTableWidgetItem* item) {
    int column = previewTable->column(item);
    configureColumn(column);
}

void ImportAnnotationsFromCSVDialog::sl_tableHeaderClicked(int column) {
    configureColumn(column);
}

void ImportAnnotationsFromCSVDialog::sl_separatorRadioToggled(bool checked) {
    separatorEdit->setEnabled(checked);
    guessButton->setEnabled(checked);
}

void ImportAnnotationsFromCSVDialog::sl_scriptRadioToggled(bool checked) {
    scriptButton->setEnabled(checked);
    if(checked && parsingScript.isEmpty()) {
        sl_scriptSeparatorClicked();
    }
}

void ImportAnnotationsFromCSVDialog::configureColumn(int column) {
    assert(column >= 0 && column < columnsConfig.size());

    const ColumnConfig& config = columnsConfig.at(column);
    QObjectScopedPointer<CSVColumnConfigurationDialog> d = new CSVColumnConfigurationDialog(this, config);
    int rc = d->exec(); // TODO: set dialog position close to the header item
    CHECK(!d.isNull(), );
    if (rc == QDialog::Accepted) {
        columnsConfig[column] = d->config;
    }
    previewTable->horizontalHeaderItem(column)->setText(getHeaderItemText(column));
}

} //namespace
