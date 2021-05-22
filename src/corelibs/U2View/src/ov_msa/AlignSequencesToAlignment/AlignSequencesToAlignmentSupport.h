/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#ifndef _U2_ALIGN_SEQUENCES_TO_ALIGNMENT_SUPPORT_H_
#define _U2_ALIGN_SEQUENCES_TO_ALIGNMENT_SUPPORT_H_

#include <U2Gui/ObjectViewModel.h>

namespace U2 {

class MSAEditor;

/** MSA editor built-in support for "Align-Sequences-To-Alignment" algorithms. */
class AlignSequencesToAlignmentSupport : public GObjectViewWindowContext {
    Q_OBJECT
public:
    AlignSequencesToAlignmentSupport(QObject *parent);

protected:
    void initViewContext(GObjectView *view) override;

protected slots:
    /**
     * Runs alignment selected alignment algorithm.
     * Finds the selected algorithm using the sender action data.
     */
    void sl_alignSequencesToAlignment();
};

class AlignSequencesToAlignmentAction : public GObjectViewAction {
    Q_OBJECT
public:
    AlignSequencesToAlignmentAction(QObject *parent, MSAEditor *msaEditor, const QString &text, int order);

    /** Returns MSA editor this action is created for. */
    MSAEditor *getEditor() const;

public slots:
    void sl_updateState();

private:
    MSAEditor *msaEditor = nullptr;
};

}    // namespace U2
#endif