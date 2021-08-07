#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

#include "phylip.h"
#include "seq.h"
#include "protdist.h"
#include "dnadist.h"

#include <U2Algorithm/CreatePhyTreeSettings.h>
#include <U2Core/Task.h>

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



long chars /*,datasets ith, ctgry, categs*/;
/* spp = number of species
chars = number of positions in actual sequences */
double /*freqa, freqc, freqg, freqt,/ cvi, invarfrac, ttratio, xi, xv */ease /*fracchange*/;
boolean /*weights, justwts, progress, mulsets, gama, invar*,*/ basesequal,
usepmb, usejtt, usepam/*, kimura, similarity, firstset*/;
codetype whichcode;
cattype whichcat;
//steptr oldweight;
//double rate[maxcategs];
aas **gnode;
aas trans[4][4][4];
double pie[20];
long cat[(long)ser - (long)ala + 1], numaa[(long)ser - (long)ala + 1];
double eig[20];
matrix prob, eigvecs;
//double **d;
//char infilename[100], outfilename[100], catfilename[100], weightfilename[100];

/* Local variables for makedists, propagated globally for c version: */
double tt, p, dp, d2p, q, elambdat;

QString ProtDistModelTypes::JTT("Jones-Taylor-Thornton");
QString ProtDistModelTypes::PMB("Henikoff/Tillier PMB");
QString ProtDistModelTypes::PAM("Dayhoff PAM");
QString ProtDistModelTypes::Kimura("Kimura");

QList<QString> ProtDistModelTypes::getProtDistModelTypes()
{
    static QList<QString> list;
    if (list.isEmpty()) {
        list.append(ProtDistModelTypes::JTT);
        list.append(ProtDistModelTypes::PMB);
        list.append(ProtDistModelTypes::PAM);
        list.append(ProtDistModelTypes::Kimura);
    }

    return list;
}

void protdist_uppercase(Phylip_Char *ch)
{
 (*ch) = (isupper(*ch) ? (*ch) : toupper(*ch));
}  /* protdist_uppercase */


void protdist_inputnumbers(U2::MemoryLocker& memLocker)
{
  /* input the numbers of species and of characters */
  long i;

//  fscanf(infile, "%ld%ld", &spp, &chars);

  qint64 memSize = spp * (sizeof(aas *) + chars * sizeof(aas ));
  memSize += chars * sizeof(long) * 3 + spp * (sizeof(double) + sizeof(naym) + spp*sizeof(double));
  if(!memLocker.tryAcquire(memSize)) {
      return;
  }

  if (printdata)
    fprintf(outfile, "%2ld species, %3ld  positions\n\n", spp, chars);
  gnode = (aas **)Malloc(spp * sizeof(aas *));
  if (firstset) {
    for (i = 0; i < spp; i++)
      gnode[i] = (aas *)Malloc(chars * sizeof(aas ));
  }
  weight = (steparray)Malloc(chars*sizeof(long));
  oldweight = (steparray)Malloc(chars*sizeof(long));
  category = (steparray)Malloc(chars*sizeof(long));
  d      = (double **)Malloc(spp*sizeof(double *));
  nayme  = (naym *)Malloc(spp*sizeof(naym));

  for (i = 0; i < spp; ++i)
    d[i] = (double *)Malloc(spp*sizeof(double));
}  /* protdist_inputnumbers */


