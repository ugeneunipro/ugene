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

#include <QDir>
#include <QMessageBox>
#include <QPushButton>

#include <U2Algorithm/EnzymeModel.h>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/Settings.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2AlphabetUtils.h>

#include <U2Gui/CreateAnnotationWidgetController.h>
#include <U2Gui/DialogUtils.h>
#include <U2Gui/HelpButton.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>

#include "CreateFragmentDialog.h"

namespace U2 {

CreateFragmentDialog::CreateFragmentDialog(ADVSequenceObjectContext* ctx, QWidget* p)
    : QDialog(p), seqCtx(ctx) {

    setupUi(this);
    new HelpButton(this, buttonBox, "24748960");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    tabWidget->setCurrentIndex(0);

    seqObj = ctx->getSequenceObject();

    rs = new RegionSelector(this, ctx->getSequenceLength(), false, ctx->getSequenceSelection());
    rangeSelectorLayout->addWidget(rs);

    relatedAnnotations = ctx->getAnnotationObjects(true).toList();

    setupAnnotationsWidget();


}

CreateFragmentDialog::CreateFragmentDialog(U2SequenceObject* obj, const U2Region& region, QWidget* p)
    : QDialog(p), seqCtx(NULL) {
    setupUi(this);
    new HelpButton(this, buttonBox, "24748960");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    seqObj = obj;

    QList<GObject*> aObjects = GObjectUtils::findAllObjects(UOF_LoadedOnly, GObjectTypes::ANNOTATION_TABLE);
    QList<GObject*> related = GObjectUtils::findObjectsRelatedToObjectByRole(seqObj, GObjectTypes::ANNOTATION_TABLE,
        ObjectRole_Sequence, aObjects, UOF_LoadedOnly);

    foreach(GObject* obj, related) {
        AnnotationTableObject *aObj = qobject_cast<AnnotationTableObject *>(obj);
        assert(aObj != NULL);
        relatedAnnotations.append(aObj);
    }

    rs = new RegionSelector(this, seqObj->getSequenceLength(), false);
    rs->setCustomRegion(region);
    rangeSelectorLayout->addWidget(rs);

    setupAnnotationsWidget();
}

void CreateFragmentDialog::accept() {
    QString leftOverhang, rightOverhang;

    if (leftEndBox->isChecked()) {
        leftOverhang = lCustomOverhangEdit->text();
        const DNAAlphabet* alph = U2AlphabetUtils::findBestAlphabet(leftOverhang.toLatin1());
        if (!alph->isNucleic()) {
            QMessageBox::warning(this, windowTitle(), tr("Left end contains unsupported symbols!"));
            return;
        }

    }

    if (rightEndBox->isChecked()) {
        rightOverhang = rCustomOverhangEdit->text();
        const DNAAlphabet* alph = U2AlphabetUtils::findBestAlphabet(rightOverhang.toLatin1());
        if (!alph->isNucleic()) {
            QMessageBox::warning(this, windowTitle(), tr("Right end contains unsupported symbols!"));
            return;
        }
    }
    bool isRegionOk = false;
    U2Region reg = rs->getRegion(&isRegionOk);
    if (!isRegionOk) {
        rs->showErrorMessage();
        return;
    }
    if (reg.length <= 0) {
        QMessageBox::warning(this, windowTitle(), tr("Invalid fragment region!\nChoose another region."));
        return;
    }
    QString err = ac->validate();
    if (!err.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), err);
        return;
    }
    bool objectPrepared = ac->prepareAnnotationObject();
    if (!objectPrepared) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot create an annotation object. Please check settings"));
        return;
    }
    const CreateAnnotationModel& m = ac->getModel();
    AnnotationTableObject *obj = m.getAnnotationObject();
    QString groupName = m.groupName;
    SAFE_POINT(!groupName.isEmpty() && obj != NULL, "Invalid annotation data!", );

    SharedAnnotationData ad(new AnnotationData);
    ad->location->regions.append(reg);

    ad->qualifiers.append(U2Qualifier(QUALIFIER_LEFT_TERM, QString()));
    ad->qualifiers.append(U2Qualifier(QUALIFIER_RIGHT_TERM, QString()));
    ad->qualifiers.append(U2Qualifier(QUALIFIER_LEFT_OVERHANG, leftOverhang));
    ad->qualifiers.append(U2Qualifier(QUALIFIER_RIGHT_OVERHANG, rightOverhang));

    U1AnnotationUtils::addDescriptionQualifier(ad, m.description);

    QString leftOverhangStrand = OVERHANG_STRAND_DIRECT;
    if (lComplButton->isChecked() && !leftOverhang.isEmpty()) {
        leftOverhangStrand = OVERHANG_STRAND_COMPL;
    }
    ad->qualifiers.append(U2Qualifier(QUALIFIER_LEFT_STRAND, leftOverhangStrand));
    QString rightOverhangStrand = OVERHANG_STRAND_DIRECT;
    if (rComplButton->isChecked() && !rightOverhang.isEmpty()) {
        rightOverhangStrand = OVERHANG_STRAND_COMPL;
    }
    ad->qualifiers.append(U2Qualifier(QUALIFIER_RIGHT_STRAND, rightOverhangStrand));
    QString leftOverhangType = leftOverhang.isEmpty() ? OVERHANG_TYPE_BLUNT : OVERHANG_TYPE_STICKY;
    ad->qualifiers.append(U2Qualifier(QUALIFIER_LEFT_TYPE, leftOverhangType));
    QString rightOverhangType = rightOverhang.isEmpty() ? OVERHANG_TYPE_BLUNT : OVERHANG_TYPE_STICKY;
    ad->qualifiers.append(U2Qualifier(QUALIFIER_RIGHT_TYPE, rightOverhangType));
    ad->qualifiers.append(U2Qualifier(QUALIFIER_SOURCE, seqObj->getGObjectName()));
    ad->name = QString("Fragment (%1-%2)").arg(reg.startPos + 1).arg(reg.endPos());

    obj->addAnnotations(QList<SharedAnnotationData>() << ad, groupName);
    dnaFragment = DNAFragment(ad, seqObj, relatedAnnotations);

    if (seqCtx != NULL) {
        seqCtx->getAnnotatedDNAView()->tryAddObject(obj);
    }

    QDialog::accept();
}

void CreateFragmentDialog::setupAnnotationsWidget() {
    CreateAnnotationModel acm;
    acm.sequenceObjectRef = GObjectReference(seqObj);
    acm.hideAnnotationType = true;
    acm.hideAnnotationName = true;
    acm.hideLocation = true;
    acm.data->name = ANNOTATION_GROUP_FRAGMENTS;
    acm.sequenceLen = seqObj->getSequenceLength();
    ac = new CreateAnnotationWidgetController(acm, this);
    QWidget* caw = ac->getWidget();
    QVBoxLayout* l = new QVBoxLayout();
    l->setMargin(0);
    l->addWidget(caw);
    l->addStretch();
    annotationsWidget->setLayout(l);
    annotationsWidget->setMinimumSize(caw->layout()->minimumSize());
}

} // namespace
