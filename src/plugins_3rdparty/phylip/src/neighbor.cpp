#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

/* version 3.696.
Written by Mary Kuhner, Jon Yamato, Joseph Felsenstein, Akiko Fuseki,
Sean Lamont, and Andrew Keeffe.

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

#include <float.h>
#include <QString>
#include "dist.h"

#ifndef OLDC
/* function prototypes */
void getoptions(void);
void allocrest(void);
void doinit(void);
void inputoptions(void);
void getinput(void);
void describe(node *, double);
void summarize(void);
void nodelabel(boolean);
void jointree(void);
void maketree(void);
void freerest(void);
/* function prototypes */
#endif


static Phylip_Char infilename[FNMLNGTH], outfilename[FNMLNGTH], outtreename[FNMLNGTH];
static long nonodes2, outgrno, col, datasets, ith;
static long inseed;
static vector *x;
static intvector *reps;
static boolean jumble, lower, upper, outgropt, replicates, trout,
               printdata, progress, treeprint, mulsets, njoin;
static tree curtree;
static longer seed;
static long *enterorder;
static Phylip_Char progname[20];
static double* data;

/* variables for maketree, propagated globally for C version: */
static node **cluster;

vector* getMtx() 
{
    return x;
}

void neighbor_getoptions()
{
  /* interactively set options */
  long inseed0 = 1, loopcount;
  //Char ch;

  //fprintf(outfile, "\nNeighbor-Joining/UPGMA method version %s\n\n",PHY_VERSION);
  printf("\nNeighbor-Joining/UPGMA method version %s\n\n",PHY_VERSION);
  putchar('\n');
  jumble = false;
  lower = false;
  outgrno = 1;
  outgropt = false;
  replicates = false;
  trout = true;
  upper = false;
  printdata = false;
  progress = false;
  treeprint = false; // true;
  njoin = true;
  loopcount = 0;
  /*for(;;) {
    cleerhome();
    printf("\nNeighbor-Joining/UPGMA method version %s\n\n",PHY_VERSION);
    printf("Settings for this run:\n");
    printf("  N       Neighbor-joining or UPGMA tree?  %s\n",
           (njoin ? "Neighbor-joining" : "UPGMA"));
    if (njoin) {
      printf("  O                        Outgroup root?");
      if (outgropt)
        printf("  Yes, at species number%3ld\n", outgrno);
      else
        printf("  No, use as outgroup species%3ld\n", outgrno);
    }
    printf("  L         Lower-triangular data matrix?  %s\n",
           (lower ? "Yes" : "No"));
    printf("  R         Upper-triangular data matrix?  %s\n",
           (upper ? "Yes" : "No"));
    printf("  S                        Subreplicates?  %s\n",
           (replicates ? "Yes" : "No"));
    printf("  J     Randomize input order of species?");
    if (jumble)
      printf("  Yes (random number seed =%8ld)\n", inseed0);
    else
      printf("  No. Use input order\n");
    printf("  M           Analyze multiple data sets?");
    if (mulsets)
      printf("  Yes, %2ld sets\n", datasets);
    else
      printf("  No\n");
    printf("  0   Terminal type (IBM PC, ANSI, none)?  %s\n",
           (ibmpc ? "IBM PC" : ansi ? "ANSI" : "(none)"));
    printf("  1    Print out the data at start of run  %s\n",
           (printdata ? "Yes" : "No"));
    printf("  2  Print indications of progress of run  %s\n",
           (progress ? "Yes" : "No"));
    printf("  3                        Print out tree  %s\n",
           (treeprint ? "Yes" : "No"));
    printf("  4       Write out trees onto tree file?  %s\n",
           (trout ? "Yes" : "No"));
    printf("\n\n  Y to accept these or type the letter for one to change\n");
#ifdef WIN32
    phyFillScreenColor();
#endif
    fflush(stdout);
    scanf("%c%*[^\n]", &ch);
    getchar();
    if (ch == '\n')
      ch = ' ';
    uppercase(&ch);
    if  (ch == 'Y')
      break;
    if (strchr("NJOULRSM01234",ch) != NULL){
      switch (ch) {
        
      case 'J':
        jumble = !jumble;
         if (jumble)
          initseed(&inseed, &inseed0, seed);
        break;
        
      case 'L':
        lower = !lower;
        break;
        
      case 'O':
        outgropt = !outgropt;
        if (outgropt)
          initoutgroup(&outgrno, spp);
        else
          outgrno = 1;
        break;
        
      case 'R':
        upper = !upper;
        break;
        
      case 'S':
        replicates = !replicates;
        break;
        
      case 'N':
        njoin = !njoin;
        break;
        
      case 'M':
        mulsets = !mulsets;
        if (mulsets)
          initdatasets(&datasets);
        jumble = true;
         if (jumble)
          initseed(&inseed, &inseed0, seed);
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
        
      case '3':
        treeprint = !treeprint;
        break;
        
      case '4':
        trout = !trout;
        break;
      }
    } else
      printf("Not a possible option!\n");
    countup(&loopcount, 100);
  }*/

}  /* getoptions */