void prot_getoptions(const QString& matrixModel)
{
  /* interactively set options */
  long loopcount, loopcount2;
  Phylip_Char ch, ch2;
  Phylip_Char in[100];
  boolean done;

  if (printdata)
    fprintf(outfile, "\nProtein distance algorithm, version %s\n\n",PHY_VERSION);
  putchar('\n');
  weights = false;
  printdata = false;
  progress = false;
  interleaved = true;
  similarity = false;
  ttratio = 2.0;
  whichcode = universal;
  whichcat = george;
  basesequal = true;
  freqa = 0.25;
  freqc = 0.25;
  freqg = 0.25;
  freqt = 0.25;
  usejtt = true;
  usepmb = false;
  usepam = false;
  kimura = false;
  gama = false;
  invar = false;
  invarfrac = 0.0;
  ease = 0.457;
  loopcount = 0;

  if (matrixModel == ProtDistModelTypes::JTT) {
      usejtt = true;
      usepmb = false;
      usepam = false;
      kimura = false;
  } else if (matrixModel == ProtDistModelTypes::PMB) {
      usejtt = false;
      usepmb = true;
      usepam = false;
      kimura = false;
  } else if (matrixModel == ProtDistModelTypes::PAM) {
      usejtt = false;
      usepmb = false;
      usepam = true;
      kimura = false;
  } else if (matrixModel == ProtDistModelTypes::Kimura) {
      usejtt = false;
      usepmb = false;
      usepam = false;
      kimura = true;
  }


  /*do {
    cleerhome();
    printf("\nProtein distance algorithm, version %s\n\n",PHY_VERSION);
    printf("Settings for this run:\n");
    printf("  P  Use JTT, PMB, PAM, Kimura, categories model?  %s\n",
           usejtt ? "Jones-Taylor-Thornton matrix" :
           usepmb ? "Henikoff/Tillier PMB matrix" :
           usepam ? "Dayhoff PAM matrix" :
           kimura ? "Kimura formula" :
           similarity ? "Similarity table" : "Categories model");
    if (!kimura && !similarity) {
      printf("  G  Gamma distribution of rates among positions?");
      if (gama)
        printf("  Yes\n");
      else {
        if (invar)
          printf("  Gamma+Invariant\n");
        else
          printf("  No\n");
      }
    }
    printf("  C           One category of substitution rates?");
    if (!ctgry || categs == 1)
      printf("  Yes\n");
    else
      printf("  %ld categories\n", categs);
    printf("  W                    Use weights for positions?");
    if (weights)
      printf("  Yes\n");
    else
      printf("  No\n");
    if (!(usejtt || usepmb || usepam || kimura || similarity)) {
      printf("  U                       Use which genetic code?  %s\n",
             (whichcode == universal) ? "Universal"                  :
             (whichcode == ciliate)   ? "Ciliate"                    :
             (whichcode == mito)      ? "Universal mitochondrial"    :
             (whichcode == vertmito)  ? "Vertebrate mitochondrial"   :
             (whichcode == flymito)   ? "Fly mitochondrial"        :
             (whichcode == yeastmito) ? "Yeast mitochondrial"        : "");
      printf("  A          Which categorization of amino acids?  %s\n",
             (whichcat == chemical) ? "Chemical"              :
             (whichcat == george)   ? "George/Hunt/Barker"    : "Hall");
        
      printf("  E              Prob change category (1.0=easy):%8.4f\n",ease);
      printf("  T                Transition/transversion ratio:%7.3f\n",ttratio);
      printf("  F                             Base Frequencies:");
      if (basesequal)
        printf("  Equal\n");
      else
        printf("%7.3f%6.3f%6.3f%6.3f\n", freqa, freqc, freqg, freqt);
    }
    printf("  M                   Analyze multiple data sets?");
    if (mulsets)
      printf("  Yes, %2ld %s\n", datasets,
               (justwts ? "sets of weights" : "data sets"));
    else
      printf("  No\n");
    printf("  I                  Input sequences interleaved?  %s\n",
           (interleaved ? "Yes" : "No, sequential"));
    printf("  0                 Terminal type (IBM PC, ANSI)?  %s\n",
           ibmpc ? "IBM PC" :
           ansi  ? "ANSI"   : "(none)");
    printf("  1            Print out the data at start of run  %s\n",
           (printdata ? "Yes" : "No"));
    printf("  2          Print indications of progress of run  %s\n",
           progress ? "Yes" : "No");
    printf("\nAre these settings correct? (type Y or the letter for one to change)\n");
    in[0] = '\0';
    getstryng(in);
    ch=in[0];
    if (ch == '\n')
      ch = ' ';
    protdist_uppercase(&ch);
    done = (ch == 'Y');
    if (!done) {
      if (((strchr("CPGMWI120",ch) != NULL) && (usejtt || usepmb || usepam)) ||
          ((strchr("CPMWI120",ch) != NULL) && (kimura || similarity)) ||
          ((strchr("CUAPGETFMWI120",ch) != NULL) && 
            (! (usejtt || usepmb || usepam || kimura || similarity)))) {
        switch (ch) {

        case 'U':
          printf("Which genetic code?\n");
          printf(" type         for\n\n");
          printf("   U           Universal\n");
          printf("   M           Mitochondrial\n");
          printf("   V           Vertebrate mitochondrial\n");
          printf("   F           Fly mitochondrial\n");
          printf("   Y           Yeast mitochondrial\n\n");
          loopcount2 = 0;
          do {
            printf("type U, M, V, F, or Y\n");
            fflush(stdout);
            scanf("%c%*[^\n]", &ch);
            getchar();
            if (ch == '\n')
              ch = ' ';
            protdist_uppercase(&ch);
            countup(&loopcount2, 10);
          } while (ch != 'U' && ch != 'M' && ch != 'V' && ch != 'F' && ch != 'Y');
          switch (ch) {

          case 'U':
            whichcode = universal;
            break;

          case 'M':
            whichcode = mito;
            break;

          case 'V':
            whichcode = vertmito;
            break;

          case 'F':
            whichcode = flymito;
            break;

          case 'Y':
            whichcode = yeastmito;
            break;
          }
          break;

        case 'A':
          printf(
            "Which of these categorizations of amino acids do you want to use:\n\n");
          printf(
            " all have groups: (Glu Gln Asp Asn), (Lys Arg His), (Phe Tyr Trp)\n");
          printf(" plus:\n");
          printf("George/Hunt/Barker:");
          printf(" (Cys), (Met   Val  Leu  Ileu), (Gly  Ala  Ser  Thr    Pro)\n");
          printf("Chemical:          ");
          printf(" (Cys   Met), (Val  Leu  Ileu    Gly  Ala  Ser  Thr), (Pro)\n");
          printf("Hall:              ");
          printf(" (Cys), (Met   Val  Leu  Ileu), (Gly  Ala  Ser  Thr), (Pro)\n\n");
          printf("Which do you want to use (type C, H, or G)\n");
          loopcount2 = 0;
          do {
            fflush(stdout);
            scanf("%c%*[^\n]", &ch);
            getchar();
            if (ch == '\n')
              ch = ' ';
            protdist_uppercase(&ch);
            countup(&loopcount2, 10);
          } while (ch != 'C' && ch != 'H' && ch != 'G');
          switch (ch) {

          case 'C':
            whichcat = chemical;
            break;

          case 'H':
            whichcat = hall;
            break;

          case 'G':
            whichcat = george;
            break;
          }
          break;

        case 'C':
          ctgry = !ctgry;
          if (ctgry) {
            initcatn(&categs);
            initcategs(categs, rate);
          }
          break;

        case 'W':
          weights = !weights;
          break;

        case 'P':
          if (usejtt) {
            usejtt = false;
            usepmb = true;
          } else {
            if (usepmb) {
              usepmb = false;
              usepam = true;
            } else {
              if (usepam) {
                usepam = false;
                kimura = true;
              } else {
                if (kimura) {
                  kimura = false;
                  similarity = true;
                } else {
                  if (similarity)
                    similarity = false;
                  else
                    usejtt = true;
                }
              }
            }
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


        case 'E':
          printf("Ease of changing category of amino acid?\n");
          loopcount2 = 0;
          do {
            printf(" (1.0 if no difficulty of changing,\n");
            printf(" less if less easy. Can't be negative\n");
            fflush(stdout);
            scanf("%lf%*[^\n]", &ease);
            getchar();
            countup(&loopcount2, 10);
          } while (ease > 1.0 || ease < 0.0);
          break;

        case 'T':
          loopcount2 = 0;
          do {
            printf("Transition/transversion ratio?\n");
            fflush(stdout);
            scanf("%lf%*[^\n]", &ttratio);
            getchar();
            countup(&loopcount2, 10);
          } while (ttratio < 0.0);
          break;

        case 'F':
          loopcount2 = 0;
          do {
            basesequal = false;
            printf("Frequencies of bases A,C,G,T ?\n");
            fflush(stdout);
            scanf("%lf%lf%lf%lf%*[^\n]", &freqa, &freqc, &freqg, &freqt);
            getchar();
            if (fabs(freqa + freqc + freqg + freqt - 1.0) >= 1.0e-3)
              printf("FREQUENCIES MUST SUM TO 1\n");
            countup(&loopcount2, 10);
          } while (fabs(freqa + freqc + freqg + freqt - 1.0) >= 1.0e-3);
          break;

        case 'M':
          mulsets = !mulsets;
          if (mulsets) {
            printf("Multiple data sets or multiple weights?");
            loopcount2 = 0;
            do {
              printf(" (type D or W)\n");
              fflush(stdout);
              scanf("%c%*[^\n]", &ch2);
              getchar();
              if (ch2 == '\n')
                  ch2 = ' ';
              uppercase(&ch2);
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
          if (ibmpc) {
            ibmpc = false;
            ansi = true;
            } else if (ansi)
              ansi = false;
            else
              ibmpc = true;
          break;

        case '1':
          printdata = !printdata;
          break;

        case '2':
          progress = !progress;
          break;
        }
      } else {
        if (strchr("CUAPGETFMWI120",ch) == NULL)
          printf("Not a possible option!\n");
        else
          printf("That option not allowed with these settings\n");
        printf("\nPress Enter or Return key to continue\n");
        fflush(stdout);
        getchar();
      }
    }
    countup(&loopcount, 100);
  } while (!done); */
  /*if (gama || invar) {
    loopcount = 0;
    do {
      printf(
"\nCoefficient of variation of substitution rate among positions (must be positive)\n");
      printf(
  " In gamma distribution parameters, this is 1/(square root of alpha)\n");
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
      printf("Fraction of invariant positions?\n");
      fflush(stdout);
      scanf("%lf%*[^\n]", &invarfrac);
      getchar();
      countup (&loopcount, 10);
    } while ((invarfrac <= 0.0) || (invarfrac >= 1.0));
  }*/
}  /* getoptions */


