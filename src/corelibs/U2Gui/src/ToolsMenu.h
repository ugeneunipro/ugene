/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#ifndef _U2_TOOLS_MENU_H_
#define _U2_TOOLS_MENU_H_

#include <QStringList>

#include <U2Core/global.h>

class QAction;
class QMenu;

namespace U2 {

class U2GUI_EXPORT ToolsMenu : public QObject {
    Q_OBJECT
public:
    static void init();

    static void addAction(const QString &menuName, QAction *action);

    static const QString LINE;
    static const QString TOOLS;

    static const QString SANGER_MENU;
    static const QString SANGER_CONTROL;
    static const QString SANGER_DENOVO;
    static const QString SANGER_ALIGN;

    static const QString NGS_MENU;
    static const QString NGS_CONTROL;
    static const QString NGS_DENOVO;
    static const QString NGS_MAP;
    static const QString NGS_INDEX;
    static const QString NGS_SCAFFOLD;
    static const QString NGS_RAW_DNA;
    static const QString NGS_CALL_VARIANTS;
    static const QString NGS_VARIANT_EFFECT;
    static const QString NGS_RAW_RNA;
    static const QString NGS_RNA;
    static const QString NGS_TRANSCRIPT;
    static const QString NGS_RAW_CHIP;
    static const QString NGS_CHIP;
    static const QString NGS_CLASSIFICATION;
    static const QString NGS_COVERAGE;
    static const QString NGS_CONSENSUS;
    static const QString NGS_CONVERT_SAM;

    static const QString BLAST_MENU;
    static const QString BLAST_NCBI;
    static const QString BLAST_DBP;
    static const QString BLAST_SEARCHP;
    static const QString BLAST_QUERYP;
    static const QString BLAST_DB;
    static const QString BLAST_SEARCH;

    static const QString MALIGN_MENU;
    static const QString MALIGN_MUSCLE;
    static const QString MALIGN_CLUSTALW;
    static const QString MALIGN_CLUSTALO;
    static const QString MALIGN_MAFFT;
    static const QString MALIGN_TCOFFEE;
    static const QString MALIGN_KALIGN;

    static const QString CLONING_MENU;
    static const QString CLONING_FRAGMENTS;
    static const QString CLONING_CONSTRUCT;

    static const QString PRIMER_MENU;
    static const QString PRIMER_LIBRARY;

    static const QString TFBS_MENU;
    static const QString TFBS_SITECON;
    static const QString TFBS_WEIGHT;
    static const QString TFBS_SEACRH;

    static const QString HMMER_MENU;
    static const QString HMMER_BUILD3;
    static const QString HMMER_SEARCH3;
    static const QString HMMER_SEARCH3P;
    static const QString HMMER_BUILD2;
    static const QString HMMER_CALIBRATE2;
    static const QString HMMER_SEARCH2;

    static const QString DOTPLOT;
    static const QString GENERATE_SEQUENCE;
    static const QString QUERY_DESIGNER;
    static const QString WORKFLOW_DESIGNER;
    static const QString GUI_TEST_RUNNER;

private:
    static QMenu * getToolsMenu();
    static QMenu * getMenu(const QString &menuName);
    static QMenu * createMenu(QMenu *tools, const QString &menuName);
    static QAction * getPrevAction(QMenu *menu, const QString &menuName, const QString &actionName);
    static QAction * getNextAction(QMenu *menu, const QString &menuName, const QString &actionName);
    static void insertAction(QMenu *menu, const QString &menuName, QAction *action);
    static bool mustHaveSeparator(const QString &menuName, const QString &actionName1, const QString &actionName2);

private:
    static QMap<QString, QString> actionText;
    static QMap<QString, QString> actionIcon;
    static QMap<QString, QStringList> subMenuAction; // subMenuObjectName -> List(actionObjectName)
};

} // U2

#endif // _U2_TOOLS_MENU_H_
