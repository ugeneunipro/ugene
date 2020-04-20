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

#ifndef _U2_MA_CONSENSUS_MISMATCH_CONTROLLER_H_
#define _U2_MA_CONSENSUS_MISMATCH_CONTROLLER_H_

#include "MSAEditorConsensusCache.h"

#include <QBitArray>
#include <QObject>

class QAction;

namespace U2 {

class MaEditor;

class MaConsensusMismatchController : public QObject {
    Q_OBJECT
public:
    MaConsensusMismatchController(QObject* p,
                                  const QSharedPointer<MSAEditorConsensusCache>& consCache,
                                  MaEditor* editor);
    bool isMismatch(int pos) const;

    QAction *getPrevMismatchAction() const;
    QAction *getNextMismatchAction() const;

signals:
    void si_selectMismatch(int pos);

private slots:
    void sl_updateItem(int pos, char c);
    void sl_resize(int newSize);

    void sl_next();
    void sl_prev();

private:
    void selectNextMismatch(NavigationDirection direction);

private:
    QBitArray mismatchCache;
    QSharedPointer<MSAEditorConsensusCache> consCache;
    MaEditor* editor;

    QAction* nextMismatch;
    QAction* prevMismatch;
};

} // namespace U2

#endif // _U2_MA_CONSENSUS_MISMATCH_CONTROLLER_H_
