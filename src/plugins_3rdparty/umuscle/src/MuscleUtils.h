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

#ifndef _MUSCLE_UTILS_H_
#define _MUSCLE_UTILS_H_
#include <stdio.h>

// clang-format off
#include "muscle/muscle.h"
#include "muscle/distfunc.h"
#include "muscle/estring.h"
#include "muscle/msa.h"
#include "muscle/muscle_context.h"
#include "muscle/profile.h"
#include "muscle/seqvect.h"
#include "muscle/tree.h"
// clang-format on

#include <U2Core/MultipleAlignment.h>
#include <U2Core/Task.h>

class MuscleContext;

namespace U2 {

class DNAAlphabet;

struct FILEStub : public FILE {
public:
    FILEStub(TaskStateInfo& _tsi)
        : tsi(_tsi) {
    }
    TaskStateInfo& tsi;
};

class MuscleParamsHelper {
public:
    MuscleParamsHelper(TaskStateInfo& ti, MuscleContext* ctx);
    ~MuscleParamsHelper();

private:
    MuscleContext* ctx;
    FILEStub ugeneFileStub;
};

int ugene_printf(FILE* f, const char* format, ...);
ALPHA convertAlpha(const DNAAlphabet* al);
void setupAlphaAndScore(const DNAAlphabet* al, TaskStateInfo& ti);
void convertMAlignment2MSA(MSA& muscleMSA, const MultipleAlignment& ma, bool fixAlpha);
void convertMAlignment2SecVect(SeqVect& sv, const MultipleAlignment& ma, bool fixAlpha);
void convertMSA2MAlignment(MSA& msa, const DNAAlphabet* al, MultipleAlignment& res);
void prepareAlignResults(MSA& msa, const DNAAlphabet* al, MultipleAlignment& ma, bool mhack);

}  // namespace U2

#endif  //_MUSCLE_ACCESSORIES_H_
