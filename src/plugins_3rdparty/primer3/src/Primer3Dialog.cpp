/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include <cfloat>

#include <QMessageBox>
#include <QSettings>

#include <U2Algorithm/SplicedAlignmentTaskRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/L10n.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>

#include <U2View/AnnotatedDNAView.h>

#include "Primer3Dialog.h"

namespace U2 {

const QRegularExpression Primer3Dialog::MUST_MATCH_END_REGEX("^([NAGCTRYWSMKBHDV]){5}$");

Primer3Dialog::Primer3Dialog(const Primer3TaskSettings& defaultSettings, ADVSequenceObjectContext* context)
    : QDialog(context->getAnnotatedDNAView()->getWidget()),
      defaultSettings(defaultSettings),
      context(context) {
    setupUi(this);
    new HelpButton(this, helpButton, "65930919");

    //QPushButton* pbPick = pickPrimersButton;
    //QPushButton* pbReset = resetButton;

    pickPrimersButton->setDefault(true);

    connect(pickPrimersButton, SIGNAL(clicked()), SLOT(sl_pbPick_clicked()));
    connect(resetButton, SIGNAL(clicked()), SLOT(sl_pbReset_clicked()));
    connect(saveSettingsButton, SIGNAL(clicked()), SLOT(sl_saveSettings()));
    connect(loadSettingsButton, SIGNAL(clicked()), SLOT(sl_loadSettings()));

    tabWidget->setCurrentIndex(0);

    {
        CreateAnnotationModel createAnnotationModel;
        createAnnotationModel.data->name = "top_primers";
        createAnnotationModel.sequenceObjectRef = GObjectReference(context->getSequenceGObject());
        createAnnotationModel.hideAnnotationType = true;
        createAnnotationModel.hideAnnotationName = false;
        createAnnotationModel.hideLocation = true;
        createAnnotationWidgetController = new CreateAnnotationWidgetController(createAnnotationModel, this);
        annotationWidgetLayout->addWidget(createAnnotationWidgetController->getWidget());
    }

    if (!context->getSequenceSelection()->getSelectedRegions().isEmpty()) {
        selection = context->getSequenceSelection()->getSelectedRegions().first();
    }
    sequenceLength = context->getSequenceLength();
    rs = new RegionSelector(this, sequenceLength, false, context->getSequenceSelection(), true);
    rangeSelectorLayout->addWidget(rs);

    repeatLibraries.append(QPair<QString, QByteArray>(tr("NONE"), ""));
    repeatLibraries.append(QPair<QString, QByteArray>(tr("HUMAN"), "primer3/humrep_and_simple.txt"));
    repeatLibraries.append(QPair<QString, QByteArray>(tr("RODENT_AND_SIMPLE"), "primer3/rodrep_and_simple.txt"));
    repeatLibraries.append(QPair<QString, QByteArray>(tr("RODENT"), "primer3/rodent_ref.txt"));
    repeatLibraries.append(QPair<QString, QByteArray>(tr("DROSOPHILA"), "primer3/drosophila.w.transposons.txt"));

    for (int i = 0; i < repeatLibraries.size(); i++) {
        if (!repeatLibraries[i].second.isEmpty())
            repeatLibraries[i].second = QFileInfo(QString(PATH_PREFIX_DATA) + ":" + repeatLibraries[i].second).absoluteFilePath().toLatin1();
    }

    {
        for (const auto& library : repeatLibraries) {
            combobox_PRIMER_MISPRIMING_LIBRARY->addItem(library.first);
            combobox_PRIMER_INTERNAL_MISHYB_LIBRARY->addItem(library.first);
        }
    }

    int spanIntronExonIdx = -1;

    for (int i = 0; i < tabWidget->count(); ++i) {
        if (tabWidget->tabText(i).contains("Intron")) {
            spanIntronExonIdx = i;
            break;
        }
    }

    if (spanIntronExonIdx != -1) {
        SplicedAlignmentTaskRegistry* sr = AppContext::getSplicedAlignmentTaskRegistry();
        tabWidget->setTabEnabled(spanIntronExonIdx, sr->getAlgNameList().size() > 0);
    }

    reset();
}

Primer3TaskSettings Primer3Dialog::getSettings() const {
    return settings;
}

const CreateAnnotationModel& Primer3Dialog::getCreateAnnotationModel() const {
    return createAnnotationWidgetController->getModel();
}

U2Region Primer3Dialog::getRegion(bool* ok) const {
    return rs->getRegion(ok);
}

bool Primer3Dialog::prepareAnnotationObject() {
    return createAnnotationWidgetController->prepareAnnotationObject();
}

QString Primer3Dialog::intervalListToString(const QList<U2Region>& intervalList, const QString& delimiter, IntervalDefinition definition) {
    QString result;
    bool first = true;
    for (const auto& interval : intervalList) {
        if (!first) {
            result += " ";
        }
        result += QString::number(interval.startPos);
        result += delimiter;
        if (definition == IntervalDefinition::Start_End) {
            result += QString::number(interval.endPos() - 1);
        } else {
            result += QString::number(interval.length);
        }
        first = false;
    }
    return result;
}

QString Primer3Dialog::intListToString(const QList<int>& intList, const QString& delimiter) {
    QString result;
    bool first = true;
    for (int num : intList) {
        if (!first) {
            result += " ";
        }
        result += QString::number(num);
        result += delimiter;
        first = false;
    }
    return result;
}

QString Primer3Dialog::okRegions2String(const QList<QList<int>>& regionLins) {
    QString result;
    bool first = true;
    for (const auto& numList : regionLins) {
        if (!first) {
            result += " ";
        }
        for (int num : numList) {
            result += QString::number(num);
            result += ",";
        }
        first = false;
    }
    return result;
}

bool Primer3Dialog::parseIntervalList(const QString& inputString, const QString& delimiter, QList<U2Region>* outputList, IntervalDefinition definition) {
    QList<U2Region> result;
    QStringList intervalStringList = inputString.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    for (const auto& intervalString : intervalStringList) {
        QStringList valueStringList = intervalString.split(delimiter);
        if (2 != valueStringList.size()) {
            return false;
        }
        int firstValue = 0;
        {
            bool ok = false;
            firstValue = valueStringList[0].toInt(&ok);
            if (!ok) {
                return false;
            }
        }
        int secondValue = 0;
        {
            bool ok = false;
            secondValue = valueStringList[1].toInt(&ok);
            if (!ok) {
                return false;
            }
        }
        if (definition == IntervalDefinition::Start_End) {
            result.append(U2Region(firstValue, secondValue - firstValue + 1));
        } else {
            result.append(U2Region(firstValue, secondValue));
        }
    }
    *outputList = result;
    return true;
}

bool Primer3Dialog::parseIntList(const QString& inputString, QList<int>* outputList) {
    QList<int> result;
    QStringList intStringList = inputString.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    for (const auto& numString : intStringList) {
        bool ok = false;
        int num = numString.toInt(&ok);
        if (!ok) {
            return false;
        }

        result << num;
    }
    *outputList = result;
    return true;
}

bool Primer3Dialog::parseOkRegions(const QString& inputString, QList<QList<int>>* outputList) {
    QList<QList<int>> result;
    QStringList intStringList = inputString.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    for (const auto& numList : intStringList) {
        QStringList numStringList = numList.split(",");
        if (numStringList.size() != 4) {
            return false;
        }

        QList<int> res;
        for (int i = 0; i < 4; i++) {
            bool ok = false;
            res << numStringList[i].toInt(&ok);
            if (!ok) {
                return false;
            }
        }
        
        result << res;
    }
    *outputList = result;
    return true;
}

void Primer3Dialog::reset() {
    for (const auto& key : defaultSettings.getIntPropertyList()) {
        int value = 0;
        if (defaultSettings.getIntProperty(key, &value)) {
            QSpinBox* spinBox = findChild<QSpinBox*>("edit_" + key);
            if (spinBox != nullptr) {
                spinBox->setValue(value);
            }
        }
    }
    for (const auto& key : defaultSettings.getDoublePropertyList()) {
        double value = 0;
        if (defaultSettings.getDoubleProperty(key, &value)) {
            QDoubleSpinBox* spinBox = findChild<QDoubleSpinBox*>("edit_" + key);
            if (spinBox != nullptr) {
                spinBox->setValue(value);
            }
        }
    }

    edit_SEQUENCE_TARGET->setText(intervalListToString(defaultSettings.getTarget(), ","));
    edit_SEQUENCE_OVERLAP_JUNCTION_LIST->setText(intListToString(defaultSettings.getOverlapJunctionList(), ""));
    edit_SEQUENCE_EXCLUDED_REGION->setText(intervalListToString(defaultSettings.getExcludedRegion(), ","));
    edit_SEQUENCE_PRIMER_PAIR_OK_REGION_LIST->setText(okRegions2String(defaultSettings.getOkRegion()));
    edit_PRIMER_PRODUCT_SIZE_RANGE->setText(intervalListToString(defaultSettings.getProductSizeRange(), "-", IntervalDefinition::Start_End));
    edit_SEQUENCE_INTERNAL_EXCLUDED_REGION->setText(intervalListToString(defaultSettings.getInternalOligoExcludedRegion(), ","));
    edit_PRIMER_MUST_MATCH_FIVE_PRIME->setText(defaultSettings.getPrimerMustMatchFivePrime());
    edit_PRIMER_MUST_MATCH_THREE_PRIME->setText(defaultSettings.getPrimerMustMatchThreePrime());
    edit_PRIMER_INTERNAL_MUST_MATCH_FIVE_PRIME->setText(defaultSettings.getInternalPrimerMustMatchFivePrime());
    edit_PRIMER_INTERNAL_MUST_MATCH_THREE_PRIME->setText(defaultSettings.getInternalPrimerMustMatchThreePrime());
    edit_SEQUENCE_PRIMER->setText(defaultSettings.getLeftInput());
    edit_SEQUENCE_PRIMER_REVCOMP->setText(defaultSettings.getRightInput());
    edit_SEQUENCE_INTERNAL_OLIGO->setText(defaultSettings.getInternalInput());

    {
        QString qualityString;
        bool first = true;
        for (int qualityValue : defaultSettings.getSequenceQuality()) {
            if (!first) {
                qualityString += " ";
            }
            qualityString += QString::number(qualityValue);
            first = false;
        }
        edit_SEQUENCE_QUALITY->setPlainText(qualityString);
    }

    //TODO: for
    {
        int value = 0;
        defaultSettings.getIntProperty("PRIMER_TM_FORMULA", &value);
        combobox_PRIMER_TM_FORMULA->setCurrentIndex(value);
    }
    {
        int value = 0;
        defaultSettings.getIntProperty("PRIMER_SALT_CORRECTIONS", &value);
        combobox_PRIMER_SALT_CORRECTIONS->setCurrentIndex(value);
    }
    {
        int value = 0;
        defaultSettings.getIntProperty("PRIMER_LIBERAL_BASE", &value);
        checkbox_PRIMER_LIBERAL_BASE->setChecked(value);
    }
    {
        int value = 0;
        defaultSettings.getIntProperty("PRIMER_THERMODYNAMIC_OLIGO_ALIGNMENT", &value);
        checkbox_PRIMER_THERMODYNAMIC_OLIGO_ALIGNMENT->setChecked(value);
    }
    {
        int value = 0;
        defaultSettings.getIntProperty("PRIMER_THERMODYNAMIC_TEMPLATE_ALIGNMENT", &value);
        checkbox_PRIMER_THERMODYNAMIC_TEMPLATE_ALIGNMENT->setChecked(value);
    }
    {
        int value = 0;
        defaultSettings.getIntProperty("PRIMER_LIB_AMBIGUITY_CODES_CONSENSUS", &value);
        checkbox_PRIMER_LIB_AMBIGUITY_CODES_CONSENSUS->setChecked(value);
    }
    {
        int value = 0;
        defaultSettings.getIntProperty("PRIMER_LOWERCASE_MASKING", &value);
        checkbox_PRIMER_LOWERCASE_MASKING->setChecked(value);
    }
    {
        int value = 0;
        defaultSettings.getIntProperty("PRIMER_PICK_ANYWAY", &value);
        checkbox_PRIMER_PICK_ANYWAY->setChecked(value);
    }


    {
        task task = task::generic;
        switch (defaultSettings.getTask()) {
            // Shouldn't be here cuz deprecated
            /*case pick_pcr_primers_and_hyb_probe:
                checkbox_PRIMER_PICK_LEFT_PRIMER->setChecked(true);
                checkbox_PRIMER_PICK_RIGHT_PRIMER->setChecked(true);
                checkbox_PRIMER_PICK_INTERNAL_OLIGO->setChecked(true);
                break; 
            case pick_left_only:
                checkbox_PRIMER_PICK_LEFT_PRIMER->setChecked(true);
                checkbox_PRIMER_PICK_RIGHT_PRIMER->setChecked(false);
                checkbox_PRIMER_PICK_INTERNAL_OLIGO->setChecked(false);
                break;
            case pick_right_only:
                checkbox_PRIMER_PICK_LEFT_PRIMER->setChecked(false);
                checkbox_PRIMER_PICK_RIGHT_PRIMER->setChecked(true);
                checkbox_PRIMER_PICK_INTERNAL_OLIGO->setChecked(false);
                break;
            case pick_hyb_probe_only:
                checkbox_PRIMER_PICK_LEFT_PRIMER->setChecked(false);
                checkbox_PRIMER_PICK_RIGHT_PRIMER->setChecked(false);
                checkbox_PRIMER_PICK_INTERNAL_OLIGO->setChecked(true);
                break;
            case pick_pcr_primers:
                [[fallthrough]]*/
            case generic:
                checkbox_PRIMER_PICK_LEFT_PRIMER->setChecked(true);
                checkbox_PRIMER_PICK_RIGHT_PRIMER->setChecked(true);
                checkbox_PRIMER_PICK_INTERNAL_OLIGO->setChecked(false);
                task = task::generic;
                break;
            //TODO: Could be calculated from parameters
            /*case pick_cloning_primers:

                break;
            case pick_discriminative_primers:

                break;*/
            case pick_sequencing_primers:

                task = task::pick_sequencing_primers;
                break;
            case pick_primer_list:

                task = task::pick_primer_list;
                break;

            case check_primers:

                task = task::check_primers;
                break;
            default:
                coreLog.error(tr("The unknown or deprecated Primer3 task"));            
        }
    }
    edit_SEQUENCE_PRIMER->setEnabled(checkbox_PRIMER_PICK_LEFT_PRIMER->isChecked());
    label_PRIMER_LEFT_INPUT->setEnabled(checkbox_PRIMER_PICK_LEFT_PRIMER->isChecked());
    edit_SEQUENCE_PRIMER_REVCOMP->setEnabled(checkbox_PRIMER_PICK_RIGHT_PRIMER->isChecked());
    label_PRIMER_RIGHT_INPUT->setEnabled(checkbox_PRIMER_PICK_RIGHT_PRIMER->isChecked());
    edit_SEQUENCE_INTERNAL_OLIGO->setEnabled(checkbox_PRIMER_PICK_INTERNAL_OLIGO->isChecked());
    label_PRIMER_INTERNAL_OLIGO_INPUT->setEnabled(checkbox_PRIMER_PICK_INTERNAL_OLIGO->isChecked());

    combobox_PRIMER_MISPRIMING_LIBRARY->setCurrentIndex(0);
    combobox_PRIMER_INTERNAL_MISHYB_LIBRARY->setCurrentIndex(0);
    {
        for (int i = 0; i < repeatLibraries.size(); i++) {
            if (repeatLibraries[i].second == settings.getRepeatLibraryPath()) {
                combobox_PRIMER_MISPRIMING_LIBRARY->setCurrentIndex(i);
            }
            if (repeatLibraries[i].second == settings.getMishybLibraryPath()) {
                combobox_PRIMER_INTERNAL_MISHYB_LIBRARY->setCurrentIndex(i);
            }
        }
    }
}

static U2Range<int> parseExonRange(const QString& text, bool& ok) {
    U2Range<int> res;
    ok = true;

    if (text.size() > 0) {
        QStringList items = text.split("-");
        if (items.size() != 2) {
            ok = false;
            return res;
        }

        int startExon = items[0].toInt(&ok);
        if (!ok) {
            return res;
        }

        int endExon = items[1].toInt(&ok);
        if (!ok) {
            return res;
        }

        res.minValue = startExon;
        res.maxValue = endExon;
        ok = startExon <= endExon && startExon > 0;
    }

    return res;
}

bool Primer3Dialog::doDataExchange() {
    settings = defaultSettings;
    if (spanIntronExonBox->isChecked()) {
        SpanIntronExonBoundarySettings s;
        s.enabled = true;
        s.exonAnnotationName = exonNameEdit->text();
        s.maxPairsToQuery = maxPairsBox->value();
        s.minLeftOverlap = leftOverlapSizeSpinBox->value();
        s.minRightOverlap = rightOverlapSizeSpinBox->value();
        s.spanIntron = spanIntronCheckBox->isChecked();
        s.overlapExonExonBoundary = spanJunctionBox->isChecked();

        bool ok = false;
        s.exonRange = parseExonRange(exonRangeEdit->text().trimmed(), ok);
        if (!ok) {
            showInvalidInputMessage(exonRangeEdit, "Exon range");
            return false;
        }

        settings.setSpanIntronExonBoundarySettings(s);
    }
    for (const auto& key : settings.getIntPropertyList()) {
        QSpinBox* spinBox = findChild<QSpinBox*>("edit_" + key);
        if (nullptr != spinBox) {
            settings.setIntProperty(key, spinBox->value());
        }
    }
    for (const auto& key : settings.getDoublePropertyList()) {
        QDoubleSpinBox* spinBox = findChild<QDoubleSpinBox*>("edit_" + key);
        if (nullptr != spinBox) {
            settings.setDoubleProperty(key, spinBox->value());
        }
    }

    {
        QList<U2Region> list;
        if (parseIntervalList(edit_SEQUENCE_TARGET->text(), ",", &list)) {
            settings.setTarget(list);
        } else {
            showInvalidInputMessage(edit_SEQUENCE_TARGET, tr("Targets"));
            return false;
        }
    }
    {
        QList<int> list;
        if (parseIntList(edit_SEQUENCE_OVERLAP_JUNCTION_LIST->text(), &list)) {
            settings.setOverlapJunctionList(list);
        } else {
            showInvalidInputMessage(edit_SEQUENCE_OVERLAP_JUNCTION_LIST, tr("Overlap Junction List"));
            return false;
        }
    }
    {
        QList<U2Region> list;
        if (parseIntervalList(edit_SEQUENCE_EXCLUDED_REGION->text(), ",", &list)) {
            settings.setExcludedRegion(list);
        } else {
            showInvalidInputMessage(edit_SEQUENCE_EXCLUDED_REGION, tr("Excluded Regions"));
            return false;
        }
    }
    {
        QList<QList<int>> list;
        if (parseOkRegions(edit_SEQUENCE_PRIMER_PAIR_OK_REGION_LIST->text(), &list)) {
            settings.setOkRegion(list);
        } else {
            showInvalidInputMessage(edit_SEQUENCE_PRIMER_PAIR_OK_REGION_LIST, tr("Pair OK Region List"));
            return false;
        }
    }
    {
        QString text = edit_PRIMER_MUST_MATCH_FIVE_PRIME->text();
        if (!text.isEmpty()) {
            if (MUST_MATCH_END_REGEX.match(text).hasMatch()) {
                settings.setPrimerMustMatchFivePrime(text.toLocal8Bit());
            } else {
                showInvalidInputMessage(edit_PRIMER_MUST_MATCH_FIVE_PRIME, tr("Five Matches on Primer's 5'"));
                return false;
            }
        }
    }
    {
        QString text = edit_PRIMER_MUST_MATCH_THREE_PRIME->text();
        if (!text.isEmpty()) {
            if (MUST_MATCH_END_REGEX.match(text).hasMatch()) {
                settings.setPrimerMustMatchThreePrime(text.toLocal8Bit());
            } else {
                showInvalidInputMessage(edit_PRIMER_MUST_MATCH_THREE_PRIME, tr("Five Matches on Primer's 3'"));
                return false;
            }
        }
    }
    {
        QString text = edit_PRIMER_INTERNAL_MUST_MATCH_FIVE_PRIME->text();
        if (!text.isEmpty()) {
            if (!text.isEmpty() && MUST_MATCH_END_REGEX.match(text).hasMatch()) {
                settings.setInternalPrimerMustMatchFivePrime(text.toLocal8Bit());
            } else {
                showInvalidInputMessage(edit_PRIMER_INTERNAL_MUST_MATCH_FIVE_PRIME, tr("Five Matches on Internal Oligo's  5'"));
                return false;
            }
        }
    }
    {
        QString text = edit_PRIMER_INTERNAL_MUST_MATCH_THREE_PRIME->text();
        if (!text.isEmpty()) {
            if (!text.isEmpty() && MUST_MATCH_END_REGEX.match(text).hasMatch()) {
                settings.setInternalPrimerMustMatchThreePrime(text.toLocal8Bit());
            } else {
                showInvalidInputMessage(edit_PRIMER_INTERNAL_MUST_MATCH_THREE_PRIME, tr("Five Matches on Internal Oligo's 3'"));
                return false;
            }
        }
    }

    if (checkbox_PRIMER_PICK_LEFT_PRIMER->isChecked()) {
        settings.setLeftInput(edit_SEQUENCE_PRIMER->text().toLatin1());
    } else {
        settings.setLeftInput("");
    }
    if (checkbox_PRIMER_PICK_INTERNAL_OLIGO->isChecked()) {
        settings.setInternalInput(edit_SEQUENCE_INTERNAL_OLIGO->text().toLatin1());
    } else {
        settings.setInternalInput("");
    }
    if (checkbox_PRIMER_PICK_RIGHT_PRIMER->isChecked()) {
        settings.setRightInput(edit_SEQUENCE_PRIMER_REVCOMP->text().toLatin1());
    } else {
        settings.setRightInput("");
    }

    {
        QVector<int> qualityList;
        QStringList stringList = edit_SEQUENCE_QUALITY->toPlainText().split(QRegExp("\\s+"), QString::SkipEmptyParts);
        foreach (QString string, stringList) {
            bool ok = false;
            int value = string.toInt(&ok);
            if (!ok) {
                showInvalidInputMessage(edit_SEQUENCE_QUALITY, tr("Sequence Quality"));
                return false;
            }
            qualityList.append(value);
        }
        if (!qualityList.isEmpty() && (qualityList.size() != (rs->getRegion().length)))  // todo add check on wrong region
        {
            QMessageBox::critical(this, windowTitle(), tr("Sequence quality list length must be equal to the sequence length"));
            return false;
        }
        settings.setSequenceQuality(qualityList);
    }

    settings.setIntProperty("PRIMER_TM_FORMULA", combobox_PRIMER_TM_FORMULA->currentIndex());
    settings.setIntProperty("PRIMER_SALT_CORRECTIONS", combobox_PRIMER_SALT_CORRECTIONS->currentIndex());
    settings.setIntProperty("PRIMER_LIBERAL_BASE", checkbox_PRIMER_LIBERAL_BASE->isChecked());
    settings.setIntProperty("PRIMER_THERMODYNAMIC_OLIGO_ALIGNMENT", checkbox_PRIMER_THERMODYNAMIC_OLIGO_ALIGNMENT->isChecked());
    settings.setIntProperty("PRIMER_THERMODYNAMIC_TEMPLATE_ALIGNMENT", checkbox_PRIMER_THERMODYNAMIC_TEMPLATE_ALIGNMENT->isChecked());
    settings.setIntProperty("PRIMER_LIB_AMBIGUITY_CODES_CONSENSUS", checkbox_PRIMER_LIB_AMBIGUITY_CODES_CONSENSUS->isChecked());
    settings.setIntProperty("PRIMER_LOWERCASE_MASKING", checkbox_PRIMER_LOWERCASE_MASKING->isChecked());
    settings.setIntProperty("PRIMER_PICK_ANYWAY", checkbox_PRIMER_PICK_ANYWAY->isChecked());
    settings.setIntProperty("PRIMER_SECONDARY_STRUCTURE_ALIGNMENT", checkbox_PRIMER_SECONDARY_STRUCTURE_ALIGNMENT->isChecked());

    settings.setShowDebugging(checkbox_SHOW_DEBUGGING->isChecked());
    settings.setFormatOutput(checkbox_FORMAT_OUTPUT->isChecked());
    settings.setExplain(checkbox_PRIMER_EXPLAIN_FLAG->isChecked());

    settings.setTaskByName(edit_PRIMER_TASK->currentText());
    switch (settings.getTask()) {
    case pick_discriminative_primers:
        if (settings.getSeqArgs()->tar2.count != 1) {
            QMessageBox::critical(this, windowTitle(), tr("Task \"pick_discriminative_primers\" requires exactly one \"Targets\" region."));
            return false;
        }
    case pick_cloning_primers:
    case generic:
    case pick_sequencing_primers:
    case pick_primer_list:
        if (!(checkbox_PRIMER_PICK_LEFT_PRIMER->isChecked() ||
            checkbox_PRIMER_PICK_INTERNAL_OLIGO->isChecked() ||
            checkbox_PRIMER_PICK_RIGHT_PRIMER->isChecked())) {
            QMessageBox::critical(this, windowTitle(), tr("At least one primer on the \"Main\" settings page should be presented."));
            return false;
        }
        break;
    case check_primers:
        if ((checkbox_PRIMER_PICK_LEFT_PRIMER->isChecked() && edit_SEQUENCE_PRIMER->text().isEmpty()) ||
            (checkbox_PRIMER_PICK_INTERNAL_OLIGO->isChecked() && edit_SEQUENCE_INTERNAL_OLIGO->text().isEmpty()) ||
            (checkbox_PRIMER_PICK_RIGHT_PRIMER->isChecked() && edit_SEQUENCE_PRIMER_REVCOMP->text().isEmpty())) {
            QMessageBox::critical(this, windowTitle(), tr("Some primers on the \"Main\" settings page are enabled, but not presented."));
            return false;
        }
        break;
    default:
        showInvalidInputMessage(edit_PRIMER_TASK, tr("Primer3 task"));
        return false;
    }

    {
        int index = combobox_PRIMER_MISPRIMING_LIBRARY->currentIndex();
        settings.setRepeatLibraryPath(repeatLibraries[index].second);
    }
    {
        int index = combobox_PRIMER_INTERNAL_MISHYB_LIBRARY->currentIndex();
        settings.setMishybLibraryPath(repeatLibraries[index].second);
    }

    {
        QList<U2Region> list;
        if (parseIntervalList(edit_PRIMER_PRODUCT_SIZE_RANGE->text(), "-", &list, IntervalDefinition::Start_End)) {
            settings.setProductSizeRange(list);
            bool isRegionOk = false;
            U2Region includedRegion = rs->getRegion(&isRegionOk);
            if (!isRegionOk) {
                rs->showErrorMessage();
                return false;
            }
            if (!settings.isIncludedRegionValid(includedRegion)) {
                QMessageBox::critical(this, windowTitle(), tr("Included region is too small for current product size ranges"));
                return false;
            }
            settings.setIncludedRegion(includedRegion.startPos + settings.getFirstBaseIndex(), includedRegion.length);
        } else {
            showInvalidInputMessage(edit_PRIMER_PRODUCT_SIZE_RANGE, tr("Product Size Ranges"));
            return false;
        }
    }
    return true;
}

void Primer3Dialog::showInvalidInputMessage(QWidget* field, QString fieldLabel) {
    tabWidget->setCurrentWidget(field->parentWidget());
    field->setFocus(Qt::OtherFocusReason);
    QMessageBox::critical(this, windowTitle(), tr("The field '%1' has invalid value").arg(fieldLabel));
}

void Primer3Dialog::sl_pbPick_clicked() {
    bool isRegionOk = false;
    rs->getRegion(&isRegionOk);
    if (!isRegionOk) {
        rs->showErrorMessage();
        return;
    }
    if (doDataExchange()) {
        bool objectPrepared = createAnnotationWidgetController->prepareAnnotationObject();
        if (!objectPrepared) {
            QMessageBox::critical(this, L10N::errorTitle(), tr("Cannot create an annotation object. Please check settings."));
            return;
        }
        const CreateAnnotationModel& m = createAnnotationWidgetController->getModel();
        AnnotationTableObject* obj = m.getAnnotationObject();
        context->getAnnotatedDNAView()->tryAddObject(obj);
        accept();
    }
}

void Primer3Dialog::sl_saveSettings() {
    LastUsedDirHelper lod;
    QString fileName = U2FileDialog::getSaveFileName(this, tr("Save primer settings"), lod.dir, "Text files (*.txt)");
    if (!fileName.endsWith(".txt")) {
        fileName += ".txt";
    }

    QSettings diagSettings(fileName, QSettings::IniFormat);

    for (const auto& key : settings.getIntPropertyList()) {
        QSpinBox* spinBox = findChild<QSpinBox*>("edit_" + key);
        if (nullptr != spinBox) {
            diagSettings.setValue(key, spinBox->value());
        }
    }
    for (const auto& key : settings.getDoublePropertyList()) {
        QDoubleSpinBox* spinBox = findChild<QDoubleSpinBox*>("edit_" + key);
        if (nullptr != spinBox) {
            diagSettings.setValue(key, spinBox->value());
        }
    }

    diagSettings.setValue("PRIMER_TASK", edit_PRIMER_TASK->currentText());
    diagSettings.setValue("SEQUENCE_PRIMER", edit_SEQUENCE_PRIMER->text());
    diagSettings.setValue("SEQUENCE_INTERNAL_OLIGO", edit_SEQUENCE_INTERNAL_OLIGO->text());
    diagSettings.setValue("SEQUENCE_PRIMER_REVCOMP", edit_SEQUENCE_PRIMER_REVCOMP->text());
    diagSettings.setValue("SEQUENCE_TARGET", edit_SEQUENCE_TARGET->text());
    diagSettings.setValue("SEQUENCE_OVERLAP_JUNCTION_LIST", edit_SEQUENCE_OVERLAP_JUNCTION_LIST->text());
    diagSettings.setValue("SEQUENCE_EXCLUDED_REGION", edit_SEQUENCE_EXCLUDED_REGION->text());
    diagSettings.setValue("SEQUENCE_PRIMER_PAIR_OK_REGION_LIST", edit_SEQUENCE_PRIMER_PAIR_OK_REGION_LIST->text());
    diagSettings.setValue("PRIMER_MUST_MATCH_FIVE_PRIME", edit_PRIMER_MUST_MATCH_FIVE_PRIME->text());
    diagSettings.setValue("PRIMER_MUST_MATCH_THREE_PRIME", edit_PRIMER_MUST_MATCH_THREE_PRIME->text());
    diagSettings.setValue("PRIMER_INTERNAL_MUST_MATCH_FIVE_PRIME", edit_PRIMER_INTERNAL_MUST_MATCH_FIVE_PRIME->text());
    diagSettings.setValue("PRIMER_INTERNAL_MUST_MATCH_THREE_PRIME", edit_PRIMER_INTERNAL_MUST_MATCH_THREE_PRIME->text());
    diagSettings.setValue("PRIMER_PRODUCT_SIZE_RANGE", edit_PRIMER_PRODUCT_SIZE_RANGE->text());

    diagSettings.setValue("SEQUENCE_INTERNAL_EXCLUDED_REGION", edit_SEQUENCE_INTERNAL_EXCLUDED_REGION->text());
    diagSettings.setValue("SEQUENCE_QUALITY", edit_SEQUENCE_QUALITY->toPlainText());

    diagSettings.setValue("PRIMER_TM_FORMULA", combobox_PRIMER_TM_FORMULA->currentIndex());
    diagSettings.setValue("PRIMER_SALT_CORRECTIONS", combobox_PRIMER_SALT_CORRECTIONS->currentIndex());

    diagSettings.setValue("PRIMER_PICK_LEFT_PRIMER", checkbox_PRIMER_PICK_LEFT_PRIMER->isChecked());
    diagSettings.setValue("PRIMER_PICK_INTERNAL_OLIGO", checkbox_PRIMER_PICK_INTERNAL_OLIGO->isChecked());
    diagSettings.setValue("PRIMER_PICK_RIGHT_PRIMER", checkbox_PRIMER_PICK_RIGHT_PRIMER->isChecked());
    diagSettings.setValue("PRIMER_LIBERAL_BASE", checkbox_PRIMER_LIBERAL_BASE->isChecked());
    diagSettings.setValue("PRIMER_THERMODYNAMIC_OLIGO_ALIGNMENT", checkbox_PRIMER_THERMODYNAMIC_OLIGO_ALIGNMENT->isChecked());
    diagSettings.setValue("PRIMER_THERMODYNAMIC_TEMPLATE_ALIGNMENT", checkbox_PRIMER_THERMODYNAMIC_TEMPLATE_ALIGNMENT->isChecked());

    diagSettings.setValue("PRIMER_LIB_AMBIGUITY_CODES_CONSENSUS", checkbox_PRIMER_LIB_AMBIGUITY_CODES_CONSENSUS->isChecked());
    diagSettings.setValue("PRIMER_LOWERCASE_MASKING", checkbox_PRIMER_LOWERCASE_MASKING->isChecked());
    diagSettings.setValue("PRIMER_PICK_ANYWAY", checkbox_PRIMER_PICK_ANYWAY->isChecked());
    diagSettings.setValue("PRIMER_EXPLAIN_FLAG", checkbox_PRIMER_EXPLAIN_FLAG->isChecked());
    diagSettings.setValue("PRIMER_SECONDARY_STRUCTURE_ALIGNMENT", checkbox_PRIMER_SECONDARY_STRUCTURE_ALIGNMENT->isChecked());

    QString pathPrimerMisprimingLibrary;
    QString pathPrimerInternalOligoLibrary;
    for (const auto& lib : repeatLibraries) {
        if (lib.first == combobox_PRIMER_MISPRIMING_LIBRARY->currentText()) {
            pathPrimerMisprimingLibrary = lib.second;
        }
        if (lib.first == combobox_PRIMER_MISPRIMING_LIBRARY->currentText()) {
            pathPrimerInternalOligoLibrary = lib.second;
        }
    }
    diagSettings.setValue("PRIMER_MISPRIMING_LIBRARY", pathPrimerMisprimingLibrary);
    diagSettings.setValue("PRIMER_INTERNAL_MISHYB_LIBRARY", pathPrimerInternalOligoLibrary);
}

void Primer3Dialog::sl_loadSettings() {
    LastUsedDirHelper lod;
    lod.url = U2FileDialog::getOpenFileName(this, tr("Load settings"), lod.dir, "Text files (*.txt)");
    if (lod.url.isNull()) {  // user clicked 'Cancel' button
        return;
    }

    QSettings diagSettings(lod.url, QSettings::IniFormat);

    QStringList groupKeys = diagSettings.childKeys();
    for (const auto& key : groupKeys) {
        QSpinBox* spinBox = findChild<QSpinBox*>("edit_" + key);
        if (spinBox != nullptr && groupKeys.contains(key)) {
            spinBox->setValue(diagSettings.value(key).toInt());
        }
        QDoubleSpinBox* doubleSpinBox = findChild<QDoubleSpinBox*>("edit_" + key);
        if (doubleSpinBox != nullptr && groupKeys.contains(key)) {
            doubleSpinBox->setValue(diagSettings.value(key).toDouble());
        }
    }


    /*diagSettings.beginGroup("GeneralProperties");
    groupKeys = diagSettings.childKeys();
    ui.edit_PRIMER_START_CODON_POSITION->setText(
        diagSettings.value("PRIMER_START_CODON_POSITION").toString());
    ui.edit_PRIMER_PRODUCT_MIN_TM->setText(
        diagSettings.value("PRIMER_PRODUCT_MIN_TM").toString());
    ui.edit_PRIMER_PRODUCT_OPT_TM->setText(
        diagSettings.value("PRIMER_PRODUCT_OPT_TM").toString());
    ui.edit_PRIMER_PRODUCT_MAX_TM->setText(
        diagSettings.value("PRIMER_PRODUCT_MAX_TM").toString());
    ui.edit_PRIMER_OPT_GC_PERCENT->setText(
        diagSettings.value("PRIMER_OPT_GC_PERCENT").toString());
    ui.edit_PRIMER_INSIDE_PENALTY->setText(
        diagSettings.value("PRIMER_INSIDE_PENALTY").toString());
    ui.edit_PRIMER_INTERNAL_OLIGO_OPT_GC_PERCENT->setText(
        diagSettings.value("PRIMER_INTERNAL_OLIGO_OPT_GC_PERCENT").toString());
    ui.edit_EXCLUDED_REGION->setText(
        diagSettings.value("EXCLUDED_REGION").toString());
    ui.edit_PRIMER_INTERNAL_OLIGO_EXCLUDED_REGION->setText(
        diagSettings.value("PRIMER_INTERNAL_OLIGO_EXCLUDED_REGION").toString());
    ui.edit_TARGET->setText(diagSettings.value("TARGET").toString());
    ui.edit_PRIMER_PRODUCT_SIZE_RANGE->setText(
        diagSettings.value("PRIMER_PRODUCT_SIZE_RANGE").toString());
    ui.edit_PRIMER_LEFT_INPUT->setText(diagSettings.value("PRIMER_LEFT_INPUT").toString());
    ui.edit_PRIMER_RIGHT_INPUT->setText(diagSettings.value("PRIMER_RIGHT_INPUT").toString());
    ui.edit_PRIMER_INTERNAL_OLIGO_INPUT->setText(
        diagSettings.value("PRIMER_INTERNAL_OLIGO_INPUT").toString());

    ui.combobox_PRIMER_TM_SANTALUCIA->setCurrentIndex(
        diagSettings.value("PRIMER_TM_FORMULA").toInt());
    ui.combobox_PRIMER_SALT_CORRECTIONS->setCurrentIndex(
        diagSettings.value("PRIMER_SALT_CORRECTIONS").toInt());

    ui.checkbox_PRIMER_LIBERAL_BASE->setChecked(
        diagSettings.value("PRIMER_LIBERAL_BASE").toBool());
    ui.checkbox_PRIMER_LIB_AMBIGUITY_CODES_CONSENSUS->setChecked(
        diagSettings.value("PRIMER_LIB_AMBIGUITY_CODES_CONSENSUS").toBool());
    ui.checkbox_PRIMER_LOWERCASE_MASKING->setChecked(
        diagSettings.value("PRIMER_LOWERCASE_MASKING").toBool());
    ui.checkbox_PICK_HYBRO->setChecked(diagSettings.value("PICK_HYBRO").toBool());
    ui.checkbox_PICK_LEFT->setChecked(diagSettings.value("PICK_LEFT").toBool());
    ui.checkbox_PICK_RIGHT->setChecked(diagSettings.value("PICK_RIGHT").toBool());

    ui.combobox_PRIMER_MISPRIMING_LIBRARY->setCurrentIndex(
        diagSettings.value("PRIMER_MISPRIMING_LIBRARY").toInt());
    ui.combobox_PRIMER_INTERNAL_OLIGO_MISHYB_LIBRARY->setCurrentIndex(
        diagSettings.value("PRIMER_INTERNAL_OLIGO_MISHYB_LIBRARY").toInt());

    diagSettings.endGroup();*/
}

void Primer3Dialog::sl_pbReset_clicked() {
    reset();
    rs->reset();
}

QString Primer3Dialog::checkModel() {
    return createAnnotationWidgetController->validate();
}

}  // namespace U2