void transition()
{
  /* calculations related to transition-transversion ratio */
  double aa, bb, freqr, freqy, freqgr, freqty;

  freqr = freqa + freqg;
  freqy = freqc + freqt;
  freqgr = freqg / freqr;
  freqty = freqt / freqy;
  aa = ttratio * freqr * freqy - freqa * freqg - freqc * freqt;
  bb = freqa * freqgr + freqc * freqty;
  xi = aa / (aa + bb);
  xv = 1.0 - xi;
  if (xi <= 0.0 && xi >= -phylip_epsilon)
    xi = 0.0;
  if (xi < 0.0){
    printf("THIS TRANSITION-TRANSVERSION RATIO IS IMPOSSIBLE WITH");
    printf(" THESE BASE FREQUENCIES\n");
    exxit(-1);}
}  /* transition */


void prot_doinit(const U2::CreatePhyTreeSettings& settings, U2::MemoryLocker& memLocker)
{
  /* initializes variables */
  protdist_inputnumbers(memLocker);
  if(memLocker.hasError()) {
      return;
  }
  prot_getoptions(settings.matrixId);
  transition();
}  /* doinit*/


void prot_printcategories()
{ /* print out list of categories of positions */
  /*long i, j;

  fprintf(outfile, "Rate categories\n\n");
  for (i = 1; i <= nmlngth + 3; i++)
    putc(' ', outfile);
  for (i = 1; i <= chars; i++) {
    fprintf(outfile, "%ld", category[i - 1]);
    if (i % 60 == 0) {
      putc('\n', outfile);
      for (j = 1; j <= nmlngth + 3; j++)
        putc(' ', outfile);
    } else if (i % 10 == 0)
      putc(' ', outfile);
  }
  fprintf(outfile, "\n\n"); */
}  /* printcategories */


void reallocchars(void) 
{
  int i;

  free(weight);
  free(oldweight);
  free(category);
  for (i = 0; i < spp; i++) {
    free(gnode[i]);
    gnode[i] = (aas *)Malloc(chars * sizeof(aas ));
  }
  weight = (steparray)Malloc(chars*sizeof(long));
  oldweight = (steparray)Malloc(chars*sizeof(long));
  category = (steparray)Malloc(chars*sizeof(long));
}


void prot_inputoptions()
{ /* input the information on the options */
  long i;

  if (!firstset && !justwts) {
    samenumsp(&chars, ith);
    reallocchars();
  } if (firstset || !justwts) {
    for (i = 0; i < chars; i++) {
      category[i] = 1;
      oldweight[i] = 1;
      weight[i] = 1;
    }
  }
  /*  if (!justwts && weights) {*/
  if (justwts || weights)
    inputweights(chars, oldweight, &weights);
  if (printdata)
    putc('\n', outfile);
  if (usejtt && printdata)
    fprintf(outfile, "  Jones-Taylor-Thornton model distance\n");
  if (usepmb && printdata)
    fprintf(outfile, "  Henikoff/Tillier PMB model distance\n");
  if (usepam && printdata)
    fprintf(outfile, "  Dayhoff PAM model distance\n");
  if (kimura && printdata)
    fprintf(outfile, "  Kimura protein distance\n");
  if (!(usejtt || usepmb || usepam || kimura || similarity) && printdata)
    fprintf(outfile, "  Categories model distance\n");
  if (similarity)
    fprintf(outfile, "  \n  Table of similarity between sequences\n");
  if ((ctgry && categs > 1) && (firstset || !justwts)) {
    inputcategs(0, chars, category, categs, "ProtDist");
    if (printdata)
      printcategs(outfile, chars, category, "Position categories");
  } else if (printdata && (categs > 1)) {
 //   fprintf(outfile, "\nPosition category   Rate of change\n\n");
//     for (i = 1; i <= categs; i++)
//       fprintf(outfile, "%15ld%13.3f\n", i, rate[i - 1]);
//    putc('\n', outfile);
    prot_printcategories();
  }
  if (weights && printdata)
    printweights(outfile, 0, chars, oldweight, "Positions");
}  /* inputoptions */


