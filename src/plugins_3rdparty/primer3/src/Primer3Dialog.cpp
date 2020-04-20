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

#include <cfloat>

#include <QSettings>

#include <QMessageBox>

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

Primer3Dialog::Primer3Dialog(const Primer3TaskSettings &defaultSettings, ADVSequenceObjectContext *context):
        QDialog(context->getAnnotatedDNAView()->getWidget()),
        defaultSettings(defaultSettings),
        context(context)
{
    ui.setupUi(this);
    new HelpButton(this, ui.helpButton, "24749005");

    QPushButton* pbPick = ui.pickPrimersButton;
    QPushButton* pbReset = ui.resetButton;

    pbPick->setDefault(true);

    connect(pbPick, SIGNAL(clicked()), SLOT(sl_pbPick_clicked()));
    connect(pbReset, SIGNAL(clicked()), SLOT(sl_pbReset_clicked()));
    connect(ui.saveSettingsButton, SIGNAL(clicked()), SLOT(sl_saveSettings()));
    connect(ui.loadSettingsButton, SIGNAL(clicked()), SLOT(sl_loadSettings()));

    ui.tabWidget->setCurrentIndex(0);

    {
        CreateAnnotationModel createAnnotationModel;
        createAnnotationModel.data->name = "top_primers";
        createAnnotationModel.sequenceObjectRef = GObjectReference(context->getSequenceGObject());
        createAnnotationModel.hideAnnotationType = true;
        createAnnotationModel.hideAnnotationName = false;
        createAnnotationModel.hideLocation = true;
        createAnnotationWidgetController = new CreateAnnotationWidgetController(createAnnotationModel,this);
        ui.annotationWidgetLayout->addWidget(createAnnotationWidgetController->getWidget());
    }

    if(!context->getSequenceSelection()->getSelectedRegions().isEmpty())
    {
        selection = context->getSequenceSelection()->getSelectedRegions().first();
    }
    sequenceLength = context->getSequenceLength();
    rs=new RegionSelector(this, sequenceLength, false, context->getSequenceSelection(), true);
    ui.rangeSelectorLayout->addWidget(rs);

    repeatLibraries.append(QPair<QString, QByteArray>(tr("NONE"), ""));
    repeatLibraries.append(QPair<QString, QByteArray>(tr("HUMAN"), "primer3/humrep_and_simple.txt"));
    repeatLibraries.append(QPair<QString, QByteArray>(tr("RODENT_AND_SIMPLE"), "primer3/rodrep_and_simple.txt"));
    repeatLibraries.append(QPair<QString, QByteArray>(tr("RODENT"), "primer3/rodent_ref.txt"));
    repeatLibraries.append(QPair<QString, QByteArray>(tr("DROSOPHILA"), "primer3/drosophila.w.transposons.txt"));

    for(int i=0;i < repeatLibraries.size();i++)
    {
        if(!repeatLibraries[i].second.isEmpty())
        repeatLibraries[i].second = QFileInfo(QString(PATH_PREFIX_DATA) + ":" + repeatLibraries[i].second).absoluteFilePath().toLatin1();
    }

    {
        QPair<QString, QByteArray> library;
        foreach(library, repeatLibraries)
        {
            ui.combobox_PRIMER_MISPRIMING_LIBRARY->addItem(library.first);
            ui.combobox_PRIMER_INTERNAL_OLIGO_MISHYB_LIBRARY->addItem(library.first);
        }
    }

    int spanIntronExonIdx = -1;

    for (int i = 0; i < ui.tabWidget->count(); ++i) {
        if ( ui.tabWidget->tabText(i).contains("Intron") ) {
            spanIntronExonIdx = i;
            break;
        }
    }

    if (spanIntronExonIdx != -1) {
        SplicedAlignmentTaskRegistry* sr = AppContext::getSplicedAlignmentTaskRegistry();
        ui.tabWidget->setTabEnabled(spanIntronExonIdx, sr->getAlgNameList().size() > 0);
    }

    reset();

}

Primer3TaskSettings Primer3Dialog::getSettings()const
{
    return settings;
}

const CreateAnnotationModel &Primer3Dialog::getCreateAnnotationModel()const
{
    return createAnnotationWidgetController->getModel();
}

U2Region Primer3Dialog::getRegion(bool *ok) const
{
    return rs->getRegion(ok);
}

bool Primer3Dialog::prepareAnnotationObject()
{
   return createAnnotationWidgetController->prepareAnnotationObject();
}