void neighbor_allocrest()
{
  long i;

  x = (vector *)Malloc(spp*sizeof(vector));
  for (i = 0; i < spp; i++)
    x[i] = (vector)Malloc(spp*sizeof(double));
  reps = (intvector *)Malloc(spp*sizeof(intvector));
  for (i = 0; i < spp; i++)
    reps[i] = (intvector)Malloc(spp*sizeof(long));
  nayme = (naym *)Malloc(spp*sizeof(naym));
  enterorder = (long *)Malloc(spp*sizeof(long));
  cluster = (node **)Malloc(spp*sizeof(node *));
}  /* allocrest */


void freerest()
{
  long i;

  for (i = 0; i < spp; i++)
    free(x[i]);
  free(x);
  for (i = 0; i < spp; i++)
    free(reps[i]);
  free(reps);
  free(nayme);
  free(enterorder);
  free(cluster);
}  /* freerest */


void inputnumbers2_modified(long *spp, long *nonodes, long n)
{
    *nonodes = *spp * 2 - n;
}  /* inputnumbers2 */


void neighbor_doinit_modified(U2::MemoryLocker& memLocker){
    /* initializes variables */
    node *p;

    inputnumbers2_modified(&spp, &nonodes2, 2);
    nonodes2 += (njoin ? 0 : 1);
    neighbor_getoptions();
    dist_alloctree(&curtree.nodep, nonodes2+1, memLocker);
    if(memLocker.hasError()) {
        return;
    }
    p = curtree.nodep[nonodes2]->next;
    curtree.nodep[nonodes2]->next = curtree.nodep[nonodes2];
    free(p->next);
    free(p);
    neighbor_allocrest();

}  /* doinit */



void neighbor_doinit(U2::MemoryLocker& memLocker){
  /* initializes variables */
  node *p;

  inputnumbers2(&spp, &nonodes2, 2);
  nonodes2 += (njoin ? 0 : 1);
  neighbor_getoptions();
  dist_alloctree(&curtree.nodep, nonodes2+1, memLocker);
  p = curtree.nodep[nonodes2]->next;
  curtree.nodep[nonodes2]->next = curtree.nodep[nonodes2];
  free(p->next);
  free(p);
  neighbor_allocrest();
  
}  /* doinit */


void neighbor_inputoptions()
{
  /* read options information */
  assert(ith == 1);
  if (ith != 1)
    samenumsp2(ith);
  //putc('\n', outfile);
  if (njoin) {
    //fprintf(outfile, " Neighbor-joining method\n");
    printf("Neighbor-joining method\n");
  }
  else {
    //fprintf(outfile, " UPGMA method\n");
  }
  //fprintf(outfile, "\n Negative branch lengths allowed\n\n");
}  /* inputoptions */


