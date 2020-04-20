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

#include <QPushButton>

#include "AnnotHighlightWidget.h"

#include <U2Core/AnnotationSelection.h>
#include <U2Core/AnnotationSettings.h>
#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U1AnnotationUtils.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>
#include <U2View/ADVSingleSequenceWidget.h>
#include <U2View/PanView.h>
#include <limits.h>

namespace U2 {


ShowAllAnnotTypesLabel::ShowAllAnnotTypesLabel()
{
    // By default, show only types for the sequence
    showAllIsSelected = false;
    setText(QObject::tr("Show all annotation names"));

    setStyleSheet(
        "text-decoration: underline;"
        "color: gray;"
        "margin-left: 2px;"
        "margin-top: 1px;");
}

void ShowAllAnnotTypesLabel::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    if (showAllIsSelected) {
        showAllIsSelected = false;
        setText(QObject::tr("Show all annotation names"));
    }
    else {
        showAllIsSelected = true;
        setText(QObject::tr("Show names for the sequence only"));
    }

    emit si_showAllStateChanged();
}


AnnotHighlightWidget::AnnotHighlightWidget(AnnotatedDNAView* _annotatedDnaView)
    : annotatedDnaView(_annotatedDnaView)
{
    SAFE_POINT(0 != annotatedDnaView, "AnnotatedDNAView is NULL!",);
    initLayout();
    connectSlots();
    loadAnnotTypes();
}

