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

#ifndef _U2_MCA_EDITOR_H_
#define _U2_MCA_EDITOR_H_

#include <U2Core/MultipleChromatogramAlignmentObject.h>

#include "MaEditor.h"
#include "McaEditorWgt.h"
#include "view_rendering/MaEditorWgt.h"

namespace U2 {

class McaEditor;
class SequenceObjectContext;

#define     MCAE_MENU_ALIGNMENT     "MCAE_MENU_ALIGNMENT"
#define     MCAE_MENU_APPEARANCE    "MCAE_MENU_APPEARANCE"
#define     MCAE_MENU_NAVIGATION    "MCAE_MENU_NAVIGATION"
#define     MCAE_MENU_EDIT          "MCAE_MENU_EDIT"

#define MCAE_SETTINGS_SHOW_CHROMATOGRAMS    "show_chromatograms"
#define MCAE_SETTINGS_SHOW_OVERVIEW         "show_overview"
#define MCAE_SETTINGS_PEAK_HEIGHT           "peak_height"
#define MCAE_SETTINGS_CONSENSUS_TYPE        "consensus_type"

class U2VIEW_EXPORT McaEditor : public MaEditor {
    Q_OBJECT
    friend class McaEditorSequenceArea;
public:
    McaEditor(const QString& viewName,
              MultipleChromatogramAlignmentObject* obj);

    QString getSettingsRoot() const { return MCAE_SETTINGS_ROOT; }

    MultipleChromatogramAlignmentObject* getMaObject() const;
    McaEditorWgt *getUI() const;

    virtual void buildStaticToolbar(QToolBar* tb);

    virtual void buildStaticMenu(QMenu* menu);

    virtual int getRowContentIndent(int rowId) const;

    bool isChromVisible(qint64 rowId) const;
    bool isChromVisible(int rowIndex) const;
    bool isChromatogramButtonChecked() const;

    QString getReferenceRowName() const;

    char getReferenceCharAt(int pos) const;

    SequenceObjectContext* getReferenceContext() const;

protected slots:
    void sl_onContextMenuRequested(const QPoint & pos);
    void sl_showHideChromatograms(bool show);

private slots:
    void sl_showGeneralTab();
    void sl_showConsensusTab();

    void sl_saveOverviewState();
    void sl_saveChromatogramState();

protected:
    QWidget* createWidget();
    void initActions();

    QAction*          showChromatogramsAction;
    QAction*          showGeneralTabAction;
    QAction*          showConsensusTabAction;

    QMap<qint64, bool>  chromVisibility;

    SequenceObjectContext*  referenceCtx;

private:
    void addAlignmentMenu(QMenu *menu);
    void addAppearanceMenu(QMenu *menu);
    void addNavigationMenu(QMenu *menu);
    void addEditMenu(QMenu *menu);
};

}   // namespace U2

#endif // _U2_MCA_EDITOR_H_