void protdist_inputdata()
{
  /* input the names and sequences for each species */
  long i, j, k, l, aasread=0, aasnew=0;
  Phylip_Char charstate;
  boolean allread, done;
  aas aa= ala;//0;   /* temporary amino acid for input */

  if (progress)
    putchar('\n');
  j = nmlngth + (chars + (chars - 1) / 10) / 2 - 5;
  if (j < nmlngth - 1)
    j = nmlngth - 1;
  if (j > 37)
    j = 37;
  if (printdata) {
    fprintf(outfile, "\nName");
    for (i = 1; i <= j; i++)
      putc(' ', outfile);
    fprintf(outfile, "Sequences\n");
    fprintf(outfile, "----");
    for (i = 1; i <= j; i++)
      putc(' ', outfile);
    fprintf(outfile, "---------\n\n");
  }
  aasread = 0;
  allread = false;
  while (!(allread)) {
    /* eat white space -- if the separator line has spaces on it*/
    do {
      charstate = gettc(infile);
    } while (charstate == ' ' || charstate == '\t');
    ungetc(charstate, infile);
    if (eoln(infile)) 
      scan_eoln(infile);
    i = 1;
    while (i <= spp) {
      if ((interleaved && aasread == 0) || !interleaved)
        initname(i-1);
      if (interleaved)
        j = aasread;
      else
        j = 0;
      done = false;
      while (((!done) && (!(eoln(infile) || eoff(infile))))) {
        if (interleaved)
          done = true;
        while (((j < chars) & (!(eoln(infile) | eoff(infile))))) {
          charstate = gettc(infile);
          if (charstate == '\n' || charstate == '\t')
            charstate = ' ';
          if (charstate == ' ' || (charstate >= '0' && charstate <= '9'))
            continue;
          protdist_uppercase(&charstate);
          if ((!isalpha(charstate) && charstate != '.' && charstate != '?' &&
               charstate != '-' && charstate != '*') || charstate == 'J' ||
              charstate == 'O' || charstate == 'U' || charstate == '.') {
        printf("ERROR -- bad amino acid: %c at position %ld of species %3ld\n",
                   charstate, j, i);
            if (charstate == '.') {
          printf("         Periods (.) may not be used as gap characters.\n");
          printf("         The correct gap character is (-)\n");
            }
            exxit(-1);
          }
          j++;

          switch (charstate) {

          case 'A':
            aa = ala;
            break;

          case 'B':
            aa = asx;
            break;

          case 'C':
            aa = cys;
            break;

          case 'D':
            aa = asp;
            break;

          case 'E':
            aa = glu;
            break;

          case 'F':
            aa = phe;
            break;

          case 'G':
            aa = gly;
            break;

          case 'H':
            aa = his;
            break;

          case 'I':
            aa = ileu;
            break;

          case 'K':
            aa = lys;
            break;

          case 'L':
            aa = leu;
            break;

          case 'M':
            aa = met;
            break;

          case 'N':
            aa = asn;
            break;

          case 'P':
            aa = pro;
            break;

          case 'Q':
            aa = gln;
            break;

          case 'R':
            aa = arg;
            break;

          case 'S':
            aa = ser;
            break;

          case 'T':
            aa = thr;
            break;

          case 'V':
            aa = val;
            break;

          case 'W':
            aa = trp;
            break;

          case 'X':
            aa = unk;
            break;

          case 'Y':
            aa = tyr;
            break;

          case 'Z':
            aa = glx;
            break;

          case '*':
            aa = stop;
            break;

          case '?':
            aa = quest;
            break;

          case '-':
            aa = del;
            break;
          }
          gnode[i - 1][j - 1] = aa;
        }
        if (interleaved)
          continue;
        if (j < chars) 
          scan_eoln(infile);
        else if (j == chars)
          done = true;
      }
      if (interleaved && i == 1)
        aasnew = j;
      scan_eoln(infile);
      if ((interleaved && j != aasnew) || ((!interleaved) && j != chars)){
        printf("ERROR: SEQUENCES OUT OF ALIGNMENT\n");
        exxit(-1);}
      i++;
    }
    if (interleaved) {
      aasread = aasnew;
      allread = (aasread == chars);
    } else
      allread = (i > spp);
  }
  if ( printdata) {
    for (i = 1; i <= ((chars - 1) / 60 + 1); i++) {
      for (j = 1; j <= spp; j++) {
        for (k = 0; k < nmlngth; k++)
          putc(nayme[j - 1][k], outfile);
        fprintf(outfile, "   ");
        l = i * 60;
        if (l > chars)
          l = chars;
        for (k = (i - 1) * 60 + 1; k <= l; k++) {
          if (j > 1 && gnode[j - 1][k - 1] == gnode[0][k - 1])
            charstate = '.';
          else {
            switch (gnode[j - 1][k - 1]) {

            case ala:
              charstate = 'A';
              break;

            case asx:
              charstate = 'B';
              break;

            case cys:
              charstate = 'C';
              break;

            case asp:
              charstate = 'D';
              break;

            case glu:
              charstate = 'E';
              break;

            case phe:
              charstate = 'F';
              break;

            case gly:
              charstate = 'G';
              break;

            case his:
              charstate = 'H';
              break;

            case ileu:
              charstate = 'I';
              break;

            case lys:
              charstate = 'K';
              break;

            case leu:
              charstate = 'L';
              break;

            case met:
              charstate = 'M';
              break;

            case asn:
              charstate = 'N';
              break;

            case pro:
              charstate = 'P';
              break;

            case gln:
              charstate = 'Q';
              break;

            case arg:
              charstate = 'R';
              break;

            case ser:
              charstate = 'S';
              break;

            case thr:
              charstate = 'T';
              break;

            case val:
              charstate = 'V';
              break;

            case trp:
              charstate = 'W';
              break;

            case tyr:
              charstate = 'Y';
              break;

            case glx:
              charstate = 'Z';
              break;

            case del:
              charstate = '-';
              break;

            case stop:
              charstate = '*';
              break;

            case unk:
              charstate = 'X';
              break;

            case quest:
              charstate = '?';
              break;
            
            default:        /*cases ser1 and ser2 cannot occur*/
              break;
            }
          }
          putc(charstate, outfile);
          if (k % 10 == 0 && k % 60 != 0)
            putc(' ', outfile);
        }
        putc('\n', outfile);
      }
      putc('\n', outfile);
    }
    putc('\n', outfile);
  }
  if (printdata)
    putc('\n', outfile);
}  /* protdist_inputdata */


