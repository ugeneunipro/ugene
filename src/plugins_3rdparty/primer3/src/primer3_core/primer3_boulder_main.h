#include "libprimer3.h"

#ifndef PRIMER3_ADAPTER_H
#define PRIMER3_ADAPTER_H

#define FORWARD 1
#define REVERSE -1

/* Default parameter values.  */
#define OPT_SIZE            20
#define MIN_SIZE             18
#define MAX_SIZE             27

#define OPT_TM             60.0
#define MIN_TM             57.0
#define MAX_TM             63.0
#define MAX_DIFF_TM       100.0


/*
Added by T.Koressaar for updated table thermodynamics.  Specifies
details of melting temperature calculation.  (New in v. 1.1.0, added
by Maido Remm and Triinu Koressaar.)

A value of 1 (recommended) directs primer3 to use the table of
thermodynamic values and the method for melting temperature
calculation suggested in the paper [SantaLucia JR (1998) "A unified
view of polymer, dumbbell and oligonucleotide DNA nearest-neighbor
thermodynamics", Proc Natl Acad Sci 95:1460-65
http://dx.doi.org/10.1073/pnas.95.4.1460].

A value of 0 directs primer3 to a backward compatible calculation
(in other words, the only calculation availble in previous
version of primer3).

This backward compatible calculation uses the table of
thermodynamic parameters in the paper [Breslauer KJ, Frank R,
Blöcker H and Marky LA (1986) "Predicting DNA duplex stability
from the base sequence" Proc Natl Acad Sci 83:4746-50
http://dx.doi.org/10.1073/pnas.83.11.3746],
and the method in the paper [Rychlik W, Spencer WJ and Rhoads
RE (1990) "Optimization of the annealing temperature for DNA
amplification in vitro", Nucleic Acids Res 18:6409-12
http://www.pubmedcentral.nih.gov/articlerender.fcgi?tool=pubmed&pubmedid=2243783].

The default value is 0 only for backward compatibility.
*/
#define TM_SANTALUCIA       0

/*
Added by T.Koressaar for salt correction for Tm calculation.
A value of 1 (recommended) directs primer3 to use the salt
correction formula in the paper [SantaLucia JR (1998) "A unified view
of polymer, dumbbell and oligonucleotide DNA nearest-neighbor
thermodynamics", Proc Natl Acad Sci 95:1460-65
http://dx.doi.org/10.1073/pnas.95.4.1460]

A value of 0 directs primer3 to use the the salt correction
formula in the paper [Schildkraut, C, and Lifson, S (1965)
"Dependence of the melting temperature of DNA on salt
concentration", Biopolymers 3:195-208 (not available on-line)].
This was the formula used in previous version of primer3.

A value of 2 directs primer3 to use the salt correction formula
in the paper [Owczarzy R, You Y, Moreira BG, Manthey JA, Huang L,
Behlke MA and Walder JA (2004) "Effects of sodium ions on DNA
duplex oligomers: Improved predictions of melting temperatures",
Biochemistry 43:3537-54 http://dx.doi.org/10.1021/bi034621r].

The default is 0 only for backward compatibility.
*/
#define SALT_CORRECTIONS    0

#define DEFAULT_OPT_GC_PERCENT PR_UNDEFINED_INT_OPT
#define MIN_GC             20.0
#define MAX_GC             80.0
#define SALT_CONC          50.0

/*
DIVALENT_CONC and DNTP_CONC are both needed for enabling to use divalent cations for
calculation of melting temperature of short and long oligos.
The formula for converting the divalent cations to monovalent cations is in the paper
[Ahsen von N, Wittwer CT, Schutz E (2001) "Oligonucleotide Melting Temperatures under PCR Conditions:
Nearest-Neighbor Corrections for Mg^2+, Deoxynucleotide Triphosphate, and Dimethyl Sulfoxide Concentrations
with Comparision to Alternative Empirical Formulas", Clinical Chemistry 47:1956-61
http://www.clinchem.org/cgi/content/full/47/11/1956]
The default is 0. (New in v. 1.1.0, added by Maido Remm and Triinu Koressaar.)
*/
#define DIVALENT_CONC       0.0
#define DNTP_CONC           0.0
#define DNA_CONC           50.0
#define NUM_NS_ACCEPTED       0
#define MAX_POLY_X            5
#define SELF_ANY            800
#define SELF_END            300
#define PAIR_COMPL_ANY      800
#define PAIR_COMPL_END      300
#define FILE_FLAG             0
#define EXPLAIN_FLAG          0
#define GC_CLAMP              0
#define LIBERAL_BASE          0
#define PICK_INTERNAL_OLIGO   0
#define PRIMER_TASK           0
#define INTERNAL_OLIGO_OPT_SIZE   20
#define INTERNAL_OLIGO_MIN_SIZE   18
#define INTERNAL_OLIGO_MAX_SIZE   27
#define INTERNAL_OLIGO_OPT_TM     60.0
#define INTERNAL_OLIGO_MIN_TM     57.0
#define INTERNAL_OLIGO_MAX_TM     63.0
#define INTERNAL_OLIGO_MIN_GC     20.0
#define INTERNAL_OLIGO_MAX_GC     80.0
#define INTERNAL_OLIGO_SALT_CONC         50.0
#define INTERNAL_OLIGO_DIVALENT_CONC      0.0
#define INTERNAL_OLIGO_DNTP_CONC          0.0
#define INTERNAL_OLIGO_DNA_CONC          50.0
#define INTERNAL_OLIGO_NUM_NS               0
#define INTERNAL_OLIGO_MAX_POLY_X           5 
#define INTERNAL_OLIGO_SELF_ANY          1200
#define INTERNAL_OLIGO_SELF_END          1200
#define INTERNAL_OLIGO_REPEAT_SIMILARITY 1200
#define REPEAT_SIMILARITY                1200
#define PAIR_REPEAT_SIMILARITY           2400
#define FIRST_BASE_INDEX                    0
#define NUM_RETURN                          5
#define MIN_QUALITY                         0
#define QUALITY_RANGE_MIN                   0
#define QUALITY_RANGE_MAX                 100
#define DEFAULT_MAX_END_STABILITY         100.0