void describe(node *p, double height)
{
  /* print out information for one branch */
  long i;
  node *q;

  q = p->back;
  if (njoin)
    fprintf(outfile, "%4ld          ", q->index - spp);
  else
    fprintf(outfile, "%4ld     ", q->index - spp);
  if (p->tip) {
    for (i = 0; i < nmlngth; i++)
      putc(nayme[p->index - 1][i], outfile);
    putc(' ', outfile);
  } else {
    if (njoin)
      fprintf(outfile, "%4ld       ", p->index - spp);
    else {
      fprintf(outfile, "%4ld       ", p->index - spp);
    }
  }
  if (njoin)
    fprintf(outfile, "%12.5f\n", q->v);
  else
    fprintf(outfile, "%10.5f      %10.5f\n", q->v, q->v+height);
  if (!p->tip) {
    describe(p->next->back, height+q->v);
    describe(p->next->next->back, height+q->v);
  }
}  /* describe */


void summarize()
{
  /* print out branch lengths etc. */
  putc('\n', outfile);
  if (njoin) {
    fprintf(outfile, "remember:");
    if (outgropt)
      fprintf(outfile, " (although rooted by outgroup)");
    fprintf(outfile, " this is an unrooted tree!\n");
  }
  if (njoin) {
    fprintf(outfile, "\nBetween        And            Length\n");
    fprintf(outfile, "-------        ---            ------\n");
  } else {
    fprintf(outfile, "From     To            Length          Height\n");
    fprintf(outfile, "----     --            ------          ------\n");
  }
  describe(curtree.start->next->back, 0.0);
  describe(curtree.start->next->next->back, 0.0);
  if (njoin)
    describe(curtree.start->back, 0.0);
  fprintf(outfile, "\n\n");
}  /* summarize */


void nodelabel(boolean isnode)
{
  if (isnode)
    printf("node");
  else
    printf("species");
}  /* nodelabel */


