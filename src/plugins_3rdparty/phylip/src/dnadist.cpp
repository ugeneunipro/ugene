#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

//#include "phylip.h"
//#include "seq.h"
#include "dnadist.h"
/* version 3.696.
Written by Joseph Felsenstein, Akiko Fuseki, Sean Lamont, and Andrew Keeffe.

Copyright (c) 1993-2014, Joseph Felsenstein
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include <U2Algorithm/CreatePhyTreeSettings.h>
#include <U2Core/Task.h>

QString DNADistModelTypes::F84("F84");
QString DNADistModelTypes::Kimura("Kimura");
QString DNADistModelTypes::JukesCantor("Jukes-Cantor");
QString DNADistModelTypes::LogDet("LogDet");
static U2::CreatePhyTreeSettings dnaDistSettings;

void U2::setDNADistSettings( const CreatePhyTreeSettings& settings )
{
    dnaDistSettings = settings;

}

const U2::CreatePhyTreeSettings& getDNADistSettings()
{
    //TODO: use mutex here later
    return dnaDistSettings;
}



Phylip_Char infilename[FNMLNGTH], outfilename[FNMLNGTH], catfilename[FNMLNGTH], weightfilename[FNMLNGTH];
long sites, categs, weightsum, datasets, ith, rcategs;
boolean freqsfrom, jukes, kimura, logdet, gama, invar, similarity, lower, f84,
weights, progress, ctgry, mulsets, justwts, firstset, baddists;
boolean matrix_flags;       /* Matrix output format */
node **nodep = NULL;
double xi, xv, ttratio, ttratio0, freqa, freqc, freqg, freqt, freqr, freqy,
freqar, freqcy, freqgr, freqty, cvi, invarfrac, sumrates, fracchange;
steptr oldweight = NULL;
double rate[maxcategs];
double **d = NULL;
double sumweightrat = 0;                  /* these values were propagated  */
double *weightrat = NULL;                    /* to global values from         */
valrec tbl[maxcategs];                /* function makedists.           */



QList<QString> DNADistModelTypes::getDNADistModelTypes()
{
    static QList<QString> list;
    if (list.isEmpty()) {
        list.append(DNADistModelTypes::F84);
        list.append(DNADistModelTypes::Kimura);
        list.append(DNADistModelTypes::JukesCantor);
        list.append(DNADistModelTypes::LogDet);
    }

    return list;
}


