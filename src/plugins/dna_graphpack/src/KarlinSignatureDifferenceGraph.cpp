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

#include "KarlinSignatureDifferenceGraph.h"

#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/TextUtils.h>
#include "DNAGraphPackPlugin.h"
#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNATranslation.h>


/* TRANSLATOR U2::KarlinGraphFactory */

namespace U2 {
static QString nameByType() {
    return KarlinGraphFactory::tr("Karlin Signature Difference");

}

KarlinGraphFactory::KarlinGraphFactory(QObject* p)
: GSequenceGraphFactory(nameByType(), p)
{
}

//+
bool KarlinGraphFactory::isEnabled(const U2SequenceObject* o) const {
    const DNAAlphabet* al = o->getAlphabet();
    return al->isNucleic();
}

QList<QSharedPointer<GSequenceGraphData> > KarlinGraphFactory::createGraphs(GSequenceGraphView* v) {
    Q_UNUSED(v);
    QList<QSharedPointer<GSequenceGraphData> > res;
    assert(isEnabled(v->getSequenceObject()));
    QSharedPointer<GSequenceGraphData> d = QSharedPointer<GSequenceGraphData>(new GSequenceGraphData(getGraphName()));
    d->ga = new KarlinGraphAlgorithm();
    res.append(d);
    return res;
}

//////////////////////////////////////////////////////////////////////////
// KarlinGraphAlgorithm

static int getIndex(char nucl)
{
    switch (nucl) {
        case 'A' : return 0;
        case 'C' : return 1;
        case 'T' : return 2;
        case 'G' : return 3;
    }
    return -1;
}

#define IDX(x, y) ((x)*4+(y))
#define IDX_NUCL(x,y) IDX(getIndex(x), getIndex(y))

//todo:: use limits
static const float FLOAT_MIN = 0.000000001f;

KarlinGraphAlgorithm::KarlinGraphAlgorithm()
    : global_relative_abundance_values(NULL)
{

}

KarlinGraphAlgorithm::~KarlinGraphAlgorithm() {
    delete[] global_relative_abundance_values;
}

void KarlinGraphAlgorithm::calculate(QVector<float>& res, U2SequenceObject* o, const U2Region& vr,
    const GSequenceGraphWindowData* d, U2OpStatus &os)
{
    assert(d!=NULL);
    int nSteps = GSequenceGraphUtils::getNumSteps(vr, d->window, d->step);
    res.reserve(nSteps);

    const DNAAlphabet* al = o->getAlphabet();
    assert(al->isNucleic());

    DNATranslationRegistry* tr = AppContext::getDNATranslationRegistry();
    DNATranslation* complT = tr->lookupComplementTranslation(al);
    assert(complT != NULL);

    DNATranslation* complTrans = complT;
    mapTrans = complTrans->getOne2OneMapper();

    const QByteArray& seq = getSequenceData(o, os);
    CHECK_OP(os, );
    int seqLen = seq.size();
    const char* seqc = seq.constData();
    if (global_relative_abundance_values == NULL) {
        global_relative_abundance_values = new float[16];
        calculateRelativeAbundance(seqc, seqLen, global_relative_abundance_values, os);
        CHECK_OP(os, );
    }
    //check!!
    for (int i = 0; i < nSteps; i++) {
        int start = vr.startPos + i * d->step;
        int end = start + d->window;
        float val = getValue(start, end, seq, os);
        CHECK_OP(os, );
        res.append(val);
    }
}

float KarlinGraphAlgorithm::getValue (int start, int end, const QByteArray& s, U2OpStatus &os) {
    float relative_abundance_values[16];
    calculateRelativeAbundance (s.constData()+start, end - start, relative_abundance_values, os);
    float signature_difference = 0;
    for (int first_base = 0 ; first_base < 4 ; ++first_base) {
        for (int second_base = 0 ; second_base < 4 ; ++second_base) {
            CHECK_OP(os, 0);
            int idx = IDX(first_base,  second_base);
            float global_value = global_relative_abundance_values[idx];
            float local_value = relative_abundance_values[idx];
            signature_difference += qAbs(global_value - local_value);
        }
    }
    float res = signature_difference / 16.0;
    return res;
}

void KarlinGraphAlgorithm::calculateRelativeAbundance (const char* seq, int length, float* results, U2OpStatus &os) {
    QByteArray tmp;
    tmp.resize(length);

    int base_counts[4]={0, 0, 0 ,0};
    int dinucleotide_base_counts[16]={0, 0, 0 ,0,  0, 0, 0 ,0,  0, 0, 0 ,0,  0, 0, 0 ,0,};

    int next_f_base_index = 0;
    int next_r_base_index = 0;

    for (int i = 0 ; i < length - 1; ++i) {
        CHECK_OP(os, );
        char this_f_base = seq[i];
        char next_f_base = seq[i + 1];

        int this_f_base_index = getIndex(this_f_base);
        next_f_base_index = getIndex(next_f_base);

        if (this_f_base_index >= 0 && next_f_base_index >= 0) {
            ++base_counts[this_f_base_index];
            ++dinucleotide_base_counts[IDX(this_f_base_index, next_f_base_index)];
        }

        char this_r_base = mapTrans.at(this_f_base);
        char next_r_base = mapTrans.at(next_f_base);

        int this_r_base_index = getIndex(this_r_base);
        next_r_base_index = getIndex(next_r_base);

        if (this_r_base_index >= 0 && next_r_base_index >= 0) {
            ++base_counts[this_r_base_index];
            ++dinucleotide_base_counts[IDX(this_r_base_index, next_r_base_index)];
        }
    }

    if (next_f_base_index >= 0) {
        ++base_counts[next_f_base_index];
    }

    if (next_r_base_index >= 0) {
        ++base_counts[next_r_base_index];
    }

    for (int first_base_index = 0; first_base_index < 4; ++first_base_index) {
        for (int second_base_index = 0; second_base_index < 4; ++second_base_index) {
            int idx = IDX(first_base_index,second_base_index);
            float dinucleotide_frequency =  float(dinucleotide_base_counts[idx]) / (2*(length - 1));
            float first_base_frequency = float(base_counts[first_base_index]) / (2*length);
            float second_base_frequency = float(base_counts[second_base_index]) / (2*length);
            results[idx] = dinucleotide_frequency / qMax(FLOAT_MIN , first_base_frequency * second_base_frequency);
        }
    }

}


} // namespace