QString Primer3Dialog::intervalListToString(const QList< U2Region >& intervalList, const QString& delimiter,
                                            IntervalDefinition definition) {
    QString result;
    bool first = true;
    U2Region interval;
    foreach(interval, intervalList)
    {
        if(!first)
        {
            result += " ";
        }
        result += QString::number(interval.startPos);
        result += delimiter;
        if (definition == Start_End) {
            result += QString::number(interval.endPos() - 1);
        } else {
            result += QString::number(interval.length);
        }
        first = false;
    }
    return result;
}

bool Primer3Dialog::parseIntervalList(const QString& inputString, const QString& delimiter, QList< U2Region > *outputList,
                                      IntervalDefinition definition) {
    QList< U2Region > result;
    QStringList intervalStringList = inputString.split(QRegExp("\\s+"),QString::SkipEmptyParts);
    foreach(QString intervalString, intervalStringList)
    {
        QStringList valueStringList = intervalString.split(delimiter);
        if(2 != valueStringList.size())
        {
            return false;
        }
        int firstValue = 0;
        {
            bool ok = false;
            firstValue = valueStringList[0].toInt(&ok);
            if(!ok)
            {
                return false;
            }
        }
        int secondValue = 0;
        {
            bool ok = false;
            secondValue = valueStringList[1].toInt(&ok);
            if(!ok)
            {
                return false;
            }
        }
        if (definition == Start_End) {
            result.append(U2Region(firstValue, secondValue - firstValue + 1));
        } else {
            result.append(U2Region(firstValue, secondValue));
        }
    }
    *outputList = result;
    return true;
}