void getoptions()
{
  /* interactively set options */
  long loopcount, loopcount2;
  Phylip_Char ch, ch2;
  boolean ttr;
  char *str = NULL;

  ctgry = false;
  categs = 1;
  cvi = 1.0;
  rcategs = 1;
  rate[0] = 1.0;
  freqsfrom = true;
  gama = false;
  invar = false;
  invarfrac = 0.0;
  jukes = false;
  justwts = false;
  kimura = false;
  logdet = false;
  f84 = true;
  lower = false;
  matrix_flags = MAT_MACHINE;
  similarity = false;
  ttratio = 2.0;
  ttr = false;
  weights = false;
  printdata = false;
  dotdiff = true;
  progress = false;
  interleaved = true;
  loopcount = 0;

  QString matrixModel = getDNADistSettings().matrixId;
  if (matrixModel == DNADistModelTypes::F84) {
      f84 = true;
      kimura = false;
      jukes = false;
      freqsfrom = true;
      logdet = false;
      ttratio = getDNADistSettings().ttRatio;
      ttr = true;
  } else if (matrixModel == DNADistModelTypes::Kimura) {
      f84 = false;
      kimura = true;
      jukes = false;
      freqsfrom = false;
      logdet = false;
      ttratio = getDNADistSettings().ttRatio;
      ttr = true;
  } else if (matrixModel == DNADistModelTypes::JukesCantor) {
      f84 = false;
      kimura = false;
      jukes = true;
      freqsfrom = false;
      logdet = false;
  } else if (matrixModel == DNADistModelTypes::LogDet) {
      f84 = false;
      kimura = false;
      jukes = false;
      freqsfrom = false;
      logdet = true;
  }
  
  gama = getDNADistSettings().useGammaDistributionRates;
  if (gama) {
      cvi = getDNADistSettings().alphaFactor;
      cvi = 1.0 / (cvi * cvi);
  }
  
  /*for (;;) {
    cleerhome();
    printf("\nNucleic acid sequence Distance Matrix program,");
    printf(" version %s\n\n",PHY_VERSION);
    printf("Settings for this run:\n");
    printf("  D  Distance (F84, Kimura, Jukes-Cantor, LogDet)?  %s\n",
           kimura ? "Kimura 2-parameter" :
           jukes  ? "Jukes-Cantor"       :
           logdet ? "LogDet"             :
           similarity ? "Similarity table" : "F84");
    if (kimura || f84 || jukes) {
      printf("  G          Gamma distributed rates across sites?  ");
      if (gama)
        printf("Yes\n");
      else {
        if (invar)
          printf("Gamma+Invariant\n");
        else
          printf("No\n");
      }
    }
    if (kimura || f84) {
      printf("  T                 Transition/transversion ratio?");
      if (!ttr)
        printf("  2.0\n");
      else
        printf("%8.4f\n", ttratio);
    }
    if (!logdet && !similarity && !gama && !invar) {
      printf("  C            One category of substitution rates?");
      if (!ctgry || categs == 1)
        printf("  Yes\n");
      else
        printf("  %ld categories\n", categs);
    }
    printf("  W                         Use weights for sites?");
    if (weights)
      printf("  Yes\n");
    else
      printf("  No\n");
    if (f84)
      printf("  F                Use empirical base frequencies?  %s\n",
             (freqsfrom ? "Yes" : "No"));
    printf("  L                       Form of distance matrix?  ");
    switch (matrix_flags) {
      case MAT_MACHINE:
        str = "Square";
        break;
      case MAT_LOWERTRI:
        str = "Lower-triangular";
        break;
      case MAT_HUMAN:
        str = "Human-readable";
        break;
      default:  
        assert( 0 );
        str = "(unknown)";
    }
    puts(str);
    printf("  M                    Analyze multiple data sets?");
    if (mulsets)
      printf("  Yes, %2ld %s\n", datasets,
               (justwts ? "sets of weights" : "data sets"));
    else
      printf("  No\n");
    printf("  I                   Input sequences interleaved?  %s\n",
           (interleaved ? "Yes" : "No, sequential"));
    printf("  0            Terminal type (IBM PC, ANSI, none)?  %s\n",
           ibmpc ? "IBM PC" : ansi  ? "ANSI"   : "(none)");
    printf("  1             Print out the data at start of run  %s\n",
           (printdata ? "Yes" : "No"));
    printf("  2           Print indications of progress of run  %s\n",
           (progress ? "Yes" : "No"));
    printf("\n  Y to accept these or type the letter for one to change\n");
#ifdef WIN32
    phyFillScreenColor();
#endif
    fflush(stdout);
    scanf("%c%*[^\n]", &ch);
    getchar();
    uppercase(&ch);
    if  (ch == 'Y')
           break;
    if ((f84 && (strchr("CFGWLDTMI012",ch) != NULL)) ||
        (kimura && (strchr("CGWLDTMI012",ch) != NULL)) ||
        (jukes && (strchr("CGWLDMI012",ch) != NULL)) ||
        ((logdet || similarity) && (strchr("WLDMI012",ch)) != NULL) ||
        (ctgry && (strchr("CFWLDTMI012",ch) != NULL))) {
     switch (ch) {

     case 'D':
       if (kimura) {
         kimura = false;
         jukes = true;
         freqsfrom = false;
       } else if (f84) {
         f84 = false;
         kimura = true;
         freqsfrom = false;
       } else if (logdet) {
         logdet = false;
         similarity = true;
       } else if (similarity) {
         similarity = false;
         f84 = true;
         freqsfrom = true;
       } else {
         jukes = false;
         logdet = true;
         freqsfrom = false;
       }
       break;

     case 'G':
       if (!(gama || invar))
         gama = true;
       else {
         if (gama) {
           gama = false;
           invar = true;
         } else {
           if (invar)
             invar = false;
         }
       }
       break;


     case 'C':
       ctgry = !ctgry;
       if (ctgry) {
         initcatn(&categs);
         initcategs(categs, rate);
       }
       break;

     case 'F':
       freqsfrom = !freqsfrom;
       if (!freqsfrom)
         initfreqs(&freqa, &freqc, &freqg, &freqt);
       break;

     case 'W':
       weights = !weights;
       break;

     case 'L':
       switch ( matrix_flags ) {
         case MAT_HUMAN:
           matrix_flags = MAT_MACHINE;
           break;
         case MAT_LOWERTRI:
           matrix_flags = MAT_HUMAN;
           break;
         case MAT_MACHINE:
           matrix_flags = MAT_LOWERTRI;
           break;
         default:
           assert(0);
           matrix_flags = MAT_HUMAN;
       }
       break;

     case 'T':
       ttr = !ttr;
       if (ttr)
        initratio(&ttratio);
       break;

     case 'M':
       mulsets = !mulsets;
       if (mulsets) {
          printf("Multiple data sets or multiple weights?");
          loopcount2 = 0;
          do {
            printf(" (type D or W)\n");
#ifdef WIN32
            phyFillScreenColor();
#endif
            fflush(stdout);
            scanf("%c%*[^\n]", &ch2);
            uppercase(&ch2);
            getchar();
            countup(&loopcount2, 10);
          } while ((ch2 != 'W') && (ch2 != 'D'));
          justwts = (ch2 == 'W');
          if (justwts)
            justweights(&datasets);
          else
            initdatasets(&datasets);
        }
       break;

     case 'I':
       interleaved = !interleaved;
       break;

     case '0':
        initterminal(&ibmpc, &ansi);
       break;

     case '1':
       printdata = !printdata;
       break;

     case '2':
       progress = !progress;
       break;
     }
   } else {
      if (strchr("CFGWLDTMI012",ch) == NULL)
        printf("Not a possible option!\n");
      else
        printf("That option not allowed with these settings\n");
      printf("\nPress Enter or Return key to continue\n");
      getchar();
    } 
    //countup(&loopcount, 100);
  }

  if (similarity) {
    if (matrix_flags == MAT_MACHINE)
      matrix_flags = MAT_HUMAN;
    else if (matrix_flags == MAT_LOWERTRI)
      matrix_flags = MAT_LOWER | MAT_HUMAN;
  }

  if (gama || invar) {
    loopcount = 0;
    do {
      printf(
"\nCoefficient of variation of substitution rate among sites (must be positive)\n");
      printf(
  " In gamma distribution parameters, this is 1/(square root of alpha)\n");
#ifdef WIN32
      phyFillScreenColor();
#endif
      fflush(stdout);
      scanf("%lf%*[^\n]", &cvi);
      getchar();
      countup(&loopcount, 10);
    } while (cvi <= 0.0);
    cvi = 1.0 / (cvi * cvi);
  }
  if (invar) {
    loopcount = 0;
    do {
      printf("Fraction of invariant sites?\n");
      fflush(stdout);
      scanf("%lf%*[^\n]", &invarfrac);
      getchar();
      countup (&loopcount, 10);
    } while ((invarfrac <= 0.0) || (invarfrac >= 1.0));
  }
    if (!printdata)
    return;
    fprintf(outfile, "\nNucleic acid sequence Distance Matrix program,");
    fprintf(outfile, " version %s\n\n",PHY_VERSION); */
}  /* getoptions */


