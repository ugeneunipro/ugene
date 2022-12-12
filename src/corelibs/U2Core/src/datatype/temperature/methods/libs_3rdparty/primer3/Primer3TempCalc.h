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

#ifndef U2_PRIMER_3_MELT_TEMP_CALC_
#define U2_PRIMER_3_MELT_TEMP_CALC_

#include <U2Core/BaseTempCalc.h>

//#include <U2Core/global.h>

namespace U2 {

struct Primer3MeltTempCalcSettings : public TempCalcSettings {
    /* The table of nearest-neighbor thermodynamic parameters
       and method for Tm calculation */
    enum class TmMethodType {
        breslauer = 0, /* Method for Tm calculations and the thermodynamic parameters from
                          [SantaLucia JR (1998) "A unified view of
                          polymer, dumbbell and oligonucleotide DNA nearest-neighbor
                          thermodynamics", Proc Natl Acad Sci 95:1460-65
                          http://dx.doi.org/10.1073/pnas.95.4.1460] */
        santalucia = 1, /* Method for Tm
                           calculations from the paper [Rychlik W, Spencer WJ and Rhoads RE
                           (1990) "Optimization of the annealing temperature for DNA
                           amplification in vitro", Nucleic Acids Res 18:6409-12
                           http://www.pubmedcentral.nih.gov/articlerender.fcgi?tool=pubmed&pubmedid=2243783].
                           and the thermodynamic parameters from the paper [Breslauer KJ, Frank
                           R, Bl�cker H and Marky LA (1986) "Predicting DNA duplex stability
                           from the base sequence" Proc Natl Acad Sci 83:4746-50
                           http://dx.doi.org/10.1073/pnas.83.11.3746] */
    };

    /* Formula for salt correction */
    enum class SaltCorrectionType {
        schildkraut = 0, /* [Schildkraut, C, and Lifson, S (1965)
                            "Dependence of the melting temperature of DNA on salt
                            concentration", Biopolymers 3:195-208 (not available on-line)] */
        santalucia = 1, /* [SantaLucia JR (1998) "A
                           unified view of polymer, dumbbell and oligonucleotide DNA
                           nearest-neighbor thermodynamics", Proc Natl Acad Sci 95:1460-65
                           http://dx.doi.org/10.1073/pnas.95.4.1460] */
        owczarzy = 2, /* [Owczarzy, R., Moreira, B.G., You, Y., 
                          Behlke, M.A., and Walder, J.A. (2008) "Predicting stability of DNA 
                          duplexes in solutions containing magnesium and monovalent cations", 
                          Biochemistry 47:5336-53 http://dx.doi.org/10.1021/bi702363u] */
    };
   
    double dnaConc = 50.0;   /* DNA concentration (nanomolar). */
    double saltConc = 50.0;  /* Concentration of divalent cations (millimolar). */
    double divalentConc = 1.5; /* Concentration of divalent cations (millimolar) */
    double dntpConc = 0.6;     /* Concentration of dNTPs (millimolar) */
    double dmsoConc = 0.0;     /* Concentration of DMSO (%) */
    double dmsoFact = 0.6;     /* DMSO correction factor, default 0.6 */
    double formamideConc = 0.0; /* Concentration of formamide (mol/l) */
    int nnMaxLen = 36;  /* The maximum sequence length for
                            using the nearest neighbor model
                            For sequences longer than this,
                            uses the "GC%" formula.
                        */

    TmMethodType tmMethod = TmMethodType::santalucia; /* See description above. */
    SaltCorrectionType saltCorrections = SaltCorrectionType::santalucia; /* See description above. */
    
};

class Primer3MeltTempCalc : public BaseTempCalc {
public:
    Primer3MeltTempCalc(Primer3MeltTempCalcSettings* settings);

    double getMeltingTemperature(const QByteArray& sequence) override;
};

}


#endif