void doinput()
{ /* reads the input data */
  long i;
  double sumrates, weightsum;

  prot_inputoptions();
  if(!justwts || firstset)
  //  protdist_inputdata();
  if (!ctgry) {
    categs = 1;
    rate[0] = 1.0;
  }
  weightsum = 0;
  for (i = 0; i < chars; i++)
    weightsum += oldweight[i];
  sumrates = 0.0;
  for (i = 0; i < chars; i++)
    sumrates += oldweight[i] * rate[category[i] - 1];
  for (i = 0; i < categs; i++)
    rate[i] *= weightsum / sumrates;
}  /* doinput */


void code()
{
  /* make up table of the code 1 = u, 2 = c, 3 = a, 4 = g */
  long n;
  aas b;

  trans[0][0][0] = phe;
  trans[0][0][1] = phe;
  trans[0][0][2] = leu;
  trans[0][0][3] = leu;
  trans[0][1][0] = ser;
  trans[0][1][1] = ser;
  trans[0][1][2] = ser;
  trans[0][1][3] = ser;
  trans[0][2][0] = tyr;
  trans[0][2][1] = tyr;
  trans[0][2][2] = stop;
  trans[0][2][3] = stop;
  trans[0][3][0] = cys;
  trans[0][3][1] = cys;
  trans[0][3][2] = stop;
  trans[0][3][3] = trp;
  trans[1][0][0] = leu;
  trans[1][0][1] = leu;
  trans[1][0][2] = leu;
  trans[1][0][3] = leu;
  trans[1][1][0] = pro;
  trans[1][1][1] = pro;
  trans[1][1][2] = pro;
  trans[1][1][3] = pro;
  trans[1][2][0] = his;
  trans[1][2][1] = his;
  trans[1][2][2] = gln;
  trans[1][2][3] = gln;
  trans[1][3][0] = arg;
  trans[1][3][1] = arg;
  trans[1][3][2] = arg;
  trans[1][3][3] = arg;
  trans[2][0][0] = ileu;
  trans[2][0][1] = ileu;
  trans[2][0][2] = ileu;
  trans[2][0][3] = met;
  trans[2][1][0] = thr;
  trans[2][1][1] = thr;
  trans[2][1][2] = thr;
  trans[2][1][3] = thr;
  trans[2][2][0] = asn;
  trans[2][2][1] = asn;
  trans[2][2][2] = lys;
  trans[2][2][3] = lys;
  trans[2][3][0] = ser;
  trans[2][3][1] = ser;
  trans[2][3][2] = arg;
  trans[2][3][3] = arg;
  trans[3][0][0] = val;
  trans[3][0][1] = val;
  trans[3][0][2] = val;
  trans[3][0][3] = val;
  trans[3][1][0] = ala;
  trans[3][1][1] = ala;
  trans[3][1][2] = ala;
  trans[3][1][3] = ala;
  trans[3][2][0] = asp;
  trans[3][2][1] = asp;
  trans[3][2][2] = glu;
  trans[3][2][3] = glu;
  trans[3][3][0] = gly;
  trans[3][3][1] = gly;
  trans[3][3][2] = gly;
  trans[3][3][3] = gly;
  if (whichcode == mito)
    trans[0][3][2] = trp;
  if (whichcode == vertmito) {
    trans[0][3][2] = trp;
    trans[2][3][2] = stop;
    trans[2][3][3] = stop;
    trans[2][0][2] = met;
  }
  if (whichcode == flymito) {
    trans[0][3][2] = trp;
    trans[2][0][2] = met;
    trans[2][3][2] = ser;
  }
  if (whichcode == yeastmito) {
    trans[0][3][2] = trp;
    trans[1][0][2] = thr;
    trans[2][0][2] = met;
  }
  n = 0;
  for (b = ala; (long)b <= (long)val; b = (aas)((long)b + 1)) {
    if (b != ser2) {
      n++;
      numaa[(long)b - (long)ala] = n;
    }
  }
  numaa[(long)ser - (long)ala] = (long)ser1 - (long)(ala) + 1;
}  /* code */


void protdist_cats()
{
  /* define categories of amino acids */
  aas b;

  /* fundamental subgroups */
  cat[0] = 1;                        /* for alanine */
  cat[(long)cys - (long)ala] = 1;
  cat[(long)met - (long)ala] = 2;
  cat[(long)val - (long)ala] = 3;
  cat[(long)leu - (long)ala] = 3;
  cat[(long)ileu - (long)ala] = 3;
  cat[(long)gly - (long)ala] = 4;
  cat[0] = 4;
  cat[(long)ser - (long)ala] = 4;
  cat[(long)thr - (long)ala] = 4;
  cat[(long)pro - (long)ala] = 5;
  cat[(long)phe - (long)ala] = 6;
  cat[(long)tyr - (long)ala] = 6;
  cat[(long)trp - (long)ala] = 6;
  cat[(long)glu - (long)ala] = 7;
  cat[(long)gln - (long)ala] = 7;
  cat[(long)asp - (long)ala] = 7;
  cat[(long)asn - (long)ala] = 7;
  cat[(long)lys - (long)ala] = 8;
  cat[(long)arg - (long)ala] = 8;
  cat[(long)his - (long)ala] = 8;
  if (whichcat == george) {
  /* George, Hunt and Barker: sulfhydryl, small hydrophobic, small hydrophilic,
                              aromatic, acid/acid-amide/hydrophilic, basic */
    for (b = ala; (long)b <= (long)val; b = (aas)((long)b + 1)) {
      if (cat[(long)b - (long)ala] == 3)
        cat[(long)b - (long)ala] = 2;
      if (cat[(long)b - (long)ala] == 5)
        cat[(long)b - (long)ala] = 4;
    }
  }
  if (whichcat == chemical) {
    /* Conn and Stumpf:  monoamino, aliphatic, heterocyclic,
                         aromatic, dicarboxylic, basic */
    for (b = ala; (long)b <= (long)val; b = (aas)((long)b + 1)) {
      if (cat[(long)b - (long)ala] == 2)
        cat[(long)b - (long)ala] = 1;
      if (cat[(long)b - (long)ala] == 4)
        cat[(long)b - (long)ala] = 3;
    }
  }
  /* Ben Hall's personal opinion */
  if (whichcat != hall)
    return;
  for (b = ala; (long)b <= (long)val; b = (aas)((long)b + 1)) {
    if (cat[(long)b - (long)ala] == 3)
      cat[(long)b - (long)ala] = 2;
  }
}  /* protdist_cats */