void allocrest(U2::MemoryLocker& memLocker)
{
    long i;

    qint64 memSize = spp * (sizeof(Phylip_Char *) + sizeof(node *) + sites*sizeof(Phylip_Char) + sizeof(node) + sizeof(double *) + spp*sizeof(double) + sizeof(naym));
    memSize += sites * sizeof(long) * 7;
    if(!memLocker.tryAcquire(memSize)) {
          return;
    }

    y = (Phylip_Char **)Malloc(spp*sizeof(Phylip_Char *));
    nodep = (node **)Malloc(spp*sizeof(node *));
    for (i = 0; i < spp; i++) {
        y[i] = (Phylip_Char *)Malloc(sites*sizeof(Phylip_Char));
        nodep[i] = (node *)Malloc(sizeof(node));
    }
    d = (double **)Malloc(spp*sizeof(double *));
    for (i = 0; i < spp; i++)
        d[i] = (double*)Malloc(spp*sizeof(double));
    nayme = (naym *)Malloc(spp*sizeof(naym));
    category = (steptr)Malloc(sites*sizeof(long));
    oldweight = (steptr)Malloc(sites*sizeof(long));
    weight = (steptr)Malloc(sites*sizeof(long));
    alias = (steptr)Malloc(sites*sizeof(long));
    ally = (steptr)Malloc(sites*sizeof(long));
    location = (steptr)Malloc(sites*sizeof(long));
    weightrat = (double *)Malloc(sites*sizeof(double));
} /* allocrest */


void reallocsites() 
{/* The amount of sites can change between runs 
     this function reallocates all the variables 
     whose size depends on the amount of sites */
  long i;

  for (i = 0; i < spp; i++) {
    free(y[i]);  
    y[i] = (Phylip_Char *)Malloc(sites*sizeof(Phylip_Char));
  }
  free(category);
  free(oldweight);
  free(weight);
  free(alias);
  free(ally);
  free(location);
  free(weightrat);
  
  category = (steptr)Malloc(sites*sizeof(long));
  oldweight = (steptr)Malloc(sites*sizeof(long));
  weight = (steptr)Malloc(sites*sizeof(long));
  alias = (steptr)Malloc(sites*sizeof(long));
  ally = (steptr)Malloc(sites*sizeof(long));
  location = (steptr)Malloc(sites*sizeof(long));
  weightrat = (double *)Malloc(sites*sizeof(double));  
} /* reallocsites */


void doinit(U2::MemoryLocker& memLocker)
{
    /* initializes variables */

    //inputnumbers(&spp, &sites, &nonodes, 1);
    getoptions();
    if (printdata) {
        fprintf(outfile, "%2ld species, %3ld  sites\n", spp, sites);
    }
    allocrest(memLocker);
}  /* doinit */


void inputcategories()
{
  /* reads the categories for each site */
  long i;
  Phylip_Char ch;

  for (i = 1; i < nmlngth; i++)
    gettc(infile);
  for (i = 0; i < sites; i++) {
    do {
      if (eoln(infile))
        scan_eoln(infile);
      ch = gettc(infile);
    } while (ch == ' ');
    category[i] = ch - '0';
  }
  scan_eoln(infile);
}  /* inputcategories */


void printcategories()
{ /* print out list of categories of sites */
  long i, j;

  fprintf(outfile, "Rate categories\n\n");
  for (i = 1; i <= nmlngth + 3; i++)
    putc(' ', outfile);
  for (i = 1; i <= sites; i++) {
    fprintf(outfile, "%ld", category[i - 1]);
    if (i % 60 == 0) {
      putc('\n', outfile);
      for (j = 1; j <= nmlngth + 3; j++)
        putc(' ', outfile);
    } else if (i % 10 == 0)
      putc(' ', outfile);
  }
  fprintf(outfile, "\n\n");
}  /* printcategories */


void inputoptions()
{
  /* read options information */
  long i;

  if (!firstset && !justwts) {
    samenumsp(&sites, ith);
    reallocsites();
  }
  for (i = 0; i < sites; i++) {
    category[i] = 1;
    oldweight[i] = 1;
  }
  if (justwts || weights)
    inputweights(sites, oldweight, &weights);
  if (printdata)
    putc('\n', outfile);
  if (jukes && printdata)
    fprintf(outfile, "  Jukes-Cantor Distance\n");
  if (kimura && printdata)
    fprintf(outfile, "  Kimura 2-parameter Distance\n");
  if (f84 && printdata)
    fprintf(outfile, "  F84 Distance\n");
  if (similarity)
    fprintf(outfile, "  \n  Table of similarity between sequences\n");
  if (firstset && printdata && (kimura || f84))
    fprintf(outfile, "\nTransition/transversion ratio = %10.6f\n", ttratio);
  if (ctgry && categs > 1) {
    inputcategs(0, sites, category, categs, "DnaDist");
    if (printdata)
      printcategs(outfile, sites, category, "Site categories");
  } else if (printdata && (categs > 1)) {
    fprintf(outfile, "\nSite category   Rate of change\n\n");
    for (i = 1; i <= categs; i++)
      fprintf(outfile, "%12ld%13.3f\n", i, rate[i - 1]);
    putc('\n', outfile);
    printcategories();
  }
  if ((jukes || kimura || logdet) && freqsfrom) {
    printf(" WARNING: CANNOT USE EMPIRICAL BASE FREQUENCIES");
    printf(" WITH JUKES-CANTOR, KIMURA, JIN/NEI OR LOGDET DISTANCES\n");
    exxit(-1);
  }
  if (jukes)
    ttratio = 0.5000001;
  if (weights && printdata)
    printweights(outfile, 0, sites, oldweight, "Sites");
}  /* inputoptions */


