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
#include <QTextStream>

#include <U2Algorithm/SplicedAlignmentTaskRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/L10n.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>

#include <U2View/AnnotatedDNAView.h>

#include "Primer3Dialog.h"

namespace U2 {

const QMap<task, QString> Primer3Dialog::TASK_ENUM_STRING_MAP = {
                        {task::generic, "generic"},
                        {task::pick_sequencing_primers,"pick_sequencing_primers"},
                        {task::pick_primer_list, "pick_primer_list"},
                        {task::check_primers, "check_primers"},
                        {task::pick_cloning_primers, "pick_cloning_primers"},
                        {task::pick_discriminative_primers, "pick_discriminative_primers"}
    };

const QStringList Primer3Dialog::LINE_EDIT_PARAMETERS =
                        { "SEQUENCE_PRIMER",
                          "SEQUENCE_INTERNAL_OLIGO",
                          "SEQUENCE_PRIMER_REVCOMP",
                          "SEQUENCE_OVERHANG_LEFT",
                          "SEQUENCE_OVERHANG_RIGHT",
                          "SEQUENCE_TARGET",
                          "SEQUENCE_OVERLAP_JUNCTION_LIST",
                          "SEQUENCE_INTERNAL_OVERLAP_JUNCTION_LIST",
                          "SEQUENCE_EXCLUDED_REGION",
                          "SEQUENCE_PRIMER_PAIR_OK_REGION_LIST",
                          "SEQUENCE_INCLUDED_REGION",
                          "SEQUENCE_INTERNAL_EXCLUDED_REGION",
                          "SEQUENCE_START_CODON_SEQUENCE",
                          "PRIMER_MUST_MATCH_FIVE_PRIME",
                          "PRIMER_MUST_MATCH_THREE_PRIME",
                          "PRIMER_INTERNAL_MUST_MATCH_FIVE_PRIME",
                          "PRIMER_INTERNAL_MUST_MATCH_THREE_PRIME",
                          "PRIMER_PRODUCT_SIZE_RANGE",
    };

const QRegularExpression Primer3Dialog::MUST_MATCH_END_REGEX("^([NAGCTRYWSMKBHDV]){5}$");
const QRegularExpression Primer3Dialog::MUST_MATCH_START_CODON_SEQUENCE_REGEX("^([A-Z]){3}$");

Primer3Dialog::Primer3Dialog(ADVSequenceObjectContext* context)
    : QDialog(context->getAnnotatedDNAView()->getWidget()),
      context(context) {
    setupUi(this);
    new HelpButton(this, helpButton, "65930919");

    pickPrimersButton->setDefault(true);

    connect(pickPrimersButton, &QPushButton::clicked, this, &Primer3Dialog::sl_pickClicked);
    connect(resetButton, &QPushButton::clicked, this, &Primer3Dialog::sl_resetClicked);
    connect(saveSettingsButton, &QPushButton::clicked, this, &Primer3Dialog::sl_saveSettings);
    connect(loadSettingsButton, &QPushButton::clicked, this, &Primer3Dialog::sl_loadSettings);
    connect(edit_PRIMER_TASK, &QComboBox::currentTextChanged, this, &Primer3Dialog::sl_taskChanged);

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
    rs = new RegionSelector(this, context->getSequenceLength(), false, context->getSequenceSelection(), true);
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

    for (const auto& library : repeatLibraries) {
        combobox_PRIMER_MISPRIMING_LIBRARY->addItem(library.first);
        combobox_PRIMER_INTERNAL_MISHYB_LIBRARY->addItem(library.first);
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

Primer3Dialog::~Primer3Dialog() {
    if (settings != nullptr) {
        delete settings;
    }
    rs->deleteLater();
    createAnnotationWidgetController->deleteLater();
}

Primer3TaskSettings* Primer3Dialog::takeSettings() {
    auto returnValue = settings;
    settings = nullptr;
    return returnValue;
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
    QStringList intStringList = inputString.split(";", QString::SkipEmptyParts);
    for (const auto& numList : intStringList) {
        QStringList numStringList = numList.split(",");
        if (numStringList.size() != 4) {
            return false;
        }

        QList<int> res;
        for (int i = 0; i < 4; i++) {
            bool ok = false;
            int v = numStringList[i].toInt(&ok);
            if (!ok) {
                v = -1;
            }
            res << v;
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
                continue;
            }
            QCheckBox* checkBox = findChild<QCheckBox*>("checkbox_" + key);
            if (checkBox != nullptr) {
                checkBox->setChecked(value);
            }
        }
    }
    for (const auto& key : defaultSettings.getDoublePropertyList()) {
        double value = 0;
        if (defaultSettings.getDoubleProperty(key, &value)) {
            QCheckBox* checkBox = findChild<QCheckBox*>("label_" + key);
            if (checkBox != nullptr) {
                checkBox->setChecked(false);
            }
            QDoubleSpinBox* spinBox = findChild<QDoubleSpinBox*>("edit_" + key);
            if (spinBox != nullptr) {
                spinBox->setValue(value);
            }
        }
    }

    edit_SEQUENCE_TARGET->setText(intervalListToString(defaultSettings.getTarget(), ","));
    edit_SEQUENCE_OVERLAP_JUNCTION_LIST->setText(intListToString(defaultSettings.getOverlapJunctionList(), ""));
    edit_SEQUENCE_INTERNAL_OVERLAP_JUNCTION_LIST->setText(intListToString(defaultSettings.getInternalOverlapJunctionList(), ""));
    edit_SEQUENCE_EXCLUDED_REGION->setText(intervalListToString(defaultSettings.getExcludedRegion(), ","));
    edit_SEQUENCE_PRIMER_PAIR_OK_REGION_LIST->setText(okRegions2String(defaultSettings.getOkRegion()));
    edit_PRIMER_PRODUCT_SIZE_RANGE->setText(intervalListToString(defaultSettings.getProductSizeRange(), "-", IntervalDefinition::Start_End));
    edit_SEQUENCE_INTERNAL_EXCLUDED_REGION->setText(intervalListToString(defaultSettings.getInternalOligoExcludedRegion(), ","));
    edit_PRIMER_MUST_MATCH_FIVE_PRIME->setText(defaultSettings.getStartCodonSequence());
    edit_PRIMER_MUST_MATCH_FIVE_PRIME->setText(defaultSettings.getPrimerMustMatchFivePrime());
    edit_PRIMER_MUST_MATCH_THREE_PRIME->setText(defaultSettings.getPrimerMustMatchThreePrime());
    edit_PRIMER_INTERNAL_MUST_MATCH_FIVE_PRIME->setText(defaultSettings.getInternalPrimerMustMatchFivePrime());
    edit_PRIMER_INTERNAL_MUST_MATCH_THREE_PRIME->setText(defaultSettings.getInternalPrimerMustMatchThreePrime());
    edit_SEQUENCE_PRIMER->setText(defaultSettings.getLeftInput());
    edit_SEQUENCE_PRIMER_REVCOMP->setText(defaultSettings.getRightInput());
    edit_SEQUENCE_INTERNAL_OLIGO->setText(defaultSettings.getInternalInput());
    edit_SEQUENCE_OVERHANG_LEFT->setText(defaultSettings.getOverhangLeft());
    edit_SEQUENCE_OVERHANG_RIGHT->setText(defaultSettings.getOverhangRight());
    
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
        QString task = TASK_ENUM_STRING_MAP.value(defaultSettings.getTask(), "generic");
        edit_PRIMER_TASK->setCurrentText(task);
    }

    edit_SEQUENCE_PRIMER->setEnabled(checkbox_PRIMER_PICK_LEFT_PRIMER->isChecked());
    label_PRIMER_LEFT_INPUT->setEnabled(checkbox_PRIMER_PICK_LEFT_PRIMER->isChecked());
    edit_SEQUENCE_OVERHANG_LEFT->setEnabled(checkbox_PRIMER_PICK_LEFT_PRIMER->isChecked());
    label_SEQUENCE_OVERHANG_LEFT->setEnabled(checkbox_PRIMER_PICK_LEFT_PRIMER->isChecked());
    edit_SEQUENCE_PRIMER_REVCOMP->setEnabled(checkbox_PRIMER_PICK_RIGHT_PRIMER->isChecked());
    label_PRIMER_RIGHT_INPUT->setEnabled(checkbox_PRIMER_PICK_RIGHT_PRIMER->isChecked());
    edit_SEQUENCE_OVERHANG_RIGHT->setEnabled(checkbox_PRIMER_PICK_RIGHT_PRIMER->isChecked());
    label_SEQUENCE_OVERHANG_RIGHT->setEnabled(checkbox_PRIMER_PICK_RIGHT_PRIMER->isChecked());
    edit_SEQUENCE_INTERNAL_OLIGO->setEnabled(checkbox_PRIMER_PICK_INTERNAL_OLIGO->isChecked());
    label_PRIMER_INTERNAL_OLIGO_INPUT->setEnabled(checkbox_PRIMER_PICK_INTERNAL_OLIGO->isChecked());

    combobox_PRIMER_MISPRIMING_LIBRARY->setCurrentIndex(0);
    combobox_PRIMER_INTERNAL_MISHYB_LIBRARY->setCurrentIndex(0);
    {
        for (int i = 0; i < repeatLibraries.size(); i++) {
            if (repeatLibraries[i].second == defaultSettings.getRepeatLibraryPath()) {
                combobox_PRIMER_MISPRIMING_LIBRARY->setCurrentIndex(i);
            }
            if (repeatLibraries[i].second == defaultSettings.getMishybLibraryPath()) {
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
    if (settings != nullptr) {
        delete settings;
    }
    settings = new Primer3TaskSettings;

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

        settings->setSpanIntronExonBoundarySettings(s);
    }
    for (const auto& key : settings->getIntPropertyList()) {
        QSpinBox* spinBox = findChild<QSpinBox*>("edit_" + key);
        if (spinBox != nullptr) {
            settings->setIntProperty(key, spinBox->value());
            continue;
        }
        QCheckBox* checkBox = findChild<QCheckBox*>("checkbox_" + key);
        if (checkBox != nullptr) {
            settings->setIntProperty(key, checkBox->isChecked());
            continue;
        }
    }
    for (const auto& key : settings->getDoublePropertyList()) {
        QCheckBox* checkBox = findChild<QCheckBox*>("label_" + key);
        if (checkBox != nullptr && !checkBox->isChecked()) {
            continue;
        }
        QDoubleSpinBox* spinBox = findChild<QDoubleSpinBox*>("edit_" + key);
        if (spinBox != nullptr) {
            settings->setDoubleProperty(key, spinBox->value());
        }
    }

    {
        QList<U2Region> list;
        if (parseIntervalList(edit_SEQUENCE_TARGET->text(), ",", &list)) {
            settings->setTarget(list);
        } else {
            showInvalidInputMessage(edit_SEQUENCE_TARGET, tr("Targets"));
            return false;
        }
    }
    {
        QList<int> list;
        if (parseIntList(edit_SEQUENCE_OVERLAP_JUNCTION_LIST->text(), &list)) {
            settings->setOverlapJunctionList(list);
        } else {
            showInvalidInputMessage(edit_SEQUENCE_OVERLAP_JUNCTION_LIST, tr("Overlap Junction List"));
            return false;
        }
    }
    {
        QList<int> list;
        if (parseIntList(edit_SEQUENCE_INTERNAL_OVERLAP_JUNCTION_LIST->text(), &list)) {
            settings->setInternalOverlapJunctionList(list);
        } else {
            showInvalidInputMessage(edit_SEQUENCE_INTERNAL_OVERLAP_JUNCTION_LIST, tr("Internal Oligo Overlap Positions"));
            return false;
        }
    }
    {
        QList<U2Region> list;
        if (parseIntervalList(edit_SEQUENCE_EXCLUDED_REGION->text(), ",", &list)) {
            settings->setExcludedRegion(list);
        } else {
            showInvalidInputMessage(edit_SEQUENCE_EXCLUDED_REGION, tr("Excluded Regions"));
            return false;
        }
    }
    {
        QList<U2Region> list;
        if (parseIntervalList(edit_SEQUENCE_INCLUDED_REGION->text(), ",", &list)) {
            if (list.size() > 1) {
                QMessageBox::critical(this, windowTitle(), tr("The \"Include region\" should be the only one"));
                return false;
            } else if (list.size() == 1) {
                const auto & region = list.first();
                settings->setIncludedRegion(region.startPos, region.length);
            }
        } else {
            showInvalidInputMessage(edit_SEQUENCE_INCLUDED_REGION, tr("Include Regions"));
            return false;
        }
    }
    {
        QList<QList<int>> list;
        if (parseOkRegions(edit_SEQUENCE_PRIMER_PAIR_OK_REGION_LIST->text(), &list)) {
            settings->setOkRegion(list);
        } else {
            showInvalidInputMessage(edit_SEQUENCE_PRIMER_PAIR_OK_REGION_LIST, tr("Pair OK Region List"));
            return false;
        }
    }
    {
        QString text = edit_SEQUENCE_START_CODON_SEQUENCE->text();
        if (!text.isEmpty()) {
            if (MUST_MATCH_START_CODON_SEQUENCE_REGEX.match(text).hasMatch()) {
                settings->setStartCodonSequence(text.toLocal8Bit());
            } else {
                showInvalidInputMessage(edit_SEQUENCE_START_CODON_SEQUENCE, tr("Start Codon Sequence"));
                return false;
            }
        }
    }
    {
        QString text = edit_PRIMER_MUST_MATCH_FIVE_PRIME->text();
        if (!text.isEmpty()) {
            if (MUST_MATCH_END_REGEX.match(text).hasMatch()) {
                settings->setPrimerMustMatchFivePrime(text.toLocal8Bit());
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
                settings->setPrimerMustMatchThreePrime(text.toLocal8Bit());
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
                settings->setInternalPrimerMustMatchFivePrime(text.toLocal8Bit());
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
                settings->setInternalPrimerMustMatchThreePrime(text.toLocal8Bit());
            } else {
                showInvalidInputMessage(edit_PRIMER_INTERNAL_MUST_MATCH_THREE_PRIME, tr("Five Matches on Internal Oligo's 3'"));
                return false;
            }
        }
    }

    if (checkbox_PRIMER_PICK_LEFT_PRIMER->isChecked()) {
        settings->setLeftInput(edit_SEQUENCE_PRIMER->text().toLatin1());
        settings->setLeftOverhang(edit_SEQUENCE_OVERHANG_LEFT->text().toLatin1());
    } else {
        settings->setLeftInput("");
        settings->setLeftOverhang("");
    }
    if (checkbox_PRIMER_PICK_INTERNAL_OLIGO->isChecked()) {
        settings->setInternalInput(edit_SEQUENCE_INTERNAL_OLIGO->text().toLatin1());
    } else {
        settings->setInternalInput("");
    }
    if (checkbox_PRIMER_PICK_RIGHT_PRIMER->isChecked()) {
        settings->setRightInput(edit_SEQUENCE_PRIMER_REVCOMP->text().toLatin1());
        settings->setRightOverhang(edit_SEQUENCE_OVERHANG_RIGHT->text().toLatin1());
    } else {
        settings->setRightInput("");
        settings->setRightOverhang("");
    }

    {
        QVector<int> qualityList;
        QStringList stringList = edit_SEQUENCE_QUALITY->toPlainText().split(QRegExp("\\s+"), QString::SkipEmptyParts);
        for (const QString& string : stringList) {
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
        settings->setSequenceQuality(qualityList);
    }

    settings->setIntProperty("PRIMER_TM_FORMULA", combobox_PRIMER_TM_FORMULA->currentIndex());
    settings->setIntProperty("PRIMER_SALT_CORRECTIONS", combobox_PRIMER_SALT_CORRECTIONS->currentIndex());

    settings->setShowDebugging(checkbox_SHOW_DEBUGGING->isChecked());
    settings->setFormatOutput(checkbox_FORMAT_OUTPUT->isChecked());
    settings->setExplain(checkbox_PRIMER_EXPLAIN_FLAG->isChecked());

    settings->setTaskByName(edit_PRIMER_TASK->currentText());
    switch (settings->getTask()) {
    case pick_discriminative_primers:
        if (settings->getSeqArgs()->tar2.count != 1) {
            QMessageBox::critical(this, windowTitle(), tr("Task \"pick_discriminative_primers\" requires exactly one \"Targets\" region."));
            return false;
        }
    case pick_cloning_primers:
    case generic:
    case pick_sequencing_primers:
    case pick_primer_list:
    case check_primers:
        if (!(checkbox_PRIMER_PICK_LEFT_PRIMER->isChecked() ||
            checkbox_PRIMER_PICK_INTERNAL_OLIGO->isChecked() ||
            checkbox_PRIMER_PICK_RIGHT_PRIMER->isChecked())) {
            QMessageBox::critical(this, windowTitle(), tr("At least one primer on the \"Main\" settings page should be enabled."));
            return false;
        }
        break;
    default:
        showInvalidInputMessage(edit_PRIMER_TASK, tr("Primer3 task"));
        return false;
    }

    {
        int index = combobox_PRIMER_MISPRIMING_LIBRARY->currentIndex();
        settings->setRepeatLibraryPath(repeatLibraries[index].second);
    }
    {
        int index = combobox_PRIMER_INTERNAL_MISHYB_LIBRARY->currentIndex();
        settings->setMishybLibraryPath(repeatLibraries[index].second);
    }

    {
        QList<U2Region> list;
        if (parseIntervalList(edit_PRIMER_PRODUCT_SIZE_RANGE->text(), "-", &list, IntervalDefinition::Start_End)) {
            if (list.isEmpty()) {
                QMessageBox::critical(this, windowTitle(), tr("Primer Size Ranges should have at least one range"));
                return false;
            }

            settings->setProductSizeRange(list);
            bool isRegionOk = false;
            U2Region sequenceRangeRegion = rs->getRegion(&isRegionOk);
            if (!isRegionOk) {
                rs->showErrorMessage();
                return false;
            }
            if (!settings->isIncludedRegionValid(sequenceRangeRegion)) {
                QMessageBox::critical(this, windowTitle(), tr("Sequence range region is too small for current product size ranges"));
                return false;
            }
            if (sequenceRangeRegion.length > MAXIMUM_ALLOWED_SEQUENCE_LENGTH) {
                QMessageBox::critical(this, windowTitle(), tr("The priming sequence is too long, please, decrease the region"));
                return false;
            }
            
            const auto& includedRegion = settings->getIncludedRegion();
            int fbs = settings->getFirstBaseIndex();
            int includedRegionOffset = includedRegion.startPos != 0 ? includedRegion.startPos - fbs : 0;
            if (includedRegionOffset < 0) {
                QMessageBox::critical(this, windowTitle(), tr("Incorrect summ \"Included Region Start + First Base Index\" - should be more or equal than 0"));
                return false;
            }

            if (sequenceRangeRegion.endPos() > context->getSequenceLength() + includedRegionOffset && !context->getSequenceObject()->isCircular()) {
                QMessageBox::critical(this, windowTitle(), tr("The priming sequence is out of range.\n"
                                                              "Either make the priming region end \"%1\" less or equal than the sequence size \"%2\" plus the first base index value \"%3\""
                                                              "or mark the sequence as circular").arg(sequenceRangeRegion.endPos()).arg(context->getSequenceLength()).arg(settings->getFirstBaseIndex()));
                return false;
            }

            settings->setSequenceRange(sequenceRangeRegion);
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

void Primer3Dialog::sl_pickClicked() {
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

    QFile file(fileName);
    if (file.exists()) {
        file.remove();
    }
    if (!file.open(QIODevice::ReadWrite)) {
        QMessageBox::critical(this, windowTitle(), tr("Can't write to \"%1\"").arg(fileName));
        return;
    }

    QTextStream stream(&file);
    for (const auto& key : defaultSettings.getIntPropertyList()) {
        QSpinBox* spinBox = findChild<QSpinBox*>("edit_" + key);
        if (spinBox != nullptr) {
            stream << key << "=" << spinBox->value() << endl;
            continue;
        }
        QCheckBox* checkbox = findChild<QCheckBox*>("checkbox_" + key);
        if (checkbox != nullptr) {
            stream << key << "=" << (int)checkbox->isChecked() << endl;
        }
    }
    for (const auto& key : defaultSettings.getDoublePropertyList()) {
        QCheckBox* checkBox = findChild<QCheckBox*>("label_" + key);
        if (checkBox != nullptr && !checkBox->isChecked()) {
            continue;
        }
        QDoubleSpinBox* spinBox = findChild<QDoubleSpinBox*>("edit_" + key);
        if (spinBox != nullptr) {
            stream << key << "=" << spinBox->value() << endl;
        }
    }

    for (const auto& par : LINE_EDIT_PARAMETERS) {
        QLineEdit* lineEdit = findChild<QLineEdit*>("edit_" + par);
        if (lineEdit != nullptr) {
            auto text = lineEdit->text();
            if (!text.isEmpty()) {
                stream << par << "=" << text << endl;
            }
        }
    }

    U2OpStatusImpl os;
    stream << "SEQUENCE_TEMPLATE=" << context->getSequenceObject()->getWholeSequenceData(os) << endl;
    stream << "SEQUENCE_ID=" << context->getSequenceObject()->getSequenceName() << endl;

    auto qualityText = edit_SEQUENCE_QUALITY->toPlainText();
    if (!qualityText.isEmpty()) {
        stream << "SEQUENCE_QUALITY=" << qualityText << endl;
    }
    
    stream << "PRIMER_TASK=" << edit_PRIMER_TASK->currentText() << endl;

    stream << "PRIMER_TM_FORMULA=" << combobox_PRIMER_TM_FORMULA->currentIndex() << endl;
    stream << "PRIMER_SALT_CORRECTIONS=" << combobox_PRIMER_SALT_CORRECTIONS->currentIndex() << endl;
    
    QString pathPrimerMisprimingLibrary;
    QString pathPrimerInternalOligoLibrary;
    for (const auto& lib : repeatLibraries) {
        if (lib.first == combobox_PRIMER_MISPRIMING_LIBRARY->currentText() && !lib.second.isEmpty()) {
            stream << "PRIMER_MISPRIMING_LIBRARY=" << lib.second << endl;
        }
        if (lib.first == combobox_PRIMER_MISPRIMING_LIBRARY->currentText() && !lib.second.isEmpty()) {
            stream << "PRIMER_INTERNAL_MISHYB_LIBRARY=" << lib.second << endl;
        }
    }

    stream << "=" << endl;

    file.close();
}

void Primer3Dialog::sl_loadSettings() {
    LastUsedDirHelper lod;
    lod.url = U2FileDialog::getOpenFileName(this, tr("Load settings"), lod.dir, tr("All files") + " ( * );;" + tr("Text files") + "(*.txt)");
    if (lod.url.isNull()) {  // user clicked 'Cancel' button
        return;
    }

    QFile file(lod.url);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, windowTitle(), tr("Can't read to \"%1\"").arg(lod.url));
        return;
    }

    auto intPropList = defaultSettings.getIntPropertyList();
    auto doublePropList = defaultSettings.getDoublePropertyList();

    bool primerMinThreePrimeIsUsed = false;
    QTextStream stream(&file);
    QStringList changedLineEdits;
    while(!stream.atEnd()) {
        auto line = stream.readLine();
        auto par = line.split('=');
        CHECK_CONTINUE(!(primerMinThreePrimeIsUsed && (par.first() == "PRIMER_MIN_LEFT_THREE_PRIME_DISTANCE" || par.first() == "PRIMER_MIN_RIGHT_THREE_PRIME_DISTANCE")));

        if (intPropList.contains(par.first())) {
            QSpinBox* spinBox = findChild<QSpinBox*>("edit_" + par.first());
            if (spinBox != nullptr) {
                bool ok = false;
                int v = par.last().toInt(&ok);
                CHECK_EXT_CONTINUE(ok, algoLog.error(tr("Can't parse \"%1\" value: \"%2\"").arg(par.first()).arg(par.last())));

                spinBox->setValue(v);
                continue;
            }
            QCheckBox* checkbox = findChild<QCheckBox*>("checkbox_" + par.first());
            if (checkbox != nullptr) {
                bool ok = false;
                int v = par.last().toInt(&ok);
                CHECK_EXT_CONTINUE(ok, algoLog.error(tr("Can't parse \"%1\" value: \"%2\"").arg(par.first()).arg(par.last())));

                checkbox->setChecked((bool)v);
                continue;
            }
            QComboBox* combobox = findChild<QComboBox*>("combobox_" + par.first());
            if (combobox != nullptr) {
                bool ok = false;
                int v = par.last().toInt(&ok);
                CHECK_EXT_CONTINUE(ok, algoLog.error(tr("Can't parse \"%1\" value: \"%2\"").arg(par.first()).arg(par.last())));
                CHECK_EXT_CONTINUE(0 <= v && v <= combobox->maxCount(), algoLog.error(tr("Incorrect value for \"%1\" value: \"%2\"").arg(par.first()).arg(par.last())));

                combobox->setCurrentIndex(v);
                continue;
            }

        } else if (doublePropList.contains(par.first())) {
            QDoubleSpinBox* spinBox = findChild<QDoubleSpinBox*>("edit_" + par.first());
            if (spinBox != nullptr) {
                bool ok = false;
                double v = par.last().toDouble(&ok);
                CHECK_EXT_CONTINUE(ok, algoLog.error(tr("Can't parse \"%1\" value: \"%2\"").arg(par.first()).arg(par.last())));

                QCheckBox* checkBox = findChild<QCheckBox*>("label_" + par.first());
                if (checkBox != nullptr) {
                    checkBox->setChecked(true);
                }

                spinBox->setValue(v);
                continue;
            }
        } else if (LINE_EDIT_PARAMETERS.contains(par.first())) {
            QLineEdit* lineEdit = findChild<QLineEdit*>("edit_" + par.first());
            if (lineEdit != nullptr) {
                bool wasChanged = changedLineEdits.contains(par.first());
                QString text;
                if (!wasChanged) {
                    text = par.last();
                } else {
                    text = lineEdit->text();
                    if (!text.isEmpty()) {
                        text.append(" ");
                    }
                    text.append(par.last());
                }
                lineEdit->setText(text);
                changedLineEdits.append(par.first());
                continue;
            }
        } else if (par.first() == "SEQUENCE_QUALITY") {
            edit_SEQUENCE_QUALITY->setPlainText(par.last());
        } else if (par.first() == "PRIMER_TASK") {
            QString taskName = par.last();
            auto setPrimers2Pick = [&](bool left, bool internal, bool right) {
                checkbox_PRIMER_PICK_LEFT_PRIMER->setChecked(left);
                checkbox_PRIMER_PICK_INTERNAL_OLIGO->setChecked(internal);
                checkbox_PRIMER_PICK_RIGHT_PRIMER->setChecked(right);
                taskName = "generic";
            };
            if (par.last() == "pick_pcr_primers") {
                setPrimers2Pick(true, false, true);
            } else if (par.last() == "pick_pcr_primers_and_hyb_probe") {
                setPrimers2Pick(true, true, true);
            } else if (par.last() == "pick_left_only") {
                setPrimers2Pick(true, false, false);
            } else if (par.last() == "pick_right_only") {
                setPrimers2Pick(false, false, true);
            } else if (par.last() == "pick_hyb_probe_only") {
                setPrimers2Pick(false, true, false);
            }
            edit_PRIMER_TASK->setCurrentText(taskName);
        } else if (par.first() == "PRIMER_TM_FORMULA") {
            bool ok = false;
            int v = par.last().toInt(&ok);
            CHECK_EXT_CONTINUE(ok, algoLog.error(tr("Can't parse \"%1\" value: \"%2\"").arg(par.first()).arg(par.last())));

            combobox_PRIMER_TM_FORMULA->setCurrentIndex((bool)v);
        } else if (par.first() == "PRIMER_SALT_CORRECTIONS") {
            bool ok = false;
            int v = par.last().toInt(&ok);
            CHECK_EXT_CONTINUE(ok, algoLog.error(tr("Can't parse \"%1\" value: \"%2\"").arg(par.first()).arg(par.last())));

            combobox_PRIMER_SALT_CORRECTIONS->setCurrentIndex((bool)v);
        } else if (par.first() == "PRIMER_MISPRIMING_LIBRARY") {
            bool found = false;
            for (const auto& lib : repeatLibraries) {
                if (par.last() == lib.second) {
                    combobox_PRIMER_MISPRIMING_LIBRARY->setCurrentText(lib.first);
                    found = true;
                    break;
                }
            }
            if (!found) {
                algoLog.error(tr("PRIMER_MISPRIMING_LIBRARY value should points to the file from the \"%1\" directory")
                    .arg(QFileInfo(repeatLibraries.last().second).absoluteDir().absoluteFilePath("")));
            }
        } else if (par.first() == "PRIMER_INTERNAL_MISHYB_LIBRARY") {
            bool found = false;
            for (const auto& lib : repeatLibraries) {
                if (par.last() == lib.second) {
                    combobox_PRIMER_INTERNAL_MISHYB_LIBRARY->setCurrentText(lib.first);
                    found = true;
                    break;
                }
                if (!found) {
                    algoLog.error(tr("PRIMER_INTERNAL_MISHYB_LIBRARY value should points to the file from the \"%1\" directory")
                        .arg(QFileInfo(repeatLibraries.last().second).absoluteDir().absoluteFilePath("")));
                }
            }
        } else if (par.first() == "PRIMER_MIN_THREE_PRIME_DISTANCE") {
            auto res = QMessageBox::question(nullptr, line, tr("PRIMER_MIN_THREE_PRIME_DISTANCE is unused in the UGENE GUI interface. "
                "We may either skip it or set PRIMER_MIN_LEFT_THREE_PRIME_DISTANCE and PRIMER_MIN_RIGHT_THREE_PRIME_DISTANCE to %1. Do you want to set?").arg(par.last()));
            if (res == QMessageBox::StandardButton::Yes) {
                bool ok = false;
                int v = par.last().toInt(&ok);
                CHECK_EXT_CONTINUE(ok, algoLog.error(tr("Can't parse \"%1\" value: \"%2\"").arg(par.first()).arg(par.last())));

                edit_PRIMER_MIN_LEFT_THREE_PRIME_DISTANCE->setValue(v);
                edit_PRIMER_MIN_RIGHT_THREE_PRIME_DISTANCE->setValue(v);
                primerMinThreePrimeIsUsed = true;
            }
        }
    }
    file.close();
}

void Primer3Dialog::sl_resetClicked() {
    reset();
    rs->reset();
}

void Primer3Dialog::sl_taskChanged(const QString& text) {
    auto setSequenceParametersEnabled = [&](bool target, bool junctionList, bool pairOk, bool excludedRegion, bool includedRegion) {
        label_TARGET->setEnabled(target);
        edit_SEQUENCE_TARGET->setEnabled(target);
        label_OVERLAP_JUNCTION_LIST->setEnabled(junctionList);
        edit_SEQUENCE_OVERLAP_JUNCTION_LIST->setEnabled(junctionList);
        label_PAIR_OK->setEnabled(pairOk);
        edit_SEQUENCE_PRIMER_PAIR_OK_REGION_LIST->setEnabled(pairOk);
        label_EXCLUDED_REGION->setEnabled(excludedRegion);
        edit_SEQUENCE_EXCLUDED_REGION->setEnabled(excludedRegion);
        label_INCLUDED_REGION->setEnabled(includedRegion);
        edit_SEQUENCE_INCLUDED_REGION->setEnabled(includedRegion);
    };

    if (text == "generic") {
        setSequenceParametersEnabled(true, true, true, true, true);
    } else if (text == "pick_sequencing_primers") {
        setSequenceParametersEnabled(true, false, false, false, false);
    } else if (text == "pick_primer_list") {
        setSequenceParametersEnabled(true, true, true, true, true);
    } else if (text == "check_primers") {
        setSequenceParametersEnabled(false, false, false, false, false);
    } else if (text == "pick_cloning_primers") {
        setSequenceParametersEnabled(false, false, false, false, true);
    } else if (text == "pick_discriminative_primers") {
        setSequenceParametersEnabled(true, false, false, false, false);
    } else {
        FAIL("Unexpected task value", );
    }
}

QString Primer3Dialog::checkModel() {
    return createAnnotationWidgetController->validate();
}

}  // namespace U2
