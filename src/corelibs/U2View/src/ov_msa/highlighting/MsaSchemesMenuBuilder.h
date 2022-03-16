/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#ifndef _U2_MSA_SCHEMES_MENU_BUILDER_H_
#define _U2_MSA_SCHEMES_MENU_BUILDER_H_

#include <QAction>
#include <QList>
#include <QSignalMapper>

#include <U2Core/global.h>

#define SECTION_TOKEN QString("SEPARATOR")

namespace U2 {

class MsaColorSchemeFactory;
class MsaHighlightingSchemeFactory;
class MaEditorSequenceArea;

class MsaSchemesMenuBuilder : QObject {
    Q_OBJECT
public:
    enum ColorSchemeType {
        Common,
        Custom
    };

    MsaSchemesMenuBuilder()
        : QObject() {};

    static void createAndFillColorSchemeMenuActions(QList<QAction*>& actions, ColorSchemeType type, DNAAlphabetType alphabet, MaEditorSequenceArea* actionsParent);

    static void createAndFillHighlightingMenuActions(QList<QAction*>& actions, DNAAlphabetType alphabet, MaEditorSequenceArea* actionsParent);

    static void addActionOrTextSeparatorToMenu(QAction* a, QMenu* colorsSchemeMenu);

private:
    static void fillColorSchemeMenuActions(QList<QAction*>& actions, QList<MsaColorSchemeFactory*> colorFactories, MaEditorSequenceArea* actionsParent);
    static void fillHighlightingSchemeMenuActions(QList<QAction*>& actions, const QList<MsaHighlightingSchemeFactory*>& highlightingSchemeFactories, MaEditorSequenceArea* actionsParent);

    static void fillColorMenuSectionForCurrentAlphabet(QList<MsaColorSchemeFactory*>& colorSchemesFactories, QList<QAction*>& actions, const QString& alphName, MaEditorSequenceArea* actionsParent);
    static void fillHighlightingMenuSectionForCurrentAlphabet(QList<MsaHighlightingSchemeFactory*>& highlightSchemesFactories, QList<QAction*>& actions, const QString&, MaEditorSequenceArea* actionsParent);

    static QSignalMapper *signalMapper;
};

}  // namespace U2

#endif