void jointree()
{
  /* calculate the tree */
  long nc, nextnode, mini=0, minj=0, i, j, ia, ja, ii, jj, nude, iter;
  double fotu2, total, tmin, dio, djo, bi, bj, bk, dmin=0, da;
  long el[3];
  vector av;
  intvector oc;

  double *R;   /* added in revisions by Y. Ina */
  R = (double *)Malloc(spp * sizeof(double));

  for (i = 0; i <= spp - 2; i++) {
    for (j = i + 1; j < spp; j++) {
      da = (x[i][j] + x[j][i]) / 2.0;
      x[i][j] = da;
      x[j][i] = da;
    }
  }
  /* First initialization */
  fotu2 = spp - 2.0;
  nextnode = spp + 1;
  av = (vector)Malloc(spp*sizeof(double));
  oc = (intvector)Malloc(spp*sizeof(long));
  for (i = 0; i < spp; i++) {
    av[i] = 0.0;
    oc[i] = 1;
  }
  /* Enter the main cycle */
  if (njoin)
    iter = spp - 3;
  else
    iter = spp - 1;
  for (nc = 1; nc <= iter; nc++) {
    for (j = 2; j <= spp; j++) {
      for (i = 0; i <= j - 2; i++)
        x[j - 1][i] = x[i][j - 1];
    }
    tmin = DBL_MAX;
    /* Compute sij and minimize */
    if (njoin) {     /* many revisions by Y. Ina from here ... */
      for (i = 0; i < spp; i++)
        R[i] = 0.0;
      for (ja = 2; ja <= spp; ja++) {
        jj = enterorder[ja - 1];
        if (cluster[jj - 1] != NULL) {
          for (ia = 0; ia <= ja - 2; ia++) {
            ii = enterorder[ia];
            if (cluster[ii - 1] != NULL) {
              R[ii - 1] += x[ii - 1][jj - 1];
              R[jj - 1] += x[ii - 1][jj - 1];
            }
          }
        }
      }
    } /* ... to here */
    for (ja = 2; ja <= spp; ja++) {
      jj = enterorder[ja - 1];
      if (cluster[jj - 1] != NULL) {
        for (ia = 0; ia <= ja - 2; ia++) {
          ii = enterorder[ia];
          if (cluster[ii - 1] != NULL) {
            if (njoin) {
              total = fotu2 * x[ii - 1][jj - 1] - R[ii - 1] - R[jj - 1];
               /* this statement part of revisions by Y. Ina */
            } else
              total = x[ii - 1][jj - 1];
            if (total < tmin) {
              tmin = total;
              mini = ii;
              minj = jj;
            }
          }
        }
      }
    }
    /* compute lengths and print */
    if (njoin) {
      dio = 0.0;
      djo = 0.0;
      for (i = 0; i < spp; i++) {
        dio += x[i][mini - 1];
        djo += x[i][minj - 1];
      }
      dmin = x[mini - 1][minj - 1];
      dio = (dio - dmin) / fotu2;
      djo = (djo - dmin) / fotu2;
      bi = (dmin + dio - djo) * 0.5;
      bj = dmin - bi;
      bi -= av[mini - 1];
      bj -= av[minj - 1];
    } else {
      bi = x[mini - 1][minj - 1] / 2.0 - av[mini - 1];
      bj = x[mini - 1][minj - 1] / 2.0 - av[minj - 1];
      av[mini - 1] += bi;
    }
    if (progress) {
      printf("Cycle %3ld: ", iter - nc + 1);
      if (njoin)
        nodelabel((boolean)(av[mini - 1] > 0.0));
      else
        nodelabel((boolean)(oc[mini - 1] > 1.0));
      printf(" %ld (%10.5f) joins ", mini, bi);
      if (njoin)
        nodelabel((boolean)(av[minj - 1] > 0.0));
      else
        nodelabel((boolean)(oc[minj - 1] > 1.0));
      printf(" %ld (%10.5f)\n", minj, bj);
#ifdef WIN32
      phyFillScreenColor();
#endif
    }
    hookup(curtree.nodep[nextnode - 1]->next, cluster[mini - 1]);
    hookup(curtree.nodep[nextnode - 1]->next->next, cluster[minj - 1]);
    cluster[mini - 1]->v = bi;
    cluster[minj - 1]->v = bj;
    cluster[mini - 1]->back->v = bi;
    cluster[minj - 1]->back->v = bj;
    cluster[mini - 1] = curtree.nodep[nextnode - 1];
    cluster[minj - 1] = NULL;
    nextnode++;
    if (njoin)
      av[mini - 1] = dmin * 0.5;
    /* re-initialization */
    fotu2 -= 1.0;
    for (j = 0; j < spp; j++) {
      if (cluster[j] != NULL) {
        if (njoin) {
          da = (x[mini - 1][j] + x[minj - 1][j]) * 0.5;
          if (mini - j - 1 < 0)
            x[mini - 1][j] = da;
          if (mini - j - 1 > 0)
            x[j][mini - 1] = da;
        } else {
          da = x[mini - 1][j] * oc[mini - 1] + x[minj - 1][j] * oc[minj - 1];
          da /= oc[mini - 1] + oc[minj - 1];
          x[mini - 1][j] = da;
          x[j][mini - 1] = da;
        }
      }
    }
    for (j = 0; j < spp; j++) {
      x[minj - 1][j] = 0.0;
      x[j][minj - 1] = 0.0;
    }
    oc[mini - 1] += oc[minj - 1];
  }
  /* the last cycle */
  nude = 1;
  for (i = 1; i <= spp; i++) {
    if (cluster[i - 1] != NULL) {
      el[nude - 1] = i;
      nude++;
    }
  }
  if (!njoin) {
    curtree.start = cluster[el[0] - 1];
    curtree.start->back = NULL;
    free(av);
    free(oc);
    return;
  }
  bi = (x[el[0] - 1][el[1] - 1] + x[el[0] - 1][el[2] - 1] - x[el[1] - 1]
        [el[2] - 1]) * 0.5;
  bj = x[el[0] - 1][el[1] - 1] - bi;
  bk = x[el[0] - 1][el[2] - 1] - bi;
  bi -= av[el[0] - 1];
  bj -= av[el[1] - 1];
  bk -= av[el[2] - 1];
  if (progress) {
    printf("last cycle:\n");
    putchar(' ');
    nodelabel((boolean)(av[el[0] - 1] > 0.0));
    printf(" %ld  (%10.5f) joins ", el[0], bi);
    nodelabel((boolean)(av[el[1] - 1] > 0.0));
    printf(" %ld  (%10.5f) joins ", el[1], bj);
    nodelabel((boolean)(av[el[2] - 1] > 0.0));
    printf(" %ld  (%10.5f)\n", el[2], bk);
#ifdef WIN32
    phyFillScreenColor();
#endif
  }
  hookup(curtree.nodep[nextnode - 1], cluster[el[0] - 1]);
  hookup(curtree.nodep[nextnode - 1]->next, cluster[el[1] - 1]);
  hookup(curtree.nodep[nextnode - 1]->next->next, cluster[el[2] - 1]);
  cluster[el[0] - 1]->v = bi;
  cluster[el[1] - 1]->v = bj;
  cluster[el[2] - 1]->v = bk;
  cluster[el[0] - 1]->back->v = bi;
  cluster[el[1] - 1]->back->v = bj;
  cluster[el[2] - 1]->back->v = bk;
  curtree.start = cluster[el[0] - 1]->back;
  free(av);
  free(oc);
  free(R);
}  /* jointree */


