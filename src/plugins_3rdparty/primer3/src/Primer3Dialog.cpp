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

#include <QMessageBox>
#include <QSettings>
#include <QTextStream>

#include <U2Algorithm/SplicedAlignmentTaskRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/FileFilters.h>
#include <U2Core/L10n.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>
#include <U2Gui/U2WidgetStateStorage.h>

#include <U2View/AnnotatedDNAView.h>

#include "Primer3Dialog.h"

#include <U2View/TmCalculatorSelectorWidget.h>

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

const QRegularExpression Primer3Dialog::MUST_MATCH_END_REGEX("^([nagctrywsmkbhdvNAGCTRYWSMKBHDV]){5}$");
const QRegularExpression Primer3Dialog::MUST_MATCH_START_CODON_SEQUENCE_REGEX("^([a-zA-Z]){3}$");

Primer3Dialog::Primer3Dialog(ADVSequenceObjectContext* context)
    : QDialog(context->getAnnotatedDNAView()->getWidget()),
      context(context),
      savableWidget(this, GObjectViewUtils::findViewByName(context->getAnnotatedDNAView()->getName()), { "primer3RegionSelector", "primer3AnnWgt" }),
      primer3DataDirectory(QFileInfo(QString(PATH_PREFIX_DATA) + ":primer3/").absoluteFilePath().toLatin1()) {
    setupUi(this);
    new HelpButton(this, helpButton, "65930919");

    connect(closeButton, &QPushButton::clicked, this, &Primer3Dialog::close);
    connect(pickPrimersButton, &QPushButton::clicked, this, &Primer3Dialog::sl_pickClicked);
    connect(resetButton, &QPushButton::clicked, this, &Primer3Dialog::sl_resetClicked);
    connect(saveSettingsButton, &QPushButton::clicked, this, &Primer3Dialog::sl_saveSettings);
    connect(loadSettingsButton, &QPushButton::clicked, this, &Primer3Dialog::sl_loadSettings);
    connect(cbPreset, &QComboBox::currentTextChanged, this, &Primer3Dialog::sl_presetChanged);
    connect(edit_PRIMER_TASK, &QComboBox::currentTextChanged, this, &Primer3Dialog::sl_taskChanged);
    connect(edit_PRIMER_TASK, &QComboBox::currentTextChanged, this, &Primer3Dialog::sl_checkComplementStateChanged);
    connect(checkbox_PRIMER_PICK_LEFT_PRIMER, &QCheckBox::toggled, this, &Primer3Dialog::sl_checkComplementStateChanged);
    connect(checkbox_PRIMER_PICK_RIGHT_PRIMER, &QCheckBox::toggled, this, &Primer3Dialog::sl_checkComplementStateChanged);


    tabWidget->setCurrentIndex(0);

    {
        CreateAnnotationModel createAnnotationModel;
        createAnnotationModel.data->name = "top_primers";
        createAnnotationModel.sequenceObjectRef = GObjectReference(context->getSequenceGObject());
        createAnnotationModel.hideAnnotationType = true;
        createAnnotationModel.hideAnnotationName = false;
        createAnnotationModel.hideLocation = true;
        createAnnotationWidgetController = new CreateAnnotationWidgetController(createAnnotationModel, this);
        auto annWgt = createAnnotationWidgetController->getWidget();
        annWgt->setObjectName("primer3AnnWgt");
        annotationWidgetLayout->addWidget(annWgt);
    }

    if (!context->getSequenceSelection()->getSelectedRegions().isEmpty()) {
        selection = context->getSequenceSelection()->getSelectedRegions().first();
    }
    rs = new RegionSelector(this, context->getSequenceLength(), false, context->getSequenceSelection(), true);
    rs->setObjectName("primer3RegionSelector");
    rangeSelectorLayout->addWidget(rs);

    repeatLibraries.append(QPair<QString, QByteArray>(tr("NONE"), ""));
    repeatLibraries.append(QPair<QString, QByteArray>(tr("HUMAN"), "/humrep_and_simple.txt"));
    repeatLibraries.append(QPair<QString, QByteArray>(tr("RODENT_AND_SIMPLE"), "/rodrep_and_simple.txt"));
    repeatLibraries.append(QPair<QString, QByteArray>(tr("RODENT"), "/rodent_ref.txt"));
    repeatLibraries.append(QPair<QString, QByteArray>(tr("DROSOPHILA"), "/drosophila.w.transposons.txt"));

    for (int i = 0; i < repeatLibraries.size(); i++) {
        if (!repeatLibraries[i].second.isEmpty()) {
            repeatLibraries[i].second = primer3DataDirectory + repeatLibraries[i].second;
        }
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

    U2WidgetStateStorage::restoreWidgetState(savableWidget);
}

Primer3Dialog::~Primer3Dialog() {
    delete settings;
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
    for (int num : qAsConst(intList)) {
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
    for (const auto& numList : qAsConst(regionLins)) {
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
    for (const auto& intervalString : qAsConst(intervalStringList)) {
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
    for (const auto& numString : qAsConst(intStringList)) {
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
    for (const auto& numList : qAsConst(intStringList)) {
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
    sl_presetChanged(cbPreset->currentText());
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
    delete settings;
    settings = new Primer3TaskSettings;

    QMap<QWidget*, bool> widgetStates;
    QStringList errors;

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
        s.exonRange = parseExonRange(edit_exonRange->text().trimmed(), ok);
        widgetStates.insert(edit_exonRange, ok);
        if (!ok) {
            errors.append(getWidgetTemplateError(edit_exonRange));
        }

        settings->setSpanIntronExonBoundarySettings(s);
    } else {
        widgetStates.insert(edit_exonRange, true);
    }

    if (gbCheckComplementary->isEnabled() && gbCheckComplementary->isChecked()) {
        CheckComplementSettings s;
        s.enabled = true;
        s.maxComplementPairs = sbMaxComplementPairs->value();
        s.maxGcPair = sbMaxGcPairs->value();

        settings->setCheckComplementSettings(s);
    }

    const auto& intProps = settings->getIntPropertyList();
    for (const auto& key : qAsConst(intProps)) {
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
    const auto& doubleProps = settings->getDoublePropertyList();
    for (const auto& key : qAsConst(doubleProps)) {
        QCheckBox* checkBox = findChild<QCheckBox*>("label_" + key);
        if (checkBox != nullptr && !checkBox->isChecked()) {
            continue;
        }
        QDoubleSpinBox* spinBox = findChild<QDoubleSpinBox*>("edit_" + key);
        if (spinBox != nullptr) {
            settings->setDoubleProperty(key, spinBox->value());
        }
    }

    // Main
    {
        QList<U2Region> list;
        bool ok = parseIntervalList(edit_SEQUENCE_TARGET->text(), ",", &list);
        widgetStates.insert(edit_SEQUENCE_TARGET, ok);
        if (ok) {
            settings->setTarget(list);
        } else {
            errors.append(getWidgetTemplateError(edit_SEQUENCE_TARGET));
        }
    }
    {
        QList<int> list;
        bool ok = parseIntList(edit_SEQUENCE_OVERLAP_JUNCTION_LIST->text(), &list);
        widgetStates.insert(edit_SEQUENCE_OVERLAP_JUNCTION_LIST, ok);
        if (ok) {
            settings->setOverlapJunctionList(list);
        } else {
            errors.append(getWidgetTemplateError(edit_SEQUENCE_OVERLAP_JUNCTION_LIST));
        }
    }
    {
        QList<U2Region> list;
        bool ok = parseIntervalList(edit_SEQUENCE_EXCLUDED_REGION->text(), ",", &list);
        widgetStates.insert(edit_SEQUENCE_EXCLUDED_REGION, ok);
        if (ok) {
            settings->setExcludedRegion(list);
        } else {
            errors.append(getWidgetTemplateError(edit_SEQUENCE_EXCLUDED_REGION));
        }
    }
    {
        QList<QList<int>> list;
        bool ok = parseOkRegions(edit_SEQUENCE_PRIMER_PAIR_OK_REGION_LIST->text(), &list);
        widgetStates.insert(edit_SEQUENCE_PRIMER_PAIR_OK_REGION_LIST, ok);
        if (ok) {
            settings->setOkRegion(list);
        } else {
            errors.append(getWidgetTemplateError(edit_SEQUENCE_PRIMER_PAIR_OK_REGION_LIST));
        }
    }
    {
        QList<U2Region> list;
        bool ok = parseIntervalList(edit_SEQUENCE_INCLUDED_REGION->text(), ",", &list);
        if (ok) {
            if (list.size() > 1) {
                errors.append(tr("The \"Include region\" should be the only one"));
                ok = false;
            } else if (list.size() == 1) {
                const auto& region = list.first();
                settings->setIncludedRegion(region.startPos, region.length);
            }
        } else {
            errors.append(getWidgetTemplateError(edit_SEQUENCE_INCLUDED_REGION));
        }
        widgetStates.insert(edit_SEQUENCE_INCLUDED_REGION, ok);
    }
    {
        QString text = edit_SEQUENCE_START_CODON_SEQUENCE->text();
        bool ok = true;
        if (!text.isEmpty()) {
            if (MUST_MATCH_START_CODON_SEQUENCE_REGEX.match(text).hasMatch()) {
                settings->setStartCodonSequence(text.toLocal8Bit());
            } else {
                ok = false;
                errors.append(getWidgetTemplateError(edit_SEQUENCE_START_CODON_SEQUENCE));
            }
        }
        widgetStates.insert(edit_SEQUENCE_START_CODON_SEQUENCE, ok);
    }
    {
        QString text = edit_PRIMER_MUST_MATCH_FIVE_PRIME->text();
        bool ok = true;
        if (!text.isEmpty()) {
            if (MUST_MATCH_END_REGEX.match(text).hasMatch()) {
                settings->setPrimerMustMatchFivePrime(text.toLocal8Bit());
            } else {
                ok = false;
                errors.append(getWidgetTemplateError(edit_PRIMER_MUST_MATCH_FIVE_PRIME));
            }
        }
        widgetStates.insert(edit_PRIMER_MUST_MATCH_FIVE_PRIME, ok);
    }
    {
        QString text = edit_PRIMER_MUST_MATCH_THREE_PRIME->text();
        bool ok = true;
        if (!text.isEmpty()) {
            if (MUST_MATCH_END_REGEX.match(text).hasMatch()) {
                settings->setPrimerMustMatchThreePrime(text.toLocal8Bit());
            } else {
                ok = false;
                errors.append(getWidgetTemplateError(edit_PRIMER_MUST_MATCH_THREE_PRIME));
            }
        }
        widgetStates.insert(edit_PRIMER_MUST_MATCH_THREE_PRIME, ok);
    }
    // Internal
    {
        QList<U2Region> list;
        bool ok = parseIntervalList(edit_SEQUENCE_INTERNAL_EXCLUDED_REGION->text(), ",", &list);
        widgetStates.insert(edit_SEQUENCE_INTERNAL_EXCLUDED_REGION, ok);
        if (ok) {
            settings->setInternalOligoExcludedRegion(list);
        } else {
            errors.append(getWidgetTemplateError(edit_SEQUENCE_INTERNAL_EXCLUDED_REGION));
        }
    }
    {
        QList<int> list;
        bool ok = parseIntList(edit_SEQUENCE_INTERNAL_OVERLAP_JUNCTION_LIST->text(), &list);
        widgetStates.insert(edit_SEQUENCE_INTERNAL_OVERLAP_JUNCTION_LIST, ok);
        if (ok) {
            settings->setInternalOverlapJunctionList(list);
        } else {
            errors.append(getWidgetTemplateError(edit_SEQUENCE_INTERNAL_OVERLAP_JUNCTION_LIST));
        }
    }
    {
        QString text = edit_PRIMER_INTERNAL_MUST_MATCH_FIVE_PRIME->text();
        bool ok = true;
        if (!text.isEmpty()) {
            if (!text.isEmpty() && MUST_MATCH_END_REGEX.match(text).hasMatch()) {
                settings->setInternalPrimerMustMatchFivePrime(text.toLocal8Bit());
            } else {
                ok = false;
                errors.append(getWidgetTemplateError(edit_PRIMER_INTERNAL_MUST_MATCH_FIVE_PRIME));
            }
        }
        widgetStates.insert(edit_PRIMER_INTERNAL_MUST_MATCH_FIVE_PRIME, ok);
    }
    {
        QString text = edit_PRIMER_INTERNAL_MUST_MATCH_THREE_PRIME->text();
        bool ok = true;
        if (!text.isEmpty()) {
            if (!text.isEmpty() && MUST_MATCH_END_REGEX.match(text).hasMatch()) {
                settings->setInternalPrimerMustMatchThreePrime(text.toLocal8Bit());
            } else {
                ok = false;
                errors.append(getWidgetTemplateError(edit_PRIMER_INTERNAL_MUST_MATCH_THREE_PRIME));
            }
        }
        widgetStates.insert(edit_PRIMER_INTERNAL_MUST_MATCH_THREE_PRIME, ok);
    }

    auto checkSequenceAlphabet = [&errors, &widgetStates](const QByteArray& sequence, const QString& parameterName, QLineEdit* wgt) {
        static const QString INCORRECT_ALPHABET_ERROR_TEMPLATE
            = QT_TR_NOOP(tr("%1 parameter has incorrect alphabet, should be DNA"));
        bool res = true;
        if (!sequence.isEmpty()) {
            auto alphabet = U2AlphabetUtils::findAllAlphabets(sequence).first();
            bool isSimpleDna = alphabet->isNucleic() && alphabet->isDefault();
            if (!isSimpleDna) {
                res = false;
                errors.append(tr("%1 sequence has incorrect alphabet, should be be simple DNA").arg(parameterName));
            }
        }
        widgetStates.insert(wgt, res);
    };

    if (checkbox_PRIMER_PICK_LEFT_PRIMER->isChecked()) {
        {
            auto leftSequence = edit_SEQUENCE_PRIMER->text().toLatin1();
            checkSequenceAlphabet(leftSequence, tr("Left primer"), edit_SEQUENCE_PRIMER);
            settings->setLeftInput(leftSequence);
        }

        {
            auto leftOverhang = edit_SEQUENCE_OVERHANG_LEFT->text().toLatin1();
            checkSequenceAlphabet(leftOverhang, tr("Left 5' overhang"), edit_SEQUENCE_OVERHANG_LEFT);
            settings->setLeftOverhang(leftOverhang);

        }
    } else {
        settings->setLeftInput("");
        widgetStates.insert(edit_SEQUENCE_PRIMER, true);
        settings->setLeftOverhang("");
        widgetStates.insert(edit_SEQUENCE_OVERHANG_LEFT, true);
    }

    if (checkbox_PRIMER_PICK_INTERNAL_OLIGO->isChecked()) {
        {
            auto internalOligo = edit_SEQUENCE_INTERNAL_OLIGO->text().toLatin1();
            checkSequenceAlphabet(internalOligo, tr("Internal oligo"), edit_SEQUENCE_INTERNAL_OLIGO);
            settings->setInternalInput(internalOligo);

        }
    } else {
        settings->setInternalInput("");
        widgetStates.insert(edit_SEQUENCE_INTERNAL_OLIGO, true);
    }

    if (checkbox_PRIMER_PICK_RIGHT_PRIMER->isChecked()) {
        {
            auto rightSequence = edit_SEQUENCE_PRIMER_REVCOMP->text().toLatin1();
            checkSequenceAlphabet(rightSequence, tr("Right primer"), edit_SEQUENCE_PRIMER_REVCOMP);
            settings->setRightInput(rightSequence);
        }

        {
            auto rightOverhang = edit_SEQUENCE_OVERHANG_RIGHT->text().toLatin1();
            checkSequenceAlphabet(rightOverhang, tr("Right 5' overhang"), edit_SEQUENCE_OVERHANG_RIGHT);
            settings->setRightOverhang(rightOverhang);
        }
    } else {
        settings->setRightInput("");
        widgetStates.insert(edit_SEQUENCE_PRIMER_REVCOMP, true);
        settings->setRightOverhang("");
        widgetStates.insert(edit_SEQUENCE_OVERHANG_RIGHT, true);
    }

    {
        QVector<int> qualityList;
        QStringList stringList = edit_SEQUENCE_QUALITY->toPlainText().split(QRegExp("\\s+"), QString::SkipEmptyParts);
        bool ok = true;
        for (const QString& string : qAsConst(stringList)) {
            bool isInt = false;
            int value = string.toInt(&isInt);
            if (!isInt) {
                ok = false;
            }
            qualityList.append(value);
        }
        if (!qualityList.isEmpty() && (qualityList.size() != (rs->getRegion().length))) { // todo add check on wrong region
            ok = false;
            errors.append(tr("Sequence quality list length must be equal to the sequence length"));
        }
        settings->setSequenceQuality(qualityList);
        widgetStates.insert(edit_SEQUENCE_QUALITY, ok);
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
            widgetStates.insert(edit_SEQUENCE_TARGET, false);
            errors.append(tr("Task \"pick_discriminative_primers\" requires exactly one \"Targets\" region."));
        }
    case pick_cloning_primers:
    case generic:
    case pick_sequencing_primers:
    case pick_primer_list:
    case check_primers:
        if (!(checkbox_PRIMER_PICK_LEFT_PRIMER->isChecked() ||
            checkbox_PRIMER_PICK_INTERNAL_OLIGO->isChecked() ||
            checkbox_PRIMER_PICK_RIGHT_PRIMER->isChecked())) {
            errors.append(tr("At least one primer on the \"Main\" settings page should be enabled."));
        }
        break;
    default:
        FAIL("Invalid Primer3 task", false);
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
        bool ok = parseIntervalList(edit_PRIMER_PRODUCT_SIZE_RANGE->text(), "-", &list, IntervalDefinition::Start_End);
        bool alreadyHasError = false;
        if (ok) {
            if (list.isEmpty()) {
                errors.append(tr("Primer Size Ranges should have at least one range"));
                ok = false;
                alreadyHasError = true;
            } else {
                settings->setProductSizeRange(list);
                bool isRegionOk = false;
                U2Region sequenceRangeRegion = rs->getRegion(&isRegionOk);
                if (!isRegionOk) {
                    rs->showErrorMessage();
                    return false;
                }
                if (!settings->isIncludedRegionValid(sequenceRangeRegion)) {
                    errors.append(tr("Sequence range region is too small for current product size ranges"));
                    ok = false;
                    alreadyHasError = true;
                } else {
                    settings->setSequenceRange(sequenceRangeRegion);
                }
            }
        }
        widgetStates.insert(edit_PRIMER_PRODUCT_SIZE_RANGE, ok);
        if (!ok && !alreadyHasError) {
            errors.append(getWidgetTemplateError(edit_PRIMER_PRODUCT_SIZE_RANGE));
        }
    }
    {
        U2Region sequenceRangeRegion = rs->getRegion();
        bool ok = true;
        if (sequenceRangeRegion.length > MAXIMUM_ALLOWED_SEQUENCE_LENGTH) {
            errors.append(tr("The priming sequence is too long, please, decrease the region"));
            ok = false;
        }
        widgetStates.insert(rs, ok);
    }
    {
        const auto& includedRegion = settings->getIncludedRegion();
        int fbs = settings->getFirstBaseIndex();
        int includedRegionOffset = includedRegion.startPos != 0 ? includedRegion.startPos - fbs : 0;
        if (includedRegionOffset < 0) {
            errors.append(tr("Incorrect sum \"Included Region Start + First Base Index\" - should be more or equal than 0"));
        }
        U2Region sequenceRangeRegion = rs->getRegion();
        if (sequenceRangeRegion.endPos() > context->getSequenceLength() + includedRegionOffset && !context->getSequenceObject()->isCircular()) {
            errors.append(tr("The priming sequence is out of range.\n"
                "Either make the priming region end \"%1\" less or equal than the sequence size \"%2\" plus the first base index value \"%3\""
                "or mark the sequence as circular").arg(sequenceRangeRegion.endPos()).arg(context->getSequenceLength()).arg(settings->getFirstBaseIndex()));
        }
    }

    return updateErrorState(widgetStates, errors);
}

bool Primer3Dialog::updateErrorState(const QMap<QWidget*, bool>& widgetStates, const QStringList& errors) {
    const auto& widgets = widgetStates.keys();
    for (auto wgt : qAsConst(widgets)) {
        GUIUtils::setWidgetWarningStyle(wgt, !widgetStates.value(wgt));
    }
    CHECK(!errors.isEmpty(), true);

    QString generalErrorString = tr("%1 parameter(s) have an incorrect value(s), pay attention on red widgets. ").arg(errors.size());
    if (errors.size() <= 3) {
        generalErrorString += tr("The following errors are possible: \n\n");
        generalErrorString += errors.join("\n\n");
    }
    generalErrorString += tr("\n\nClick OK to continue calculation, but the incorrect values will be ignored.");

    auto result = QMessageBox::question(this, windowTitle(), generalErrorString, QMessageBox::StandardButton::Ok, QMessageBox::StandardButton::Cancel);
    return result == QMessageBox::StandardButton::Ok;
}

QString Primer3Dialog::getWidgetTemplateError(QWidget* wgt, const QString& errorWgtLabe) const {
    static const char* ERROR_TEMPLATE
        = QT_TR_NOOP("The \"%1\" parameter has incorrect value, please"
        ", read the tooltip of this parameter to find out how the correct one looks like.");

    auto name = errorWgtLabe;
    if (name.isEmpty()) {
        auto labelWidgetName = wgt->objectName().replace("edit_", "label_");
        auto parentWgt = wgt->parentWidget();
        auto wgtLabel = parentWgt->findChild<QLabel*>(labelWidgetName);
        SAFE_POINT(wgtLabel != nullptr, "No label was found", QString());

        name = wgtLabel->text();
        if (name.endsWith(":")) {
            name = name.left(name.size() - 1);
        }
    }

    return tr(ERROR_TEMPLATE).arg(name);
}

void Primer3Dialog::sl_pickClicked() {
    bool isRegionOk = false;
    rs->getRegion(&isRegionOk);
    if (!isRegionOk) {
        rs->showErrorMessage();
        return;
    }
    if (doDataExchange()) {
        accept();
    }
}

void Primer3Dialog::sl_saveSettings() {
    LastUsedDirHelper lod;
    QString fileName = U2FileDialog::getSaveFileName(this, tr("Save primer settings"), lod.dir, "Text files (*.txt)");
    if (!fileName.endsWith(".txt")) {
        fileName += ".txt";
    }

    saveSettings(fileName);
}

void Primer3Dialog::sl_loadSettings() {
    LastUsedDirHelper lod;
    QStringList filters;
    filters.append(tr("Text files") + "(*.txt)");
    lod.url = U2FileDialog::getOpenFileName(this, tr("Load settings"), lod.dir, FileFilters::withAllFilesFilter(filters));
    if (lod.url.isNull()) {  // user clicked 'Cancel' button
        return;
    }

    loadSettings(lod.url);
}


void Primer3Dialog::sl_resetClicked() {
    reset();
    rs->reset();
}

void Primer3Dialog::sl_taskChanged(const QString& text) {
    auto setSequenceParametersEnabled = [&](bool target, bool junctionList, bool pairOk, bool excludedRegion, bool includedRegion) {
        label_SEQUENCE_TARGET->setEnabled(target);
        edit_SEQUENCE_TARGET->setEnabled(target);
        label_SEQUENCE_OVERLAP_JUNCTION_LIST->setEnabled(junctionList);
        edit_SEQUENCE_OVERLAP_JUNCTION_LIST->setEnabled(junctionList);
        label_SEQUENCE_PRIMER_PAIR_OK_REGION_LIST->setEnabled(pairOk);
        edit_SEQUENCE_PRIMER_PAIR_OK_REGION_LIST->setEnabled(pairOk);
        label_SEQUENCE_EXCLUDED_REGION->setEnabled(excludedRegion);
        edit_SEQUENCE_EXCLUDED_REGION->setEnabled(excludedRegion);
        label_SEQUENCE_INCLUDED_REGION->setEnabled(includedRegion);
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

void Primer3Dialog::sl_presetChanged(const QString& text) {
    if (text == tr("Default")) {
        loadSettings(primer3DataDirectory + "/presets/Default.txt");
        gbCheckComplementary->setChecked(false);
        lbPresetInfo->clear();
    } else if (text == tr("Recombinase Polymerase Amplification")) {
        loadSettings(primer3DataDirectory + "/presets/RPA.txt");
        gbCheckComplementary->setChecked(true);
        lbPresetInfo->setText(tr("Info: \"Check complementary\" has been enabled (see the \"Posterior Actions\" tab)"));
    } else {
        FAIL("Unexpected preset", );
    }
}

void Primer3Dialog::sl_checkComplementStateChanged() {
    QString warning;
    if (!checkbox_PRIMER_PICK_LEFT_PRIMER->isChecked() || !checkbox_PRIMER_PICK_RIGHT_PRIMER->isChecked()) {
        warning = tr("Warning: \"Check complementary\" requires left and right primers enabled (\"Main\" page).");
    } else if (edit_PRIMER_TASK->currentText() == "pick_primer_list") {
        warning = tr("Warning: \"Check complementary\" requires any task but \"pick_primer_list\" (\"Main\" page).");
    }

    if (warning.isEmpty()) {
        lbPosteriorActionsWarning->clear();
        gbCheckComplementary->setEnabled(true);
    } else {
        lbPosteriorActionsWarning->setText(warning);
        gbCheckComplementary->setEnabled(false);
    }
}

void Primer3Dialog::saveSettings(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
        QMessageBox::critical(this, windowTitle(), L10N::errorWritingFile(filePath));
        return;
    }

    QTextStream stream(&file);
    const auto& intProps = defaultSettings.getIntPropertyList();
    for (const auto& key : qAsConst(intProps)) {
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
    const auto& doubleProps = defaultSettings.getDoublePropertyList();
    for (const auto& key : qAsConst(doubleProps)) {
        QCheckBox* checkBox = findChild<QCheckBox*>("label_" + key);
        if (checkBox != nullptr && !checkBox->isChecked()) {
            continue;
        }
        QDoubleSpinBox* spinBox = findChild<QDoubleSpinBox*>("edit_" + key);
        if (spinBox != nullptr) {
            stream << key << "=" << spinBox->value() << endl;
        }
    }

    for (const auto& par : qAsConst(LINE_EDIT_PARAMETERS)) {
        QLineEdit* lineEdit = findChild<QLineEdit*>("edit_" + par);
        if (lineEdit != nullptr) {
            stream << par << "=" << lineEdit->text() << endl;
        }
    }

    U2OpStatusImpl os;
    stream << "SEQUENCE_TEMPLATE=" << context->getSequenceObject()->getWholeSequenceData(os) << endl;
    stream << "SEQUENCE_ID=" << context->getSequenceObject()->getSequenceName() << endl;
    stream << "SEQUENCE_QUALITY=" << edit_SEQUENCE_QUALITY->toPlainText() << endl;

    stream << "PRIMER_TASK=" << edit_PRIMER_TASK->currentText() << endl;
    stream << "PRIMER_TM_FORMULA=" << combobox_PRIMER_TM_FORMULA->currentIndex() << endl;
    stream << "PRIMER_SALT_CORRECTIONS=" << combobox_PRIMER_SALT_CORRECTIONS->currentIndex() << endl;

    QString pathPrimerMisprimingLibrary;
    QString pathPrimerInternalOligoLibrary;
    for (const auto& lib : qAsConst(repeatLibraries)) {
        if (lib.first == combobox_PRIMER_MISPRIMING_LIBRARY->currentText()) {
            QFileInfo fi(lib.second);
            stream << "PRIMER_MISPRIMING_LIBRARY=" << fi.fileName() << endl;
        }
        if (lib.first == combobox_PRIMER_INTERNAL_MISHYB_LIBRARY->currentText()) {
            QFileInfo fi(lib.second);
            stream << "PRIMER_INTERNAL_MISHYB_LIBRARY=" << fi.fileName() << endl;
        }
    }

    stream << "=" << endl;

    file.close();
}

void Primer3Dialog::loadSettings(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, windowTitle(), L10N::errorReadingFile(filePath));
        return;
    }

    auto intPropList = defaultSettings.getIntPropertyList();
    auto doublePropList = defaultSettings.getDoublePropertyList();

    bool primerMinThreePrimeIsUsed = false;
    QTextStream stream(&file);
    QStringList changedLineEdits;
    while (!stream.atEnd()) {
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
                if (QString(lib.second).endsWith(par.last())) {
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
                if (QString(lib.second).endsWith(par.last())) {
                    combobox_PRIMER_INTERNAL_MISHYB_LIBRARY->setCurrentText(lib.first);
                    found = true;
                    break;
                }
            }
            if (!found) {
                algoLog.error(tr("PRIMER_INTERNAL_MISHYB_LIBRARY value should points to the file from the \"%1\" directory")
                    .arg(QFileInfo(repeatLibraries.last().second).absoluteDir().absoluteFilePath("")));
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

QString Primer3Dialog::checkModel() {
    return createAnnotationWidgetController->validate();
}

}  // namespace U2