void dnadist_sitesort()
{
  /* Shell sort of sites lexicographically */
  long gap, i, j, jj, jg, k, itemp;
  boolean flip, tied;

  gap = sites / 2;
  while (gap > 0) {
    for (i = gap + 1; i <= sites; i++) {
      j = i - gap;
      flip = true;
      while (j > 0 && flip) {
        jj = alias[j - 1];
        jg = alias[j + gap - 1];
        tied = (oldweight[jj - 1] == oldweight[jg - 1]);
        flip = (oldweight[jj - 1] < oldweight[jg - 1] ||
                (tied && category[jj - 1] > category[jg - 1]));
        tied = (tied && category[jj - 1] == category[jg - 1]);
        k = 1;
        while (k <= spp && tied) {
          flip = (y[k - 1][jj - 1] > y[k - 1][jg - 1]);
          tied = (tied && y[k - 1][jj - 1] == y[k - 1][jg - 1]);
          k++;
        }
        if (!flip)
          break;
        itemp = alias[j - 1];
        alias[j - 1] = alias[j + gap - 1];
        alias[j + gap - 1] = itemp;
        j -= gap;
      }
    }
    gap /= 2;
  }
}  /* dnadist_sitesort */


void dnadist_sitecombine()
{
  /* combine sites that have identical patterns */
  long i, j, k;
  boolean tied;

  i = 1;
  while (i < sites) {
    j = i + 1;
    tied = true;
    while (j <= sites && tied) {
      tied = (category[alias[i - 1] - 1] == category[alias[j - 1] - 1]);
      k = 1;
      while (k <= spp && tied) {
        tied = (tied &&
            y[k - 1][alias[i - 1] - 1] == y[k - 1][alias[j - 1] - 1]);
        k++;
      }
      if (!tied)
        break;
      ally[alias[j - 1] - 1] = alias[i - 1];
      j++;
    }
    i = j;
  }
}  /* dnadist_sitecombine */


void dnadist_sitescrunch()
{
  /* move so one representative of each pattern of
     sites comes first */
  long i, j, itemp;
  boolean done, found, completed;

  done = false;
  i = 1;
  j = 2;
  while (!done) {
    if (ally[alias[i - 1] - 1] != alias[i - 1]) {
      if (j <= i)
        j = i + 1;
      if (j <= sites) {
        do {
          found = (ally[alias[j - 1] - 1] == alias[j - 1]);
          j++;
          completed = (j > sites);
        } while (!(found || completed));
        if (found) {
          j--;
          itemp = alias[i - 1];
          alias[i - 1] = alias[j - 1];
          alias[j - 1] = itemp;
        } else
          done = true;
      } else
        done = true;
    }
    i++;
    done = (done || i >= sites);
  }
}  /* dnadist_sitescrunch */


void makeweights()
{
  /* make up weights vector to avoid duplicate computations */
  long i;

  for (i = 1; i <= sites; i++) {
    alias[i - 1] = i;
    ally[i - 1] = i;
    location[i - 1] = 0;
    weight[i - 1] = 0;
  }
  dnadist_sitesort();
  dnadist_sitecombine();
  dnadist_sitescrunch();
  endsite = 0;
  for (i = 1; i <= sites; i++) {
    if (ally[i - 1] == i)
      endsite++;
  }
  for (i = 1; i <= endsite; i++)
    location[alias[i - 1] - 1] = i;
  weightsum = 0;
  for (i = 0; i < sites; i++)
    weightsum += oldweight[i];
  sumrates = 0.0;
  for (i = 0; i < sites; i++)
    sumrates += oldweight[i] * rate[category[i] - 1];
  for (i = 0; i < categs; i++)
    rate[i] *= weightsum / sumrates;
  for (i = 0; i < sites; i++)
    if (location[ally[i] - 1] > 0)
      weight[location[ally[i] - 1] - 1] += oldweight[i];
}  /* makeweights */


void dnadist_makevalues()
{
  /* set up fractional likelihoods at tips */
  long i, j, k;
  bases b;

  for (i = 0; i < spp; i++) {
    nodep[i]->x = (phenotype)Malloc(endsite*sizeof(ratelike));
    for (j = 0; j < endsite; j++)
      nodep[i]->x[j]  = (ratelike)Malloc(rcategs*sizeof(sitelike));
  }
  for (k = 0; k < endsite; k++) {
    j = alias[k];
    for (i = 0; i < spp; i++) {
      for (b = A; (long)b <= (long)T; b = (bases)((long)b + 1))
        nodep[i]->x[k][0][(long)b - (long)A] = 0.0;
      switch (y[i][j - 1]) {

      case 'A':
        nodep[i]->x[k][0][0] = 1.0;
        break;

      case 'C':
        nodep[i]->x[k][0][(long)C - (long)A] = 1.0;
        break;

      case 'G':
        nodep[i]->x[k][0][(long)G - (long)A] = 1.0;
        break;

      case 'T':
        nodep[i]->x[k][0][(long)T - (long)A] = 1.0;
        break;

      case 'U':
        nodep[i]->x[k][0][(long)T - (long)A] = 1.0;
        break;

      case 'M':
        nodep[i]->x[k][0][0] = 1.0;
        nodep[i]->x[k][0][(long)C - (long)A] = 1.0;
        break;

      case 'R':
        nodep[i]->x[k][0][0] = 1.0;
        nodep[i]->x[k][0][(long)G - (long)A] = 1.0;
        break;

      case 'W':
        nodep[i]->x[k][0][0] = 1.0;
        nodep[i]->x[k][0][(long)T - (long)A] = 1.0;
        break;

      case 'S':
        nodep[i]->x[k][0][(long)C - (long)A] = 1.0;
        nodep[i]->x[k][0][(long)G - (long)A] = 1.0;
        break;

      case 'Y':
        nodep[i]->x[k][0][(long)C - (long)A] = 1.0;
        nodep[i]->x[k][0][(long)T - (long)A] = 1.0;
        break;

      case 'K':
        nodep[i]->x[k][0][(long)G - (long)A] = 1.0;
        nodep[i]->x[k][0][(long)T - (long)A] = 1.0;
        break;

      case 'B':
        nodep[i]->x[k][0][(long)C - (long)A] = 1.0;
        nodep[i]->x[k][0][(long)G - (long)A] = 1.0;
        nodep[i]->x[k][0][(long)T - (long)A] = 1.0;
        break;

      case 'D':
        nodep[i]->x[k][0][0] = 1.0;
        nodep[i]->x[k][0][(long)G - (long)A] = 1.0;
        nodep[i]->x[k][0][(long)T - (long)A] = 1.0;
        break;

      case 'H':
        nodep[i]->x[k][0][0] = 1.0;
        nodep[i]->x[k][0][(long)C - (long)A] = 1.0;
        nodep[i]->x[k][0][(long)T - (long)A] = 1.0;
        break;

      case 'V':
        nodep[i]->x[k][0][0] = 1.0;
        nodep[i]->x[k][0][(long)C - (long)A] = 1.0;
        nodep[i]->x[k][0][(long)G - (long)A] = 1.0;
        break;

      case 'N':
        for (b = A; (long)b <= (long)T; b = (bases)((long)b + 1))
          nodep[i]->x[k][0][(long)b - (long)A] = 1.0;
        break;

      case 'X':
        for (b = A; (long)b <= (long)T; b = (bases)((long)b + 1))
          nodep[i]->x[k][0][(long)b - (long)A] = 1.0;
        break;

      case '?':
        for (b = A; (long)b <= (long)T; b = (bases)((long)b + 1))
          nodep[i]->x[k][0][(long)b - (long)A] = 1.0;
        break;

      case 'O':
        for (b = A; (long)b <= (long)T; b = (bases)((long)b + 1))
          nodep[i]->x[k][0][(long)b - (long)A] = 1.0;
        break;

      case '-':
        for (b = A; (long)b <= (long)T; b = (bases)((long)b + 1))
          nodep[i]->x[k][0][(long)b - (long)A] = 1.0;
        break;
      }
    }
  }
}  /* dnadist_makevalues */