void maketree()
{
  /* construct the tree */
  long i ;

  dist_inputdata_modified(replicates, printdata, lower, upper, x, reps);
  if (njoin && (spp < 3)) {
    printf("\nERROR: Neighbor-Joining runs must have at least 3 species\n\n");
    exxit(-1);
  }
  if (progress)
    putchar('\n');
  if (ith == 1)
    dist_setuptree(&curtree, nonodes2 + 1);
  for (i = 1; i <= spp; i++)
    enterorder[i - 1] = i;
  if (jumble)
    randumize(seed, enterorder);
  for (i = 0; i < spp; i++)
    cluster[i] = curtree.nodep[i];
  jointree();
  if (njoin)
    curtree.start = curtree.nodep[outgrno - 1]->back;
  dist_printree(curtree.start, treeprint, njoin, (boolean)(!njoin));
  if (treeprint)
    summarize();
  if (trout) {
    col = 0;
    if (njoin)
      dist_treeout(curtree.start, &col, 0.43429448222, njoin, curtree.start);
    else
      curtree.root = curtree.start,
      treeoutr(curtree.start,&col,&curtree);
  }
  if (progress) {
    printf("\nOutput written on file \"%s\"\n\n", outfilename);
    if (trout)
      printf("Tree written on file \"%s\"\n\n", outtreename);
  }
}  /* maketree */


void neighbour_init(int num, U2::MemoryLocker& memLocker, const QString& filename) 
{
    int argc = 1;                
    const char* argv[] = { "Neighbor" };
    spp = num;
    //openfile(&infile,INFILE,"input file", "r",argv[0],infilename);
    //openfile(&outfile,OUTFILE,"output file", "w",argv[0],outfilename);
    ibmpc = IBMCRT;
    ansi = ANSICRT;
    mulsets = false;
    datasets = 1;
    neighbor_doinit_modified(memLocker);
    if(filename == NULL){
        trout = false;
    }
    if (trout) {
        //openfile(&outtree,OUTTREE,"output tree file", "w",argv[0],outtreename);
        outtree = fopen(filename.toStdString().c_str(), "a");
    }

}


const tree* neighbour_calc_tree() 
{
    ith = 1;
    while (ith <= datasets) {
        if (datasets > 1) {
            fprintf(outfile, "Data set # %ld:\n",ith);
            if (progress)
                printf("Data set # %ld:\n",ith);
        }
        neighbor_inputoptions();
        maketree();
        //if (eoln(infile) && (ith < datasets)) 
        //    scan_eoln(infile);
        ith++;
    }
    return &curtree;
}

void neighbour_free_resources()
{
    //FClose(infile);
    //FClose(outfile);
    FClose(outtree);
    freerest();
    dist_freetree(&curtree.nodep, nonodes2+1);
#ifdef MAC
    fixmacfile(outfilename);
    fixmacfile(outtreename);
#endif
    printf("Done.\n\n");
//#ifdef WIN32
//    phyRestoreConsoleAttributes();
//#endif

}

naym* getNayme()
{
    return nayme;
}