void maketrans()
{
  /* Make up transition probability matrix from code and category tables */
  long i, j, k, m, n, s, nb1, nb2;
  double x, sum;
  long sub[3], newsub[3];
  double f[4], g[4];
  aas b1, b2;
  double TEMP, TEMP1, TEMP2, TEMP3;

  for (i = 0; i <= 19; i++) {
    pie[i] = 0.0;
    for (j = 0; j <= 19; j++)
      prob[i][j] = 0.0;
  }
  f[0] = freqt;
  f[1] = freqc;
  f[2] = freqa;
  f[3] = freqg;
  g[0] = freqc + freqt;
  g[1] = freqc + freqt;
  g[2] = freqa + freqg;
  g[3] = freqa + freqg;
  TEMP = f[0];
  TEMP1 = f[1];
  TEMP2 = f[2];
  TEMP3 = f[3];
  fracchange = xi * (2 * f[0] * f[1] / g[0] + 2 * f[2] * f[3] / g[2]) +
      xv * (1 - TEMP * TEMP - TEMP1 * TEMP1 - TEMP2 * TEMP2 - TEMP3 * TEMP3);
  sum = 0.0;
  for (i = 0; i <= 3; i++) {
    for (j = 0; j <= 3; j++) {
      for (k = 0; k <= 3; k++) {
        if (trans[i][j][k] != stop)
          sum += f[i] * f[j] * f[k];
      }
    }
  }
  for (i = 0; i <= 3; i++) {
    sub[0] = i + 1;
    for (j = 0; j <= 3; j++) {
      sub[1] = j + 1;
      for (k = 0; k <= 3; k++) {
        sub[2] = k + 1;
        b1 = trans[i][j][k];
        for (m = 0; m <= 2; m++) {
          s = sub[m];
          for (n = 1; n <= 4; n++) {
            memcpy(newsub, sub, sizeof(long) * 3L);
            newsub[m] = n;
            x = f[i] * f[j] * f[k] / (3.0 * sum);
            if (((s == 1 || s == 2) && (n == 3 || n == 4)) ||
                ((n == 1 || n == 2) && (s == 3 || s == 4)))
              x *= xv * f[n - 1];
            else
              x *= xi * f[n - 1] / g[n - 1] + xv * f[n - 1];
            b2 = trans[newsub[0] - 1][newsub[1] - 1][newsub[2] - 1];
            if (b1 != stop) {
              nb1 = numaa[(long)b1 - (long)ala];
              pie[nb1 - 1] += x;
              if (b2 != stop) {
                nb2 = numaa[(long)b2 - (long)ala];
                if (cat[(long)b1 - (long)ala] != cat[(long)b2 - (long)ala]) {
                  prob[nb1 - 1][nb2 - 1] += x * ease;
                  prob[nb1 - 1][nb1 - 1] += x * (1.0 - ease);
                } else
                  prob[nb1 - 1][nb2 - 1] += x;
              } else
                prob[nb1 - 1][nb1 - 1] += x;
            }
          }
        }
      }
    }
  }
  for (i = 0; i <= 19; i++)
    prob[i][i] -= pie[i];
  for (i = 0; i <= 19; i++) {
    for (j = 0; j <= 19; j++)
      prob[i][j] /= sqrt(pie[i] * pie[j]);
  }
  /* computes pi^(1/2)*B*pi^(-1/2)  */
}  /* maketrans */


void givens(double (*a)[20], long i, long j, long n, double ctheta,
                        double stheta, boolean left)
{ /* Givens transform at i,j for 1..n with angle theta */
  long k;
  double d;

  for (k = 0; k < n; k++) {
    if (left) {
      d = ctheta * a[i - 1][k] + stheta * a[j - 1][k];
      a[j - 1][k] = ctheta * a[j - 1][k] - stheta * a[i - 1][k];
      a[i - 1][k] = d;
    } else {
      d = ctheta * a[k][i - 1] + stheta * a[k][j - 1];
      a[k][j - 1] = ctheta * a[k][j - 1] - stheta * a[k][i - 1];
      a[k][i - 1] = d;
    }
  }
}  /* givens */


void coeffs(double x, double y, double *c, double *s, double accuracy)
{ /* compute cosine and sine of theta */
  double root;

  root = sqrt(x * x + y * y);
  if (root < accuracy) {
    *c = 1.0;
    *s = 0.0;
  } else {
    *c = x / root;
    *s = y / root;
  }
}  /* coeffs */


void tridiag(double (*a)[20], long n, double accuracy)
{ /* Givens tridiagonalization */
  long i, j;
  double s, c;

  for (i = 2; i < n; i++) {
    for (j = i + 1; j <= n; j++) {
      coeffs(a[i - 2][i - 1], a[i - 2][j - 1], &c, &s,accuracy);
      givens(a, i, j, n, c, s, true);
      givens(a, i, j, n, c, s, false);
      givens(eigvecs, i, j, n, c, s, true);
    }
  }
}  /* tridiag */


void shiftqr(double (*a)[20], long n, double accuracy)
{ /* QR eigenvalue-finder */
  long i, j;
  double approx, s, c, d, TEMP, TEMP1;

  for (i = n; i >= 2; i--) {
    do {
      TEMP = a[i - 2][i - 2] - a[i - 1][i - 1];
      TEMP1 = a[i - 1][i - 2];
      d = sqrt(TEMP * TEMP + TEMP1 * TEMP1);
      approx = a[i - 2][i - 2] + a[i - 1][i - 1];
      if (a[i - 1][i - 1] < a[i - 2][i - 2])
        approx = (approx - d) / 2.0;
      else
        approx = (approx + d) / 2.0;
      for (j = 0; j < i; j++)
        a[j][j] -= approx;
      for (j = 1; j < i; j++) {
        coeffs(a[j - 1][j - 1], a[j][j - 1], &c, &s, accuracy);
        givens(a, j, j + 1, i, c, s, true);
        givens(a, j, j + 1, i, c, s, false);
        givens(eigvecs, j, j + 1, n, c, s, true);
      }
      for (j = 0; j < i; j++)
        a[j][j] += approx;
    } while (fabs(a[i - 1][i - 2]) > accuracy);
  }
}  /* shiftqr */


