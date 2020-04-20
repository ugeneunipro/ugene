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

#include <U2Algorithm/SecStructPredictAlgRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/BioStruct3D.h>
#include <U2Core/GAutoDeleteList.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Core/QObjectScopedPointer.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/ADVUtils.h>
#include <U2View/AnnotatedDNAView.h>

#include "SecStructDialog.h"
#include "SecStructPredictUtils.h"

namespace U2 {

SecStructPredictViewAction::SecStructPredictViewAction(AnnotatedDNAView* v) :
    ADVGlobalAction(v, QIcon(":core//images//ssp_logo.png"),  tr("Predict secondary structure..."))
{
    connect(this, SIGNAL(triggered()), SLOT(sl_execute()));
    setObjectName("Predict secondary structure");
    addAlphabetFilter(DNAAlphabet_AMINO);

}


void SecStructPredictViewAction::sl_execute() {
    QAction *a = dynamic_cast<QAction*>(sender());
    GObjectViewAction *viewAction = dynamic_cast<GObjectViewAction*>(a);
    SAFE_POINT(NULL != viewAction, "NULL action",);

    AnnotatedDNAView *av = qobject_cast<AnnotatedDNAView*>(viewAction->getObjectView());
    SAFE_POINT(NULL != av, "NULL dna view",);

    SecStructPredictAlgRegistry *sspar = AppContext::getSecStructPredictAlgRegistry();
    SAFE_POINT(NULL != sspar, "NULL SecStructPredictAlgRegistry",);

    if (sspar->getAlgNameList().isEmpty()) {
        QMessageBox::information(av->getWidget(),
            tr("Secondary Structure Prediction"),
            tr("No algorithms for secondary structure prediction are available.\nPlease, load the corresponding plugins."));
        return;
    }

    ADVSequenceObjectContext *seqCtx = av->getSequenceInFocus();
    SAFE_POINT(NULL != seqCtx, "NULL sequence context",);
    SAFE_POINT(NULL != seqCtx->getAlphabet(), "NULL alphabet",);
    SAFE_POINT(seqCtx->getAlphabet()->isAmino(), "Wrong alphabet",);

    QObjectScopedPointer<SecStructDialog> secStructDialog = new SecStructDialog(seqCtx, av->getWidget());
    secStructDialog->exec();
}

ADVGlobalAction* SecStructPredictViewAction::createAction(AnnotatedDNAView* av)
{
    ADVGlobalAction* action = new SecStructPredictViewAction(av);

    return action;
}

SecStructPredictViewAction::~SecStructPredictViewAction()
{

}

QString SecStructPredictUtils::getStructNameForCharTag(char tag)
{
    SecondaryStructure::Type type = SecondaryStructure::Type_None;
    switch(tag) {
        case 'H':
            type = SecondaryStructure::Type_AlphaHelix;
            break;
        case 'G':
            type = SecondaryStructure::Type_310Helix;
            break;
        case 'I':
            type = SecondaryStructure::Type_PiHelix;
            break;
        case 'B':
            type = SecondaryStructure::Type_BetaBridge;
            break;
        case 'E':
            type = SecondaryStructure::Type_BetaStrand;
            break;
        case 'T':
            type = SecondaryStructure::Type_Turn;
            break;
        case 'S':
            type = SecondaryStructure::Type_BendRegion;
            break;
        default:
            Q_ASSERT(0);
    }

    return BioStruct3D::getSecStructTypeName(type);
}

QList<SharedAnnotationData> SecStructPredictUtils::saveAlgorithmResultsAsAnnotations(const QByteArray& predicted, const QString& annotationName)
{
    char emptyCoil = 'C';

    int numAcronyms = predicted.length();
    QList<SharedAnnotationData> predictedStructures;
    char prevChar = predicted.at(0);
    int lastRecordedPos = 0;
    for (int i = 1; i < numAcronyms; ++i) {
        char curChar = predicted.at(i);
        if ((curChar != prevChar) || (i == numAcronyms - 1)) {
            if (prevChar != emptyCoil) {
                SharedAnnotationData sd(new AnnotationData);
                sd->type = U2FeatureTypes::SeconadaryStructure;
                sd->name = annotationName;
                sd->location->regions.append(U2Region(lastRecordedPos, i - lastRecordedPos));
                sd->qualifiers.append(U2Qualifier(BioStruct3D::SecStructTypeQualifierName, getStructNameForCharTag(prevChar)));
                predictedStructures.append(sd);
            }
            lastRecordedPos = i;
        }
        prevChar = curChar;
    }

    return predictedStructures;
}
} //namespace



