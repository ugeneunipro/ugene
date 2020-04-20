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

#include <QMessageBox>
#include <QButtonGroup>

#include <U2Core/AnnotationSettings.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/GObject.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2View/ADVSequenceObjectContext.h>

#include "DotPlotFilterDialog.h"

namespace U2 {

DotPlotFilterDialog::DotPlotFilterDialog(QWidget *parent, ADVSequenceObjectContext *sequenceX, ADVSequenceObjectContext *sequenceY)
: QDialog(parent)
,xSeq(sequenceX)
,ySeq(sequenceY)
,fType(All)
,filterGroup(NULL)
,seqXItem(NULL)
,seqYItem(NULL)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "24748831");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    {
        filterGroup = new QButtonGroup(this);
        filterGroup->addButton(noFiltrationRadio, All);
        filterGroup->addButton(featuresRadio, Features);

        connect(filterGroup, SIGNAL(buttonClicked(int)), SLOT(sl_filterTypeChanged(int)));
        sl_filterTypeChanged(filterGroup->checkedId());
    }

    {
        if (xSeq == ySeq){
            differentButton->setEnabled(false);
        }
    }
    {
        QSet<QString> uniqueAnnotationNames = getUniqueAnnotationNames(xSeq);
        seqXItem = new QTreeWidgetItem(featuresTree);
        seqXItem->setText(0, "Sequence X: " + xSeq->getSequenceGObject()->getGObjectName());
        AnnotationSettingsRegistry* asr = AppContext::getAnnotationsSettingsRegistry();
        foreach(const QString& aName, uniqueAnnotationNames){
            QTreeWidgetItem* aItem = new QTreeWidgetItem(seqXItem);
            aItem->setText(0, aName);
            AnnotationSettings* as = asr->getAnnotationSettings(aName);
            QIcon icon = GUIUtils::createSquareIcon(as->color, 14);
            aItem->setIcon(0, icon);
            aItem->setCheckState(0, Qt::Unchecked);
        }
        seqXItem->setExpanded(true);

        if(xSeq != ySeq){
            uniqueAnnotationNames = getUniqueAnnotationNames(ySeq);
            seqYItem = new QTreeWidgetItem(featuresTree);
            seqYItem->setText(0, "Sequence Y: " + ySeq->getSequenceGObject()->getGObjectName());
            foreach(const QString& aName, uniqueAnnotationNames){
                QTreeWidgetItem* aItem = new QTreeWidgetItem(seqYItem);
                aItem->setText(0, aName);
                AnnotationSettings* as = asr->getAnnotationSettings(aName);
                QIcon icon = GUIUtils::createSquareIcon(as->color, 14);
                aItem->setIcon(0, icon);
                aItem->setCheckState(0, Qt::Unchecked);
            }
            seqYItem->setExpanded(true);
        }
    }
    {
        connect(allButton, SIGNAL(clicked()), SLOT(sl_selectAll()));
        connect(differentButton, SIGNAL(clicked()), SLOT(sl_selectDifferent()));
        connect(invertButton, SIGNAL(clicked()), SLOT(sl_invertSelection()));
        connect(clearButton, SIGNAL(clicked()), SLOT(sl_clearSelection()));
    }
}

QSet<QString> DotPlotFilterDialog::getUniqueAnnotationNames(ADVSequenceObjectContext *seq) {
    QSet<QString> uniqueAnnotationNames;
    CHECK(seq != NULL, uniqueAnnotationNames);

    QSet<AnnotationTableObject *> annotationObjects = seq->getAnnotationObjects(true);
    foreach (AnnotationTableObject *atObj, annotationObjects) {
        QList<Annotation *> annotations = atObj->getAnnotations();
        foreach (Annotation *a, annotations) {
            uniqueAnnotationNames.insert(a->getName());
        }
    }
    return uniqueAnnotationNames;
}

