/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include "EnzymeModel.h"

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>

namespace U2 {

const QString EnzymeSettings::DATA_DIR_KEY("enzymes");
const QString EnzymeSettings::DATA_FILE_KEY("plugin_enzymes/lastFile");
const QString EnzymeSettings::LAST_SELECTION("plugin_enzymes/selection");
const QString EnzymeSettings::CHECKED_SUPPLIERS("plugin_enzymes/checkedSuppliers");
const QString EnzymeSettings::MIN_ENZYME_LENGTH("plugin_enzymes/min_enzyme_length");
const QString EnzymeSettings::MAX_ENZYME_LENGTH("plugin_enzymes/max_enzyme_length");
const QString EnzymeSettings::OVERHANG_TYPE("plugin_enzymes/overhang_type");
const QString EnzymeSettings::SHOW_PALINDROMIC("plugin_enzymes/show_palindromic");
const QString EnzymeSettings::SHOW_UNINTERRUPTED("plugin_enzymes/show_uninterrupted");
const QString EnzymeSettings::SHOW_NONDEGENERATE("plugin_enzymes/show_nondegenerate");
const QString EnzymeSettings::ENABLE_HIT_COUNT("plugin_enzymes/enable_hit_count");
const QString EnzymeSettings::MAX_HIT_VALUE("plugin_enzymes/max_hit_value");
const QString EnzymeSettings::MIN_HIT_VALUE("plugin_enzymes/min_hit_value");
const QString EnzymeSettings::MAX_RESULTS("plugin_enzymes/max_results");
const QString EnzymeSettings::COMMON_ENZYMES("ClaI,BamHI,BglII,DraI,EcoRI,EcoRV,HindIII,PstI,SalI,SmaI,XmaI");

static const QString ENZYME_TOOLTIP_DARK = "#FF7F7F";
static const QString ENZYME_TOOLTIP_LIGHT = "#FF0000";

void EnzymeData::calculateLeadingAndTrailingLengths(int& leadingNsNumber, int& trailingNsNumber) const {
    int seqSize = seq.size();
    auto calculateDirect = [&leadingNsNumber, &trailingNsNumber, seqSize](int cut) {
        CHECK(cut != ENZYME_CUT_UNKNOWN, );

        if (cut < 0) {
            leadingNsNumber = qMax(leadingNsNumber, qAbs(cut));
        } else if (cut > seqSize) {
            trailingNsNumber = qMax(trailingNsNumber, cut - seqSize);
        }
    };
    auto calculateComplement = [&leadingNsNumber, &trailingNsNumber, seqSize](int cut) {
        CHECK(cut != ENZYME_CUT_UNKNOWN, );

        if (cut < 0) {
            trailingNsNumber = qMax(trailingNsNumber, qAbs(cut));
        } else if (cut > seqSize) {
            leadingNsNumber = qMax(leadingNsNumber, cut - seqSize);
        }
    };

    calculateDirect(cutDirect);
    calculateComplement(cutComplement);
    calculateDirect(secondCutDirect);
    calculateComplement(secondCutComplement);
}

int EnzymeData::getFullLength() const {
    int leadingNsNumber = 0;
    int trailingNsNumber = 0;
    calculateLeadingAndTrailingLengths(leadingNsNumber, trailingNsNumber);
    return leadingNsNumber + seq.size() + trailingNsNumber;
}

static const QString TOOLTIP_TAG = "<p style='font-family:Courier,monospace'><br><strong>5'&nbsp;</strong>%1<strong>&nbsp;3'</strong><br><strong>3'&nbsp;</strong>%2<strong>&nbsp;5'</strong><br></p>";
static const QString TOOLTIP_N_MARKER = "(N)<sub>%1</sub>";
static const QString TOOLTIP_FORWARD_MARKER = "<sup>&#x25BC;</sup>";
static const QString TOOLTIP_REVERSE_MARKER = "<sub>&#x25B2;</sub>";
static const QString TOOLTIP_SPACE = "<sub>&nbsp;</sub>";

namespace {

// Enum, which shows either enzyme has a cut inside of the sequence,
// to the left of the sequence or to the right of the sequence
// Ns in this scope means "if this enzyme has N character on to the left or
// to the right of the enzyme, or no N charactest at all"
enum class Ns {
    // Enzyme has N charactes to the left
    // Example:
    // N N N A C G T
    //       T G C A
    Left,
    // Enzyme has N charactes to the right
    // Example:
    // A C G T N N N
    // T G C A
    Right,
    // Enzyme doesn't have N characters
    // Example:
    // A C G T
    // T G C A
    No
};

// Look at the enxyme:
// N N N N N A C G T
//       N N T G C A
// This enzyme is separated to the several parts (name "parts" is not official, just used here)
//   Out     In    Main
// |N N N| |N N| |A C G T|
//         |N N| |T G C A|
// @generateOutPartElements generates the "Out" part
// Returns the list, which contains the cut elements and Ns if this enzyme has the "Out" part,
// Or spaces if it does not have.
QStringList generateOutPartElements(int out, bool forward, Ns type, bool otherHasLeftOut, bool otherHasRightOut) {
    QStringList generateOutPartElementsResult;
    if (out != 0) {
        switch (type) {
            case Ns::Left:
                generateOutPartElementsResult << (forward ? TOOLTIP_FORWARD_MARKER : TOOLTIP_REVERSE_MARKER);
                generateOutPartElementsResult << TOOLTIP_N_MARKER.arg(out);
                break;
            case Ns::Right:
                generateOutPartElementsResult << TOOLTIP_N_MARKER.arg(out);
                generateOutPartElementsResult << (forward ? TOOLTIP_FORWARD_MARKER : TOOLTIP_REVERSE_MARKER);
                break;
            case Ns::No:
                break;
        }
    } else if (out == 0 && otherHasLeftOut && type == Ns::Left) {
        generateOutPartElementsResult << QString("%1&nbsp;&nbsp;&nbsp;%1").arg(TOOLTIP_SPACE);
    } else if (out == 0 && otherHasRightOut && type == Ns::Right) {
        generateOutPartElementsResult << QString("%1&nbsp;&nbsp;&nbsp;%1").arg(TOOLTIP_SPACE);
    }
    return generateOutPartElementsResult;
};

// The same as @generateOutPartElements but for the "In" part
QStringList generateInPartElements(int out, int in, bool forward, Ns type, bool otherHasLeftOut, bool otherHasLeftIn, bool otherHasRightIn, bool otherHasRightOut) {
    QStringList generateInPartElementsResult;
    if (in != 0) {
        if (out == 0) {
            switch (type) {
                case Ns::Left:
                    generateInPartElementsResult << (forward ? TOOLTIP_FORWARD_MARKER : TOOLTIP_REVERSE_MARKER);
                    generateInPartElementsResult << TOOLTIP_N_MARKER.arg(in);
                    break;
                case Ns::Right:
                    generateInPartElementsResult << TOOLTIP_N_MARKER.arg(in);
                    generateInPartElementsResult << (forward ? TOOLTIP_FORWARD_MARKER : TOOLTIP_REVERSE_MARKER);
                    break;
                case Ns::No:
                    break;
            }
        } else if (out != 0) {
            generateInPartElementsResult << TOOLTIP_N_MARKER.arg(in);
        }
    } else if (in == 0 && otherHasLeftIn && type == Ns::Left) {
        if (otherHasLeftOut) {
            generateInPartElementsResult << QString("%1&nbsp;&nbsp;&nbsp;%1").arg(TOOLTIP_SPACE);
        } else {
            generateInPartElementsResult << QString("&nbsp;&nbsp;&nbsp;%1").arg(TOOLTIP_SPACE);
        }
    } else if (in == 0 && otherHasRightIn && type == Ns::Right) {
        if (otherHasRightOut) {
            generateInPartElementsResult << QString("%1&nbsp;&nbsp;&nbsp;%1").arg(TOOLTIP_SPACE);
        } else {
            generateInPartElementsResult << QString("&nbsp;&nbsp;&nbsp;%1").arg(TOOLTIP_SPACE);
        }
    }
    return generateInPartElementsResult;
};

// generates the "Main" part (see @generateOutPartElements for details)
QString generateMainPart(const QByteArray& seq, int cut, bool forward, int enzymeSize) {
    QString result;
    auto append2Result = [&result, forward](const QString& add) {
        if (forward) {
            result += add;
        } else {
            result.insert(0, add);
        }
    };
    auto removeSpaceFromResult = [&result, forward]() {
        if (forward) {
            result = result.left(result.size() - TOOLTIP_SPACE.size());
        } else {
            result = result.right(result.size() - TOOLTIP_SPACE.size());
        }
    };
    append2Result(TOOLTIP_SPACE);
    for (int i = 0; i < enzymeSize; i++) {
        if (i == cut) {
            removeSpaceFromResult();
            append2Result(forward ? TOOLTIP_FORWARD_MARKER : TOOLTIP_REVERSE_MARKER);
        }
        QString ch(seq.at(i));
        if (ch != "N") {
            ch = QString("<span style=\"color: %1; \">%2</span>")
                     .arg(AppContext::getMainWindow()->isDarkTheme() ? ENZYME_TOOLTIP_DARK : ENZYME_TOOLTIP_LIGHT)
                     .arg(ch);
        }
        append2Result(ch);
        append2Result(TOOLTIP_SPACE);
    }
    if (seq.size() == cut) {
        removeSpaceFromResult();
        append2Result(forward ? TOOLTIP_FORWARD_MARKER : TOOLTIP_REVERSE_MARKER);
    }
    return result;
};

// Calculates nubmer of Ns outside of enzyme
// returns side to shift and shift number
QPair<Ns, int> calculateNShift(int cut, bool forward, int enzymeSize) {
    QPair<Ns, int> calculateNShiftRes = {Ns::No, 0};
    if (cut < 0) {
        calculateNShiftRes.first = forward ? Ns::Left : Ns::Right;
        calculateNShiftRes.second = qAbs(cut);
    } else if (enzymeSize < cut) {
        calculateNShiftRes.first = forward ? Ns::Right : Ns::Left;
        calculateNShiftRes.second = cut - enzymeSize;
    }
    return calculateNShiftRes;
};

// generates tooltop elements, which are located outside of enzyme
// these elements will be joined in the end
// returns list of elements
QStringList generateTooltipElements(int out, int in, bool forward, Ns type, bool otherHasLeftOut, bool otherHasLeftIn, bool otherHasRightIn, bool otherHasRightOut) {
    QStringList result;
    switch (type) {
        case Ns::Left:
            result << generateOutPartElements(out, forward, type, otherHasLeftOut, otherHasRightOut);
            result << generateInPartElements(out, in, forward, type, otherHasLeftOut, otherHasLeftIn, otherHasRightIn, otherHasRightOut);
            break;
        case Ns::Right:
            result << generateInPartElements(out, in, forward, type, otherHasLeftOut, otherHasLeftIn, otherHasRightIn, otherHasRightOut);
            result << generateOutPartElements(out, forward, type, otherHasLeftOut, otherHasRightOut);
            break;
        case Ns::No:
            break;
    }
    return result;
};

// Join elements which are calculated in @generateTooltipElements
QString joinTooltipElements(const QStringList& elements) {
    QString joinElementsResult;
    for (const auto& el : qAsConst(elements)) {
        if (el == TOOLTIP_FORWARD_MARKER || el == TOOLTIP_REVERSE_MARKER) {
            if (!joinElementsResult.isEmpty()) {
                joinElementsResult = joinElementsResult.left(joinElementsResult.size() - TOOLTIP_SPACE.size());
            }
            joinElementsResult += el;
        } else {
            joinElementsResult += el;
            if (el != TOOLTIP_SPACE) {
                joinElementsResult += TOOLTIP_SPACE;
            }
        }
    }
    if (joinElementsResult.endsWith(TOOLTIP_SPACE)) {
        joinElementsResult = joinElementsResult.left(joinElementsResult.size() - TOOLTIP_SPACE.size());
    }
    return joinElementsResult;
};

// Generates forward and reverse side parts of the enzyme.
// For example:
//    Side        Main
// |N N N N N| |A C G T|
//        N N| |T G C A|
//
// Forward side part here is "N N N N N"
// Reverse side part here is "N N"
void generateForwardAndReverseTooltipSideParts(const QPair<Ns, int>& forwardNShift, const QPair<Ns, int>& reverseNShift, QString& forwardTooltipSidePart, QString& reverseTooltipSidePart) {
    QStringList forwardTooltipElements;
    QStringList reverseTooltipElements;
    if (forwardNShift.first == reverseNShift.first && forwardNShift.first != Ns::No) {
        int forwardNOut = 0;
        int forwardNIn = 0;
        int reverseNOut = 0;
        int reverseNIn = 0;
        if (forwardNShift.second > reverseNShift.second) {
            forwardNOut = forwardNShift.second - reverseNShift.second;
            forwardNIn = reverseNShift.second;
            reverseNOut = 0;
            reverseNIn = reverseNShift.second;
        } else if (forwardNShift.second == reverseNShift.second) {
            forwardNOut = 0;
            forwardNIn = forwardNShift.second;
            reverseNOut = 0;
            reverseNIn = reverseNShift.second;
        } else if (forwardNShift.second < reverseNShift.second) {
            forwardNOut = 0;
            forwardNIn = forwardNShift.second;
            reverseNOut = reverseNShift.second - forwardNShift.second;
            reverseNIn = forwardNShift.second;
        }

        forwardTooltipElements = generateTooltipElements(forwardNOut, forwardNIn, true, forwardNShift.first, (reverseNOut != 0 && reverseNShift.first == Ns::Left), (reverseNIn != 0 && reverseNShift.first == Ns::Left), (reverseNIn != 0 && reverseNShift.first == Ns::Right), (reverseNOut != 0 && reverseNShift.first == Ns::Right));
        reverseTooltipElements = generateTooltipElements(reverseNOut, reverseNIn, false, reverseNShift.first, (forwardNOut != 0 && forwardNShift.first == Ns::Left), (forwardNIn != 0 && forwardNShift.first == Ns::Left), (forwardNIn != 0 && forwardNShift.first == Ns::Right), (forwardNOut != 0 && forwardNShift.first == Ns::Right));
    } else {
        if (forwardNShift.first != Ns::No) {
            forwardTooltipElements = generateTooltipElements(0, forwardNShift.second, true, forwardNShift.first, false, (reverseNShift.second != 0 && reverseNShift.first == Ns::Left), (reverseNShift.second != 0 && reverseNShift.first == Ns::Right), false);
        }
        if (reverseNShift.first != Ns::No) {
            reverseTooltipElements = generateTooltipElements(0, reverseNShift.second, false, reverseNShift.first, false, (forwardNShift.second != 0 && forwardNShift.first == Ns::Left), (forwardNShift.second != 0 && forwardNShift.first == Ns::Right), false);
        }
    }

    forwardTooltipSidePart = joinTooltipElements(forwardTooltipElements);
    reverseTooltipSidePart = joinTooltipElements(reverseTooltipElements);
}

// Join parts to a single tooltip
QString generateTooltip(Ns type, const QString& sidePart, const QString& secondSidePart, const QString& mainPart) {
    QString result;
    if (type == Ns::Left) {
        result += sidePart;
    } else if (!secondSidePart.isEmpty()) {
        result += secondSidePart;
    }
    result += mainPart;
    if (type == Ns::Right) {
        result += sidePart;
    } else if (!secondSidePart.isEmpty()) {
        result += secondSidePart;
    }
    return result;
};

}  // namespace

QString EnzymeData::generateEnzymeTooltip() const {
    auto alphabet = AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_EXTENDED());
    auto seqComplement = DNASequenceUtils::reverseComplement(seq, alphabet);
    if (cutDirect == ENZYME_CUT_UNKNOWN) {
        auto getColoredSequence = [](const QString& sequence) -> QString {
            QString result;
            for (QString ch : qAsConst(sequence)) {
                if (ch != "N") {
                    ch = QString("<span style=\"color: %1; \">%2</span>")
                        .arg(AppContext::getMainWindow()->isDarkTheme() ? ENZYME_TOOLTIP_DARK : ENZYME_TOOLTIP_LIGHT).arg(ch);
                }
                result += ch;
            }
            return result;
        };
        return TOOLTIP_TAG.arg(getColoredSequence(seq)).arg(getColoredSequence(DNASequenceUtils::complement(seq, alphabet)));
    }

