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

#pragma once

#include <U2Core/Task.h>

#include "Primer3TaskSettings.h"

#include <QSharedPointer>

namespace U2 {

class AnnotationTableObject;
struct PrimerPair;
struct PrimerSingle;

/**
 * This class represents a task, which runs Primer3 itself.
 */
class Primer3Task : public Task {
    Q_OBJECT
public:
    Primer3Task(const QSharedPointer<Primer3TaskSettings>& settings);

    void prepare() override;
    void run() override;
    Task::ReportResult report() override;

    const QList<QSharedPointer<PrimerPair>>& getBestPairs() const;
    const QList<QSharedPointer<PrimerSingle>>& getSinglePrimers() const;

private:
    void selectPairsSpanningExonJunction(p3retval* primers, int toReturn);
    void selectPairsSpanningIntron(p3retval* primers, int toReturn);

    QSharedPointer<Primer3TaskSettings> settings;
    QList<QSharedPointer<PrimerPair>> bestPairs;
    QList<QSharedPointer<PrimerSingle>> singlePrimers;

    int offset = 0;
};

}  // namespace U2
