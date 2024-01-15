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
#ifdef __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-variable"
#endif
// clang-format off
#include "seqboot.h"
#include "cons.h"
// clang-format on
#ifdef __GNUC__
#    pragma GCC diagnostic pop
#endif

#include <iostream>

#include <QVector>

#include <U2Algorithm/CreatePhyTreeSettings.h>

#include <U2Core/Msa.h>
#include <U2Core/PhyTree.h>

namespace U2 {

typedef QVector<float> matrixrow;
typedef QVector<matrixrow> matrix;

class SeqBoot {
private:
    Msa malignment;
    QList<Msa> generatedSeq;
    int seqLen;

public:
    QString getTmpFileTemplate();
    void clearGeneratedSequences();  // to free memory
    void generateSequencesFromAlignment(const Msa& ma, const CreatePhyTreeSettings& settings);

    const Msa& getMSA(int pos) const;

    void initGenerSeq(int reps, int seqLen);

    SeqBoot();
    ~SeqBoot();
};

}  // namespace U2