    int enzymeSize = seq.size();

    QPair<Ns, int> forwardNShift = calculateNShift(cutDirect, true, enzymeSize);
    QPair<Ns, int> reverseNShift = calculateNShift(cutComplement, false, enzymeSize);
    QString forwardTooltipSidePart;
    QString reverseTooltipSidePart;
    generateForwardAndReverseTooltipSideParts(forwardNShift, reverseNShift, forwardTooltipSidePart, reverseTooltipSidePart);

    QString secondForwardTooltipSidePart;
    QString secondReverseTooltipSidePart;
    if (secondCutDirect != ENZYME_CUT_UNKNOWN) {
        QPair<Ns, int> secondForwardNShift = calculateNShift(secondCutDirect, true, enzymeSize);
        QPair<Ns, int> secondReverseNShift = calculateNShift(secondCutComplement, false, enzymeSize);
        generateForwardAndReverseTooltipSideParts(secondForwardNShift, secondReverseNShift, secondForwardTooltipSidePart, secondReverseTooltipSidePart);
    }

    QString forwardMainPart = generateMainPart(seq, cutDirect, true, enzymeSize);
    QString reverseMainPart = generateMainPart(seqComplement, cutComplement, false, enzymeSize);

    QString forwardTooltip = generateTooltip(forwardNShift.first, forwardTooltipSidePart, secondForwardTooltipSidePart, forwardMainPart);
    QString reverseTooltip = generateTooltip(reverseNShift.first, reverseTooltipSidePart, secondReverseTooltipSidePart, reverseMainPart);
    if (forwardNShift.first == Ns::Left && reverseNShift.first != Ns::Left) {
        if (reverseNShift.first == Ns::Right) {
            forwardTooltip += QString("%1&nbsp;&nbsp;&nbsp;%1").arg(TOOLTIP_SPACE);
        }
        reverseTooltip.insert(0, QString("%1&nbsp;&nbsp;&nbsp;%1").arg(TOOLTIP_SPACE));
    } else if (reverseNShift.first == Ns::Left && forwardNShift.first != Ns::Left) {
        forwardTooltip.insert(0, QString("%1&nbsp;&nbsp;&nbsp;%1").arg(TOOLTIP_SPACE));
        if (forwardNShift.first == Ns::Right) {
            reverseTooltip += QString("%1&nbsp;&nbsp;&nbsp;%1").arg(TOOLTIP_SPACE);
        }
    }

    return TOOLTIP_TAG.arg(forwardTooltip).arg(reverseTooltip);
}

}  // namespace U2