void dnadist_empiricalfreqs()
{
  /* Get empirical base frequencies from the data */
  long i, j, k;
  double sum, suma, sumc, sumg, sumt, w;

  freqa = 0.25;
  freqc = 0.25;
  freqg = 0.25;
  freqt = 0.25;
  for (k = 1; k <= 8; k++) {
    suma = 0.0;
    sumc = 0.0;
    sumg = 0.0;
    sumt = 0.0;
    for (i = 0; i < spp; i++) {
      for (j = 0; j < endsite; j++) {
        w = weight[j];
        sum = freqa * nodep[i]->x[j][0][0];
        sum += freqc * nodep[i]->x[j][0][(long)C - (long)A];
        sum += freqg * nodep[i]->x[j][0][(long)G - (long)A];
        sum += freqt * nodep[i]->x[j][0][(long)T - (long)A];
        suma += w * freqa * nodep[i]->x[j][0][0] / sum;
        sumc += w * freqc * nodep[i]->x[j][0][(long)C - (long)A] / sum;
        sumg += w * freqg * nodep[i]->x[j][0][(long)G - (long)A] / sum;
        sumt += w * freqt * nodep[i]->x[j][0][(long)T - (long)A] / sum;
      }
    }
    sum = suma + sumc + sumg + sumt;
    freqa = suma / sum;
    freqc = sumc / sum;
    freqg = sumg / sum;
    freqt = sumt / sum;
  }
}  /* dnadist_empiricalfreqs */


void getinput()
{
  /* reads the input data */
  inputoptions();
  if ((!freqsfrom) && !logdet && !similarity) {
    if (kimura || jukes) {
      freqa = 0.25;
      freqc = 0.25;
      freqg = 0.25;
      freqt = 0.25;
    }
    getbasefreqs(freqa, freqc, freqg, freqt, &freqr, &freqy, &freqar, &freqcy,
                   &freqgr, &freqty, &ttratio, &xi, &xv, &fracchange,
                   freqsfrom, printdata);
    if (freqa < 0.00000001) {
      freqa = 0.000001;
      freqc = 0.999999*freqc;
      freqg = 0.999999*freqg;
      freqt = 0.999999*freqt;
    }
    if (freqc < 0.00000001) {
      freqa = 0.999999*freqa;
      freqc = 0.000001;
      freqg = 0.999999*freqg;
      freqt = 0.999999*freqt;
    }
    if (freqg < 0.00000001) {
      freqa = 0.999999*freqa;
      freqc = 0.999999*freqc;
      freqg = 0.000001;
      freqt = 0.999999*freqt;
    }
    if (freqt < 0.00000001) {
      freqa = 0.999999*freqa;
      freqc = 0.999999*freqc;
      freqg = 0.999999*freqg;
      freqt = 0.000001;
    }
  }
  if (!justwts || firstset)
    inputdata(sites);
  makeweights();
  dnadist_makevalues();
  if (freqsfrom) {
    dnadist_empiricalfreqs();
    getbasefreqs(freqa, freqc, freqg, freqt, &freqr, &freqy, &freqar, &freqcy,
                   &freqgr, &freqty, &ttratio, &xi, &xv, &fracchange,
                   freqsfrom, printdata);
  }
}  /* getinput */


void inittable()
{
  /* Define a lookup table. Precompute values and store in a table */
  long i;

  for (i = 0; i < categs; i++) {
    tbl[i].rat = rate[i];
    tbl[i].ratxv = rate[i] * xv;
  }
}  /* inittable */


double lndet(double (*a)[4])
{
  long i, j, k;
  double temp, ld;

  /*Gauss-Jordan reduction -- invert matrix a in place,
         overwriting previous contents of a.  On exit, matrix a
         contains the inverse, lndet contains the log of the determinant */
  ld = 1.0;
  for (i = 0; i < 4; i++) {
    ld *= a[i][i];
    temp = 1.0 / a[i][i];
    a[i][i] = 1.0;
    for (j = 0; j < 4; j++)
      a[i][j] *= temp;
    for (j = 0; j < 4; j++) {
      if (j != i) {
        temp = a[j][i];
        a[j][i] = 0.0;
        for (k = 0; k < 4; k++)
          a[j][k] -= temp * a[i][k];
      }
    }
  }
  if (ld <= 0.0)
    return(99.0);
  else
    return(log(ld));
}  /* lndet */


