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

#pragma once

#include <U2Gui/ObjectViewModel.h>

namespace U2 {

class MSAEditor;

/** MSA editor built-in support for "Align-Sequences-To-Alignment" algorithms. */
class AlignSequencesToAlignmentSupport : public GObjectViewWindowContext {
    Q_OBJECT
public:
    AlignSequencesToAlignmentSupport(QObject* parent);

protected:
    void initViewContext(GObjectViewController* view) override;
};

/** Base action for all 'align-to-alignment' actions. */
class BaseObjectViewAlignmentAction : public GObjectViewAction {
    Q_OBJECT
public:
    BaseObjectViewAlignmentAction(QObject* parent, MSAEditor* msaEditor, const QString& algorithmId, const QString& text, int order);

    /** Returns MSA editor this action is created for. */
    MSAEditor* getEditor() const;

public slots:
    virtual void sl_activate() = 0;

protected:
    MSAEditor* msaEditor = nullptr;

    QString algorithmId;
};

class AlignSequencesToAlignmentAction : public BaseObjectViewAlignmentAction {
    Q_OBJECT
public:
    AlignSequencesToAlignmentAction(QObject* parent, MSAEditor* msaEditor, const QString& algorithmId, const QString& text, int order);

public slots:

    void sl_updateState();

    /** Runs AlignNewSequencesToAlignment or AlignNewAlignmentToAlignment algorithm. */
    void sl_activate() override;
};

class AlignSelectedSequencesAction : public BaseObjectViewAlignmentAction {
    Q_OBJECT
public:
    AlignSelectedSequencesAction(QObject* parent, MSAEditor* msaEditor, const QString& algorithmId, const QString& text, int order);
public slots:

    /** Runs AlignSelectionToAlignment algorithm. */
    void sl_activate() override;
};

}  // namespace U2
