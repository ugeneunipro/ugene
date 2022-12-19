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

#include <U2Algorithm/BaseTempCalc.h>

#include <QMap>

//#include <U2Core/global.h>

namespace U2 {

struct Primer3TempCalcSettings : public TempCalcSettings {
    QMap<QString, QVariant> toVariantMap() const override;
    void fromVariantMap(const QMap<QString, QVariant>& mapSettings) override;

    /* The table of nearest-neighbor thermodynamic parameters
       and method for Tm calculation */
    enum class TmMethodType {
        Breslauer = 0, /* Method for Tm calculations and the thermodynamic parameters from
                          [SantaLucia JR (1998) "A unified view of
                          polymer, dumbbell and oligonucleotide DNA nearest-neighbor
                          thermodynamics", Proc Natl Acad Sci 95:1460-65
                          http://dx.doi.org/10.1073/pnas.95.4.1460] */
        Santalucia = 1, /* Method for Tm
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
        Schildkraut = 0, /* [Schildkraut, C, and Lifson, S (1965)
                            "Dependence of the melting temperature of DNA on salt
                            concentration", Biopolymers 3:195-208 (not available on-line)] */
        Santalucia = 1, /* [SantaLucia JR (1998) "A
                           unified view of polymer, dumbbell and oligonucleotide DNA
                           nearest-neighbor thermodynamics", Proc Natl Acad Sci 95:1460-65
                           http://dx.doi.org/10.1073/pnas.95.4.1460] */
        Owczarzy = 2, /* [Owczarzy, R., Moreira, B.G., You, Y., 
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

    TmMethodType tmMethod = TmMethodType::Santalucia; /* See description above. */
    SaltCorrectionType saltCorrections = SaltCorrectionType::Santalucia; /* See description above. */    

    static const QString KEY_DNA_CONC;
    static const QString KEY_SALT_CONC;
    static const QString KEY_DIVALENT_CONC;
    static const QString KEY_DNTP_CONC;
    static const QString KEY_DMSO_CONC;
    static const QString KEY_DMSO_FACT;
    static const QString KEY_FPRMAMIDE_CONC;
    static const QString KEY_MAX_LEN;
    static const QString KEY_TM_METHOD;
    static const QString KEY_SALT_CORRECTION;
};

class Primer3TempCalc : public BaseTempCalc {
public:
    Primer3TempCalc(Primer3TempCalcSettings* settings);

    double getMeltingTemperature(const QByteArray& sequence) override;
};

}


#endif