void Primer3Dialog::reset()
{
    foreach(QString key, defaultSettings.getIntPropertyList())
    {
        int value = 0;
        if(defaultSettings.getIntProperty(key,&value))
        {
            QSpinBox *spinBox = findChild<QSpinBox *>("edit_" + key);
            if(NULL != spinBox)
            {
                spinBox->setValue(value);
            }
        }
    }
    foreach(QString key, defaultSettings.getDoublePropertyList())
    {
        double value = 0;
        if(defaultSettings.getDoubleProperty(key,&value))
        {
            QDoubleSpinBox *spinBox = findChild<QDoubleSpinBox *>("edit_" + key);
            if(NULL != spinBox)
            {
                spinBox->setValue(value);
            }
        }
    }
    foreach(QString key, defaultSettings.getAlignPropertyList())
    {
        short value = 0;
        if(defaultSettings.getAlignProperty(key,&value))
        {
            QDoubleSpinBox *spinBox = findChild<QDoubleSpinBox *>("edit_" + key);
            if(NULL != spinBox)
            {
                spinBox->setValue((double)value/100);
            }
        }
    }
    ui.edit_PRIMER_START_CODON_POSITION->clear();
    ui.edit_PRIMER_PRODUCT_MIN_TM->clear();
    ui.edit_PRIMER_PRODUCT_OPT_TM->clear();
    ui.edit_PRIMER_PRODUCT_MAX_TM->clear();
    ui.edit_PRIMER_OPT_GC_PERCENT->clear();
    ui.edit_PRIMER_INSIDE_PENALTY->clear();
    ui.edit_PRIMER_INTERNAL_OLIGO_OPT_GC_PERCENT->clear();
    ui.edit_EXCLUDED_REGION->setText(intervalListToString(defaultSettings.getExcludedRegion(),","));
    ui.edit_PRIMER_INTERNAL_OLIGO_EXCLUDED_REGION->setText(intervalListToString(defaultSettings.getInternalOligoExcludedRegion(),","));
    ui.edit_TARGET->setText(intervalListToString(defaultSettings.getTarget(),","));
    ui.edit_PRIMER_PRODUCT_SIZE_RANGE->setText(intervalListToString(defaultSettings.getProductSizeRange(),"-", Start_End));
    ui.edit_PRIMER_LEFT_INPUT->setText(defaultSettings.getLeftInput());
    ui.edit_PRIMER_RIGHT_INPUT->setText(defaultSettings.getRightInput());
    ui.edit_PRIMER_INTERNAL_OLIGO_INPUT->setText(defaultSettings.getInternalInput());
    {
        QString qualityString;
        bool first = true;
        foreach(int qualityValue, defaultSettings.getSequenceQuality())
        {
            if(!first)
            {
                qualityString += " ";
            }
            qualityString += QString::number(qualityValue);
            first = false;
        }
        ui.edit_PRIMER_SEQUENCE_QUALITY->setPlainText(qualityString);
    }
    {
        int value = 0;
        defaultSettings.getIntProperty("PRIMER_TM_SANTALUCIA",&value);
        ui.combobox_PRIMER_TM_SANTALUCIA->setCurrentIndex(value);
    }
    {
        int value = 0;
        defaultSettings.getIntProperty("PRIMER_SALT_CORRECTIONS",&value);
        ui.combobox_PRIMER_SALT_CORRECTIONS->setCurrentIndex(value);
    }
    {
        int value = 0;
        defaultSettings.getIntProperty("PRIMER_LIBERAL_BASE",&value);
        ui.checkbox_PRIMER_LIBERAL_BASE->setChecked(value);
    }
    {
        int value = 0;
        defaultSettings.getIntProperty("PRIMER_LIB_AMBIGUITY_CODES_CONSENSUS",&value);
        ui.checkbox_PRIMER_LIB_AMBIGUITY_CODES_CONSENSUS->setChecked(value);
    }
    {
        int value = 0;
        defaultSettings.getIntProperty("PRIMER_LOWERCASE_MASKING",&value);
        ui.checkbox_PRIMER_LOWERCASE_MASKING->setChecked(value);
    }

    {
        switch(defaultSettings.getTask())
        {
        case pick_pcr_primers:
            ui.checkbox_PICK_LEFT->setChecked(true);
            ui.checkbox_PICK_RIGHT->setChecked(true);
            ui.checkbox_PICK_HYBRO->setChecked(false);
            break;
        case pick_left_only:
            ui.checkbox_PICK_LEFT->setChecked(true);
            ui.checkbox_PICK_RIGHT->setChecked(false);
            ui.checkbox_PICK_HYBRO->setChecked(false);
            break;
        case pick_right_only:
            ui.checkbox_PICK_LEFT->setChecked(false);
            ui.checkbox_PICK_RIGHT->setChecked(true);
            ui.checkbox_PICK_HYBRO->setChecked(false);
            break;
        case pick_hyb_probe_only:
            ui.checkbox_PICK_LEFT->setChecked(false);
            ui.checkbox_PICK_RIGHT->setChecked(false);
            ui.checkbox_PICK_HYBRO->setChecked(true);
            break;
        case pick_pcr_primers_and_hyb_probe:
            ui.checkbox_PICK_LEFT->setChecked(true);
            ui.checkbox_PICK_RIGHT->setChecked(true);
            ui.checkbox_PICK_HYBRO->setChecked(true);
            break;
        }
    }
    ui.edit_PRIMER_LEFT_INPUT->setEnabled(ui.checkbox_PICK_LEFT->isChecked());
    ui.edit_PRIMER_RIGHT_INPUT->setEnabled(ui.checkbox_PICK_RIGHT->isChecked());
    ui.edit_PRIMER_INTERNAL_OLIGO_INPUT->setEnabled(ui.checkbox_PICK_HYBRO->isChecked());


    ui.combobox_PRIMER_MISPRIMING_LIBRARY->setCurrentIndex(0);
    ui.combobox_PRIMER_INTERNAL_OLIGO_MISHYB_LIBRARY->setCurrentIndex(0);
    {
        for(int i=0;i < repeatLibraries.size();i++)
        {
            if(repeatLibraries[i].second == settings.getRepeatLibrary())
            {
                ui.combobox_PRIMER_MISPRIMING_LIBRARY->setCurrentIndex(i);
            }
            if(repeatLibraries[i].second == settings.getMishybLibrary())
            {
                ui.combobox_PRIMER_INTERNAL_OLIGO_MISHYB_LIBRARY->setCurrentIndex(i);
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
        if (!ok ) {
            return res;
        }

        res.minValue = startExon;
        res.maxValue = endExon;
        ok = startExon <= endExon && startExon > 0;

    }


    return res;
}

bool Primer3Dialog::doDataExchange()
{
    settings = defaultSettings;
    if (ui.spanIntronExonBox->isChecked()) {
        SpanIntronExonBoundarySettings s;
        s.enabled = true;
        s.exonAnnotationName = ui.exonNameEdit->text();
        s.maxPairsToQuery = ui.maxPairsBox->value();
        s.minLeftOverlap = ui.leftOverlapSizeSpinBox->value();
        s.minRightOverlap = ui.rightOverlapSizeSpinBox->value();
        s.spanIntron = ui.spanIntronCheckBox->isChecked();
        s.overlapExonExonBoundary = ui.spanJunctionBox->isChecked();

        bool ok = false;
        s.exonRange = parseExonRange(ui.exonRangeEdit->text().trimmed(), ok);
        if (!ok) {
            showInvalidInputMessage(ui.exonRangeEdit, "Exon range");
            return false;
        }


        settings.setSpanIntronExonBoundarySettings(s);

    }
    foreach(QString key, settings.getIntPropertyList())
    {
        QSpinBox *spinBox = findChild<QSpinBox *>("edit_" + key);
        if(NULL != spinBox)
        {
            settings.setIntProperty(key,spinBox->value());
        }
    }
    foreach(QString key, settings.getDoublePropertyList())
    {
        QDoubleSpinBox *spinBox = findChild<QDoubleSpinBox *>("edit_" + key);
        if(NULL != spinBox)
        {
            settings.setDoubleProperty(key,spinBox->value());
        }
    }
    foreach(QString key, settings.getAlignPropertyList())
    {
        QDoubleSpinBox *spinBox = findChild<QDoubleSpinBox *>("edit_" + key);
        if(NULL != spinBox)
        {
            settings.setAlignProperty(key,(short)(spinBox->value()*100));
        }
    }
    if(!ui.edit_PRIMER_START_CODON_POSITION->text().isEmpty())
    {
        bool ok = false;
        int value = ui.edit_PRIMER_START_CODON_POSITION->text().toInt(&ok);
        if(ok)
        {
            settings.setIntProperty("PRIMER_START_CODON_POSITION", value);
        }
        else
        {
            showInvalidInputMessage(ui.edit_PRIMER_START_CODON_POSITION, tr("Start Codon Position"));
            return false;
        }
    }
    if(!ui.edit_PRIMER_PRODUCT_MIN_TM->text().isEmpty())
    {
        bool ok = false;
        double value = ui.edit_PRIMER_PRODUCT_MIN_TM->text().toDouble(&ok);
        if(ok)
        {
            settings.setDoubleProperty("PRIMER_PRODUCT_MIN_TM", value);
        }
        else
        {
            showInvalidInputMessage(ui.edit_PRIMER_PRODUCT_MIN_TM, tr("Product Min Tm"));
            return false;
        }
    }
    if(!ui.edit_PRIMER_PRODUCT_OPT_TM->text().isEmpty())
    {
        bool ok = false;
        double value = ui.edit_PRIMER_PRODUCT_OPT_TM->text().toDouble(&ok);
        if(ok)
        {
            settings.setDoubleProperty("PRIMER_PRODUCT_OPT_TM", value);
        }
        else
        {
            showInvalidInputMessage(ui.edit_PRIMER_PRODUCT_OPT_TM, tr("Product Opt Tm"));
            return false;
        }
    }
    if(!ui.edit_PRIMER_PRODUCT_MAX_TM->text().isEmpty())
    {
        bool ok = false;
        double value = ui.edit_PRIMER_PRODUCT_MAX_TM->text().toDouble(&ok);
        if(ok)
        {
            settings.setDoubleProperty("PRIMER_PRODUCT_MAX_TM", value);
        }
        else
        {
            showInvalidInputMessage(ui.edit_PRIMER_PRODUCT_MAX_TM, tr("Product Max Tm"));
            return false;
        }
    }
    if(!ui.edit_PRIMER_OPT_GC_PERCENT->text().isEmpty())
    {
        bool ok = false;
        short value = (short)(ui.edit_PRIMER_OPT_GC_PERCENT->text().toDouble(&ok)*100);
        if(ok)
        {
            settings.setAlignProperty("PRIMER_OPT_GC_PERCENT", value);
        }
        else
        {
            showInvalidInputMessage(ui.edit_PRIMER_OPT_GC_PERCENT, tr("Opt GC%"));
            return false;
        }
    }
    if(!ui.edit_PRIMER_INSIDE_PENALTY->text().isEmpty())
    {
        bool ok = false;
        double value = ui.edit_PRIMER_INSIDE_PENALTY->text().toDouble(&ok);
        if(ok)
        {
            settings.setDoubleProperty("PRIMER_INSIDE_PENALTY", value);
        }
        else
        {
            showInvalidInputMessage(ui.edit_PRIMER_INSIDE_PENALTY, tr("Inside Penalty"));
            return false;
        }
    }
    if(!ui.edit_PRIMER_INTERNAL_OLIGO_OPT_GC_PERCENT->text().isEmpty())
    {
        bool ok = false;
        short value = (short)(ui.edit_PRIMER_INTERNAL_OLIGO_OPT_GC_PERCENT->text().toDouble(&ok)*100);
        if(ok)
        {
            settings.setAlignProperty("PRIMER_INTERNAL_OLIGO_OPT_GC_PERCENT", value);
        }
        else
        {
            showInvalidInputMessage(ui.edit_PRIMER_INTERNAL_OLIGO_OPT_GC_PERCENT, tr("Internal Oligo Opt Tm"));
            return false;
        }
    }
    {
        QList< U2Region > list;
        if(parseIntervalList(ui.edit_EXCLUDED_REGION->text(), ",", &list))
        {
            settings.setExcludedRegion(list);
        }
        else
        {
            showInvalidInputMessage(ui.edit_EXCLUDED_REGION, tr("Excluded Regions"));
            return false;
        }
    }
    {
        QList< U2Region > list;
        if(parseIntervalList(ui.edit_PRIMER_INTERNAL_OLIGO_EXCLUDED_REGION->text(), ",", &list))
        {
            settings.setInternalOligoExcludedRegion(list);
        }
        else
        {
            showInvalidInputMessage(ui.edit_PRIMER_INTERNAL_OLIGO_EXCLUDED_REGION, tr("Internal Oligo Excluded Regions"));
            return false;
        }
    }
    {
        QList< U2Region > list;
        if(parseIntervalList(ui.edit_TARGET->text(), ",", &list))
        {
            settings.setTarget(list);
        }
        else
        {
            showInvalidInputMessage(ui.edit_TARGET, tr("Targets"));
            return false;
        }
    }
    if(ui.checkbox_PICK_LEFT->isChecked())
    {
        settings.setLeftInput(ui.edit_PRIMER_LEFT_INPUT->text().toLatin1());
    }
    else
    {
        settings.setLeftInput("");
    }
    if(ui.checkbox_PICK_RIGHT->isChecked())
    {
        settings.setRightInput(ui.edit_PRIMER_RIGHT_INPUT->text().toLatin1());
    }
    else
    {
        settings.setRightInput("");
    }
    if(ui.checkbox_PICK_HYBRO->isChecked())
    {
        settings.setInternalInput(ui.edit_PRIMER_INTERNAL_OLIGO_INPUT->text().toLatin1());
    }
    else
    {
        settings.setInternalInput("");
    }
    {
        QVector<int> qualityList;
        QStringList stringList = ui.edit_PRIMER_SEQUENCE_QUALITY->toPlainText().split(QRegExp("\\s+"),QString::SkipEmptyParts);
        foreach(QString string, stringList)
        {
            bool ok = false;
            int value = string.toInt(&ok);
            if(!ok)
            {
                showInvalidInputMessage(ui.edit_PRIMER_SEQUENCE_QUALITY, tr("Sequence Quality"));
                return false;
            }
            qualityList.append(value);
        }
        if(!qualityList.isEmpty() && (qualityList.size() != (rs->getRegion().length)))//todo add check on wrong region
        {
            QMessageBox::critical(this, windowTitle(), tr("Sequence quality list length must be equal to the sequence length"));
            return false;
        }
        settings.setSequenceQuality(qualityList);
    }
    settings.setIntProperty("PRIMER_TM_SANTALUCIA", ui.combobox_PRIMER_TM_SANTALUCIA->currentIndex());
    settings.setIntProperty("PRIMER_SALT_CORRECTIONS", ui.combobox_PRIMER_SALT_CORRECTIONS->currentIndex());
    settings.setIntProperty("PRIMER_LIBERAL_BASE", ui.checkbox_PRIMER_LIBERAL_BASE->isChecked());
    settings.setIntProperty("PRIMER_LIB_AMBIGUITY_CODES_CONSENSUS", ui.checkbox_PRIMER_LIB_AMBIGUITY_CODES_CONSENSUS->isChecked());
    settings.setIntProperty("PRIMER_LOWERCASE_MASKING", ui.checkbox_PRIMER_LOWERCASE_MASKING->isChecked());
    if(ui.checkbox_PICK_HYBRO->isChecked())
    {
        if(ui.checkbox_PICK_LEFT->isChecked() && ui.checkbox_PICK_RIGHT->isChecked())
        {
            settings.setTask(pick_pcr_primers_and_hyb_probe);
        }
        else if(!ui.checkbox_PICK_LEFT->isChecked() && !ui.checkbox_PICK_RIGHT->isChecked())
        {
            settings.setTask(pick_hyb_probe_only);
        }
        else
        {
            QMessageBox::critical(this, windowTitle(), tr("Can't pick hyb oligo and only one primer"));
            return false;
        }
    }
    else
    {
        if(ui.checkbox_PICK_LEFT->isChecked() && ui.checkbox_PICK_RIGHT->isChecked())
        {
            settings.setTask(pick_pcr_primers);
        }
        else if(ui.checkbox_PICK_LEFT->isChecked())
        {
            settings.setTask(pick_left_only);
        }
        else if(ui.checkbox_PICK_RIGHT->isChecked())
        {
            settings.setTask(pick_right_only);
        }
        else
        {
            QMessageBox::critical(this, windowTitle(), tr("Nothing to pick"));
            return false;
        }
    }
    {
        int index = ui.combobox_PRIMER_MISPRIMING_LIBRARY->currentIndex();
        settings.setRepeatLibrary(repeatLibraries[index].second);
    }
    {
        int index = ui.combobox_PRIMER_INTERNAL_OLIGO_MISHYB_LIBRARY->currentIndex();
        settings.setMishybLibrary(repeatLibraries[index].second);
    }
    {
        QList< U2Region > list;
        if(parseIntervalList(ui.edit_PRIMER_PRODUCT_SIZE_RANGE->text(), "-", &list, Start_End))
        {
            settings.setProductSizeRange(list);
            bool isRegionOk = false;
            U2Region includedRegion = rs->getRegion(&isRegionOk);
            if (!settings.checkIncludedRegion(includedRegion)) {
                QMessageBox::critical(this, windowTitle(), tr("Included region is too small for current product size ranges"));
                return false;
            }
            settings.setIncludedRegion( includedRegion.startPos + settings.getFirstBaseIndex(), includedRegion.length);
        }
        else
        {
            showInvalidInputMessage(ui.edit_PRIMER_PRODUCT_SIZE_RANGE, tr("Product Size Ranges"));
            return false;
        }
    }
    return true;
}

void Primer3Dialog::showInvalidInputMessage(QWidget *field, QString fieldLabel)
{
    ui.tabWidget->setCurrentWidget(field->parentWidget());
    field->setFocus(Qt::OtherFocusReason);
    QMessageBox::critical(this, windowTitle(), tr("The field '%1' has invalid value").arg(fieldLabel));
}

void Primer3Dialog::sl_pbPick_clicked()
{
    bool isRegionOk=false;
    rs->getRegion(&isRegionOk);
    if(!isRegionOk){
        rs->showErrorMessage();
        return;
    }
    if(doDataExchange())
    {
        bool objectPrepared = createAnnotationWidgetController->prepareAnnotationObject();
        if (!objectPrepared){
            QMessageBox::critical(this, L10N::errorTitle(), tr("Cannot create an annotation object. Please check settings."));
            return;
        }
        const CreateAnnotationModel& m = createAnnotationWidgetController->getModel();
        AnnotationTableObject *obj = m.getAnnotationObject();
        context->getAnnotatedDNAView()->tryAddObject(obj);
        accept();
    }
}


void Primer3Dialog::sl_saveSettings()
{
    LastUsedDirHelper lod;
    QString fileName = U2FileDialog::getSaveFileName(this, tr("Save primer settings"),  lod.dir, "Text files (*.txt)");
    if (!fileName.endsWith(".txt")) {
        fileName += ".txt";
    }

    QSettings diagSettings(fileName, QSettings::IniFormat);

    diagSettings.beginGroup("IntProperties");
    foreach(const QString& key, settings.getIntPropertyList())
    {
        QSpinBox *spinBox = findChild<QSpinBox *>("edit_" + key);
        if(NULL != spinBox)
        {
            diagSettings.setValue( key, spinBox->value() );
        }
    }
    diagSettings.endGroup();

    diagSettings.beginGroup("DoubleProperties");
    foreach(const QString& key, settings.getDoublePropertyList())
    {

        QDoubleSpinBox *spinBox = findChild<QDoubleSpinBox *>("edit_" + key);
        if(NULL != spinBox)
        {
            diagSettings.setValue(key, spinBox->value());

        }
    }
    diagSettings.endGroup();


    diagSettings.beginGroup("AlignProperties");
    foreach(const QString& key, settings.getAlignPropertyList())
    {
        QDoubleSpinBox *spinBox = findChild<QDoubleSpinBox *>("edit_" + key);
        if(NULL != spinBox)
        {
            diagSettings.setValue(key,  spinBox->value());
        }
    }
    diagSettings.endGroup();

    diagSettings.beginGroup("GeneralProperties");

    diagSettings.setValue("PRIMER_START_CODON_POSITION", ui.edit_PRIMER_START_CODON_POSITION->text());
    diagSettings.setValue("PRIMER_PRODUCT_MIN_TM", ui.edit_PRIMER_PRODUCT_MIN_TM->text());
    diagSettings.setValue("PRIMER_PRODUCT_OPT_TM", ui.edit_PRIMER_PRODUCT_OPT_TM->text());
    diagSettings.setValue("PRIMER_PRODUCT_MAX_TM", ui.edit_PRIMER_PRODUCT_MAX_TM->text());
    diagSettings.setValue("PRIMER_OPT_GC_PERCENT", ui.edit_PRIMER_OPT_GC_PERCENT->text());
    diagSettings.setValue("PRIMER_INSIDE_PENALTY", ui.edit_PRIMER_INSIDE_PENALTY->text());
    diagSettings.setValue("PRIMER_INTERNAL_OLIGO_OPT_GC_PERCENT", ui.edit_PRIMER_INTERNAL_OLIGO_OPT_GC_PERCENT->text());

    diagSettings.setValue("EXCLUDED_REGION", ui.edit_EXCLUDED_REGION->text());
    diagSettings.setValue("PRIMER_INTERNAL_OLIGO_EXCLUDED_REGION", ui.edit_PRIMER_INTERNAL_OLIGO_EXCLUDED_REGION->text());
    diagSettings.setValue("TARGET", ui.edit_TARGET->text());
    diagSettings.setValue("PRIMER_PRODUCT_SIZE_RANGE", ui.edit_PRIMER_PRODUCT_SIZE_RANGE->text());
    diagSettings.setValue("PRIMER_LEFT_INPUT", ui.edit_PRIMER_LEFT_INPUT->text());
    diagSettings.setValue("PRIMER_RIGHT_INPUT", ui.edit_PRIMER_RIGHT_INPUT->text());
    diagSettings.setValue("PRIMER_INTERNAL_OLIGO_INPUT", ui.edit_PRIMER_INTERNAL_OLIGO_INPUT->text());

    diagSettings.setValue("PRIMER_TM_SANTALUCIA", ui.combobox_PRIMER_TM_SANTALUCIA->currentIndex());
    diagSettings.setValue("PRIMER_SALT_CORRECTIONS", ui.combobox_PRIMER_SALT_CORRECTIONS->currentIndex());

    diagSettings.setValue("PRIMER_LIBERAL_BASE", ui.checkbox_PRIMER_LIBERAL_BASE->isChecked());
    diagSettings.setValue("PRIMER_LIB_AMBIGUITY_CODES_CONSENSUS", ui.checkbox_PRIMER_LIB_AMBIGUITY_CODES_CONSENSUS->isChecked());
    diagSettings.setValue("PRIMER_LOWERCASE_MASKING", ui.checkbox_PRIMER_LOWERCASE_MASKING->isChecked());
    diagSettings.setValue("PICK_HYBRO", ui.checkbox_PICK_HYBRO->isChecked() );
    diagSettings.setValue("PICK_LEFT", ui.checkbox_PICK_LEFT->isChecked() );
    diagSettings.setValue("PICK_RIGHT", ui.checkbox_PICK_RIGHT->isChecked() );

    diagSettings.setValue("PRIMER_MISPRIMING_LIBRARY", ui.combobox_PRIMER_MISPRIMING_LIBRARY->currentIndex());
    diagSettings.setValue("PRIMER_INTERNAL_OLIGO_MISHYB_LIBRARY", ui.combobox_PRIMER_INTERNAL_OLIGO_MISHYB_LIBRARY->currentIndex());

    diagSettings.endGroup();


}

void Primer3Dialog::sl_loadSettings()
{
    LastUsedDirHelper lod;
    lod.url = U2FileDialog::getOpenFileName(this, tr("Load settings"), lod.dir, "Text files (*.txt)");
    if (lod.url.isNull()) { // user clicked 'Cancel' button
        return; 
    }

    QSettings diagSettings(lod.url, QSettings::IniFormat);

    QStringList groups = diagSettings.childGroups();
    bool validSettings = groups.contains("IntProperties") && groups.contains("DoubleProperties")
        && groups.contains("AlignProperties") && groups.contains("GeneralProperties");

    if (!validSettings) {
        QMessageBox::warning(this, windowTitle(),
            tr("Can not load settings file: invalid format.") );
        return;
    }

    diagSettings.beginGroup("IntProperties");
    QStringList groupKeys = diagSettings.childKeys();
    foreach(const QString& key, settings.getIntPropertyList())
    {

        QSpinBox *spinBox = findChild<QSpinBox *>("edit_" + key);
        if(NULL != spinBox && groupKeys.contains(key))
        {
            spinBox->setValue( diagSettings.value(key).toInt());
        }
    }
    diagSettings.endGroup();

    diagSettings.beginGroup("DoubleProperties");
    groupKeys = diagSettings.childKeys();
    foreach(const QString& key, settings.getDoublePropertyList())
    {

        QDoubleSpinBox *spinBox = findChild<QDoubleSpinBox *>("edit_" + key);
        if(NULL != spinBox && groupKeys.contains(key))
        {
            spinBox->setValue(diagSettings.value(key).toDouble());

        }
    }
    diagSettings.endGroup();


    diagSettings.beginGroup("AlignProperties");
    groupKeys = diagSettings.childKeys();
    foreach(const QString& key, settings.getAlignPropertyList())
    {
        QDoubleSpinBox *spinBox = findChild<QDoubleSpinBox *>("edit_" + key);
        if(NULL != spinBox && groupKeys.contains(key))
        {
            spinBox->setValue(diagSettings.value(key).toDouble());
        }
    }
    diagSettings.endGroup();


    diagSettings.beginGroup("GeneralProperties");
    groupKeys = diagSettings.childKeys();
    ui.edit_PRIMER_START_CODON_POSITION->setText(
                diagSettings.value("PRIMER_START_CODON_POSITION").toString() );
    ui.edit_PRIMER_PRODUCT_MIN_TM->setText(
                diagSettings.value("PRIMER_PRODUCT_MIN_TM").toString() );
    ui.edit_PRIMER_PRODUCT_OPT_TM->setText(
                diagSettings.value("PRIMER_PRODUCT_OPT_TM").toString() );
    ui.edit_PRIMER_PRODUCT_MAX_TM->setText(
                diagSettings.value("PRIMER_PRODUCT_MAX_TM").toString() );
    ui.edit_PRIMER_OPT_GC_PERCENT->setText(
                diagSettings.value("PRIMER_OPT_GC_PERCENT").toString() );
    ui.edit_PRIMER_INSIDE_PENALTY->setText(
                diagSettings.value("PRIMER_INSIDE_PENALTY").toString() );
    ui.edit_PRIMER_INTERNAL_OLIGO_OPT_GC_PERCENT->setText(
                diagSettings.value("PRIMER_INTERNAL_OLIGO_OPT_GC_PERCENT").toString());
    ui.edit_EXCLUDED_REGION->setText(
                diagSettings.value("EXCLUDED_REGION").toString() );
    ui.edit_PRIMER_INTERNAL_OLIGO_EXCLUDED_REGION->setText(
                diagSettings.value("PRIMER_INTERNAL_OLIGO_EXCLUDED_REGION").toString() );
    ui.edit_TARGET->setText( diagSettings.value("TARGET").toString() );
    ui.edit_PRIMER_PRODUCT_SIZE_RANGE->setText(
                diagSettings.value("PRIMER_PRODUCT_SIZE_RANGE").toString() );
    ui.edit_PRIMER_LEFT_INPUT->setText( diagSettings.value("PRIMER_LEFT_INPUT").toString() );
    ui.edit_PRIMER_RIGHT_INPUT->setText( diagSettings.value("PRIMER_RIGHT_INPUT").toString() );
    ui.edit_PRIMER_INTERNAL_OLIGO_INPUT->setText(
                diagSettings.value("PRIMER_INTERNAL_OLIGO_INPUT").toString() );

    ui.combobox_PRIMER_TM_SANTALUCIA->setCurrentIndex(
                diagSettings.value("PRIMER_TM_SANTALUCIA").toInt() );
    ui.combobox_PRIMER_SALT_CORRECTIONS->setCurrentIndex(
                diagSettings.value("PRIMER_SALT_CORRECTIONS").toInt() );

    ui.checkbox_PRIMER_LIBERAL_BASE->setChecked(
                diagSettings.value("PRIMER_LIBERAL_BASE").toBool() );
    ui.checkbox_PRIMER_LIB_AMBIGUITY_CODES_CONSENSUS->setChecked(
                diagSettings.value("PRIMER_LIB_AMBIGUITY_CODES_CONSENSUS").toBool() );
    ui.checkbox_PRIMER_LOWERCASE_MASKING->setChecked(
                diagSettings.value("PRIMER_LOWERCASE_MASKING").toBool() );
    ui.checkbox_PICK_HYBRO->setChecked( diagSettings.value("PICK_HYBRO").toBool() );
    ui.checkbox_PICK_LEFT->setChecked( diagSettings.value("PICK_LEFT").toBool() );
    ui.checkbox_PICK_RIGHT->setChecked( diagSettings.value("PICK_RIGHT").toBool() );

    ui.combobox_PRIMER_MISPRIMING_LIBRARY->setCurrentIndex(
                diagSettings.value("PRIMER_MISPRIMING_LIBRARY").toInt() );
    ui.combobox_PRIMER_INTERNAL_OLIGO_MISHYB_LIBRARY->setCurrentIndex(
                diagSettings.value("PRIMER_INTERNAL_OLIGO_MISHYB_LIBRARY").toInt() );

    diagSettings.endGroup();




}

void Primer3Dialog::sl_pbReset_clicked()
{
    reset();
    rs->reset();
}

QString Primer3Dialog::checkModel()
{
    return createAnnotationWidgetController->validate();
}

} // namespace U2