void makev(long m, long n, double *v)
{
  /* compute one distance */
  long i, j, k, l, it, num1, num2, idx;
  long numerator = 0, denominator = 0;
  double sum, sum1, sum2, sumyr, lz, aa, bb, cc, vv=0,
         p1, p2, p3, q1, q2, q3, tt, delta = 0, slope,
         xx1freqa, xx1freqc, xx1freqg, xx1freqt;
  double *prod, *prod2, *prod3;
  boolean quick, jukesquick, kimquick, logdetquick, overlap;
  bases b;
  node *p, *q;
  sitelike xx1, xx2;
  double basetable[4][4];  /* for quick logdet */
  double basefreq1[4], basefreq2[4];

  p = nodep[m - 1];
  q = nodep[n - 1];

  /* check for overlap between sequences */
  overlap = false;
  for(i=0 ; i < sites ; i++){
    if((strchr("NX?O-",y[m-1][i])==NULL) && 
       (strchr("NX?O-",y[n-1][i])==NULL)){
      overlap = true;
      break;
    }
  }
  if(!overlap){
    printf("\nWARNING: NO OVERLAP BETWEEN SEQUENCES %ld AND %ld; -1.0 WAS WRITTEN\n", m, n);
    baddists = true;
    return;
  }

  quick = (!ctgry || categs == 1);
  if (jukes || kimura || logdet || similarity) {
    numerator = 0;
    denominator = 0;
    for (i = 0; i < endsite; i++) {
      memcpy(xx1, p->x[i][0], sizeof(sitelike));
      memcpy(xx2, q->x[i][0], sizeof(sitelike));
      sum = 0.0;
      sum1 = 0.0;
      sum2 = 0.0;
      for (b = A; (long)b <= (long)T; b = (bases)((long)b + 1)) {
        sum1 += xx1[(long)b - (long)A];
        sum2 += xx2[(long)b - (long)A];
        sum += xx1[(long)b - (long)A] * xx2[(long)b - (long)A];
      }
      quick = (quick && (sum1 == 1.0 || sum1 == 4.0) &&
               (sum2 == 1.0 || sum2 == 4.0));
      if (sum1 == 1.0 && sum2 == 1.0) {
        numerator += (long)(weight[i] * sum);
        denominator += weight[i];
      }
    }
  }
  jukesquick = ((jukes || similarity) && quick);
  kimquick = (kimura && quick);
  logdetquick = (logdet && quick);
  if (logdet && !quick) {
    printf(" WARNING: CANNOT CALCULATE LOGDET DISTANCE\n");
    printf("  WITH PRESENT PROGRAM IF PARTIALLY AMBIGUOUS NUCLEOTIDES\n");
    printf("  -1.0 WAS WRITTEN\n");
    baddists = true;
  }
  if (jukesquick && jukes && (numerator * 4 <= denominator)) {
    printf("\nWARNING: INFINITE DISTANCE BETWEEN ");
    printf(" SPECIES %3ld AND %3ld\n", m, n);
    printf("  -1.0 WAS WRITTEN\n");
    baddists = true;
  }
  if (jukesquick && invar
      && (4 * (((double)numerator / denominator) - invarfrac)
          <= (1.0 - invarfrac))) {
    printf("\nWARNING: DIFFERENCE BETWEEN SPECIES %3ld AND %3ld", m, n);
    printf(" TOO LARGE FOR INVARIABLE SITES\n");
    printf("  -1.0 WAS WRITTEN\n");
    baddists = true;
  }
  if (jukesquick) {
    if (!gama && !invar)
      vv = -0.75 * log((4.0*((double)numerator / denominator) - 1.0) / 3.0);
    else if (!invar)
        vv = 0.75 * cvi * (exp(-(1/cvi)*
           log((4.0 * ((double)numerator / denominator) - 1.0) / 3.0)) - 1.0);
      else
        vv = 0.75 * cvi * (exp(-(1/cvi)*
           log((4.0 * ((double)numerator / denominator - invarfrac)/
                        (1.0-invarfrac) - 1.0) / 3.0)) - 1.0);
  }
  if (kimquick) {
    num1 = 0;
    num2 = 0;
    denominator = 0;
    for (i = 0; i < endsite; i++) {
      memcpy(xx1, p->x[i][0], sizeof(sitelike));
      memcpy(xx2, q->x[i][0], sizeof(sitelike));
      sum = 0.0;
      sum1 = 0.0;
      sum2 = 0.0;
      for (b = A; (long)b <= (long)T; b = (bases)((long)b + 1)) {
        sum1 += xx1[(long)b - (long)A];
        sum2 += xx2[(long)b - (long)A];
        sum += xx1[(long)b - (long)A] * xx2[(long)b - (long)A];
      }
      sumyr = (xx1[0] + xx1[(long)G - (long)A])
            * (xx2[0] + xx2[(long)G - (long)A]) +
              (xx1[(long)C - (long)A] + xx1[(long)T - (long)A]) *
              (xx2[(long)C - (long)A] + xx2[(long)T - (long)A]);
      if (sum1 == 1.0 && sum2 == 1.0) {
        num1 += (long)(weight[i] * sum);
        num2 += (long)(weight[i] * (sumyr - sum));
        denominator += weight[i];
      }
    }
    tt = ((1.0 - (double)num1 / denominator)-invarfrac)/(1.0-invarfrac);
    if (tt > 0.0) {
      delta = 0.1;
      tt = delta;
      it = 0;
      while (fabs(delta) > 0.0000002 && it < iterationsd) {
        it++;
        if (!gama) {
          p1 = exp(-tt);
          p2 = exp(-xv * tt) - exp(-tt);
          p3 = 1.0 - exp(-xv * tt);
        } else {
          p1 = exp(-cvi * log(1 + tt / cvi));
          p2 = exp(-cvi * log(1 + xv * tt / cvi))
              - exp(-cvi * log(1 + tt / cvi));
          p3 = 1.0 - exp(-cvi * log(1 + xv * tt / cvi));
        }
        q1 = p1 + p2 / 2.0 + p3 / 4.0;
        q2 = p2 / 2.0 + p3 / 4.0;
        q3 = p3 / 2.0;
        q1 = q1 * (1.0-invarfrac) + invarfrac;
        q2 *= (1.0 - invarfrac);
        q3 *= (1.0 - invarfrac);
        if (!gama && !invar)
          slope = 0.5 * exp(-tt) * (num2 / q2 - num1 / q1) +
                  0.25 * xv * exp(-xv * tt) *
                 ((denominator - num1 - num2) * 2 / q3 - num2 / q2 - num1 / q1);
        else
          slope = 0.5 * (1 / (1 + tt / cvi)) * exp(-cvi * log(1 + tt / cvi)) *
                  (num2 / q2 - num1 / q1) + 0.25 * (xv / (1 + xv * tt / cvi)) *
                    exp(-cvi * log(1 + xv * tt / cvi)) *
                 ((denominator - num1 - num2) * 2 / q3 - num2 / q2 - num1 / q1);
        slope *= (1.0-invarfrac);
        if (slope < 0.0)
          delta = fabs(delta) / -2.0;
        else
          delta = fabs(delta);
        tt += delta;
      }
    }
    if ((delta >= 0.1) && (!similarity)) {
      printf("\nWARNING: DIFFERENCE BETWEEN SPECIES %3ld AND %3ld", m, n);
      if (invar)
        printf(" TOO LARGE FOR INVARIABLE SITES\n");
      else
        printf(" TOO LARGE TO ESTIMATE DISTANCE\n");
      printf("  -1.0 WAS WRITTEN\n");
      baddists = true;
    }
    vv = fracchange * tt;
  }
  if (!(jukesquick || kimquick || logdet)) {
    prod = (double *)Malloc(sites*sizeof(double));
    prod2 = (double *)Malloc(sites*sizeof(double));
    prod3 = (double *)Malloc(sites*sizeof(double));
    for (i = 0; i < endsite; i++) {
      memcpy(xx1, p->x[i][0], sizeof(sitelike));
      memcpy(xx2, q->x[i][0], sizeof(sitelike));
      xx1freqa = xx1[0] * freqa;
      xx1freqc = xx1[(long)C - (long)A] * freqc;
      xx1freqg = xx1[(long)G - (long)A] * freqg;
      xx1freqt = xx1[(long)T - (long)A] * freqt;
      sum1 = xx1freqa + xx1freqc + xx1freqg + xx1freqt;
      sum2 = freqa * xx2[0] + freqc * xx2[(long)C - (long)A] +
             freqg * xx2[(long)G - (long)A] + freqt * xx2[(long)T - (long)A];
      prod[i] = sum1 * sum2;
      prod2[i] = (xx1freqa + xx1freqg) *
                 (xx2[0] * freqar + xx2[(long)G - (long)A] * freqgr) +
          (xx1freqc + xx1freqt) *
          (xx2[(long)C - (long)A] * freqcy + xx2[(long)T - (long)A] * freqty);
      prod3[i] = xx1freqa * xx2[0] + xx1freqc * xx2[(long)C - (long)A] +
         xx1freqg * xx2[(long)G - (long)A] + xx1freqt * xx2[(long)T - (long)A];
    }
    tt = 0.1;
    delta = 0.1;
    it = 1;
    while (it < iterationsd && fabs(delta) > 0.0000002) {
      slope = 0.0;
      if (tt > 0.0) {
        lz = -tt;
        for (i = 0; i < categs; i++) {
          if (!gama) {
            tbl[i].z1 = exp(tbl[i].ratxv * lz);
            tbl[i].z1zz = exp(tbl[i].rat * lz);
          }
          else {
            tbl[i].z1 = exp(-cvi*log(1.0-tbl[i].ratxv * lz/cvi));
            tbl[i].z1zz = exp(-cvi*log(1.0-tbl[i].rat * lz/cvi));
          }
          tbl[i].y1 = 1.0 - tbl[i].z1;
          tbl[i].z1yy = tbl[i].z1 - tbl[i].z1zz;
          tbl[i].z1xv = tbl[i].z1 * xv;
        }
        for (i = 0; i < endsite; i++) {
          idx = category[alias[i] - 1];
          cc = prod[i];
          bb = prod2[i];
          aa = prod3[i];
          if (!gama && !invar)
            slope += weightrat[i] * (tbl[idx - 1].z1zz * (bb - aa) +
                                     tbl[idx - 1].z1xv * (cc - bb)) /
                         (aa * tbl[idx - 1].z1zz + bb * tbl[idx - 1].z1yy +
                          cc * tbl[idx - 1].y1);
          else
            slope += (1.0-invarfrac) * weightrat[i] * (
                    ((tbl[idx-1].rat)/(1.0-tbl[idx-1].rat * lz/cvi))
                       * tbl[idx - 1].z1zz * (bb - aa) +
                    ((tbl[idx-1].ratxv)/(1.0-tbl[idx-1].ratxv * lz/cvi))
                       * tbl[idx - 1].z1 * (cc - bb)) /
                (aa * ((1.0-invarfrac)*tbl[idx - 1].z1zz + invarfrac)
                  + bb * (1.0-invarfrac)*tbl[idx - 1].z1yy
                  + cc * (1.0-invarfrac)*tbl[idx - 1].y1);
        }
      }
      if (slope < 0.0)
        delta = fabs(delta) / -2.0;
      else
        delta = fabs(delta);
      tt += delta;
      it++;
    }
    if ((delta >= 0.1) && (!similarity)) {
      printf("\nWARNING: DIFFERENCE BETWEEN SPECIES %3ld AND %3ld", m, n);
      if (invar)
        printf(" TOO LARGE FOR INVARIABLE SITES\n");
      else
        printf(" TOO LARGE TO ESTIMATE DISTANCE\n");
      printf("  -1.0 WAS WRITTEN\n");
      baddists = true;
    }
    vv = tt * fracchange;
    free(prod);
    free(prod2);
    free(prod3);
  }
  if (logdetquick) {  /* compute logdet when no ambiguous nucleotides */
    for (i = 0; i < 4; i++) {
      basefreq1[i] = 0.0;
      basefreq2[i] = 0.0;
      for (j = 0; j < 4; j++)
        basetable[i][j] = 0.0;
    }
    for (i = 0; i < endsite; i++) {
      k = 0;
      while (p->x[i][0][k] == 0.0)
        k++;
      basefreq1[k] += weight[i];
      l = 0;
      while (q->x[i][0][l] == 0.0)
        l++;
      basefreq2[l] += weight[i];
      basetable[k][l] += weight[i];
    }
    vv = lndet(basetable);
    if (vv == 99.0) {
      printf("\nNegative or zero determinant for distance between species");
      printf(" %ld and %ld\n", m, n);
      printf("  -1.0 WAS WRITTEN\n");
      baddists = true;
    }
    vv = -0.25*(vv - 0.5*(log(basefreq1[0])+log(basefreq1[1])
                                        +log(basefreq1[2])+log(basefreq1[3])
                                        +log(basefreq2[0])+log(basefreq2[1])
                                        +log(basefreq2[2])+log(basefreq2[3])));
  }
  if (similarity) {
    if (denominator < 1.0) {
      printf("\nWARNING: SPECIES %3ld AND %3ld HAVE NO BASES THAT", m, n);
      printf(" CAN BE COMPARED\n");
      printf("  -1.0 WAS WRITTEN\n");
      baddists = true;
  }
    vv = (double)numerator / denominator;
  }
  *v = vv;
}  /* makev */


