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

#include <math.h>

#include <QMessageBox>
#include <QPushButton>

#include <U2Core/Annotation.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/GenbankFeatures.h>
#include <U2Core/L10n.h>
#include <U2Core/Settings.h>
#include <U2Core/TaskWatchdog.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/CreateAnnotationWidgetController.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/RegionSelector.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>

#include "FindRepeatsDialog.h"

namespace U2 {

#define SETTINGS_ROOT               QString("plugin_find_repeats/")
#define MIN_LEN_SETTINGS            QString("min_len")
#define IDENTITY_SETTINGS           QString("identity")
#define MIN_DIST_SETTINGS           QString("min_dist")
#define MAX_DIST_SETTINGS           QString("max_dist")
#define MIN_DIST_CHECK_SETTINGS     QString("min_dist_check")
#define MAX_DIST_CHECK_SETTINGS     QString("max_dist_check")
#define INVERT_CHECK_SETTINGS       QString("invert_check")
#define TANDEMS_CHECK_SETTINGS      QString("exclude_tandems_check")

FindRepeatsTaskSettings FindRepeatsDialog::defaultSettings()
{
    FindRepeatsTaskSettings res;
    Settings* s = AppContext::getSettings();
    res.minLen = (s->getValue(SETTINGS_ROOT + MIN_LEN_SETTINGS, 5).toInt());
    res.setIdentity(s->getValue(SETTINGS_ROOT + IDENTITY_SETTINGS, 100).toInt());
    bool minDistCheck = (s->getValue(SETTINGS_ROOT + MIN_DIST_CHECK_SETTINGS, true).toBool());
    bool maxDistCheck = (s->getValue(SETTINGS_ROOT + MAX_DIST_CHECK_SETTINGS, true).toBool());
    res.minDist = !minDistCheck ? 0 : (s->getValue(SETTINGS_ROOT + MIN_DIST_SETTINGS, 0).toInt());
    res.maxDist = !maxDistCheck ? 0 : (s->getValue(SETTINGS_ROOT + MAX_DIST_SETTINGS, 5000).toInt());
    res.inverted = (s->getValue(SETTINGS_ROOT + INVERT_CHECK_SETTINGS, false).toBool());
    res.excludeTandems = (s->getValue(SETTINGS_ROOT + TANDEMS_CHECK_SETTINGS, false).toBool());
    res.filter = DisjointRepeats;
    return res;
}

FindRepeatsDialog::FindRepeatsDialog(ADVSequenceObjectContext* _sc)
: QDialog(_sc->getAnnotatedDNAView()->getWidget())
{
    sc = _sc;
    setupUi(this);
    new HelpButton(this, buttonBox, "24748948");

    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Start"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    tabWidget->setCurrentIndex(0);

    CreateAnnotationModel m;
    m.hideAnnotationType = true;
    m.hideLocation = true;
    m.data->name = GBFeatureUtils::getKeyInfo(GBFeatureKey_repeat_unit).text;
    m.sequenceObjectRef = sc->getSequenceObject();
    m.useUnloadedObjects = true;
    m.sequenceLen = sc->getSequenceObject()->getSequenceLength();
    ac = new CreateAnnotationWidgetController(m, this);

    QWidget* caw = ac->getWidget();
    QVBoxLayout* l = new QVBoxLayout();
    l->setMargin(0);
    l->addWidget(caw);
    annotationsWidget->setLayout(l);

    algoCombo->addItem(tr("Auto"), RFAlgorithm_Auto);
    algoCombo->addItem(tr("Suffix index"), RFAlgorithm_Suffix);
    algoCombo->addItem(tr("Diagonals"), RFAlgorithm_Diagonal);

    filterAlgorithms->addItem(tr("Disjoint repeats"), DisjointRepeats);
    filterAlgorithms->addItem(tr("No filtering"), NoFiltering);
    filterAlgorithms->addItem(tr("Unique repeats"), UniqueRepeats);

    qint64 seqLen = sc->getSequenceLength();

    Settings* s = AppContext::getSettings();
    minLenBox->setValue(s->getValue(SETTINGS_ROOT + MIN_LEN_SETTINGS, qBound(5, int(seqLen / 100), 100)).toInt());
    minLenBox->setMaximum(seqLen);
    identityBox->setValue(s->getValue(SETTINGS_ROOT + IDENTITY_SETTINGS, 100).toInt());
    minDistBox->setValue(s->getValue(SETTINGS_ROOT + MIN_DIST_SETTINGS, 0).toInt());
    maxDistBox->setValue(s->getValue(SETTINGS_ROOT + MAX_DIST_SETTINGS, 5000).toInt());
    minDistCheck->setChecked(s->getValue(SETTINGS_ROOT + MIN_DIST_CHECK_SETTINGS, true).toBool());
    maxDistCheck->setChecked(s->getValue(SETTINGS_ROOT + MAX_DIST_CHECK_SETTINGS, true).toBool());
    invertCheck->setChecked(s->getValue(SETTINGS_ROOT + INVERT_CHECK_SETTINGS, false).toBool());
    excludeTandemsBox->setChecked(s->getValue(SETTINGS_ROOT + TANDEMS_CHECK_SETTINGS, false).toBool());

    connect(minLenHeuristicsButton, SIGNAL(clicked()), SLOT(sl_minLenHeuristics()));
    connect(hundredPercentButton, SIGNAL(clicked()), SLOT(sl_hundredPercent()));

    connect(minLenBox, SIGNAL(valueChanged(int)), SLOT(sl_repeatParamsChanged(int)));
    connect(identityBox, SIGNAL(valueChanged(int)), SLOT(sl_repeatParamsChanged(int)));

    rs=new RegionSelector(this, seqLen, false, sc->getSequenceSelection());
    rangeSelectorLayout->addWidget(rs);
    connect(rs,SIGNAL(si_regionChanged(const U2Region&)),SLOT(sl_onRegionChanged(const U2Region&)));

    QStringList annotationNames = getAvailableAnnotationNames();
    bool haveAnnotations = !annotationNames.isEmpty();
    annotationFitCheck->setEnabled(haveAnnotations);
    annotationAroundKeepCheck->setEnabled(haveAnnotations);
    annotationAroundFilterCheck->setEnabled(haveAnnotations);
    if (haveAnnotations) {
        prepareAMenu(annotationFitButton, annotationFitEdit, annotationNames);
        prepareAMenu(annotationAroundKeepButton, annotationAroundKeepEdit, annotationNames);
        prepareAMenu(annotationAroundFilterButton, annotationAroundFilterEdit, annotationNames);
    }

    connect(minDistBox, SIGNAL(valueChanged(int)), SLOT(sl_minDistChanged(int)));
    connect(maxDistBox, SIGNAL(valueChanged(int)), SLOT(sl_maxDistChanged(int)));
    connect(minDistCheck, SIGNAL(toggled(bool)), SLOT(sl_minMaxToggle(bool)));
    connect(maxDistCheck, SIGNAL(toggled(bool)), SLOT(sl_minMaxToggle(bool)));

    updateStatus();

    setWindowIcon(QIcon(":/ugene/images/ugene_16.png"));

}

void FindRepeatsDialog::prepareAMenu(QToolButton* tb, QLineEdit* le, const QStringList& names) {
    assert(!names.isEmpty());
    QMenu* m = new QMenu(this);
    foreach(const QString& n, names) {
        QAction* a = new SetAnnotationNameAction(n, this, le);
        connect(a, SIGNAL(triggered()), SLOT(sl_setPredefinedAnnotationName()));
        m->addAction(a);
    }
    tb->setMenu(m);
    tb->setPopupMode(QToolButton::InstantPopup);
    tb->setIcon(QIcon(":/core/images/predefined_annotation_groups.png"));
}

QStringList FindRepeatsDialog::getAvailableAnnotationNames() const {
    QStringList res;
    const QSet<AnnotationTableObject *>& objs = sc->getAnnotationObjects();
    QSet<QString> names;
    foreach (AnnotationTableObject *o, objs) {
        foreach (Annotation *a, o->getAnnotations()) {
            names.insert(a->getName());
        }
    }
    res = names.toList();
    res.sort();
    return res;
}

void FindRepeatsDialog::sl_setPredefinedAnnotationName() {
    SetAnnotationNameAction* a = qobject_cast<SetAnnotationNameAction*>(sender());
    QString text = a->text();
    a->le->setText(text);
}

void FindRepeatsDialog::sl_minDistChanged(int i) {
    if (i > maxDistBox->value()) {
        maxDistBox->setValue(i);
    }
    updateStatus();
}

void FindRepeatsDialog::sl_maxDistChanged(int i) {
    if (i < minDistBox->value()) {
        minDistBox->setValue(i);
    }
    updateStatus();
}

void FindRepeatsDialog::sl_onRegionChanged(const U2Region&) {
    updateStatus();
}

bool FindRepeatsDialog::getRegions(QCheckBox* cb, QLineEdit* le, QVector<U2Region>& res) {
    bool enabled = cb->isChecked();
    QString names = le->text();
    if (!enabled || names.isEmpty()) {
        return true;
    }
    QSet<QString> aNames = names.split(',', QString::SkipEmptyParts).toSet();
    const QSet<AnnotationTableObject *> aObjs = sc->getAnnotationObjects();
    foreach (AnnotationTableObject *obj, aObjs) {
        foreach (Annotation *a, obj->getAnnotations()) {
            if (aNames.contains(a->getName())) {
                res << a->getRegions();
            }
        }
    }
    if (res.isEmpty()) {
        le->setFocus();
        QMessageBox::critical(this, L10N::errorTitle(), tr("No annotations found: %1").arg(names));
        return false;
    }
    return true;
}

// This is maximum sequence size we allow to use on 32bit OS to search for repeats.
#define MAX_REPEAT_SEQUENCE_LENGTH_32_BIT_OS (300 * 1000 * 1000)

void FindRepeatsDialog::accept() {
    int minLen = minLenBox->value();
    int identPerc = identityBox->value();
    qint64 sequenceLen = sc->getSequenceLength();

    // find repeats algorithm operates with 32 bit values and can't process 64-bit coordinates.
    // so we reject sequences that do not fit into 32 bit length range.
    if (sequenceLen > INT_MAX) {
        QMessageBox::warning(this, L10N::warningTitle(),  tr("Sequence size is too large!"));
        return;
    }
    int sequenceLenAsInt = int(sequenceLen);
    int minDist = minDistCheck->isChecked() ? minDistBox->value() : 0;
    int maxDist = maxDistCheck->isChecked() ? maxDistBox->value() : sequenceLenAsInt;
    bool inverted = invertCheck->isChecked();

    if (AppResourcePool::is32BitBuild()) {
        if (sequenceLen > MAX_REPEAT_SEQUENCE_LENGTH_32_BIT_OS) {
            QMessageBox::warning(this, L10N::warningTitle(),  tr("Sequence size is too large!"));
            return;
        }
    }

    bool isRegionOk = false;
    U2Region range = rs->getRegion(&isRegionOk);
    if (!isRegionOk){
        rs->showErrorMessage();
        return;
    }
    assert(range.length > 0);
    assert(minDist <= maxDist);
    QString err = ac->validate();
    if (!err.isEmpty()) {
        QMessageBox::critical(this, L10N::errorTitle(), err);
        return;
    }
    QVector<U2Region> fitRegions, aroundRegions, filterRegions;
    if (!getRegions(annotationFitCheck, annotationFitEdit, fitRegions)
        || !getRegions(annotationAroundKeepCheck, annotationAroundKeepEdit, aroundRegions)
        || !getRegions(annotationAroundFilterCheck, annotationAroundFilterEdit, filterRegions))
    {
        return;
    }

    RFAlgorithm algo = algoCheck->isChecked() ? RFAlgorithm(algoCombo->itemData(algoCombo->currentIndex()).toInt()) : RFAlgorithm_Auto;

    RepeatsFilterAlgorithm locFilter = RepeatsFilterAlgorithm(filterAlgorithms->itemData(filterAlgorithms->currentIndex()).toInt());

    FindRepeatsTaskSettings settings;
    const CreateAnnotationModel& cam = ac->getModel();
    settings.minLen = minLen;
    settings.mismatches = (100-identPerc) * minLen / 100;
    settings.inverted = inverted;
    settings.maxDist = maxDist;
    settings.minDist = minDist;
    settings.algo = algo;
    settings.filter = locFilter;
    settings.allowedRegions = fitRegions;
    settings.midRegionsToInclude = aroundRegions;
    settings.midRegionsToExclude = filterRegions;
    settings.reportReflected = false;
    settings.excludeTandems = excludeTandemsBox->isChecked();

    U2OpStatusImpl os;
    DNASequence seqPart = sc->getSequenceObject()->getSequence(range, os);
    CHECK_OP_EXT(os, QMessageBox::critical(this, L10N::errorTitle(), os.getError()), );
    if (seqPart.isNull() || !seqPart.alphabet) {
        QMessageBox::warning(this, tr("Error"), tr("Not enough memory error ocurred while preparing data. Try to set smaller region."));
        return;
    }
    Q_ASSERT(seqPart.alphabet && seqPart.alphabet->isNucleic());

    bool objectPrepared = ac->prepareAnnotationObject();
    if (!objectPrepared){
        QMessageBox::warning(this, tr("Error"), tr("Cannot create an annotation object. Please check settings"));
        return;
    }

    settings.seqRegion = U2Region(0, seqPart.length());
    settings.reportSeqShift = settings.reportSeq2Shift = range.startPos;

    if(settings.seqRegion.length >= 80000000 && identPerc < 100){
        if(QMessageBox::warning(QApplication::activeWindow(), tr("Warning"),
            tr("Search with given identity %1% and length more then 80m bps can take very long time. Approximate repeat searching time for 80m bp with 95% identity is 40 minutes on Intel Core 2 Quad Q9500. Do you want to continue?")
            .arg(identPerc), QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Cancel){
                return QDialog::reject();
        }
    }
    sc->getAnnotatedDNAView()->tryAddObject(cam.getAnnotationObject());

    FindRepeatsToAnnotationsTask* t = new FindRepeatsToAnnotationsTask(settings, seqPart,
        cam.data->name, cam.groupName, cam.description, cam.annotationObjectRef);
    TaskWatchdog::trackResourceExistence(sc->getSequenceObject(), t, tr("A problem occurred during finding repeats. The sequence is no more available."));

    AppContext::getTaskScheduler()->registerTopLevelTask(t);

    saveState();
    QDialog::accept();
}

void FindRepeatsDialog::saveState() {
    Settings* s = AppContext::getSettings();

    int minLen = minLenBox->value();
    int identPerc = identityBox->value();
    int minDist = minDistBox->value();
    int maxDist = maxDistBox->value();
    bool minDistChecked = minDistCheck->isChecked();
    bool maxDistChecked = maxDistCheck->isChecked();
    bool invertChecked = invertCheck->isChecked();
    bool tandemsChecked = excludeTandemsBox->isChecked();

    s->setValue(SETTINGS_ROOT + MIN_LEN_SETTINGS, minLen);
    s->setValue(SETTINGS_ROOT + IDENTITY_SETTINGS, identPerc);
    s->setValue(SETTINGS_ROOT + MIN_DIST_SETTINGS, minDist);
    s->setValue(SETTINGS_ROOT + MAX_DIST_SETTINGS, maxDist);
    s->setValue(SETTINGS_ROOT + MIN_DIST_CHECK_SETTINGS, minDistChecked);
    s->setValue(SETTINGS_ROOT + MAX_DIST_CHECK_SETTINGS, maxDistChecked);
    s->setValue(SETTINGS_ROOT + INVERT_CHECK_SETTINGS, invertChecked);
    s->setValue(SETTINGS_ROOT + TANDEMS_CHECK_SETTINGS, tandemsChecked);
}

quint64 FindRepeatsDialog::areaSize() const {
    quint64 range = rs->getRegion().length;
    if (range <= 0) {
        return 0;
    }
    int minDist = minDistCheck->isChecked() ? minDistBox->value() : 0;
    int maxDist = maxDistCheck->isChecked() ? maxDistBox->value(): sc->getSequenceLength();

    quint64 dRange = qMax(0, maxDist - minDist);

    quint64 res = range * dRange;
    return res;
}

int FindRepeatsDialog::estimateResultsCount() const {
    assert(identityBox->value() == 100);
    int len = minLenBox->value();

    quint64 nVariations  = areaSize(); //max possible results
    double variationsPerLen = pow(double(4), double(len));
    quint64 res = quint64(nVariations / variationsPerLen);
    res = (res > 20) ? (res / 10) * 10 : res;
    res = (res > 200) ? (res / 100) * 100 : res;
    res = (res > 2000) ? (res / 1000) * 1000 : res;
    return res;
}

void FindRepeatsDialog::sl_minLenHeuristics() {
    identityBox->setValue(100);

    double nVariations  = areaSize();
    double resCount = 1000;
    double len = log(nVariations / resCount) / log(double(4));
    minLenBox->setValue((int)len);
}

void FindRepeatsDialog::sl_repeatParamsChanged(int) {
    updateStatus();

    minLenBox->setSingleStep(minLenBox->value() >= 20 ? 10 : 1);
}

void FindRepeatsDialog::sl_minMaxToggle(bool) {
    updateStatus();
}

void FindRepeatsDialog::updateStatus() {
    if (identityBox->value() == 100) {
        int r = estimateResultsCount();
        statusLabel->setText(tr("Estimated repeats count: %1").arg(r));
        statusLabel->setToolTip(tr("Estimated repeats count hint is based on the active settings and random sequence model"));
    } else {
        statusLabel->setText("");
        statusLabel->setToolTip("");
    }
}

void FindRepeatsDialog::sl_hundredPercent() {
    identityBox->setValue(100);
}

}//namespace

