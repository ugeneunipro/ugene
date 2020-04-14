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

#include "SequenceObjectContext.h"

#include "ADVSingleSequenceWidget.h"
#include "AnnotatedDNAView.h"


#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/GHints.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/GObjectUtils.h>
#include "U2Core/Settings.h"
#include <U2Core/U2SafePoints.h>

#include <U2Gui/MultiClickMenu.h>

#include <U2View/CodonTable.h>


namespace U2 {

const QString SequenceObjectContext::MANUAL_FRAMES = "sequenceViewSettings/manualFrames";
const QVariantList SequenceObjectContext::DEFAULT_TRANSLATIONS = { 0, 1, 2, 3, 4, 5 };

SequenceObjectContext::SequenceObjectContext (U2SequenceObject* obj, QObject* parent)
    : QObject(parent),
      seqObj(obj),
      aminoTT(NULL),
      complTT(NULL),
      selection(NULL),
      translations(NULL),
      visibleFrames(NULL),
      rowChoosed(false)
{
    selection = new DNASequenceSelection(seqObj, this);
    clarifyAminoTT = false;
    const DNAAlphabet* al  = getAlphabet();
    if (al->isNucleic()) {
        DNATranslationRegistry* translationRegistry = AppContext::getDNATranslationRegistry();
        complTT = GObjectUtils::findComplementTT(seqObj->getAlphabet());
        aminoTT = GObjectUtils::findAminoTT(seqObj, true);
        clarifyAminoTT = aminoTT == NULL;

        QList<DNATranslation*> aminoTs = translationRegistry->lookupTranslation(al, DNATranslationType_NUCL_2_AMINO);
        if (!aminoTs.empty()) {
            aminoTT = aminoTT == NULL ? translationRegistry->getStandardGeneticCodeTranslation(al) : aminoTT;
            translations = new QActionGroup(this);
            foreach(DNATranslation* t, aminoTs) {
                QAction* a = translations->addAction(t->getTranslationName());
                a->setObjectName(t->getTranslationName());
                a->setCheckable(true);
                a->setChecked(aminoTT == t);
                a->setData(QVariant(t->getTranslationId()));
                connect(a, SIGNAL(triggered()), SLOT(sl_setAminoTranslation()));
            }
            visibleFrames = new QActionGroup(this);
            visibleFrames->setExclusive(false);
            QVariantList translationStates = AppContext::getSettings()->getValue(MANUAL_FRAMES, QVariant(DEFAULT_TRANSLATIONS)).toList();
            for (int i = 0; i < 6; i++) {
                QAction* a;
                if( i < 3) {
                    a = visibleFrames->addAction(tr("Frame +%1").arg(i + 1));
                } else {
                    a = visibleFrames->addAction(tr("Frame -%1").arg(i + 1 - 3));
                }
                a->setCheckable(true);
                a->setChecked(false);
                a->setEnabled(false);
                //set row id
                a->setData(i);
                //save status
                if (translationStates.contains(i)) {
                    translationRowsStatus.append(a);
                }
                connect(a, SIGNAL(triggered()), SLOT(sl_toggleTranslations()));
            }
        }
    }
    annSelection = new AnnotationSelection(this);
    translationMenuActions = new QActionGroup(this);

    connect(seqObj, SIGNAL(si_sequenceChanged()), &commonStatisticsCache, SLOT(sl_invalidate()));
    connect(seqObj, SIGNAL(si_sequenceChanged()), &charactersOccurrenceCache, SLOT(sl_invalidate()));
    connect(seqObj, SIGNAL(si_sequenceChanged()), &dinucleotidesOccurrenceCache, SLOT(sl_invalidate()));

    connect(selection, SIGNAL(si_onSelectionChanged(GSelection *)), &commonStatisticsCache, SLOT(sl_invalidate()));
    connect(selection, SIGNAL(si_onSelectionChanged(GSelection *)), &charactersOccurrenceCache, SLOT(sl_invalidate()));
    connect(selection, SIGNAL(si_onSelectionChanged(GSelection *)), &dinucleotidesOccurrenceCache, SLOT(sl_invalidate()));
}

void SequenceObjectContext::guessAminoTT(const AnnotationTableObject *ao) {
    const DNAAlphabet *al  = getAlphabet();
    SAFE_POINT(al->isNucleic(), "Unexpected DNA alphabet detected!",);
    DNATranslation *res = NULL;
    DNATranslationRegistry *tr = AppContext::getDNATranslationRegistry();
    // try to guess relevant translation from a CDS feature (if any)
    foreach (Annotation *ann, ao->getAnnotationsByName("CDS")) {
        QList<U2Qualifier> ql;
        ann->findQualifiers("transl_table", ql);
        if (ql.size() > 0) {
            QString guess = "NCBI-GenBank #"+ql.first().value;
            res = tr->lookupTranslation(al, DNATranslationType_NUCL_2_AMINO, guess);
            if (res !=NULL) {
                break;
            }
        }
    }
    if (res != NULL) {
        clarifyAminoTT = false;
        setAminoTranslation(res->getTranslationId());
    }
}

qint64 SequenceObjectContext::getSequenceLength() const {
    return seqObj->getSequenceLength();
}

const DNAAlphabet* SequenceObjectContext::getAlphabet() const {
    return seqObj->getAlphabet();
}

QByteArray SequenceObjectContext::getSequenceData(const U2Region &r, U2OpStatus &os) const {
    return seqObj->getSequenceData(r, os);
}

U2EntityRef SequenceObjectContext::getSequenceRef() const {
    return seqObj->getSequenceRef();
}

QList<GObject *> SequenceObjectContext::getAnnotationGObjects() const {
    QList<GObject *> res;
    foreach (AnnotationTableObject *ao, annotations) {
        res.append(ao);
    }
    return res;
}

void SequenceObjectContext::sl_showDirectOnly(){
    GCOUNTER( cvar, tvar, "SequenceView::DetView::ShowDirectTranslationsOnly" );
    bool needUpdate = false;
    QList<QAction*> actionList = visibleFrames->actions();
    translationRowsStatus.clear();
    int i = 0;
    for(; i < 3; i++){
        QAction *a = actionList[i];
        if(!a->isChecked()){
            needUpdate = true;
            a->setChecked(true);
            translationRowsStatus.append(a);
        }
    }
    for(; i < 6; i++){
        QAction *a = actionList[i];
        if(a->isChecked()){
            needUpdate = true;
            a->setChecked(false);
        }
    }
    if(needUpdate){
        emit si_translationRowsChanged();
    }
}

void SequenceObjectContext::sl_showComplOnly(){
    GCOUNTER( cvar, tvar, "SequenceView::DetView::ShowComplementTranslationsOnly" );
    bool needUpdate = false;
    QList<QAction*> actionList = visibleFrames->actions();
    translationRowsStatus.clear();
    int i = 0;
    for(; i < 3; i++){
        QAction *a = actionList[i];
        if(a->isChecked()){
            needUpdate = true;
            a->setChecked(false);
        }
    }
    for(; i < 6; i++){
        QAction *a = actionList[i];
        if(!a->isChecked()){
            needUpdate = true;
            a->setChecked(true);
            translationRowsStatus.append(a);
        }
    }
    if(needUpdate){
        emit si_translationRowsChanged();
    }
}

void SequenceObjectContext::sl_showShowAll() {
    GCOUNTER( cvar, tvar, "SequenceView::DetView::ShowAllTranslations" );
    bool needUpdate = false;
    translationRowsStatus.clear();
    foreach(QAction* a, visibleFrames->actions()){
        a->setEnabled(true);
        if (!a->isChecked()) {
            needUpdate = true;
            a->setChecked(true);
            translationRowsStatus.append(a);
        }
    }
    if(needUpdate){
        emit si_translationRowsChanged();
    }
}

void SequenceObjectContext::setTranslationState(const SequenceObjectContext::TranslationState state) {
    CHECK(nullptr != visibleFrames, );

    bool needUpdate = false;

    const bool enableActions = state == SequenceObjectContext::TS_SetUpFramesManually;
    foreach(QAction* a, visibleFrames->actions()) {
        a->setEnabled(enableActions);
        bool isActionCheck = false;
        if (enableActions) {
            isActionCheck = translationRowsStatus.contains(a);
        } else if (state == SequenceObjectContext::TS_ShowAllFrames) {
            isActionCheck = true;
        }

        if (a->isChecked() != isActionCheck) {
            needUpdate = true;
            a->setChecked(isActionCheck);
        }
    }

    if (needUpdate) {
        emit si_translationRowsChanged();
    }
}

SequenceObjectContext::TranslationState SequenceObjectContext::getTranslationState() const {
    CHECK(translationMenuActions->actions().size() == 4, SequenceObjectContext::TS_DoNotTranslate);
    return (SequenceObjectContext::TranslationState)translationMenuActions->checkedAction()->data().toInt();
}

StatisticsCache<DNAStatistics> *SequenceObjectContext::getCommonStatisticsCache() {
    return &commonStatisticsCache;
}

StatisticsCache<CharactersOccurrence> *SequenceObjectContext::getCharactersOccurrenceCache() {
    return &charactersOccurrenceCache;
}

StatisticsCache<DinucleotidesOccurrence> *SequenceObjectContext::getDinucleotidesOccurrenceCache() {
    return &dinucleotidesOccurrenceCache;
}

void SequenceObjectContext::sl_onAnnotationRelationChange() {
    AnnotationTableObject* obj = qobject_cast<AnnotationTableObject*>(sender());
    SAFE_POINT(obj != NULL, tr("Incorrect signal sender!"),);

    if (!obj->hasObjectRelation(seqObj, ObjectRole_Sequence)) {
        disconnect(obj, SIGNAL(si_relationChanged(const QList<GObjectRelation> &)), this, SLOT(sl_onAnnotationRelationChange()));
    }
}

QMenu * SequenceObjectContext::createGeneticCodeMenu() {
    CHECK(NULL != translations, NULL);
    QMenu *menu = new QMenu(tr("Select genetic code"));
    menu->setIcon(QIcon(":core/images/tt_switch.png"));
    menu->menuAction()->setObjectName("AminoTranslationAction");

    foreach (QAction *a, translations->actions()) {
        menu->addAction(a);
    }
    return menu;
}

QMenu * SequenceObjectContext::createTranslationFramesMenu(QList<QAction*> menuActions) {
    SAFE_POINT(visibleFrames != NULL, "SequenceObjectContext: visibleFrames is NULL ?!", NULL);
    QMenu *menu = new QMenu(tr("Show/hide amino acid translations"));
    menu->setIcon(QIcon(":core/images/show_trans.png"));
    menu->menuAction()->setObjectName("Translation frames");
    new MultiClickMenu(menu);

    foreach(QAction* a, menuActions) {
        translationMenuActions->addAction(a);
        menu->addAction(a);
    }
    translationMenuActions->setExclusive(true);

    menu->addSeparator();

    foreach(QAction* a, visibleFrames->actions()) {
        menu->addAction(a);
    }
    return menu;
}

void SequenceObjectContext::setAminoTranslation(const QString& tid) {
    const DNAAlphabet* al = getAlphabet();
    DNATranslation* aTT = AppContext::getDNATranslationRegistry()->lookupTranslation(al, DNATranslationType_NUCL_2_AMINO, tid);
    assert(aTT!=NULL);
    if (aTT == aminoTT) {
        return;
    }
    aminoTT = aTT;
    foreach(QAction* a, translations->actions()) {
        if (a->data().toString() == tid) {
            a->setChecked(true);
            break;
        }
    }
    seqObj->getGHints()->set(AMINO_TT_GOBJECT_HINT, tid);
    emit si_aminoTranslationChanged();
}

void SequenceObjectContext::sl_setAminoTranslation() {
    GCOUNTER( cvar, tvar, "DetView_SetAminoTranslation" );
    QAction* a = qobject_cast<QAction*>(sender());
    QString tid = a->data().toString();
    setAminoTranslation(tid);
}

AnnotationSelection* SequenceObjectContext::getAnnotationsSelection() const {
    return annSelection;
}

void SequenceObjectContext::removeSequenceWidget(ADVSequenceWidget* w) {
    assert(seqWidgets.contains(w));
    seqWidgets.removeOne(w);
}

void SequenceObjectContext::addSequenceWidget(ADVSequenceWidget* w) {
    assert(!seqWidgets.contains(w));
    seqWidgets.append(w);
}

void SequenceObjectContext::addAnnotationObject(AnnotationTableObject *obj) {
    SAFE_POINT(!annotations.contains(obj), "Unexpected annotation table!",);
    SAFE_POINT(obj->hasObjectRelation(seqObj, ObjectRole_Sequence), "Annotation table relates to unexpected sequence!",);
    connect(obj, SIGNAL(si_relationChanged(const QList<GObjectRelation> &)), SLOT(sl_onAnnotationRelationChange()));
    annotations.insert(obj);
    emit si_annotationObjectAdded(obj);
    if (clarifyAminoTT) {
        guessAminoTT(obj);
    }
}

void SequenceObjectContext::removeAnnotationObject(AnnotationTableObject *obj) {
    SAFE_POINT(annotations.contains(obj), "Unexpected annotation table!",);
    annotations.remove(obj);
    emit si_annotationObjectRemoved(obj);
}

void SequenceObjectContext::emitAnnotationActivated(Annotation* annotation, int regionIndex) {
    emit si_annotationActivated(annotation, regionIndex);
}

void SequenceObjectContext::emitAnnotationDoubleClicked(Annotation* annotation, int regionIndex) {
    emit si_annotationDoubleClicked(annotation, regionIndex);
}

void SequenceObjectContext::emitClearSelectedAnnotationRegions() {
    emit si_clearSelectedAnnotationRegions();
}

QList<Annotation *> SequenceObjectContext::selectRelatedAnnotations(const QList<Annotation *> &alist) const {
    QList<Annotation *> res;
    foreach (Annotation *a, alist) {
        AnnotationTableObject* o = a->getGObject();
        if (annotations.contains(o) || autoAnnotations.contains(o)) {
            res.append(a);
        }
    }
    return res;
}

GObject * SequenceObjectContext::getSequenceGObject() const {
    return seqObj;
}

void SequenceObjectContext::addAutoAnnotationObject(AnnotationTableObject *obj) {
    autoAnnotations.insert(obj);
    emit si_annotationObjectAdded(obj);
}

QSet<AnnotationTableObject *> SequenceObjectContext::getAnnotationObjects(bool includeAutoAnnotations) const {
    QSet<AnnotationTableObject *> result = annotations;
    if (includeAutoAnnotations) {
        result += autoAnnotations;
    }

    return result;
}

void SequenceObjectContext::sl_toggleTranslations() {
    QAction* a = qobject_cast<QAction*>(QObject::sender());
    CHECK(a != nullptr, );

    if (a->isChecked()) {
        translationRowsStatus.append(a);
    } else {
        translationRowsStatus.removeOne(a);
    }

    QVariantList translationStates;
    foreach(QAction* act, translationRowsStatus) {
        translationStates.append(act->data().toInt());
    }
    AppContext::getSettings()->setValue(MANUAL_FRAMES, translationStates);

    rowChoosed = true;
    emit si_translationRowsChanged();
    rowChoosed = false;
}

bool SequenceObjectContext::isRowChoosed(){
    return rowChoosed;
}

QVector<bool> SequenceObjectContext::getTranslationRowsVisibleStatus() {
    QVector<bool> result;
    if (visibleFrames != NULL) {
        foreach(QAction* a, visibleFrames->actions()) {
            result.append(a->isChecked());
        }
    }
    return result;
}
void SequenceObjectContext::setTranslationsVisible(bool visible) {
    bool needUpdate = false;
    foreach(QAction* a, visibleFrames->actions()) {
        if (!visible) {
            if(a->isChecked()) {
                needUpdate = true;
                a->setChecked(false);
            }
        } else {
            if(!a->isChecked() && (translationRowsStatus.contains(a) || translationRowsStatus.isEmpty())) {
                needUpdate = true;
                a->setChecked(true);
            }
        }
    }
    if(needUpdate){
        emit si_translationRowsChanged();
    }
}

void SequenceObjectContext::showComplementActions(bool show) {
    CHECK(nullptr != visibleFrames, );

    QList<QAction*> actions = visibleFrames->actions();
    for (int i = 3; i < 6; i++) {
        actions[i]->setVisible(show);
    }
}

void SequenceObjectContext::showTranslationFrame(const int numOfAction, const bool setChecked) {
    QList<QAction*> actions = visibleFrames->actions();
    SAFE_POINT(0 <= numOfAction && numOfAction < 6, "Incorrect action", );
    actions[numOfAction]->setChecked(setChecked);
}

} // namespace U2
