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

#include "BioStruct3DColorScheme.h"

#include <U2Core/AppContext.h>
#include <U2Core/BioStruct3DObject.h>
#include <U2Core/AnnotationSettings.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/FeatureColors.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/GObjectUtils.h>

#include <QObject>

namespace U2 {

/* class BioStruct3DColorSchemeRegistry */
const QString BioStruct3DColorSchemeRegistry::defaultFactoryName() {
    return SecStructColorScheme::schemeName;
}

const QList<QString> BioStruct3DColorSchemeRegistry::factoriesNames() {
    return getInstance()->factories.keys();
}

const BioStruct3DColorSchemeFactory* BioStruct3DColorSchemeRegistry::getFactory(const QString &name) {
    return getInstance()->factories.value(name, 0);
}

BioStruct3DColorScheme* BioStruct3DColorSchemeRegistry::createColorScheme(const QString &name, const BioStruct3DObject *biostruct) {
    const BioStruct3DColorSchemeFactory *fact = getFactory(name);

    if (fact) {
        return fact->createInstance(biostruct);
    }

    return 0;
}

BioStruct3DColorSchemeRegistry::BioStruct3DColorSchemeRegistry() {
    registerFactories();
}

BioStruct3DColorSchemeRegistry* BioStruct3DColorSchemeRegistry::getInstance() {
    static BioStruct3DColorSchemeRegistry *reg = new BioStruct3DColorSchemeRegistry();
    return reg;
}

#define REGISTER_FACTORY(c) factories.insert(c::schemeName, new c::Factory)
void BioStruct3DColorSchemeRegistry::registerFactories() {
    REGISTER_FACTORY(ChainsColorScheme);
    REGISTER_FACTORY(SecStructColorScheme);
    REGISTER_FACTORY(ChemicalElemColorScheme);
    REGISTER_FACTORY(SimpleColorScheme);
}


const QString ChainsColorScheme::schemeName(QObject::tr("Molecular Chains"));
const QString ChemicalElemColorScheme::schemeName(QObject::tr("Chemical Elements"));
const QString SecStructColorScheme::schemeName(QObject::tr("Secondary Structure"));
const QString SimpleColorScheme::schemeName(QObject::tr("Simple colors"));



/*class BioStruct3DColorScheme */

BioStruct3DColorScheme::BioStruct3DColorScheme(const BioStruct3DObject *biostruct)
        : defaultAtomColor(0.25f, 0.25f, 0.25f), selectionColor(1.0f, 1.0f, 0),
          selection(biostruct->getBioStruct3D()),
          unselectedShading(0.0)
{}

Color4f BioStruct3DColorScheme::getAtomColor(const SharedAtom& atom) const {
    Color4f c;

    if (isInSelection(atom)) {
        c = selectionColor;
    }
    else {
        c = getSchemeAtomColor(atom);
        if (!selection.isEmpty() && unselectedShading > 0.0) {  // dim unselected
            c[3] *= (1.0 - unselectedShading);
        }
    }

    return c;
}

void BioStruct3DColorScheme::setSelectionColor(QColor color) {
    this->selectionColor = color;
}

void BioStruct3DColorScheme::setUnselectedShadingLevel(float shading) {
    assert(shading >= 0.0 && shading <= 1.0);
    unselectedShading = shading;
}

void BioStruct3DColorScheme::updateSelectionRegion(int chainId, const QVector<U2Region>& added, const QVector<U2Region>& removed) {
    selection.update(chainId, added, removed);
}

bool BioStruct3DColorScheme::isInSelection(const SharedAtom &atom) const {
   return selection.inSelection(atom->chainIndex, atom->residueIndex.toInt());
}

Color4f BioStruct3DColorScheme::getSchemeAtomColor(const SharedAtom&) const {
    return defaultAtomColor;
}

///////////////////////////////////////////////////////////////////////////////////////////
// ChemicalElemColorScheme

Color4f ChemicalElemColorScheme::getSchemeAtomColor( const SharedAtom& a ) const
{
    Color4f color;
    if (elementColorMap.contains(a->atomicNumber)) {
        return elementColorMap.value(a->atomicNumber);
    } else {
        return defaultAtomColor;
    }

}

ChemicalElemColorScheme::ChemicalElemColorScheme(const BioStruct3DObject *biostruct)
        : BioStruct3DColorScheme(biostruct)
{
    //CPK colors

    elementColorMap.insert(1, Color4f(1.0f, 1.0f, 1.0f));
    elementColorMap.insert(6, Color4f(0.8f, 0.8f, 0.8f));
    elementColorMap.insert(7, Color4f(0.7f, 0.7f, 1.0f));
    elementColorMap.insert(8, Color4f(0.95f, 0.0f , 0.0f));
    elementColorMap.insert(11, Color4f(0.0f, 0.0f , 1.0f));
    elementColorMap.insert(12, Color4f(0.16f, 0.5f , 0.16f));
    elementColorMap.insert(15, Color4f(1.0f, 0.63f , 0.0f));
    elementColorMap.insert(16, Color4f(1.0f, 0.8f , 0.2f));
    elementColorMap.insert(17, Color4f(0.0f, 1.0f , 0.0f));
    elementColorMap.insert(20, Color4f(0.5f, 0.5f , 0.5f));
    elementColorMap.insert(26, Color4f(1.0f, 0.63f , 0.0f));
    elementColorMap.insert(30, Color4f(0.63f, 0.2f , 0.2f));
    elementColorMap.insert(35, Color4f(0.63f, 0.2f , 0.2f));

}


/* class ChainsColorScheme : public BioStruct3DColorScheme */

const QMap<int, QColor> ChainsColorScheme::getChainColors(const BioStruct3DObject *biostructObj) {
    QMap<int, QColor> colorMap;

    if (NULL != biostructObj->getDocument()) {
        QList<GObject *> aObjs = GObjectUtils::selectRelationsFromParentDoc(biostructObj, GObjectTypes::ANNOTATION_TABLE, ObjectRole_AnnotationTable);
        foreach (GObject* obj, aObjs ) {
            AnnotationTableObject* ao = qobject_cast<AnnotationTableObject *>(obj);
            SAFE_POINT(NULL != ao, "Invalid annotation table!", colorMap);

            foreach (Annotation *a, ao->getAnnotationsByName(BioStruct3D::MoleculeAnnotationTag)) {
                QString chainQualifier = a->findFirstQualifierValue(BioStruct3D::ChainIdQualifierName);
                SAFE_POINT(chainQualifier.size() == 1, "Invalid chain id qualifier", colorMap);
                const char chainId = chainQualifier.toLatin1().at(0);
                const int index = biostructObj->getBioStruct3D().getIndexByChainId(chainId);
                SAFE_POINT(index >= 0, QString("Invalid chain id: %1").arg(chainId), colorMap);
                const QColor color = FeatureColors::genLightColor(QString("chain_%1").arg(index));
                colorMap.insert(index, color);
            }
        }
    }

    return colorMap;
}

ChainsColorScheme::ChainsColorScheme(const BioStruct3DObject *biostruct)
        : BioStruct3DColorScheme(biostruct)
{
    const QMap<int, QColor> chainColors = getChainColors(biostruct);
    if (!chainColors.empty()) {
        QMapIterator<int, QColor> i(chainColors);
        while (i.hasNext()) {
            i.next();
            chainColorMap.insert(i.key(), Color4f(i.value()));
        }
    }
}

Color4f ChainsColorScheme::getSchemeAtomColor(const SharedAtom& atom) const
{
    Color4f color;
    if (chainColorMap.contains(atom->chainIndex)) {
        return chainColorMap.value(atom->chainIndex);
    } else {
        return defaultAtomColor;
    }
}

/* class SecStructColorScheme : public BioStruct3DColorScheme */

const QMap<QString, QColor> SecStructColorScheme::getSecStructAnnotationColors(const BioStruct3DObject *biostruct) {
    QMap<QString, QColor> colors;
    AnnotationSettingsRegistry* asr = AppContext::getAnnotationsSettingsRegistry();

    Document *doc = biostruct->getDocument();
    if (doc) {
        QList<GObject *> targetAnnotations = GObjectUtils::selectRelationsFromParentDoc(biostruct, GObjectTypes::ANNOTATION_TABLE, ObjectRole_AnnotationTable);
        foreach (GObject* obj, targetAnnotations) {
            AnnotationTableObject *ao = qobject_cast<AnnotationTableObject *>(obj);
            SAFE_POINT(NULL != ao, "Invalid annotation table!", colors);

            foreach (Annotation *a, ao->getAnnotationsByName(BioStruct3D::SecStructAnnotationTag)) {
                QString ssName = a->getQualifiers().first().value;
                AnnotationSettings* as = asr->getAnnotationSettings(ssName);
                colors.insert(ssName, as->color);
            }
        }
    }

    return colors;
}

SecStructColorScheme::SecStructColorScheme(const BioStruct3DObject *biostruct)
    : BioStruct3DColorScheme(biostruct)
{
    defaultAtomColor = Color4f(0.5f,0.9f,0.9f);
    const QMap<QString, QColor> secStrucColors = getSecStructAnnotationColors(biostruct);
    if (!secStrucColors.isEmpty()) {
        QMapIterator<QString, QColor> i(secStrucColors);
        while (i.hasNext()) {
            i.next();
            secStrucColorMap.insert(i.key().toLatin1(), Color4f(i.value()));
        }

        foreach (const SharedSecondaryStructure& struc, biostruct->getBioStruct3D().secondaryStructures) {
            for (int index = struc->startSequenceNumber; index <= struc->endSequenceNumber; ++index ) {
                QByteArray type = BioStruct3D::getSecStructTypeName(struc->type).toLatin1();
                Q_ASSERT( secStrucColorMap.contains(type));
                Q_ASSERT( struc->chainIndex != 0);
                molMap[struc->chainIndex].strucResidueTable.insert(index, type);
            }
        }

    #ifdef _DEBUG
        // Verify indices with biostruct3d
        const BioStruct3D& bioStruc = biostruct->getBioStruct3D();
        QMapIterator<int,MolStructs> iter(molMap);
        while (iter.hasNext()) {
            iter.next();
            assert(bioStruc.moleculeMap.contains(iter.key()));
        }
    #endif
    }
}

Color4f SecStructColorScheme::getSchemeAtomColor( const SharedAtom& atom ) const
{
    Color4f c = defaultAtomColor;
    int residueIndex = atom->residueIndex.toInt();

    if (molMap.contains(atom->chainIndex)) {
        const QHash<int, QByteArray>& residueTable = molMap.value(atom->chainIndex).strucResidueTable;
        if (residueTable.contains(residueIndex)) {
            QByteArray type = residueTable.value(residueIndex);
            c = secStrucColorMap.value(type);
        }
    }

    return c;
}

/* class SimpleColorScheme : public BioStruct3DColorScheme */
QVector<Color4f> SimpleColorScheme::colors;

void SimpleColorScheme::createColors() {
    if (colors.isEmpty()) {
        // rainbow colors
        colors.append( Color4f(QColor(0xff,0x00,0x00)) );
        colors.append( Color4f(QColor(0xff,0x7f,0x00)) );
        colors.append( Color4f(QColor(0xff,0xff,0x00)) );
        colors.append( Color4f(QColor(0x00,0xff,0x00)) );
        colors.append( Color4f(QColor(0x00,0xff,0xff)) );
        colors.append( Color4f(QColor(0x00,0x00,0xff)) );
        colors.append( Color4f(QColor(0x8b,0x00,0xff)) );
    }
}

SimpleColorScheme::SimpleColorScheme(const BioStruct3DObject *biostruct) : BioStruct3DColorScheme(biostruct)
{
    createColors();
    static int idx = 0;
    defaultAtomColor = colors[(idx++) % colors.size()];
}

}   // namespace U2