void makedists()
{
  /* compute distance matrix */
  long i, j;
  double v;

  inittable();
  for (i = 0; i < endsite; i++)
    weightrat[i] = weight[i] * rate[category[alias[i] - 1] - 1];
  if (progress) {
    printf("Distances calculated for species\n");
#ifdef WIN32
    phyFillScreenColor();
#endif
  }
  for (i = 0; i < spp; i++)
    if (similarity)
      d[i][i] = 1.0;
    else
      d[i][i] = 0.0;
  baddists = false;
  
  float cur_prog = 0;
  int total = (spp*spp / 2) + 1;
  float step = (1.0f / total) * 100.0f;

  for (i = 1; i < spp; i++) {
    if (progress) {
      printf("    ");
      for (j = 0; j < nmlngth; j++)
        putchar(nayme[i - 1][j]);
      printf("   ");
    }
    for (j = i + 1; j <= spp; j++) {
      makev(i, j, &v);
      v = fabs(v);     
      if ( baddists == true ) {
        v = -1;
        baddists = false;
      }
      d[i - 1][j - 1] = v;
      d[j - 1][i - 1] = v;
      
      U2::TaskStateInfo* ts = U2::getTaskInfo();
      if (ts->cancelFlag != 0) {
          ugene_exit("Task canceled!");
      } else if(!U2::isBootstr()){
          cur_prog += step;
          ts->progress = int (cur_prog);   
      }
      
      if (progress) {
        putchar('.');
        fflush(stdout);
      }
    }
    if (progress) {
      putchar('\n');
#ifdef WIN32
      phyFillScreenColor();
#endif
    }
  }
  if (progress) {
    printf("    ");
    for (j = 0; j < nmlngth; j++)
      putchar(nayme[spp - 1][j]);
    putchar('\n');
  }
}  /* makedists */


