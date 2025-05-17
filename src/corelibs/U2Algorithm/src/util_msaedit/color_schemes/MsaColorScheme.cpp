/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "MsaColorScheme.h"

#include <QColor>
#include <QPalette>

#include <U2Core/FeatureColors.h>
#include <U2Core/Msa.h>
#include <U2Core/U2SafePoints.h>

#include "ColorSchemeUtils.h"
#include "MsaColorSchemeClustalX.h"
#include "MsaColorSchemeCustom.h"
#include "MsaColorSchemePercentageIdententityGrayscale.h"
#include "MsaColorSchemePercentageIdentity.h"
#include "MsaColorSchemeStatic.h"
#include "MsaColorSchemeWeakSimilarities.h"
#include "percentage_idententity/colored/MsaColorSchemePercentageIdententityColoredFactory.h"

namespace U2 {

const QString MsaColorScheme::EMPTY = "COLOR_SCHEME_EMPTY";

const QString MsaColorScheme::UGENE_NUCL_LIGHT = "COLOR_SCHEME_UGENE_NUCL";
const QString MsaColorScheme::UGENE_NUCL_DARK = "COLOR_SCHEME_UGENE_NUCL_DARK";
const QString MsaColorScheme::UGENE_SANGER_NUCL_LIGHT = "COLOR_SCHEME_UGENE_SANGER_NUCL";
const QString MsaColorScheme::UGENE_SANGER_NUCL_DARK = "COLOR_SCHEME_UGENE_SANGER_NUCL_DARK";
const QString MsaColorScheme::JALVIEW_NUCL = "COLOR_SCHEME_JALVIEW_NUCL";
const QString MsaColorScheme::IDENTPERC_NUCL = "COLOR_SCHEME_IDENTPERC_NUCL";
const QString MsaColorScheme::IDENTPERC_NUCL_COLORED = "COLOR_SCHEME_IDENTPERC_NUCL_COLORED";
const QString MsaColorScheme::IDENTPERC_NUCL_GRAY = "COLOR_SCHEME_IDENTPERC_NUCL_GRAY";
const QString MsaColorScheme::CUSTOM_NUCL = "COLOR_SCHEME_CUSTOM_NUCL";
const QString MsaColorScheme::WEAK_SIMILARITIES_NUCL = "COLOR_SCHEME_WEAK_SIMILARITIES_NUCL";

const QString MsaColorScheme::UGENE_AMINO_LIGHT = "COLOR_SCHEME_UGENE_AMINO";
const QString MsaColorScheme::UGENE_AMINO_DARK = "COLOR_SCHEME_UGENE_AMINO_DARK";
const QString MsaColorScheme::ZAPPO_AMINO = "COLOR_SCHEME_ZAPPO_AMINO";
const QString MsaColorScheme::TAILOR_AMINO = "COLOR_SCHEME_TAILOR_AMINO";
const QString MsaColorScheme::HYDRO_AMINO = "COLOR_SCHEME_HYDRO_AMINO";
const QString MsaColorScheme::HELIX_AMINO = "COLOR_SCHEME_HELIX_AMINO";
const QString MsaColorScheme::STRAND_AMINO = "COLOR_SCHEME_STRAND_AMINO";
const QString MsaColorScheme::TURN_AMINO = "COLOR_SCHEME_TURN_AMINO";
const QString MsaColorScheme::BURIED_AMINO = "COLOR_SCHEME_BURIED_AMINO";
const QString MsaColorScheme::IDENTPERC_AMINO = "COLOR_SCHEME_IDENTPERC_AMINO";
const QString MsaColorScheme::IDENTPERC_AMINO_GRAY = "COLOR_SCHEME_IDENTPERC_AMINO_GRAY";
const QString MsaColorScheme::CLUSTALX_AMINO = "COLOR_SCHEME_CLUSTALX_AMINO";
const QString MsaColorScheme::CUSTOM_AMINO = "COLOR_SCHEME_CUSTOM_AMINO";

const QString MsaColorScheme::THRESHOLD_PARAMETER_NAME = "threshold";

MsaColorScheme::MsaColorScheme(QObject* parent, const MsaColorSchemeFactory* factory, MsaObject* maObj)
    : QObject(parent),
      factory(factory),
      maObj(maObj) {
}

QColor MsaColorScheme::getFontColor(int, int, char) const {
    return QPalette().text().color();
}

void MsaColorScheme::applySettings(const QVariantMap& settings) {
    Q_UNUSED(settings);
}

const MsaColorSchemeFactory* MsaColorScheme::getFactory() const {
    return factory;
}

MsaColorSchemeFactory::MsaColorSchemeFactory(QObject* parent, const QString& id, const QString& name, const AlphabetFlags& supportedAlphabets)
    : QObject(parent),
      id(id),
      name(name),
      supportedAlphabets(supportedAlphabets),
      needThreshold(false) {
}

const QString& MsaColorSchemeFactory::getId() const {
    return id;
}

const QString MsaColorSchemeFactory::getName() const {
    return name;
}

bool MsaColorSchemeFactory::isAlphabetTypeSupported(const DNAAlphabetType& alphabetType) const {
    return supportedAlphabets.testFlag(alphabetType);
}

const AlphabetFlags MsaColorSchemeFactory::getSupportedAlphabets() const {
    return supportedAlphabets;
}

bool MsaColorSchemeFactory::isThresholdNeeded() const {
    return needThreshold;
}

MsaColorSchemeRegistry::MsaColorSchemeRegistry() {
    initBuiltInSchemes();
    initCustomSchema();
}

MsaColorSchemeRegistry::~MsaColorSchemeRegistry() {
    deleteOldCustomFactories();
}

const QList<MsaColorSchemeFactory*>& MsaColorSchemeRegistry::getSchemes() const {
    return colorers;
}

const QList<MsaColorSchemeCustomFactory*>& MsaColorSchemeRegistry::getCustomColorSchemes() const {
    return customColorers;
}

QList<MsaColorSchemeFactory*> MsaColorSchemeRegistry::getAllSchemes(DNAAlphabetType alphabet) const {
    return QList<MsaColorSchemeFactory*>() << getSchemes(alphabet) << getCustomSchemes(alphabet);
}

QList<MsaColorSchemeFactory*> MsaColorSchemeRegistry::getSchemes(DNAAlphabetType alphabetType) const {
    QList<MsaColorSchemeFactory*> res;
    foreach (MsaColorSchemeFactory* factory, colorers) {
        if (factory->isAlphabetTypeSupported(alphabetType)) {
            res.append(factory);
        }
    }
    return res;
}

QList<MsaColorSchemeFactory*> MsaColorSchemeRegistry::getCustomSchemes(DNAAlphabetType alphabetType) const {
    QList<MsaColorSchemeFactory*> res;
    foreach (MsaColorSchemeFactory* factory, customColorers) {
        if (factory->isAlphabetTypeSupported(alphabetType)) {
            res.append(factory);
        }
    }
    return res;
}

QList<MsaColorSchemeFactory*> MsaColorSchemeRegistry::customSchemesToCommon() const {
    QList<MsaColorSchemeFactory*> res;
    foreach (MsaColorSchemeFactory* factory, customColorers) {
        res.append(factory);
    }
    return res;
}

QMap<AlphabetFlags, QList<MsaColorSchemeFactory*>> MsaColorSchemeRegistry::getAllSchemesGrouped() const {
    QList<MsaColorSchemeFactory*> allSchemes;
    allSchemes << colorers << customSchemesToCommon();
    QMap<AlphabetFlags, QList<MsaColorSchemeFactory*>> result;
    foreach (MsaColorSchemeFactory* factory, allSchemes) {
        result[factory->getSupportedAlphabets()].append(factory);
    }
    return result;
}

QMap<AlphabetFlags, QList<MsaColorSchemeFactory*>> MsaColorSchemeRegistry::getSchemesGrouped() const {
    QMap<AlphabetFlags, QList<MsaColorSchemeFactory*>> result;
    foreach (MsaColorSchemeFactory* factory, colorers) {
        result[factory->getSupportedAlphabets()].append(factory);
    }
    return result;
}

QMap<AlphabetFlags, QList<MsaColorSchemeFactory*>> MsaColorSchemeRegistry::getCustomSchemesGrouped() const {
    QMap<AlphabetFlags, QList<MsaColorSchemeFactory*>> result;
    foreach (MsaColorSchemeFactory* factory, customColorers) {
        result[factory->getSupportedAlphabets()].append(factory);
    }
    return result;
}

MsaColorSchemeCustomFactory* MsaColorSchemeRegistry::getCustomSchemeFactoryById(const QString& id) const {
    foreach (MsaColorSchemeCustomFactory* customFactory, customColorers) {
        if (customFactory->getId() == id) {
            return customFactory;
        }
    }

    return nullptr;
}

MsaColorSchemeFactory* MsaColorSchemeRegistry::getSchemeFactoryById(const QString& id) const {
    foreach (MsaColorSchemeFactory* commonFactory, colorers) {
        if (commonFactory->getId() == id) {
            return commonFactory;
        }
    }

    return getCustomSchemeFactoryById(id);
}

MsaColorSchemeFactory* MsaColorSchemeRegistry::getEmptySchemeFactory() const {
    return getSchemeFactoryById(MsaColorScheme::EMPTY);
}

void MsaColorSchemeRegistry::addCustomScheme(const ColorSchemeData& scheme) {
    addMsaCustomColorSchemeFactory(new MsaColorSchemeCustomFactory(nullptr, scheme));
}

namespace {

bool compareNames(const MsaColorSchemeFactory* a1, const MsaColorSchemeFactory* a2) {
    return a1->getName() < a2->getName();
}

}  // namespace

void MsaColorSchemeRegistry::addMsaColorSchemeFactory(MsaColorSchemeFactory* commonFactory) {
    assert(getSchemeFactoryById(commonFactory->getId()) == nullptr);
    colorers.append(commonFactory);
    std::stable_sort(colorers.begin(), colorers.end(), compareNames);
}

void MsaColorSchemeRegistry::addMsaCustomColorSchemeFactory(MsaColorSchemeCustomFactory* customFactory) {
    assert(getSchemeFactoryById(customFactory->getId()) == nullptr);
    customColorers.append(customFactory);
    std::stable_sort(colorers.begin(), colorers.end(), compareNames);
}

void MsaColorSchemeRegistry::sl_onCustomSettingsChanged() {
    bool schemesListChanged = false;

    QList<MsaColorSchemeCustomFactory*> factoriesToRemove = customColorers;
    foreach (const ColorSchemeData& scheme, ColorSchemeUtils::getSchemas()) {
        MsaColorSchemeCustomFactory* customSchemeFactory = getCustomSchemeFactoryById(scheme.name);
        if (customSchemeFactory == nullptr) {
            addCustomScheme(scheme);
            schemesListChanged = true;
        } else {
            customSchemeFactory->setScheme(scheme);
            factoriesToRemove.removeOne(customSchemeFactory);
        }
    }

    schemesListChanged |= !factoriesToRemove.isEmpty();
    CHECK(schemesListChanged, );

    foreach (MsaColorSchemeCustomFactory* factory, factoriesToRemove) {
        customColorers.removeOne(factory);
    }

    emit si_customSettingsChanged();
    qDeleteAll(factoriesToRemove);
}

void MsaColorSchemeRegistry::deleteOldCustomFactories() {
    qDeleteAll(customColorers);
    customColorers.clear();
}

namespace {

#define SET_C(ch, cl) colorsPerChar[ch] = colorsPerChar[ch + ('a' - 'A')] = cl

void fillLightColorsColorScheme(QVector<QColor>& colorsPerChar) {
    for (int i = 0; i < 256; i++) {
        colorsPerChar[i] = FeatureColors::genLightColor(QString((char)i));
    }
    colorsPerChar[U2Msa::GAP_CHAR] = QColor();  // invalid color -> no color at all
}

void fillDarkColorsColorScheme(QVector<QColor>& colorsPerChar) {
    for (int i = 0; i < 256; i++) {
        colorsPerChar[i] = FeatureColors::transformLightToDark(FeatureColors::genLightColor(QString((char)i)));
    }
    colorsPerChar[U2Msa::GAP_CHAR] = QColor();  // invalid color -> no color at all
}

void addUgeneAminoLight(QVector<QColor>& colorsPerChar) {
    Q_UNUSED(colorsPerChar);

    // Note: Using direct RGB colors instead of .lighter()/.darker()
    // Reason: lighter() & darker() produce a color that can be mapped back to the RGB space differently on different OSes (RBG space is limited).
    // To make GUI testing stable across different OS-es we hard-code some RBB values.

    // Amino group "HKR", distinct shades of yellow.
    SET_C('H', "#ffff66");  // Light pastel yellow (lighter than K).
    SET_C('K', "#ffe600");  // Vibrant yellow.
    SET_C('R', "#d4b700");  // Golden yellow (darker than K).

    // Amino group "GPST", distinct shades of red.
    SET_C('G', "#ffe4ec");  // Lightest blush pink (lightest shade).
    SET_C('P', "#ff83a7");  // Light pink (lighter than #ff5082).
    SET_C('S', "#ff5082");  // Vivid red-pink (middle shade).
    SET_C('T', "#c4375e");  // Deep rose-red (darker than #ff5082).

    // Amino group "FWY", distinct shades of green.
    SET_C('F', "#65ffab");  // Mint Green,
    SET_C('W', "#3df490");  // Fresh Emerald.
    SET_C('Y', "#33cc78");  // Deep Jade.

    // Amino group "ILM", distinct shades of blue.
    SET_C('I', "#1dc0ff");
    SET_C('L', "#00abed");
    SET_C('M', "#008fc6");

    // Non-group amino acids have distinct, light colors for visibility.
    SET_C('A', "#ffdab3");  // Light orange-peach.
    SET_C('C', "#ccffcc");  // Soft mint.
    SET_C('D', "#ffc2c2");  // Warm pastel pink.
    SET_C('E', "#c0bdbb");  // Gray.
    SET_C('N', "#b3e6ff");  // Sky blue.
    SET_C('Q', "#e6ccff");  // Soft lilac.
    SET_C('V', "#d6ebff");  // Pale baby blue.
    SET_C('X', "#fcfcfc");  // Light gray, almost white.
}

void addUgeneAminoDark(QVector<QColor>& colorsPerChar) {
    Q_UNUSED(colorsPerChar);

    // amino groups: "KRH", "GPST", "FWY", "ILM"
    SET_C('K', "#CCBB00");
    SET_C('R', "#B2A000");
    SET_C('H', "#E5CA00");

    SET_C('G', "#FF195E");
    SET_C('P', "#FF004C");
    SET_C('S', "#FF3270");
    SET_C('T', "#FF3F7B");

    SET_C('F', "#00CC54");
    SET_C('W', "#00B247");
    SET_C('Y', "#00E55B");

    QColor ilm("#00abed");
    SET_C('I', ilm);
    SET_C('L', "#008fc6");  // ilm.darker(120).
    SET_C('M', "#1dc0ff");  // ilm.lighter(120)

    SET_C('E', "#595857");  // gray
    SET_C('X', "#333333");
}

void addUgeneNucleotideLight(QVector<QColor>& colorsPerChar) {
    Q_UNUSED(colorsPerChar);

    SET_C('A', "#FCFF92");  // yellow
    SET_C('C', "#70F970");  // green
    SET_C('T', "#FF99B1");  // light red
    SET_C('G', "#4EADE1");  // light blue
    SET_C('U', colorsPerChar['T'].lighter(120));
    SET_C('N', "#FCFCFC");
}

void addUgeneNucleotideDark(QVector<QColor>& colorsPerChar) {
    Q_UNUSED(colorsPerChar);

    SET_C('A', "#C8CC00");  // yellow
    SET_C('C', "#00BF00");  // green
    SET_C('T', "#FF4271");  // light red
    SET_C('G', "#4EADE1");  // light blue
    SET_C('U', colorsPerChar['T'].darker(120));
    SET_C('N', "#FCFCFC");
}

void addUgeneSangerNucleotideLight(QVector<QColor>& colorsPerChar) {
    Q_UNUSED(colorsPerChar);

    SET_C('A', "#36D695");
    SET_C('C', "#3C9DD0");
    SET_C('G', "#DADADA");
    SET_C('T', "#FE7276");
    SET_C('N', Qt::magenta);
    SET_C('M', Qt::magenta);
    SET_C('R', Qt::magenta);
    SET_C('W', Qt::magenta);
    SET_C('S', Qt::magenta);
    SET_C('Y', Qt::magenta);
    SET_C('K', Qt::magenta);
    SET_C('V', Qt::magenta);
    SET_C('H', Qt::magenta);
    SET_C('D', Qt::magenta);
    SET_C('B', Qt::magenta);
    SET_C('X', Qt::magenta);

    colorsPerChar[U2Msa::GAP_CHAR] = "#FF9700";
}

void addUgeneSangerNucleotideDark(QVector<QColor>& colorsPerChar) {
    Q_UNUSED(colorsPerChar);

    SET_C('A', "#30BA80");
    SET_C('C', "#348AB5");
    SET_C('G', "#595959");
    SET_C('T', "#E2666A");
    SET_C('N', "#C100C1"); // dark magneta
    SET_C('M', "#C100C1");
    SET_C('R', "#C100C1");
    SET_C('W', "#C100C1");
    SET_C('S', "#C100C1");
    SET_C('Y', "#C100C1");
    SET_C('K', "#C100C1");
    SET_C('V', "#C100C1");
    SET_C('H', "#C100C1");
    SET_C('D', "#C100C1");
    SET_C('B', "#C100C1");
    SET_C('X', "#C100C1");

    colorsPerChar[U2Msa::GAP_CHAR] = "#FF9700";
}

void addZappoAmino(QVector<QColor>& colorsPerChar) {
    Q_UNUSED(colorsPerChar);

    // Aliphatic/hydrophobic:    ILVAM       #ffafaf
    SET_C('I', "#ffafaf");
    SET_C('L', "#ffafaf");
    SET_C('V', "#ffafaf");
    SET_C('A', "#ffafaf");
    SET_C('M', "#ffafaf");

    // Aromatic:  FWY         #ffc800
    SET_C('F', "#ffc800");
    SET_C('W', "#ffc800");
    SET_C('Y', "#ffc800");

    // Positive   KRH         #6464ff
    SET_C('K', "#6464ff");
    SET_C('R', "#6464ff");
    SET_C('H', "#6464ff");

    // Negative   DE          #ff0000
    SET_C('D', "#ff0000");
    SET_C('E', "#ff0000");

    // Hydrophil  STNQ        #00ff00
    SET_C('S', "#00ff00");
    SET_C('T', "#00ff00");
    SET_C('N', "#00ff00");
    SET_C('Q', "#00ff00");

    // conformat  PG          #ff00ff
    SET_C('P', "#ff00ff");
    SET_C('G', "#ff00ff");

    // Cysteine   C           #ffff00
    SET_C('C', "#ffff00");
}

void addTailorAmino(QVector<QColor>& colorsPerChar) {
    Q_UNUSED(colorsPerChar);

    SET_C('A', "#ccff00");
    SET_C('V', "#99ff00");
    SET_C('I', "#66ff00");
    SET_C('L', "#33ff00");
    SET_C('M', "#00ff00");
    SET_C('F', "#00ff66");
    SET_C('Y', "#00ffcc");
    SET_C('W', "#00ccff");
    SET_C('H', "#0066ff");
    SET_C('R', "#0000ff");
    SET_C('K', "#6600ff");
    SET_C('N', "#cc00ff");
    SET_C('Q', "#ff00cc");
    SET_C('E', "#ff0066");
    SET_C('D', "#ff0000");
    SET_C('S', "#ff3300");
    SET_C('T', "#ff6600");
    SET_C('G', "#ff9900");
    SET_C('P', "#ffcc00");
    SET_C('C', "#ffff00");
}

void addHydroAmino(QVector<QColor>& colorsPerChar) {
    Q_UNUSED(colorsPerChar);

    // The most hydrophobic residues according to this table are colored red and the most hydrophilic ones are colored blue.
    SET_C('I', "#ff0000");
    SET_C('V', "#f60009");
    SET_C('L', "#ea0015");
    SET_C('F', "#cb0034");
    SET_C('C', "#c2003d");
    SET_C('M', "#b0004f");
    SET_C('A', "#ad0052");
    SET_C('G', "#6a0095");
    SET_C('X', "#680097");
    SET_C('T', "#61009e");
    SET_C('S', "#5e00a1");
    SET_C('W', "#5b00a4");
    SET_C('Y', "#4f00b0");
    SET_C('P', "#4600b9");
    SET_C('H', "#1500ea");
    SET_C('E', "#0c00f3");
    SET_C('Z', "#0c00f3");
    SET_C('Q', "#0c00f3");
    SET_C('D', "#0c00f3");
    SET_C('B', "#0c00f3");
    SET_C('N', "#0c00f3");
    SET_C('K', "#0000ff");
    SET_C('R', "#0000ff");
}

void addHelixAmino(QVector<QColor>& colorsPerChar) {
    Q_UNUSED(colorsPerChar);

    SET_C('E', "#ff00ff");
    SET_C('M', "#ef10ef");
    SET_C('A', "#e718e7");
    SET_C('Z', "#c936c9");
    SET_C('L', "#ae51ae");
    SET_C('K', "#a05fa0");
    SET_C('F', "#986798");
    SET_C('Q', "#926d92");
    SET_C('I', "#8a758a");
    SET_C('W', "#8a758a");
    SET_C('V', "#857a85");
    SET_C('D', "#778877");
    SET_C('X', "#758a75");
    SET_C('H', "#758a75");
    SET_C('R', "#6f906f");
    SET_C('B', "#49b649");
    SET_C('T', "#47b847");
    SET_C('S', "#36c936");
    SET_C('C', "#23dc23");
    SET_C('Y', "#21de21");
    SET_C('N', "#1be41b");
    SET_C('G', "#00ff00");
    SET_C('P', "#00ff00");
}

void addStrandAmino(QVector<QColor>& colorsPerChar) {
    Q_UNUSED(colorsPerChar);

    SET_C('V', "#ffff00");
    SET_C('I', "#ecec13");
    SET_C('Y', "#d3d32c");
    SET_C('F', "#c2c23d");
    SET_C('W', "#c0c03f");
    SET_C('L', "#b2b24d");
    SET_C('T', "#9d9d62");
    SET_C('C', "#9d9d62");
    SET_C('Q', "#8c8c73");
    SET_C('M', "#82827d");
    SET_C('X', "#797986");
    SET_C('R', "#6b6b94");
    SET_C('N', "#64649b");
    SET_C('H', "#60609f");
    SET_C('A', "#5858a7");
    SET_C('S', "#4949b6");
    SET_C('G', "#4949b6");
    SET_C('Z', "#4747b8");
    SET_C('K', "#4747b8");
    SET_C('B', "#4343bc");
    SET_C('P', "#2323dc");
    SET_C('D', "#2121de");
    SET_C('E', "#0000ff");
}

void addTurnAmino(QVector<QColor>& colorsPerChar) {
    Q_UNUSED(colorsPerChar);

    SET_C('N', "#ff0000");
    SET_C('G', "#ff0000");
    SET_C('P', "#f60909");
    SET_C('B', "#f30c0c");
    SET_C('D', "#e81717");
    SET_C('S', "#e11e1e");
    SET_C('C', "#a85757");
    SET_C('Y', "#9d6262");
    SET_C('K', "#7e8181");
    SET_C('X', "#7c8383");
    SET_C('Q', "#778888");
    SET_C('W', "#738c8c");
    SET_C('T', "#738c8c");
    SET_C('R', "#708f8f");
    SET_C('H', "#708f8f");
    SET_C('Z', "#5ba4a4");
    SET_C('E', "#3fc0c0");
    SET_C('A', "#2cd3d3");
    SET_C('F', "#1ee1e1");
    SET_C('M', "#1ee1e1");
    SET_C('L', "#1ce3e3");
    SET_C('V', "#07f8f8");
    SET_C('I', "#00ffff");
}

void addBuriedAmino(QVector<QColor>& colorsPerChar) {
    Q_UNUSED(colorsPerChar);

    SET_C('C', "#0000ff");
    SET_C('I', "#0054ab");
    SET_C('V', "#005fa0");
    SET_C('L', "#007b84");
    SET_C('F', "#008778");
    SET_C('M', "#009768");
    SET_C('G', "#009d62");
    SET_C('A', "#00a35c");
    SET_C('W', "#00a857");
    SET_C('X', "#00b649");
    SET_C('S', "#00d52a");
    SET_C('H', "#00d52a");
    SET_C('T', "#00db24");
    SET_C('P', "#00e01f");
    SET_C('Y', "#00e619");
    SET_C('N', "#00eb14");
    SET_C('B', "#00eb14");
    SET_C('D', "#00eb14");
    SET_C('Q', "#00f10e");
    SET_C('Z', "#00f10e");
    SET_C('E', "#00f10e");
    SET_C('R', "#00fc03");
    SET_C('K', "#00ff00");
}

void addJalviewNucleotide(QVector<QColor>& colorsPerChar) {
    Q_UNUSED(colorsPerChar);

    SET_C('A', "#64F73F");
    SET_C('C', "#FFB340");
    SET_C('G', "#EB413C");
    SET_C('T', "#3C88EE");
    SET_C('U', colorsPerChar['T'].lighter(105));
}

}  // namespace

void MsaColorSchemeRegistry::initBuiltInSchemes() {
    QVector<QColor> colorsPerChar;

    // nucleic
    ColorSchemeUtils::fillEmptyColorScheme(colorsPerChar);
    addMsaColorSchemeFactory(new MsaColorSchemeStaticFactory(this, MsaColorScheme::EMPTY, tr("No colors"), DNAAlphabet_NUCL | DNAAlphabet_AMINO | DNAAlphabet_RAW, colorsPerChar));

    fillLightColorsColorScheme(colorsPerChar);
    addUgeneNucleotideLight(colorsPerChar);
    addMsaColorSchemeFactory(new MsaColorSchemeStaticFactory(this, MsaColorScheme::UGENE_NUCL_LIGHT, U2_APP_TITLE, DNAAlphabet_NUCL | DNAAlphabet_RAW, colorsPerChar));

    ColorSchemeUtils::fillEmptyColorScheme(colorsPerChar);
    fillDarkColorsColorScheme(colorsPerChar);
    addUgeneNucleotideDark(colorsPerChar);
    addMsaColorSchemeFactory(new MsaColorSchemeStaticFactory(this, MsaColorScheme::UGENE_NUCL_DARK, tr("UGENE Dark"), DNAAlphabet_NUCL | DNAAlphabet_RAW, colorsPerChar));

    ColorSchemeUtils::fillEmptyColorScheme(colorsPerChar);
    addUgeneSangerNucleotideLight(colorsPerChar);
    addMsaColorSchemeFactory(new MsaColorSchemeStaticFactory(this, MsaColorScheme::UGENE_SANGER_NUCL_LIGHT, tr("UGENE Sanger"), DNAAlphabet_NUCL | DNAAlphabet_RAW, colorsPerChar));

    ColorSchemeUtils::fillEmptyColorScheme(colorsPerChar);
    addUgeneSangerNucleotideDark(colorsPerChar);
    addMsaColorSchemeFactory(new MsaColorSchemeStaticFactory(this, MsaColorScheme::UGENE_SANGER_NUCL_DARK, tr("UGENE Sanger Dark"), DNAAlphabet_NUCL | DNAAlphabet_RAW, colorsPerChar));

    ColorSchemeUtils::fillEmptyColorScheme(colorsPerChar);
    addJalviewNucleotide(colorsPerChar);
    addMsaColorSchemeFactory(new MsaColorSchemeStaticFactory(this, MsaColorScheme::JALVIEW_NUCL, tr("Jalview"), DNAAlphabet_NUCL | DNAAlphabet_RAW, colorsPerChar));

    addMsaColorSchemeFactory(new MsaColorSchemePercentageIdentityFactory(this, MsaColorScheme::IDENTPERC_NUCL, tr("Percentage identity"), DNAAlphabet_NUCL | DNAAlphabet_RAW));
    addMsaColorSchemeFactory(new MsaColorSchemePercentageIdententityColoredFactory(this, MsaColorScheme::IDENTPERC_NUCL_COLORED, tr("Percentage identity (colored)"), DNAAlphabet_NUCL | DNAAlphabet_RAW));
    addMsaColorSchemeFactory(new MsaColorSchemePercentageIdententityGrayscaleFactory(this, MsaColorScheme::IDENTPERC_NUCL_GRAY, tr("Percentage identity (gray)"), DNAAlphabet_NUCL | DNAAlphabet_RAW));

    addMsaColorSchemeFactory(new MsaColorSchemeWeakSimilaritiesFactory(this, MsaColorScheme::WEAK_SIMILARITIES_NUCL, tr("Weak similarities"), DNAAlphabet_NUCL));

    // amino
    ColorSchemeUtils::fillEmptyColorScheme(colorsPerChar);
    fillLightColorsColorScheme(colorsPerChar);
    addUgeneAminoLight(colorsPerChar);
    addMsaColorSchemeFactory(new MsaColorSchemeStaticFactory(this, MsaColorScheme::UGENE_AMINO_LIGHT, U2_APP_TITLE, DNAAlphabet_AMINO | DNAAlphabet_RAW, colorsPerChar));

    ColorSchemeUtils::fillEmptyColorScheme(colorsPerChar);
    fillDarkColorsColorScheme(colorsPerChar);
    addUgeneAminoDark(colorsPerChar);
    addMsaColorSchemeFactory(new MsaColorSchemeStaticFactory(this, MsaColorScheme::UGENE_AMINO_DARK, tr("UGENE Dark"), DNAAlphabet_AMINO | DNAAlphabet_RAW, colorsPerChar));

    ColorSchemeUtils::fillEmptyColorScheme(colorsPerChar);
    addZappoAmino(colorsPerChar);
    addMsaColorSchemeFactory(new MsaColorSchemeStaticFactory(this, MsaColorScheme::ZAPPO_AMINO, tr("Zappo"), DNAAlphabet_AMINO | DNAAlphabet_RAW, colorsPerChar));

    ColorSchemeUtils::fillEmptyColorScheme(colorsPerChar);
    addTailorAmino(colorsPerChar);
    addMsaColorSchemeFactory(new MsaColorSchemeStaticFactory(this, MsaColorScheme::TAILOR_AMINO, tr("Tailor"), DNAAlphabet_AMINO | DNAAlphabet_RAW, colorsPerChar));

    ColorSchemeUtils::fillEmptyColorScheme(colorsPerChar);
    addHydroAmino(colorsPerChar);
    addMsaColorSchemeFactory(new MsaColorSchemeStaticFactory(this, MsaColorScheme::HYDRO_AMINO, tr("Hydrophobicity"), DNAAlphabet_AMINO | DNAAlphabet_RAW, colorsPerChar));

    ColorSchemeUtils::fillEmptyColorScheme(colorsPerChar);
    addHelixAmino(colorsPerChar);
    addMsaColorSchemeFactory(new MsaColorSchemeStaticFactory(this, MsaColorScheme::HELIX_AMINO, tr("Helix propensity"), DNAAlphabet_AMINO | DNAAlphabet_RAW, colorsPerChar));

    ColorSchemeUtils::fillEmptyColorScheme(colorsPerChar);
    addStrandAmino(colorsPerChar);
    addMsaColorSchemeFactory(new MsaColorSchemeStaticFactory(this, MsaColorScheme::STRAND_AMINO, tr("Strand propensity"), DNAAlphabet_AMINO | DNAAlphabet_RAW, colorsPerChar));

    ColorSchemeUtils::fillEmptyColorScheme(colorsPerChar);
    addTurnAmino(colorsPerChar);
    addMsaColorSchemeFactory(new MsaColorSchemeStaticFactory(this, MsaColorScheme::TURN_AMINO, tr("Turn propensity"), DNAAlphabet_AMINO | DNAAlphabet_RAW, colorsPerChar));

    ColorSchemeUtils::fillEmptyColorScheme(colorsPerChar);
    addBuriedAmino(colorsPerChar);
    addMsaColorSchemeFactory(new MsaColorSchemeStaticFactory(this, MsaColorScheme::BURIED_AMINO, tr("Buried index"), DNAAlphabet_AMINO | DNAAlphabet_RAW, colorsPerChar));

    addMsaColorSchemeFactory(new MsaColorSchemePercentageIdentityFactory(this, MsaColorScheme::IDENTPERC_AMINO, tr("Percentage identity"), DNAAlphabet_AMINO | DNAAlphabet_RAW));
    addMsaColorSchemeFactory(new MsaColorSchemePercentageIdententityGrayscaleFactory(this, MsaColorScheme::IDENTPERC_AMINO_GRAY, tr("Percentage identity (gray)"), DNAAlphabet_AMINO | DNAAlphabet_RAW));

    addMsaColorSchemeFactory(new MsaColorSchemeClustalXFactory(this, MsaColorScheme::CLUSTALX_AMINO, tr("Clustal X"), DNAAlphabet_AMINO | DNAAlphabet_RAW));
}

void MsaColorSchemeRegistry::initCustomSchema() {
    foreach (const ColorSchemeData& schema, ColorSchemeUtils::getSchemas()) {
        addCustomScheme(schema);
    }
}

ColorSchemeData::ColorSchemeData()
    : defaultAlpType(false),
      type(DNAAlphabet_RAW) {
}

}  // namespace U2