void AnnotHighlightWidget::initLayout() {
    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(10);
    mainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    noAnnotTypesLabel = new QLabel("");
    noAnnotTypesLabel->setWordWrap(true);
    setNoAnnotTypesLabelValue();
    noAnnotTypesLabel->setStyleSheet("margin-left: 2px;");
    noAnnotTypesLabel->setObjectName("noAnnotTypesLabel");

    annotTreeTitle = new QLabel(tr("Select an annotation name:"));

    // Tree
    QVBoxLayout* treeLayout = new QVBoxLayout();
    treeLayout->setContentsMargins(0, 0, 0, 10);
    treeLayout->setSpacing(0);

    annotTree = new AnnotHighlightTree();

    showAllLabel = new ShowAllAnnotTypesLabel();
    showAllLabel->setObjectName("show_all_annotation_types");

    treeLayout->addWidget(annotTree);
    treeLayout->addWidget(showAllLabel);

    // Configure settings
    QVBoxLayout* settingsLayout = new QVBoxLayout();
    settingsLayout->setContentsMargins(0, 0, 0, 0);
    settingsLayout->setSpacing(0);
    settingsLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    settingsTitle = new QLabel(tr("Configure the annotations:"));
    annotSettingsWidget = new AnnotHighlightSettingsWidget();

    settingsLayout->addWidget(settingsTitle);
    settingsLayout->addWidget(annotSettingsWidget);

    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->setSpacing(0);

    prevAnnotationButton = new QPushButton(QIcon(":core/images/backward.png"), "");
    prevAnnotationButton->setFixedSize(32,32);
    prevAnnotationButton->setToolTip(AnnotHighlightWidget::tr("Previous annotation"));
    prevAnnotationButton->setDisabled(true);
    prevAnnotationButton->setObjectName("prevAnnotationButton");
    buttonsLayout->addWidget(prevAnnotationButton);
    buttonsLayout->addSpacerItem(new QSpacerItem(0,0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    nextAnnotationButton = new QPushButton(QIcon(":core/images/forward.png"), "");
    nextAnnotationButton->setFixedSize(32,32);
    nextAnnotationButton->setToolTip(AnnotHighlightWidget::tr("Next annotation"));
    nextAnnotationButton->setObjectName("nextAnnotationButton");
    buttonsLayout->addWidget(nextAnnotationButton);
    if (noAnnotatedRegions()) {
        nextAnnotationButton->setDisabled(true);
    } else {
        sl_onAnnotationSelectionChanged();
    }

    // Init main layout
    mainLayout->addWidget(noAnnotTypesLabel);
    mainLayout->addWidget(annotTreeTitle);
    mainLayout->addLayout(treeLayout);
    mainLayout->addLayout(settingsLayout);
    mainLayout->addLayout(buttonsLayout);

    setLayout(mainLayout);
}

bool AnnotHighlightWidget::isFirstAnnotatedRegion(Annotation *annotation, const U2Region &region, bool fromTheBeginning) const {
    AnnotatedRegion annRegion;
    if (findFirstAnnotatedRegion(annRegion, fromTheBeginning)) {
        if (annRegion.annotation == annotation) {
            SAFE_POINT(annRegion.annotation->getRegions().size() > annRegion.regionIdx, "Invalid annotation region", false);
            if (annRegion.annotation->getRegions()[annRegion.regionIdx] == region) {
                return true;
            }
        }
    }
    return false;
}

bool AnnotHighlightWidget::noAnnotatedRegions() const {
    const QList<AnnotationTableObject *> items = annotatedDnaView->getAnnotationObjects(true);
    foreach (AnnotationTableObject *object, items) {
        SAFE_POINT(object != NULL, "Annotation table object is NULL", true);
        if (object->hasAnnotations()) {
            return false;
        }
    }
    return true;
}

void AnnotHighlightWidget::selectNextAnnotation(bool isForward) const {
    AnnotationSelection *as = annotatedDnaView->getAnnotationsSelection();
    CHECK(as != NULL,);

    bool isAnnRegionValid = false;
    AnnotatedRegion annRegion;

    if (as->isEmpty() && isForward) {
        // no selected annotation regions - choose first one
        isAnnRegionValid = findFirstAnnotatedRegion(annRegion);
    } else {
        // choose next acoording to selection
        isAnnRegionValid = findNextUnselectedAnnotatedRegion(annRegion, isForward);
    }

    if (isAnnRegionValid) {
        as->clear();
        as->add(annRegion.annotation);
    }
    return;
}

bool AnnotHighlightWidget::findFirstAnnotatedRegion(AnnotatedRegion &annRegion, bool fromTheBeginging) const {
    return findFirstAnnotatedRegionAfterPos(annRegion,
                                            fromTheBeginging ? -1 : LLONG_MAX,
                                            fromTheBeginging);
}

bool AnnotHighlightWidget::findFirstAnnotatedRegionAfterPos(AnnotatedRegion &annRegion, qint64 startPos, bool isForward) const {
    qint64 boundary = isForward ? LLONG_MAX : -1;
    qint64 sign = isForward ? 1 : -1;
    qint64 pos = boundary;

    const QList<AnnotationTableObject *> annObjects = annotatedDnaView->getAnnotationObjects(true);
    foreach (AnnotationTableObject *annObject, annObjects) {
        SAFE_POINT(annotatedDnaView->getSequenceContext(annObject) != NULL, tr("Sequence context is NULL"), false);
        qint64 seqLen = annotatedDnaView->getSequenceContext(annObject)->getSequenceLength();
        QList<Annotation *> annots = annObject->getAnnotationsByRegion(U2Region(isForward ? startPos : 0, isForward ? seqLen - startPos : startPos));
        foreach (Annotation *a, annots) {
            QVector <U2Region> regions = a->getRegions();
            for (int i = 0; i < regions.size(); i++){
                if (sign * regions[i].startPos > sign * startPos && sign *regions[i].startPos < sign * pos) {
                    pos = regions[i].startPos;
                    annRegion.annotation = a;
                    annRegion.regionIdx = i;
                }
            }
        }
    }

    if (pos == boundary) {
        return false;
    }

    QList<AnnotatedRegion> regionsAtTheSamePosition = getAllAnnotatedRegionsByStartPos(pos);
    if (!regionsAtTheSamePosition.isEmpty()) {
        annRegion = regionsAtTheSamePosition[isForward ? 0 : (regionsAtTheSamePosition.size() - 1)];
        return true;
    }

    return false;
}

bool AnnotHighlightWidget::findNextUnselectedAnnotatedRegion(AnnotatedRegion &annRegion, bool fromTheBeginning) const {

    AnnotationSelection* as = annotatedDnaView->getAnnotationsSelection();
    CHECK(as != NULL, false);
    CHECK(!as->isEmpty(), false);

    // detect the most right/left start position in selection
    const QList<Annotation*> selectionData = as->getAnnotations();
    int start = -1;
    foreach (Annotation* selectionItem, selectionData) {
        foreach (U2Region region, selectionItem->getRegions()) {
            if (start == -1) {
                start = region.startPos;
            } else {
                if ((region.startPos - start) * (fromTheBeginning ? 1 : -1) > 0) {
                    start = region.startPos;
                }
            }
        }
    }

    QList<AnnotatedRegion> regionsAtTheSamePosition = getAllAnnotatedRegionsByStartPos(start);
    // find the next unselected
    for (int i = 0; i < regionsAtTheSamePosition.size(); i++) {
        int idx = fromTheBeginning ? regionsAtTheSamePosition.size() - 1 - i : i;
        if (as->contains(regionsAtTheSamePosition[idx].annotation)) {
            idx += (fromTheBeginning ? 1 : -1);
            if (idx < 0 || idx == regionsAtTheSamePosition.size()) {
                break;
            }
            annRegion = regionsAtTheSamePosition[idx];
            return true;
        }

    }

    return findFirstAnnotatedRegionAfterPos(annRegion, start, fromTheBeginning);
}

QList<AnnotatedRegion> AnnotHighlightWidget::getAllAnnotatedRegionsByStartPos(qint64 startPos) const {
    const QList<AnnotationTableObject *> annObjects = annotatedDnaView->getAnnotationObjects(true);
    return U1AnnotationUtils::getAnnotatedRegionsByStartPos(annObjects, startPos);
}

void AnnotHighlightWidget::sl_onNextAnnotationClick() {
    GCOUNTER(cvar, tvar, "Annotations navigation: next annotation");
    selectNextAnnotation(true);
}

void AnnotHighlightWidget::sl_onPrevAnnotationClick() {
    GCOUNTER(cvar, tvar, "Annotations navigation: previous annotation");
    selectNextAnnotation(false);
}

void AnnotHighlightWidget::sl_onAnnotationSelectionChanged() {
    AnnotationSelection* as = annotatedDnaView->getAnnotationsSelection();
    CHECK(as != NULL,);

    if (as->isEmpty()) {
        nextAnnotationButton->setDisabled(noAnnotatedRegions());
        prevAnnotationButton->setDisabled(true);
    } else {
        nextAnnotationButton->setDisabled(false);
        prevAnnotationButton->setDisabled(false);

        // find first or last annotation region
        foreach (Annotation* a, as->getAnnotations()) {
            foreach (U2Region region, a->getRegions()) {
                if (isFirstAnnotatedRegion(a, region, false)) {
                    nextAnnotationButton->setDisabled(true);
                }
                if (isFirstAnnotatedRegion(a, region)) {
                    prevAnnotationButton->setDisabled(true);
                }
            }
        }

    }
}

void AnnotHighlightWidget::setNoAnnotTypesLabelValue() {
    QList<ADVSequenceObjectContext*> seqContexts = annotatedDnaView->getSequenceContexts();

    if (1 == seqContexts.count()) {
        noAnnotTypesLabel->setText(tr("The sequence doesn't have any annotations."));
    }
    else {
        noAnnotTypesLabel->setText(tr("The sequences do not have any annotations."));
    }
}


void AnnotHighlightWidget::setNoAnnotsLayout() {
    noAnnotTypesLabel->show();
    annotTreeTitle->hide();
    annotTree->hide();
    settingsTitle->hide();
    annotSettingsWidget->hide();
}


void AnnotHighlightWidget::setLayoutWithAnnotsSelection() {
    noAnnotTypesLabel->hide();
    annotTreeTitle->show();
    annotTree->show();
    settingsTitle->show();
    annotSettingsWidget->show();
}

void AnnotHighlightWidget::connectSlots() {
    // Show annotation types for the sequence or all annotation types
    connect(showAllLabel, SIGNAL(si_showAllStateChanged()), this, SLOT(sl_onShowAllStateChanged()));

    // Selected annotation type has changed
    connect(annotTree, SIGNAL(si_selectedItemChanged(QString)), SLOT(sl_onSelectedItemChanged(QString)));

    // Another color or a setting has been selected for an annotation type
    connect(annotTree, SIGNAL(si_colorChanged(QString, QColor)), SLOT(sl_storeNewColor(QString, QColor)));
    connect(annotSettingsWidget, SIGNAL(si_annotSettingsChanged(AnnotationSettings*)),
        SLOT(sl_storeNewSettings(AnnotationSettings*)));

    // A sequence has been modified (a subsequence added, removed, etc.)
    connect(annotatedDnaView, SIGNAL(si_sequenceModified(ADVSequenceObjectContext*)),
        this, SLOT(sl_onSequenceModified(ADVSequenceObjectContext*)));

    // An annotation object has been added/removed - connect/disconnect slots
    connect(annotatedDnaView, SIGNAL(si_annotationObjectAdded(AnnotationTableObject*)),
        SLOT(sl_onAnnotationObjectAdded(AnnotationTableObject*)));
    connect(annotatedDnaView, SIGNAL(si_annotationObjectRemoved(AnnotationTableObject*)),
        SLOT(sl_onAnnotationObjectRemoved(AnnotationTableObject*)));

    // An annotation has been added/removed/modified
    QList<AnnotationTableObject *> seqAnnotTableObjs = annotatedDnaView->getAnnotationObjects(true); // "true" to include auto-annotations
    foreach (const AnnotationTableObject *annotTableObj, seqAnnotTableObjs) {
        connectSlotsForAnnotTableObj(annotTableObj);
    }

    connect(prevAnnotationButton, SIGNAL(clicked()),this, SLOT(sl_onPrevAnnotationClick()));

    connect(nextAnnotationButton, SIGNAL(clicked()), this, SLOT(sl_onNextAnnotationClick()));

    AnnotationSelection *as = annotatedDnaView->getAnnotationsSelection();
    CHECK(as != NULL, );
    connect(as, SIGNAL(si_selectionChanged(AnnotationSelection *, const QList<Annotation *> &, const QList<Annotation *> &)),
            SLOT(sl_onAnnotationSelectionChanged()));
}

void AnnotHighlightWidget::connectSlotsForAnnotTableObj(const AnnotationTableObject *annotTableObj) {
    connect(annotTableObj, SIGNAL(si_onAnnotationsAdded(const QList<Annotation *> &)),
        SLOT(sl_onAnnotationsAdded(const QList<Annotation *> &)));
    connect(annotTableObj, SIGNAL(si_onAnnotationsRemoved(const QList<Annotation *> &)),
        SLOT(sl_onAnnotationsRemoved(const QList<Annotation *> &)));
    connect(annotTableObj, SIGNAL(si_onAnnotationsModified(const QList<AnnotationModification> &)),
            SLOT(sl_onAnnotationsModified()));
}

void AnnotHighlightWidget::disconnectSlotsForAnnotTableObj(const AnnotationTableObject *annotTableObj) {
    disconnect(annotTableObj, SIGNAL(si_onAnnotationsAdded(const QList<Annotation *> &)),
        this, SLOT(sl_onAnnotationsAdded(const QList<Annotation *> &)));
    disconnect(annotTableObj, SIGNAL(si_onAnnotationsRemoved(const QList<Annotation *> &)),
        this, SLOT(sl_onAnnotationsRemoved(const QList<Annotation *> &)));
    disconnect(annotTableObj, SIGNAL(si_onAnnotationsModified(const QList<AnnotationModification> &)),
        this, SLOT(sl_onAnnotationsModified()));
}

void AnnotHighlightWidget::sl_onShowAllStateChanged() {
    loadAnnotTypes();
}

void AnnotHighlightWidget::sl_onSelectedItemChanged(const QString &annotName) {
    AnnotationSettingsRegistry *annotRegistry = AppContext::getAnnotationsSettingsRegistry();

    AnnotationSettings *selectedAnnotSettings = annotRegistry->getAnnotationSettings(annotName);
    annotSettingsWidget->setSettings(selectedAnnotSettings, annotNamesWithAminoInfo.value(annotName));
}

void AnnotHighlightWidget::findAllAnnotationsNamesForSequence() {
    annotNamesWithAminoInfo.clear();

    QList<ADVSequenceObjectContext*> seqObjContexts = annotatedDnaView->getSequenceContexts();

    foreach (ADVSequenceObjectContext *seqContext, seqObjContexts) {
        const DNAAlphabet *seqAlphabet = seqContext->getAlphabet();
        bool isAminoSeq = seqAlphabet->isAmino();

        QSet<AnnotationTableObject *> seqAnnotTableObjects = seqContext->getAnnotationObjects(true);

        foreach (AnnotationTableObject *annotTableObj, seqAnnotTableObjects) {
            const QList<Annotation *> annotations = annotTableObj->getAnnotations();

            foreach (Annotation *annot, annotations) {
                const QString annotName = annot->getName();

                // If the annotation was found on a nucleotide sequence
                if (!isAminoSeq) {
                    // The "Show on translation" option should be enabled for this annotation type
                    annotNamesWithAminoInfo.insert(annotName, false);
                }
                // The annotation was found on an amino acid sequence
                else {
                    if (annotNamesWithAminoInfo.contains(annotName)) {
                        // Do nothing:
                        // if the value is "true", then it doesn't make sense to rewrite the same value
                        // if the value is "false", then the annotation was previously found on a nucleotide
                        // sequence and therefore the "Show on translation" option should be enabled
                    }
                    else {
                        // Disable the option
                        annotNamesWithAminoInfo.insert(annotName, true);
                    }
                }
            }
        }
    }
}


void AnnotHighlightWidget::findAllAnnotationsNamesInSettings()
{
    annotNamesWithAminoInfo.clear();

    AnnotationSettingsRegistry* registry = AppContext::getAnnotationsSettingsRegistry();
    SAFE_POINT(0 != registry, "AnnotationSettingsRegistry is NULL!",);

    QStringList annotSettings = registry->getAllSettings();
    foreach (QString setting, annotSettings) {
        annotNamesWithAminoInfo.insert(setting, false);
    }
}

void AnnotHighlightWidget::updateAnnotationNames() {
    if (showAllLabel->isShowAllSelected()) {
        findAllAnnotationsNamesInSettings();
    } else {
        findAllAnnotationsNamesForSequence();
    }
}

void AnnotHighlightWidget::loadAnnotTypes()
{
    // Get the annotation names
    updateAnnotationNames();

    QList<QString> annotNames = annotNamesWithAminoInfo.keys();
    qSort(annotNames);

    // Get the currently selected annotation name
    QString currentAnnotName = annotTree->getCurrentItemAnnotName();

    // Clear the old items and restore the initial height
    annotTree->clear();

    // If there types to show
    if (!annotNames.isEmpty()) {
        setLayoutWithAnnotsSelection();

        // Add the tree items
        AnnotationSettingsRegistry* annotRegistry = AppContext::getAnnotationsSettingsRegistry();
        foreach(const QString& name, annotNames) {
            AnnotationSettings* annotSettings = annotRegistry->getAnnotationSettings(name);
            annotTree->addItem(name, annotSettings->color);
        }

        // By default, select either previously selected item (if it is present) or the first item
        if (annotNames.contains(currentAnnotName)) {
            annotTree->setItemSelectedWithAnnotName(currentAnnotName);
        }
        else {
            annotTree->setFirstItemSelected();
            currentAnnotName = annotTree->getFirstItemAnnotName();
            SAFE_POINT(currentAnnotName != QString(), "Failed to get first annotation name!",);
        }

        // Set the configuration settings for the item
        AnnotationSettings* currentAnnotSettings = annotRegistry->getAnnotationSettings(currentAnnotName);
        annotSettingsWidget->setSettings(currentAnnotSettings, annotNamesWithAminoInfo.value(currentAnnotName));
    }
    else {
        setNoAnnotsLayout();
    }

    if (noAnnotatedRegions()) {
        nextAnnotationButton->setDisabled(true);
    }
}

void AnnotHighlightWidget::sl_storeNewColor(const QString& annotName, const QColor& newColor)
{
    QList<AnnotationSettings*> annotToWrite;
    AnnotationSettingsRegistry* annotRegistry = AppContext::getAnnotationsSettingsRegistry();
    AnnotationSettings* annotSettings = annotRegistry->getAnnotationSettings(annotName);
    if (annotSettings->color != newColor) {
        annotSettings->color = newColor;
        annotToWrite.append(annotSettings);
        annotRegistry->changeSettings(annotToWrite, true);
    }
}

void AnnotHighlightWidget::sl_storeNewSettings(AnnotationSettings* annotSettings)
{
    QList<AnnotationSettings*> annotToWrite;
    AnnotationSettingsRegistry* annotRegistry = AppContext::getAnnotationsSettingsRegistry();
    annotToWrite.append(annotSettings);
    annotRegistry->changeSettings(annotToWrite, true);
}

void AnnotHighlightWidget::sl_onSequenceModified(ADVSequenceObjectContext* /* seqContext */)
{
    loadAnnotTypes();
}

void AnnotHighlightWidget::sl_onAnnotationsAdded(const QList<Annotation *> & /* annotations */) {
    loadAnnotTypes();
}

void AnnotHighlightWidget::sl_onAnnotationsRemoved(const QList<Annotation *> &annotations) {
    CHECK(!showAllLabel->isShowAllSelected(), );

    const QString selectedAnnotName = annotTree->getCurrentItemAnnotName();

    QMap<QString, int> observedAnnNames;
    foreach (Annotation *a, annotations) {
        const QString annotName = a->getName();
        if (observedAnnNames.contains(annotName)) {
            observedAnnNames[annotName]++;
            continue;
        }
        observedAnnNames.insert(annotName, 1);
    }

    bool isSelectedItemRemoved = false;
    QList<AnnotationTableObject*> annTables = annotatedDnaView->getAnnotationObjects(true);
    foreach (const QString& annotName, observedAnnNames.keys()) {
        int count = 0;
        foreach (AnnotationTableObject* t, annTables) {
            count += t->getAnnotationsByName(annotName).size();
        }
        if (count == observedAnnNames[annotName]) {
            QList<QTreeWidgetItem*> itemList = annotTree->findItems(annotName, Qt::MatchExactly);
            SAFE_POINT(itemList.size() == 1, "Annotation Highlight tree should contain only one item per annotation type", );
            QTreeWidgetItem* item = itemList.first();
            delete annotTree->takeTopLevelItem(annotTree->indexOfTopLevelItem(item));
            annotNamesWithAminoInfo.remove(annotName);
            if (selectedAnnotName == annotName) {
                isSelectedItemRemoved = true;
            }
        }
    }

    if (annotNamesWithAminoInfo.count() == 0) {
        annotTree->clear();
        setNoAnnotsLayout();
        nextAnnotationButton->setDisabled(true);
    } else {
        setLayoutWithAnnotsSelection();
        if (isSelectedItemRemoved || selectedAnnotName.isEmpty()) {
            annotTree->setFirstItemSelected();
        } else {
            annotTree->setItemSelectedWithAnnotName(selectedAnnotName);
        }
    }
}

void AnnotHighlightWidget::sl_onAnnotationsModified() {
    loadAnnotTypes();
}

void AnnotHighlightWidget::sl_onAnnotationObjectAdded(AnnotationTableObject *annotTableObj)
{
    connectSlotsForAnnotTableObj(annotTableObj);
    loadAnnotTypes();
}

void AnnotHighlightWidget::sl_onAnnotationObjectRemoved(AnnotationTableObject *annotTableObj)
{
    disconnectSlotsForAnnotTableObj(annotTableObj);
    loadAnnotTypes();
}

} // namespace
