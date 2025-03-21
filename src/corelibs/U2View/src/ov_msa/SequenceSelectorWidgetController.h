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

#pragma once

#include <U2Gui/SuggestCompleter.h>

#include <U2View/MsaEditor.h>

#include "ui_SequenceSelectorWidget.h"

namespace U2 {

class SequenceSelectorWidgetController : public QWidget, Ui_SequenceSelectorWidget {
    Q_OBJECT
public:
    SequenceSelectorWidgetController(MsaEditor* _msa);
    ~SequenceSelectorWidgetController();

    QString text() const;

    void setSequenceId(qint64 newId);
    qint64 sequenceId() const;

signals:
    void si_selectionChanged();

private slots:
    void sl_seqLineEditEditingFinished();
    void sl_seqLineEditEditingFinished(const Msa&, const MaModificationInfo&);
    void sl_addSeqClicked();
    void sl_deleteSeqClicked();
    void sl_setDefaultLineEditValue();

private:
    void updateCompleter();

    MsaEditor* msa;
    MSACompletionFiller* filler;
    BaseCompleter* completer;
    QString defaultSeqName;
    qint64 seqId;
};

};  // namespace U2