void DotPlotFilterDialog::accept() {
    QMultiMap<FilterIntersectionParameter, QString> selectedItems;
    bool sIempty = true;

    int childCount = seqXItem->childCount();
    for (int i = 0; i < childCount; i++){
        QTreeWidgetItem* tItem = seqXItem->child(i);
        if(tItem->checkState(0) == Qt::Checked){
            selectedItems.insert(SequenceX, tItem->text(0));
            sIempty = false;
        }
    }

    if(seqYItem!=NULL){
        childCount = seqYItem->childCount();
        for (int i = 0; i < childCount; i++){
            QTreeWidgetItem* tItem = seqYItem->child(i);
            if(tItem->checkState(0) == Qt::Checked){
                selectedItems.insert(SequenceY, tItem->text(0));
                sIempty = false;
            }
        }
    }

    if(sIempty && getFilterType() == Features){
        QObjectScopedPointer<QMessageBox> mb = new QMessageBox(QMessageBox::Warning, tr("Feature names"), tr("No feature names have been selected. In that case dotplot will be empty. Note, If the feature names list is empty your sequences don't have annotations. Select some feature names or choose another filtration method"));
        mb->exec();
        CHECK(!mb.isNull(), );
    }else{
        featureNames = selectedItems;
        QDialog::accept();
    }
}

FilterType DotPlotFilterDialog::getFilterType(){
    SAFE_POINT(filterGroup, "Filter Button Group is NULL", All);

    return FilterType(filterGroup->checkedId());
}

void DotPlotFilterDialog::sl_filterTypeChanged(int id){
    FilterType curType = FilterType(id);
    switch (curType){
    case All:
        intersectionParams->setEnabled(false);
        break;
    case Features:
        intersectionParams->setEnabled(true);
        break;
    }
}

void DotPlotFilterDialog::sl_selectAll(){
    int childCount = seqXItem->childCount();
    for (int i = 0; i < childCount; i++){
        QTreeWidgetItem* tItem = seqXItem->child(i);
        tItem->setCheckState(0, Qt::Checked);
    }

    if(seqYItem!=NULL){
        childCount = seqYItem->childCount();
        for (int i = 0; i < childCount; i++){
            QTreeWidgetItem* tItem = seqYItem->child(i);
            tItem->setCheckState(0, Qt::Checked);
        }
    }
}

void DotPlotFilterDialog::sl_selectDifferent(){
    CHECK(seqYItem!=NULL, );
    QSet<QString> commonNames;

    int childCount = seqXItem->childCount();
    for (int i = 0; i < childCount; i++){
        QTreeWidgetItem* tItem = seqXItem->child(i);
        commonNames.insert(tItem->text(0));
    }

    childCount = seqYItem->childCount();
    foreach(const QString& n, commonNames){
        bool contain = false;
        for (int i = 0; i < childCount; i++){
            QTreeWidgetItem* tItem = seqYItem->child(i);
            if(n == tItem->text(0)){
                contain = true;
                break;
            }
        }
        if(!contain){
            commonNames.remove(n);
        }
    }

    childCount = seqXItem->childCount();
    for (int i = 0; i < childCount; i++){
        QTreeWidgetItem* tItem = seqXItem->child(i);
        if(commonNames.contains(tItem->text(0))){
            tItem->setCheckState(0, Qt::Unchecked);
        }else{
            tItem->setCheckState(0, Qt::Checked);
        }
    }

    childCount = seqYItem->childCount();
    for (int i = 0; i < childCount; i++){
        QTreeWidgetItem* tItem = seqYItem->child(i);
        if(commonNames.contains(tItem->text(0))){
            tItem->setCheckState(0, Qt::Unchecked);
        }else{
            tItem->setCheckState(0, Qt::Checked);
        }
    }

}
void DotPlotFilterDialog::sl_clearSelection(){
    int childCount = seqXItem->childCount();
    for (int i = 0; i < childCount; i++){
        QTreeWidgetItem* tItem = seqXItem->child(i);
        tItem->setCheckState(0, Qt::Unchecked);
    }

    if(seqYItem!=NULL){
        childCount = seqYItem->childCount();
        for (int i = 0; i < childCount; i++){
            QTreeWidgetItem* tItem = seqYItem->child(i);
            tItem->setCheckState(0, Qt::Unchecked);
        }
    }
}
void DotPlotFilterDialog::sl_invertSelection(){
    int childCount = seqXItem->childCount();
    for (int i = 0; i < childCount; i++){
        QTreeWidgetItem* tItem = seqXItem->child(i);
        tItem->setCheckState(0, tItem->checkState(0) == Qt::Unchecked ? Qt::Checked : Qt::Unchecked);
    }

    if(seqYItem!=NULL){
        childCount = seqYItem->childCount();
        for (int i = 0; i < childCount; i++){
            QTreeWidgetItem* tItem = seqYItem->child(i);
            tItem->setCheckState(0, tItem->checkState(0) == Qt::Unchecked ? Qt::Checked : Qt::Unchecked);
        }
    }
}

}//namespace
