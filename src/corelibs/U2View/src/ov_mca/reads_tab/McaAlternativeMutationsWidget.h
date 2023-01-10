/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#ifndef _U2_MCA_ALTERNATIVE_MUTATIONS_WIDGET_H_
#define _U2_MCA_ALTERNATIVE_MUTATIONS_WIDGET_H_

#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2Type.h>

#include "ui_McaAlternativeMutationsWidget.h"

namespace U2 {

class MaEditorSequenceArea;
class McaEditorSequenceArea;
class MaEditorStatusBar;
class McaEditorStatusBar;
class MultipleAlignmentObject;
class MultipleChromatogramAlignmentObject;

/*
 * Widget to set up alternative mutations. Relates to UGENE-7105.
 * When the "Alternative mutations" feature is enabled,
 * all symbols with the second chromatogram trace percentage more than the defined "threshold" value
 * has to be replaced with the symbol, which corresponds to the second chromatogram trace.
 */
class McaAlternativeMutationsWidget : public QWidget, private Ui_McaAlternativeMutationsWidget {
    Q_OBJECT
public:
    McaAlternativeMutationsWidget(QWidget* parent = nullptr);

    /*
     * Initialize object.
     * Call this function right after instance creation.
     */
    void init(MultipleAlignmentObject* maObject, MaEditorSequenceArea* seqArea, MaEditorStatusBar* statusBar);

    static const QString getAlternativeMutationsCheckedId();

private slots:
    /*
     * Update the sequence area and write new alternative mutations settings to the database
     */
    void sl_updateAlternativeMutations();
    /*
     * Lock widget if the document is locked, unlock otherwise
     */
    void sl_updateLockState();

private:
    void showEvent(QShowEvent* e) override;
    /*
     * Updates GUI with values from the database
     **/
    void updateValuesFromDb();
    /*
     * Update the database with values from GUI
     **/
    void updateDb(U2OpStatus& os);

    McaEditorSequenceArea* seqArea = nullptr;
    MultipleChromatogramAlignmentObject* mcaObject = nullptr;
    McaEditorStatusBar* statusBar = nullptr;
    U2IntegerAttribute checkedStateAttribute;
    U2IntegerAttribute thresholdAttribute;
    U2Object mcaDbiObj;

    static const QString ALTERNATIVE_MUTATIONS_CHECKED;
    static const QString ALTERNATIVE_MUTATIONS_THRESHOLD;
};

}  // namespace U2

#endif