void qreigen(double (*prob)[20], long n)
{ /* QR eigenvector/eigenvalue method for symmetric matrix */
  double accuracy;
  long i, j;

  accuracy = 1.0e-6;
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++)
      eigvecs[i][j] = 0.0;
    eigvecs[i][i] = 1.0;
  }
  tridiag(prob, n, accuracy);
  shiftqr(prob, n, accuracy);
  for (i = 0; i < n; i++)
    eig[i] = prob[i][i];
  for (i = 0; i <= 19; i++) {
    for (j = 0; j <= 19; j++)
      prob[i][j] = sqrt(pie[j]) * eigvecs[i][j];
  }
  /* prob[i][j] is the value of U' times pi^(1/2) */
}  /* qreigen */


void jtteigen()
{ /* eigenanalysis for JTT matrix, precomputed */
  memcpy(prob,jttprobs,sizeof(jttprobs));
  memcpy(eig,jtteigs,sizeof(jtteigs));
  fracchange = 1.0;     /** changed from 0.01   **/
}  /* jtteigen */


void pmbeigen()
{ /* eigenanalysis for PMB matrix, precomputed */
  memcpy(prob,pmbprobs,sizeof(pmbprobs));
  memcpy(eig,pmbeigs,sizeof(pmbeigs));
  fracchange = 1.0;
}  /* pmbeigen */


void pameigen()
{ /* eigenanalysis for PAM matrix, precomputed */
  memcpy(prob,pamprobs,sizeof(pamprobs));
  memcpy(eig,pameigs,sizeof(pameigs));
  fracchange = 1.0;     /** changed from 0.01   **/
}  /* pameigen */


void predict(long nb1, long nb2, long cat)
{ /* make contribution to prediction of this aa pair */
  long m;
  double TEMP;

  for (m = 0; m <= 19; m++) {
    if (gama || invar)
      elambdat = exp(-cvi*log(1.0-rate[cat-1]*tt*(eig[m]/(1.0-invarfrac))/cvi));
    else
      elambdat = exp(rate[cat-1]*tt * eig[m]);
    q = prob[m][nb1 - 1] * prob[m][nb2 - 1] * elambdat;
    p += q;
    if (!gama && !invar)
      dp += rate[cat-1]*eig[m] * q;
    else
      dp += (rate[cat-1]*eig[m]/(1.0-rate[cat-1]*tt*(eig[m]/(1.0-invarfrac))/cvi)) * q;
    TEMP = eig[m];
    if (!gama && !invar)
      d2p += TEMP * TEMP * q;
    else
      d2p += (rate[cat-1]*rate[cat-1]*eig[m]*eig[m]*(1.0+1.0/cvi)/
              ((1.0-rate[cat-1]*tt*eig[m]/cvi)
              *(1.0-rate[cat-1]*tt*eig[m]/cvi))) * q;
  }
  if (nb1 == nb2) {
    p *= (1.0 - invarfrac);
    p += invarfrac;
  }
  dp *= (1.0 - invarfrac);
  d2p *= (1.0 - invarfrac);
}  /* predict */


