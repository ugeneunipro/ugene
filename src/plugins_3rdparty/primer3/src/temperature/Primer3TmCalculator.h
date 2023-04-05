/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include <U2Algorithm/TmCalculator.h>

#include <QMap>

namespace U2 {


class Primer3TmCalculator : public TmCalculator {
public:
    Primer3TmCalculator(const QVariantMap& settings);

    double getMeltingTemperature(const QByteArray& sequence) override;

    /* DNA concentration (nanomolar). */
    static const QString KEY_DNA_CONC;
    /* Concentration of divalent cations (millimolar). */
    static const QString KEY_SALT_CONC;
    /* Concentration of divalent cations (millimolar) */
    static const QString KEY_DIVALENT_CONC;
    /* Concentration of dNTPs (millimolar) */
    static const QString KEY_DNTP_CONC;
    /* Concentration of DMSO (%) */
    static const QString KEY_DMSO_CONC;
    /* DMSO correction factor, default 0.6 */
    static const QString KEY_DMSO_FACT;
    /* Concentration of formamide (mol/l) */
    static const QString KEY_FORMAMIDE_CONC;
     /* The maximum sequence length for
        using the nearest neighbor model
        For sequences longer than this,
        uses the "GC%" formula. */
    static const QString KEY_MAX_LEN;
    /* The table of nearest-neighbor thermodynamic parameters
       and method for Tm calculation
       0 - Method for Tm
           calculations from the paper[Rychlik W, Spencer WJ and Rhoads RE
           (1990) "Optimization of the annealing temperature for DNA
           amplification in vitro", Nucleic Acids Res 18:6409-12
           http://www.pubmedcentral.nih.gov/articlerender.fcgi?tool=pubmed&pubmedid=2243783].
           and the thermodynamic parameters from the paper[Breslauer KJ, Frank
           R, Bl�cker H and Marky LA(1986) "Predicting DNA duplex stability
           from the base sequence" Proc Natl Acad Sci 83:4746-50
           http://dx.doi.org/10.1073/pnas.83.11.3746]
       1 - Method for Tm calculations and the thermodynamic parameters from
           [SantaLucia JR (1998) "A unified view of
           polymer, dumbbell and oligonucleotide DNA nearest-neighbor
           thermodynamics", Proc Natl Acad Sci 95:1460-65
           http://dx.doi.org/10.1073/pnas.95.4.1460] */
    static const QString KEY_TM_METHOD;
    /* Formula for salt correction
       0 - [Schildkraut, C, and Lifson, S(1965)
           "Dependence of the melting temperature of DNA on salt
           concentration", Biopolymers 3:195-208 (not available on-line)]
       1 - [SantaLucia JR(1998) "A
           unified view of polymer, dumbbell and oligonucleotide DNA
           nearest - neighbor thermodynamics", Proc Natl Acad Sci 95:1460-65
           http://dx.doi.org/10.1073/pnas.95.4.1460]
       2 - [Owczarzy, R., Moreira, B.G., You, Y.,
           Behlke, M.A., and Walder, J.A. (2008) "Predicting stability of DNA
           duplexes in solutions containing magnesium and monovalent cations",
           Biochemistry 47 : 5336 - 53 http ://dx.doi.org/10.1021/bi702363u] */
    static const QString KEY_SALT_CORRECTION;

    static constexpr double DNA_CONC_DEFAULT = 50.0;
    static constexpr double SALT_CONC_DEFAULT = 50.0;
    static constexpr double DIVALENT_CONC_DEFAULT = 1.5;
    static constexpr double DNTP_CONC_DEFAULT = 0.6;
    static constexpr double DMSO_CONC_DEFAULT = 0.0;
    static constexpr double DMSO_FACT_DEFAULT = 0.6;
    static constexpr double FORMAMIDE_CONC_DEFAULT = 0.0;
    static constexpr int NN_MAX_LEN_DEFAULT = 36;
    static constexpr int TM_METHOD_DEFAULT = 1;
    static constexpr int SALT_CORRECTIONS_DEFAULT = 1;
};

}