/*
Added by T.Koressaar. Enables design of primers from lowercase masked
template.  A value of 1 directs primer3 to reject primers overlapping
lowercase a base exactly at the 3' end.

This property relies on the assumption that masked features
(e.g. repeats) can partly overlap primer, but they cannot overlap the
3'-end of the primer.  In other words, lowercase bases at other
positions in the primer are accepted, assuming that the masked
features do not influence the primer performance if they do not
overlap the 3'-end of primer.
*/
#define LOWERCASE_MASKING                   0

#define PRIMER_PRODUCT_OPT_SIZE      PR_UNDEFINED_INT_OPT
#define PRIMER_PRODUCT_OPT_TM        PR_UNDEFINED_DBL_OPT
#define MAX_TEMPLATE_MISPRIMING      PR_UNDEFINED_ALIGN_OPT
#define PAIR_MAX_TEMPLATE_MISPRIMING PR_UNDEFINED_ALIGN_OPT
#define IO_MAX_TEMPLATE_MISHYB       PR_UNDEFINED_ALIGN_OPT

#define LIB_AMBIGUITY_CODES_CONSENSUS 1
/*  For backward compatibility. It turns out that
this _not_ what one normally wants, since many
libraries contain strings of N, which then match
every oligo (very bad).
*/

/* Weights for objective functions for oligos and pairs. */
#define PRIMER_WT_TM_GT          1
#define PRIMER_WT_TM_LT          1
#define PRIMER_WT_SIZE_LT        1
#define PRIMER_WT_SIZE_GT        1
#define PRIMER_WT_GC_PERCENT_LT  0
#define PRIMER_WT_GC_PERCENT_GT  0
#define PRIMER_WT_COMPL_ANY      0
#define PRIMER_WT_COMPL_END      0
#define PRIMER_WT_NUM_NS         0
#define PRIMER_WT_REP_SIM        0
#define PRIMER_WT_SEQ_QUAL       0
#define PRIMER_WT_END_QUAL       0
#define PRIMER_WT_POS_PENALTY    1
#define PRIMER_WT_END_STABILITY  0

#define IO_WT_TM_GT          1
#define IO_WT_TM_LT          1
#define IO_WT_SIZE_LT        1
#define IO_WT_SIZE_GT        1
#define IO_WT_GC_PERCENT_LT  0
#define IO_WT_GC_PERCENT_GT  0
#define IO_WT_COMPL_ANY      0
#define IO_WT_COMPL_END      0
#define IO_WT_NUM_NS         0
#define IO_WT_REP_SIM        0
#define IO_WT_SEQ_QUAL       0
#define IO_WT_END_QUAL       0

#define PAIR_WT_PRIMER_PENALTY      1
#define PAIR_WT_IO_PENALTY          0
#define PAIR_WT_DIFF_TM             0
#define PAIR_WT_COMPL_ANY           0
#define PAIR_WT_COMPL_END           0
#define PAIR_WT_REP_SIM             0
#define PAIR_WT_PRODUCT_TM_LT       0
#define PAIR_WT_PRODUCT_TM_GT       0
#define PAIR_WT_PRODUCT_SIZE_LT     0
#define PAIR_WT_PRODUCT_SIZE_GT     0

#ifdef __cplusplus
extern "C" { /* } */
#endif

    p3retval* runPrimer3(p3_global_settings*, seq_args*, int* cancelFlag, int* progress);
#ifdef __cplusplus
    /* { */ }
#endif

#endif