void prot_makedists()
{ /* compute the distances */
  long i, j, k, m, n, itterations, nb1, nb2, cat;
  double delta, lnlike, slope, curv;
  boolean neginfinity, inf, overlap;
  aas b1, b2;

  //if (!(printdata || similarity))
  //  fprintf(outfile, "%5ld\n", spp);
  //if (progress)
  //  printf("Computing distances:\n");
  
  float cur_prog = 0;
  int total = (spp*spp / 2) + 1;
  float step = (1.0f / total) * 100.0f;  
  
  for (i = 1; i <= spp; i++) {
    if (progress)
      printf("  ");
    if (progress) {
      for (j = 0; j < nmlngth; j++)
        putchar(nayme[i - 1][j]);
    }
    if (progress) {
      printf("   ");
      fflush(stdout);
    }
    if (similarity)
      d[i-1][i-1] = 1.0;
    else
      d[i-1][i-1] = 0.0;
    for (j = 0; j <= i - 2; j++) {
      if (!(kimura || similarity)) {
        if (usejtt || usepmb || usepam)
          tt = 0.1/fracchange;
        else
          tt = 1.0;
        delta = tt / 2.0;
        itterations = 0;
        inf = false;
        do {
          lnlike = 0.0;
          slope = 0.0;
          curv = 0.0;
          neginfinity = false;
          overlap = false;
          for (k = 0; k < chars; k++) {
            if (oldweight[k] > 0) {
              cat = category[k];
              b1 = gnode[i - 1][k];
              b2 = gnode[j][k];
              if (b1 != stop && b1 != del && b1 != quest && b1 != unk &&
                  b2 != stop && b2 != del && b2 != quest && b2 != unk) {
                overlap = true;
                p = 0.0;
                dp = 0.0;
                d2p = 0.0;
                nb1 = numaa[(long)b1 - (long)ala];
                nb2 = numaa[(long)b2 - (long)ala];
                if (b1 != asx && b1 != glx && b2 != asx && b2 != glx)
                  predict(nb1, nb2, cat);
                else {
                  if (b1 == asx) {
                    if (b2 == asx) {
                      predict(3L, 3L, cat);
                      predict(3L, 4L, cat);
                      predict(4L, 3L, cat);
                      predict(4L, 4L, cat);
                    } else {
                      if (b2 == glx) {
                        predict(3L, 6L, cat);
                        predict(3L, 7L, cat);
                        predict(4L, 6L, cat);
                        predict(4L, 7L, cat);
                      } else {
                        predict(3L, nb2, cat);
                        predict(4L, nb2, cat);
                      }
                    }
                  } else {
                    if (b1 == glx) {
                      if (b2 == asx) {
                        predict(6L, 3L, cat);
                        predict(6L, 4L, cat);
                        predict(7L, 3L, cat);
                        predict(7L, 4L, cat);
                      } else {
                        if (b2 == glx) {
                          predict(6L, 6L, cat);
                          predict(6L, 7L, cat);
                          predict(7L, 6L, cat);
                          predict(7L, 7L, cat);
                        } else {
                          predict(6L, nb2, cat);
                          predict(7L, nb2, cat);
                        }
                      }
                    } else {
                      if (b2 == asx) {
                        predict(nb1, 3L, cat);
                        predict(nb1, 4L, cat);
                        predict(nb1, 3L, cat);
                        predict(nb1, 4L, cat);
                      } else if (b2 == glx) {
                        predict(nb1, 6L, cat);
                        predict(nb1, 7L, cat);
                        predict(nb1, 6L, cat);
                        predict(nb1, 7L, cat);
                      }
                    }
                  }
                }
                if (p <= 0.0)
                  neginfinity = true;
                else {
                  lnlike += oldweight[k]*log(p);
                  slope += oldweight[k]*dp / p;
                  curv += oldweight[k]*(d2p / p - dp * dp / (p * p));
                }
              }
            }
          }
          itterations++;
          if (!overlap){
            printf("\nWARNING: NO OVERLAP BETWEEN SEQUENCES %ld AND %ld; -1.0 WAS WRITTEN\n", i, j+1);
            tt = -1.0/fracchange;
            itterations = 20;
            inf = true;
          } else if (!neginfinity) {
            if (curv < 0.0) {
              tt -= slope / curv;
              if (tt > 10000.0) {
                printf("\nWARNING: INFINITE DISTANCE BETWEEN SPECIES %ld AND %ld; -1.0 WAS WRITTEN\n", i, j+1);
                tt = -1.0/fracchange;
                inf = true;
                itterations = 20;
              }
            }
            else {
              if ((slope > 0.0 && delta < 0.0) || (slope < 0.0 && delta > 0.0))
                delta /= -2;
              tt += delta;
            }
          } else {
            delta /= -2;
            tt += delta;
          }
          if (tt < protepsilon && !inf)
            tt = protepsilon;
        } while (itterations != 20);
      } else {
        m = 0;
        n = 0;
        for (k = 0; k < chars; k++) {
          b1 = gnode[i - 1][k];
          b2 = gnode[j][k];
          if ((((long)b1 <= (long)val) || ((long)b1 == (long)ser))
           && (((long)b2 <= (long)val) || ((long)b2 == (long)ser))) {
            if (b1 == b2)
              m++;
            n++;
          }
        }
        p = 1 - (double)m / n;
        if (kimura) {
          dp = 1.0 - p - 0.2 * p * p;
          if (dp < 0.0) {
            printf(
"\nDISTANCE BETWEEN SEQUENCES %3ld AND %3ld IS TOO LARGE FOR KIMURA FORMULA\n",
              i, j + 1);
            tt = -1.0;
          } else
            tt = -log(dp);
        } else {              /* if similarity */
            tt = 1.0 - p;
        }
      }
      d[i - 1][j] = fracchange * tt;
      d[j][i - 1] = d[i - 1][j];
      
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
      fflush(stdout);
    }
  }
  if (!similarity) {
    /*for (i = 0; i < spp; i++) {
      for (j = 0; j < nmlngth; j++)
        putc(nayme[i][j], outfile);
      k = spp;
      for (j = 1; j <= k; j++) {
        if (d[i][j-1] < 100.0)
          fprintf(outfile, "%10.6f", d[i][j-1]);
        else if (d[i][j-1] < 1000.0)
          fprintf(outfile, " %10.6f", d[i][j-1]);
          else 
            fprintf(outfile, " %11.6f", d[i][j-1]);
        if ((j + 1) % 7 == 0 && j < k)
          putc('\n', outfile);
      }
      putc('\n', outfile);
    }*/
  } else {
 /*   for (i = 0; i < spp; i += 6) {
      if ((i+6) < spp)
        n = i+6;
      else
        n = spp;
      fprintf(outfile, "            ");
      for (j = i; j < n ; j++) {
        for (k = 0; k < (nmlngth-2); k++)
          putc(nayme[j][k], outfile);
        putc(' ', outfile);
        putc(' ', outfile);
      }
      putc('\n', outfile);
      for (j = 0; j < spp; j++) {
        for (k = 0; k < nmlngth; k++)
          putc(nayme[j][k], outfile);
        if ((i+6) < spp)
          n = i+6;
        else
          n = spp;
        for (k = i; k < n ; k++)
          if (d[j][k] < 100.0)
            fprintf(outfile, "%10.6f", d[j][k]);
          else if (d[j][k] < 1000.0)
            fprintf(outfile, " %10.6f", d[j][k]);
            else 
              fprintf(outfile, " %11.6f", d[j][k]);
        putc('\n', outfile);
      }
      putc('\n', outfile);
    }*/
  }
  //if (progress)
    //printf("\nOutput written to file \"%s\"\n\n", outfilename);
}  /* makedists */


// int prot_main(int argc, Char *argv[])
// {  /* ML Protein distances by PMB, JTT, PAM or categories model */
// #ifdef MAC
//    argc = 1;   /* macsetup("Protdist",""); */
//    argv[0] = "Protdist";
// #endif
//   init(argc, argv);
//   openfile(&infile,INFILE,"input file","r",argv[0],infilename);
//   openfile(&outfile,OUTFILE,"output file","w",argv[0],outfilename);
//   ibmpc = IBMCRT;
//   ansi = ANSICRT;
//   mulsets = false;
//   datasets = 1;
//   firstset = true;
//   prot_doinit();
//   if (!(kimura || similarity))
//     code();
//   if (!(usejtt || usepmb || usepam ||  kimura || similarity)) {
//     protdist_cats();
//     maketrans();
//     qreigen(prob, 20L);
//   } else {
//     if (kimura || similarity)
//       fracchange = 1.0;
//     else {
//       if (usejtt)
//         jtteigen();
//       else {
//         if (usepmb)
//           pmbeigen();
//         else
//           pameigen();
//         }
//     }
//   }
//  // if (ctgry)
//  //   openfile(&catfile,CATFILE,"categories file","r",argv[0],catfilename);
//  // if (weights || justwts)
//  //   openfile(&weightfile,WEIGHTFILE,"weights file","r",argv[0],weightfilename);
//  // for (ith = 1; ith <= datasets; ith++) {
//     doinput();
//     if (ith == 1)
//       firstset = false;
//     //if ((datasets > 1) && progress)
//     //  printf("\nData set # %ld:\n\n", ith);
//     prot_makedists();
// //  }
//   FClose(outfile);
//   FClose(infile);
// #ifdef MAC
//   fixmacfile(outfilename);
// #endif
//   printf("Done.\n\n");
// 
// #ifdef WIN32
//   phyRestoreConsoleAttributes();
// #endif
// 
//   return 0;
// }  /* Protein distances */

