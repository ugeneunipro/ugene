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

#include "MsaSchemesMenuBuilder.h"

#include <QLabel>
#include <QMenu>
#include <QSignalMapper>
#include <QWidgetAction>

#include <U2Algorithm/MsaColorScheme.h>
#include <U2Algorithm/MsaHighlightingScheme.h>

#include <U2Core/AppContext.h>
#include <U2Core/U2SafePoints.h>

#include "ov_msa/MaEditorSequenceArea.h"
#include "ov_msa/MsaEditor.h"

namespace U2 {

QSignalMapper* MsaSchemesMenuBuilder::signalMapper = new QSignalMapper();

void MsaSchemesMenuBuilder::createAndFillColorSchemeMenuActions(QList<QAction*>& actions, ColorSchemeType type, DNAAlphabetType alphabet, MaEditorSequenceArea* actionsParent) {
    MsaColorSchemeRegistry* msaColorSchemeRegistry = AppContext::getMsaColorSchemeRegistry();
    MsaColorSchemeFactory* noColorsFactory = msaColorSchemeRegistry->getSchemeFactoryById(MsaColorScheme::EMPTY);

    if (alphabet == DNAAlphabet_RAW) {
        QMap<AlphabetFlags, QList<MsaColorSchemeFactory*>> factories;
        if (type == Common) {
            factories = msaColorSchemeRegistry->getSchemesGrouped();
        } else if (type == Custom) {
            factories = msaColorSchemeRegistry->getCustomSchemesGrouped();
        } else {
            FAIL("Unknown color scheme type", );
        }

        QList<MsaColorSchemeFactory*> rawColorSchemesFactories = factories[DNAAlphabet_RAW | DNAAlphabet_AMINO | DNAAlphabet_NUCL];
        QList<MsaColorSchemeFactory*> aminoColorSchemesFactories = factories[DNAAlphabet_RAW | DNAAlphabet_AMINO];
        QList<MsaColorSchemeFactory*> nucleotideColorSchemesFactories = factories[DNAAlphabet_RAW | DNAAlphabet_NUCL];

        if (type == Common) {
            rawColorSchemesFactories.removeAll(noColorsFactory);
            rawColorSchemesFactories.prepend(noColorsFactory);
        }

        fillColorMenuSectionForCurrentAlphabet(rawColorSchemesFactories, actions, tr("All alphabets"), actionsParent);
        fillColorMenuSectionForCurrentAlphabet(aminoColorSchemesFactories, actions, tr("Amino acid alphabet"), actionsParent);
        fillColorMenuSectionForCurrentAlphabet(nucleotideColorSchemesFactories, actions, tr("Nucleotide alphabet"), actionsParent);
    } else {
        QList<MsaColorSchemeFactory*> factories;
        if (type == Common) {
            factories = msaColorSchemeRegistry->getSchemes(alphabet);
            factories.removeAll(noColorsFactory);
            factories.prepend(noColorsFactory);
        } else if (type == Custom) {
            factories = msaColorSchemeRegistry->getCustomSchemes(alphabet);
        } else {
            FAIL("Unknown color scheme type", );
        }
        fillColorSchemeMenuActions(actions, factories, actionsParent);
    }
}

void MsaSchemesMenuBuilder::createAndFillHighlightingMenuActions(QList<QAction*>& actions, DNAAlphabetType alphabet, MaEditorSequenceArea* actionsParent) {
    MsaHighlightingSchemeRegistry* msaHighlightingSchemeRegistry = AppContext::getMsaHighlightingSchemeRegistry();
    MsaHighlightingSchemeFactory* nohighlightingFactory = msaHighlightingSchemeRegistry->getEmptySchemeFactory();

    if (alphabet == DNAAlphabet_RAW) {
        QMap<AlphabetFlags, QList<MsaHighlightingSchemeFactory*>> highlightingSchemesFactories = msaHighlightingSchemeRegistry->getAllSchemesGrouped();
        QList<MsaHighlightingSchemeFactory*> commonHighlightSchemesFactories = highlightingSchemesFactories[DNAAlphabet_RAW | DNAAlphabet_AMINO | DNAAlphabet_NUCL];
        QList<MsaHighlightingSchemeFactory*> aminoHighlightSchemesFactories = highlightingSchemesFactories[DNAAlphabet_RAW | DNAAlphabet_AMINO];
        QList<MsaHighlightingSchemeFactory*> nucleotideHighlightSchemesFactories = highlightingSchemesFactories[DNAAlphabet_RAW | DNAAlphabet_NUCL];

        commonHighlightSchemesFactories.removeAll(nohighlightingFactory);
        commonHighlightSchemesFactories.prepend(nohighlightingFactory);

        fillHighlightingMenuSectionForCurrentAlphabet(commonHighlightSchemesFactories, actions, tr("All alphabets"), actionsParent);
        fillHighlightingMenuSectionForCurrentAlphabet(aminoHighlightSchemesFactories, actions, tr("Amino acid alphabet"), actionsParent);
        fillHighlightingMenuSectionForCurrentAlphabet(nucleotideHighlightSchemesFactories, actions, tr("Nucleotide alphabet"), actionsParent);
    } else {
        QList<MsaHighlightingSchemeFactory*> highlightingSchemesFactories = msaHighlightingSchemeRegistry->getAllSchemes(alphabet);
        highlightingSchemesFactories.removeAll(nohighlightingFactory);
        highlightingSchemesFactories.prepend(nohighlightingFactory);
        fillHighlightingSchemeMenuActions(actions, highlightingSchemesFactories, actionsParent);
    }
}

void MsaSchemesMenuBuilder::addActionOrTextSeparatorToMenu(QAction* a, QMenu* colorsSchemeMenu) {
    if (a->text().contains(SECTION_TOKEN)) {
        QString text = a->text().replace(SECTION_TOKEN, QString());
        auto pLabel = new QLabel(text);
        pLabel->setAlignment(Qt::AlignCenter);
        pLabel->setStyleSheet("font: bold;");
        auto separator = new QWidgetAction(a);
        separator->setDefaultWidget(pLabel);
        colorsSchemeMenu->addAction(separator);
    } else {
        colorsSchemeMenu->addAction(a);
    }
}

void MsaSchemesMenuBuilder::fillColorSchemeMenuActions(QList<QAction*>& actions, QList<MsaColorSchemeFactory*> colorFactories, MaEditorSequenceArea* actionsParent) {
    auto msa = qobject_cast<MsaEditor*>(actionsParent->getEditor());

    foreach (MsaColorSchemeFactory* factory, colorFactories) {
        QString name = factory->getName();
        auto action = new QAction(name, actionsParent);
        action->setObjectName(name);
        action->setCheckable(true);
        action->setData(factory->getId());
        actions.append(action);

        if (msa != nullptr && msa->isMultilineMode()) {
            signalMapper->setMapping(action, action->data().toString());
            connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
        } else {
            connect(action, SIGNAL(triggered()), actionsParent, SLOT(sl_changeColorScheme()));
        }
    }
    if (msa != nullptr && msa->isMultilineMode()) {
        connect(signalMapper, SIGNAL(mapped(const QString&)), msa->getMainWidget(), SLOT(sl_changeColorScheme(const QString&)));
    }
}

void MsaSchemesMenuBuilder::fillHighlightingSchemeMenuActions(QList<QAction*>& actions, const QList<MsaHighlightingSchemeFactory*>& highlightingSchemeFactories, MaEditorSequenceArea* actionsParent) {
    foreach (MsaHighlightingSchemeFactory* factory, highlightingSchemeFactories) {
        QString name = factory->getName();
        auto action = new QAction(name, actionsParent);
        action->setObjectName(name);
        action->setCheckable(true);
        action->setData(factory->getId());
        connect(action, SIGNAL(triggered()), actionsParent, SLOT(sl_changeHighlightScheme()));
        actions.append(action);
    }
}

void MsaSchemesMenuBuilder::fillColorMenuSectionForCurrentAlphabet(QList<MsaColorSchemeFactory*>& colorSchemesFactories, QList<QAction*>& actions, const QString& alphName, MaEditorSequenceArea* actionsParent) {
    if (!colorSchemesFactories.isEmpty()) {
        actions.append(new QAction(SECTION_TOKEN + alphName, actionsParent));
        fillColorSchemeMenuActions(actions, colorSchemesFactories, actionsParent);
    }
}

void MsaSchemesMenuBuilder::fillHighlightingMenuSectionForCurrentAlphabet(QList<MsaHighlightingSchemeFactory*>& highlightSchemesFactories, QList<QAction*>& actions, const QString& alphabet, MaEditorSequenceArea* actionsParent) {
    if (!highlightSchemesFactories.isEmpty()) {
        actions.append(new QAction(SECTION_TOKEN + alphabet, actionsParent));
        fillHighlightingSchemeMenuActions(actions, highlightSchemesFactories, actionsParent);
    }
}

}  // namespace U2