void writedists()
{
  /* write out distances */
  char **names;

  names = stringnames_new();
  output_matrix_d(outfile, d, spp, spp, names, names, matrix_flags);
  stringnames_delete(names);

  if (progress)
    printf("\nDistances written to file \"%s\"\n\n", outfilename);
}  /* writedists */


// int main(int argc, Char *argv[])
// {  /* DNA Distances by Maximum Likelihood */
// #ifdef MAC
//   argc = 1;                /* macsetup("Dnadist","");        */
//   argv[0] = "Dnadist";
// #endif
//   init(argc, argv);
//   openfile(&infile,INFILE,"input file","r",argv[0],infilename);
//   openfile(&outfile,OUTFILE,"output file","w",argv[0],outfilename);
// 
//   ibmpc = IBMCRT;
//   ansi = ANSICRT;
//   mulsets = false;
//   datasets = 1;
//   firstset = true;
//   doinit();
//   ttratio0 = ttratio;
//   if (ctgry)
//     openfile(&catfile,CATFILE,"categories file","r",argv[0],catfilename);
//   if (weights || justwts)
//     openfile(&weightfile,WEIGHTFILE,"weights file","r",argv[0],weightfilename);
//   for (ith = 1; ith <= datasets; ith++) {
//     ttratio = ttratio0;
//     getinput();
//     if (ith == 1)
//       firstset = false;
//     if (datasets > 1 && progress)
//       printf("Data set # %ld:\n\n",ith);
//     makedists();
//     writedists();
//   }
//   FClose(infile);
//   FClose(outfile);
// #ifdef MAC
//   fixmacfile(outfilename);
// #endif
//   printf("Done.\n\n");
// #ifdef WIN32
//   phyRestoreConsoleAttributes();
// #endif
//   return 0;
// }  /* DNA Distances by Maximum Likelihood */




