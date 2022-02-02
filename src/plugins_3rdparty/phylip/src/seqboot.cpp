#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

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

#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/U2OpStatusUtils.h>

#include "seqboot.h"


long loci, maxalleles, groups, 
nenzymes, reps, ws, blocksize, maxnewsites;

Phylip_Char **nodep_boot;           /* molecular or morph data */

Phylip_Char *factor = NULL;  /* factor[sites] - direct read-in of factors file */
long *factorr = NULL; /* [0..sites-1] => nondecreasing [1..groups] */

long *alleles = NULL;

/* Mapping with read-in weights eliminated
* Allocated once in allocnew() */
long newsites;
long newgroups;
long *newwhere   = NULL;    /* Map [0..newgroups-1] => [1..newsites] */
long *newhowmany = NULL;    /* Number of chars for each [0..newgroups-1] */

/* Mapping with bootstrapped weights applied */
/* (re)allocated by allocnewer() */
long newersites, newergroups;
long *newerfactor  = NULL;  /* Map [0..newersites-1] => [1..newergroups] */
long *newerwhere   = NULL;  /* Map [0..newergroups-1] => [1..newersites] */
long *newerhowmany = NULL;  /* Number of chars for each [0..newergroups-1] */
long **charorder   = NULL;  /* Permutation [0..spp-1][0..newergroups-1] */
long **sppord      = NULL;  /* Permutation [0..newergroups-1][0..spp-1] */

longer seed_boot;



Phylip_Char ** getData(){
    return nodep_boot;
}


void seqboot_getoptions(void)
{
  /* interactively set options */
//  long reps0;
//  long inseed, inseed0, loopcount, loopcount2;
//  Phylip_Char ch;
//  boolean done1;

  data = seqs;
  seq = dnaSeq;
  bootstrap = true;
  jackknife = false;
  permute = false;
  ild = false;
  lockhart = false;
  blocksize = 1;
  regular = true;
  fracsample = 1.0;
  all = false;
  reps = 100;
  weights = false;
  mixture = false;
  ancvar = false;
  categories = false;
  justwts = false;
  printdata = false;
  dotdiff = true;
  progress = false;
  interleaved = true;
  xml = false;
  nexus = false;
  factors = false;
  //loopcount = 0;

 }  /* seqboot_getoptions */


void seqboot_inputnumbers()
{
  /* read numbers of species and of sites */
  long i;

  //fscanf(infile, "%ld%ld", &spp, &sites);
  loci = sites;
  maxalleles = 1;
  if (data == restsites && enzymes)
    //fscanf(infile, "%ld", &nenzymes);
  if (data == genefreqs) {
    alleles = (long *)Malloc(sites*sizeof(long));
    scan_eoln(infile);
    sites = 0;
    for (i = 0; i < (loci); i++) {
      if (eoln(infile)) 
        scan_eoln(infile);
      //fscanf(infile, "%ld", &alleles[i]);
      if (alleles[i] > maxalleles)
         maxalleles = alleles[i];
      if (all)
         sites += alleles[i];
      else
         sites += alleles[i] - 1;
    }
    if (!all)
      maxalleles--;
    scan_eoln(infile);
  }
}  /* seqboot_inputnumbers */


void seqboot_inputfactors()
{
  long i, j;
  Phylip_Char ch, prevch;

  prevch = ' ';
  j = 0;
  for (i = 0; i < (sites); i++) {
    do {
      if (eoln(factfile)) 
        scan_eoln(factfile);
      ch = gettc(factfile);
    } while (ch == ' ');
    if (ch != prevch)
      j++;
    prevch = ch;
    factorr[i] = j;
  }
  scan_eoln(factfile);
}  /* seqboot_inputfactors */


void seq_inputoptions()
{
  /* input the information on the options */
  long weightsum, maxfactsize, i, j, k, l, m;

  if (data == genefreqs) {
    k = 0;
    l = 0;
    for (i = 0; i < (loci); i++) {
      if (all)
        m = alleles[i];
      else
        m = alleles[i] - 1;
      k++;
      for (j = 1; j <= m; j++) {
        l++;
        factorr[l - 1] = k;
      }
    }
  } else {
    for (i = 1; i <= (sites); i++)
      factorr[i - 1] = i;
  }
  if(factors){
    seqboot_inputfactors();
  }
  for (i = 0; i < (sites); i++)
    oldweight[i] = 1;
  if (weights)
    inputweights2(0, sites, &weightsum, oldweight, &weights, "seqboot");
  if (factors && printdata) {
    for(i = 0; i < sites; i++)
      factor[i] = (char)('0' + (factorr[i]%10));
    printfactors(outfile, sites, factor, " (least significant digit)");
  }
  if (weights && printdata)
    printweights(outfile, 0, sites, oldweight, "Sites");
  for (i = 0; i < (loci); i++)
    how_many[i] = 0;
  for (i = 0; i < (loci); i++)
    where[i] = 0;
  for (i = 1; i <= (sites); i++) {
    how_many[factorr[i - 1] - 1]++;
    if (where[factorr[i - 1] - 1] == 0)
      where[factorr[i - 1] - 1] = i;
  }
  groups = factorr[sites - 1];
  newgroups = 0;
  newsites = 0;
  maxfactsize = 0;
  for(i = 0 ; i < loci ; i++){
    if(how_many[i] > maxfactsize){
      maxfactsize = how_many[i];
    }
  }
  maxnewsites = groups * maxfactsize;
  allocnew();
  for (i = 0; i < groups; i++) {
    if (oldweight[where[i] - 1] > 0) {
      newgroups++;
      newsites += how_many[i];
      newwhere[newgroups - 1] = where[i];
      newhowmany[newgroups - 1] = how_many[i];
    }
  }
}  /* inputoptions */


char **matrix_char_new(long rows, long cols)
{
  char **mat;
  long i;

  assert(rows > 0); assert(cols > 0);

  mat = (char **)Malloc(rows*sizeof(char *));
  for (i = 0; i < rows; i++)
    mat[i] = (char *)Malloc(cols*sizeof(char));

  return mat;
}


void matrix_char_delete(char **mat, long rows)
{
  long i;
  
  assert(mat != NULL);
  for (i = 0; i < rows; i++)
    free(mat[i]);
  free(mat);
}


double **matrix_double_new(long rows, long cols)
{
  double **mat;
  long i;

  assert(rows > 0); assert(cols > 0);
  
  mat = (double **)Malloc(rows*sizeof(double *));
  for (i = 0; i < rows; i++)
    mat[i] = (double *)Malloc(cols*sizeof(double));

  return mat;
}


void matrix_double_delete(double **mat, long rows)
{
  long i;
  
  assert(mat != NULL);
  for (i = 0; i < rows; i++)
    free(mat[i]);
  free(mat);
}


void seqboot_inputdata()
{
  /* input the names and sequences for each species */
  long i, j, k, l, m, n, basesread, basesnew=0;
  double x;
  Phylip_Char charstate;
  boolean allread, done;

  if (data == genefreqs) {
    nodef = matrix_double_new(spp, sites);
  } else {
    nodep_boot = matrix_char_new(spp, sites);
  }
  j = nmlngth + (sites + (sites - 1) / 10) / 2 - 5;
  if (j < nmlngth - 1)
    j = nmlngth - 1;
  if (j > 37)
    j = 37;
  if (printdata) {
    fprintf(outfile, "\nBootstrapping algorithm, version %s\n\n\n",PHY_VERSION);
    if (bootstrap)  {
      if (blocksize > 1) {
        if (regular)      
      fprintf(outfile, "Block-bootstrap with block size %ld\n\n", blocksize);
        else
          fprintf(outfile, "Partial (%2.0f%%) block-bootstrap with block size %ld\n\n",
                  100*fracsample, blocksize);
      } else {
        if (regular)
          fprintf(outfile, "Bootstrap\n\n");
        else 
          fprintf(outfile, "Partial (%2.0f%%) bootstrap\n\n", 100*fracsample);
      }
    } else {
      if (jackknife) {
        if (regular)
          fprintf(outfile, "Delete-half Jackknife\n\n");
        else
    fprintf(outfile, "Delete-%2.0f%% Jackknife\n\n", 100*(1.0-fracsample));
      } else {
        if (permute) {
          fprintf(outfile, "Species order permuted separately for each");
          if (data == genefreqs)
            fprintf(outfile, " locus\n\n");
          if (data == seqs)
            fprintf(outfile, " site\n\n");
          if (data == morphology)
            fprintf(outfile, " character\n\n");
          if (data == restsites)
            fprintf(outfile, " site\n\n");
        }
        else {
          if (ild) {
            if (data == genefreqs)
              fprintf(outfile, "Locus");
            if (data == seqs)
              fprintf(outfile, "Site");
            if (data == morphology)
              fprintf(outfile, "Character");
            if (data == restsites)
              fprintf(outfile, "Site");
            fprintf(outfile, " order permuted\n\n");
          } else {
            if (lockhart)
              if (data == genefreqs)
                fprintf(outfile, "Locus");
              if (data == seqs)
                fprintf(outfile, "Site");
              if (data == morphology)
                fprintf(outfile, "Character");
              if (data == restsites)
                fprintf(outfile, "Site");
         fprintf(outfile, " order permuted separately for each species\n\n");
          }
        }
      }
    }
    if (data == genefreqs)
      fprintf(outfile, "%3ld species, %3ld  loci\n\n", spp, loci);
    else {
      fprintf(outfile, "%3ld species, ", spp);
      if (data == seqs)
        fprintf(outfile, "%3ld  sites\n\n", sites);
        else if (data == morphology)
          fprintf(outfile, "%3ld  characters\n\n", sites);
          else if (data == restsites)
            fprintf(outfile, "%3ld  sites\n\n", sites);
    }
    fprintf(outfile, "Name");
    for (i = 1; i <= j; i++)
      putc(' ', outfile);
    fprintf(outfile, "Data\n");
    fprintf(outfile, "----");
    for (i = 1; i <= j; i++)
      putc(' ', outfile);
    fprintf(outfile, "----\n\n");
  }
  interleaved = (interleaved && ((data == seqs) || (data == restsites)));
  if (data == genefreqs) {
    for (i = 1; i <= (spp); i++) {
      initname(i - 1);
      j = 1;
      while (j <= sites && !eoff(infile)) {
        if (eoln(infile)) 
          scan_eoln(infile);
        if ( fscanf(infile, "%lf", &x) != 1) {
          printf("ERROR: Invalid value for locus %ld of species %ld\n", j, i);
          exxit(-1);
        } else if ((unsigned)x > 1.0) {
          printf("GENE FREQ OUTSIDE [0,1] in species %ld\n", i);
          exxit(-1);
        } else {
          nodef[i - 1][j - 1] = x;
          j++;
        }
      }
      scan_eoln(infile);
    }
    return;
  }
  basesread = 0;
  allread = false;
  while (!allread) {
    /* eat white space -- if the separator line has spaces on it*/
    do {
      charstate = gettc(infile);
    } while (charstate == ' ' || charstate == '\t');
    ungetc(charstate, infile);
    if (eoln(infile)) 
      scan_eoln(infile);
    i = 1;
    while (i <= spp) {
      if ((interleaved && basesread == 0) || !interleaved)
        initname(i-1);
      j = interleaved ? basesread : 0;
      done = false;
      while (!done && !eoff(infile)) {
        if (interleaved)
          done = true;
        while (j < sites && !(eoln(infile) ||eoff(infile))) {
          charstate = gettc(infile);
          if (charstate == '\n' || charstate == '\t')
            charstate = ' ';
          if (charstate == ' ' ||
              (data == seqs && charstate >= '0' && charstate <= '9'))
            continue;
          uppercase(&charstate);
          j++;
          if (charstate == '.')
            charstate = nodep_boot[0][j-1];
          nodep_boot[i-1][j-1] = charstate;
        }
        if (interleaved)
          continue;
        if (j < sites) 
          scan_eoln(infile);
        else if (j == sites)
          done = true;
      }
      if (interleaved && i == 1)
        basesnew = j;
      scan_eoln(infile);
      if ((interleaved && j != basesnew) || ((!interleaved) && j != sites)){
        printf("\n\nERROR: sequences out of alignment at site %ld", j+1);
        printf(" of species %ld\n\n", i);
        exxit(-1);}
      i++;
    }
    if (interleaved) {
      basesread = basesnew;
      allread = (basesread == sites);
    } else
      allread = (i > spp);
  }
  if (!printdata)
    return;
  if (data == genefreqs)
    m = (sites - 1) / 8 + 1;
  else
    m = (sites - 1) / 60 + 1;
  for (i = 1; i <= m; i++) {
    for (j = 0; j < spp; j++) {
      for (k = 0; k < nmlngth; k++)
        putc(nayme[j][k], outfile);
      fprintf(outfile, "   ");
      if (data == genefreqs)
        l = i * 8;
      else
        l = i * 60;
      if (l > sites)
        l = sites;
      if (data == genefreqs)
        n = (i - 1) * 8;
      else
        n = (i - 1) * 60;
      for (k = n; k < l; k++) {
        if (data == genefreqs)
          fprintf(outfile, "%8.5f", nodef[j][k]);
        else {
          if (j + 1 > 1 && nodep_boot[j][k] == nodep_boot[0][k])
            charstate = '.';
          else
            charstate = nodep_boot[j][k];
          putc(charstate, outfile);
          if ((k + 1) % 10 == 0 && (k + 1) % 60 != 0)
            putc(' ', outfile);
        
        }
      }
      putc('\n', outfile);
    }
    putc('\n', outfile);
  }
  putc('\n', outfile);
}  /* seqboot_inputdata */


void seq_allocrest()
{ /* allocate memory for bookkeeping arrays */

  oldweight = (steptr)Malloc(sites*sizeof(long));
  weight = (steptr)Malloc(sites*sizeof(long));
  if (categories)
    category = (steptr)Malloc(sites*sizeof(long));
  if (mixture)
    mixdata = (steptr)Malloc(sites*sizeof(long));
  if (ancvar)
    ancdata = (steptr)Malloc(sites*sizeof(long));
  where = (steptr)Malloc(loci*sizeof(long));
  how_many = (steptr)Malloc(loci*sizeof(long));
  factor = (Phylip_Char *)Malloc(sites*sizeof(Phylip_Char));
  factorr = (steptr)Malloc(sites*sizeof(long));
  nayme = (naym *)Malloc(spp*sizeof(naym));
}  /* allocrest */

void seq_freerest()
{
  /* Free bookkeeping arrays */
    if (alleles){
        free(alleles);
        alleles = NULL;
    }
  free(oldweight);
  free(weight);
  if (categories)
    free(category);
  if (mixture)
    free(mixdata);
  if (ancvar)
    free(ancdata);
  free(where);
  free(how_many);
  free(factor);
  factor = NULL;
  free(factorr);
  factorr = NULL;
  free(nayme);
}


void allocnew(void)
{ /* allocate memory for arrays that depend on the lenght of the 
     output sequence*/
  /* Only call this function once */
  assert(newwhere == NULL && newhowmany == NULL); 

  newwhere = (steptr)Malloc(loci*sizeof(long));
  newhowmany = (steptr)Malloc(loci*sizeof(long));
} /* allocnew */


void freenew(void)
{ /* free arrays allocated by allocnew() */
  /* Only call this function once */
  assert(newwhere != NULL);
  assert(newhowmany != NULL);

  free(newwhere);
  free(newhowmany);

  newwhere = NULL;
  newhowmany = NULL;
}


void allocnewer(long newergroups, long newersites)
{ /* allocate memory for arrays that depend on the length of the bootstrapped
     output sequence */
  /* Assumes that spp remains constant */
  static long curnewergroups = 0;
  static long curnewersites  = 0;

  long i;

  if (newerwhere != NULL) {
    if (newergroups > curnewergroups) {
      free(newerwhere);
      newerwhere = NULL;
      free(newerhowmany);
      for (i = 0; i < spp; i++)
        free(charorder[i]);
      newerwhere = NULL;
    }
    if (newersites > curnewersites) {
      free(newerfactor);
      newerfactor = NULL;
    }
  }

  if (charorder == NULL)
    charorder = (steptr *)Malloc(spp*sizeof(steptr));

  /* Malloc() will fail if either is 0, so add a dummy element */
  if (newergroups == 0)
    newergroups++;
  if (newersites == 0)
    newersites++;
  
  if (newerwhere == NULL) {
    newerwhere = (steptr)Malloc(newergroups*sizeof(long));
    newerhowmany = (steptr)Malloc(newergroups*sizeof(long));
    for (i = 0; i < spp; i++)
      charorder[i] = (steptr)Malloc(newergroups*sizeof(long));
    curnewergroups = newergroups;
  }
  if (newerfactor == NULL) {
    newerfactor = (steptr)Malloc(newersites*sizeof(long));
    curnewersites = newersites;
  }
}


void freenewer()
{
  /* Free memory allocated by allocnewer() */
  /* spp must be the same as when allocnewer was called */
  long i;

  if (newerwhere) {
    free(newerwhere);
    newerwhere = NULL;
    free(newerhowmany);
    newerhowmany = NULL;

    free(newerfactor);
    newerfactor = NULL;
    for (i = 0; i < spp; i++)
      free(charorder[i]);
    free(charorder);
    charorder = NULL;
  }
}


void doinput(int /*argc*/, Phylip_Char *argv[])
{ /* reads the input data */
  seqboot_getoptions();
  seqboot_inputnumbers();
  seq_allocrest();
  if (weights)
    openfile(&weightfile,WEIGHTFILE,"input weight file",
               "r",argv[0],weightfilename);
  if (mixture){
    openfile(&mixfile,MIXFILE,"mixture file", "r",argv[0],mixfilename);
    openfile(&outmixfile,"outmixture","output mixtures file","w",argv[0],
               outmixfilename);
//    seqboot_inputaux(mixdata, mixfile);
  }
  if (ancvar){
    openfile(&ancfile,ANCFILE,"ancestor file", "r",argv[0],ancfilename);
    openfile(&outancfile,"outancestors","output ancestors file","w",argv[0],
               outancfilename);
//    seqboot_inputaux(ancdata, ancfile);
  }
  if (categories) {
    openfile(&catfile,CATFILE,"input category file","r",argv[0],catfilename);
    openfile(&outcatfile,"outcategories","output category file","w",argv[0],
               outcatfilename);
    inputcategs(0, sites, category, 9, "SeqBoot");
  }
  if (factors){
    openfile(&factfile,FACTFILE,"factors file","r",argv[0],factfilename);
    openfile(&outfactfile,"outfactors","output factors file","w",argv[0],
               outfactfilename);
  }
  if (justwts && !permute)
    openfile(&outweightfile,"outweights","output weight file",
               "w",argv[0],outweightfilename);
  else {
    openfile(&outfile,OUTFILE,"output data file","w",argv[0],outfilename);
  }
  seq_inputoptions();
  seqboot_inputdata();
}  /* doinput */


void bootweights()
{ /* sets up weights by resampling data */
  long i, j, k, blocks;
  double p, q, r;
  long grp = 0, site = 0;

  ws = newgroups;
  for (i = 0; i < (ws); i++)
    weight[i] = 0;
  if (jackknife) {
    if (fabs(newgroups*fracsample - (long)(newgroups*fracsample+0.5))
       > 0.00001) {
      if (randum(seed_boot)
          < (newgroups*fracsample - (long)(newgroups*fracsample))
            /((long)(newgroups*fracsample+1.0)-(long)(newgroups*fracsample)))
        q = (long)(newgroups*fracsample)+1;
      else
        q = (long)(newgroups*fracsample);
    } else
      q = (long)(newgroups*fracsample+0.5);
    r = newgroups;
    p = q / r;
    ws = 0;
    for (i = 0; i < (newgroups); i++) {
      if (randum(seed_boot) < p) {
        weight[i]++;
        ws++;
        q--;
      }
      r--;
      if (i + 1 < newgroups)
        p = q / r;
    }
  } else if (permute) {
    for (i = 0; i < (newgroups); i++)
      weight[i] = 1;
  } else if (bootstrap) {
    blocks = fracsample * newgroups / blocksize;
    for (i = 1; i <= (blocks); i++) {
      j = (long)(newgroups * randum(seed_boot)) + 1;
      for (k = 0; k < blocksize; k++) {
        weight[j - 1]++;
        j++;
        if (j > newgroups)
          j = 1;
      }
    }
  } else             /* case of rewriting data */
    for (i = 0; i < (newgroups); i++)
      weight[i] = 1;

  /* Count number of replicated groups */
  newergroups = 0;
  newersites  = 0;
  for (i = 0; i < newgroups; i++) {
    newergroups += weight[i];
    newersites  += newhowmany[i] * weight[i];
  }

  if (newergroups < 1) {
    fprintf(stdout, "ERROR: sampling frequency or number of sites is too small\n");
    exxit(-1);
  }
  
  /* reallocate "newer" arrays, sized by output groups:
   * newerfactor, newerwhere, newerhowmany, and charorder */
  allocnewer(newergroups, newersites);
  
  /* Replicate each group i weight[i] times */
  grp = 0;
  site = 0;
  for (i = 0; i < newgroups; i++) {
    for (j = 0; j < weight[i]; j++) {
      for (k = 0; k < newhowmany[i]; k++) {
        newerfactor[site] = grp + 1;
        site++;
      }
      newerwhere[grp] = newwhere[i];
      newerhowmany[grp] = newhowmany[i];
      grp++;
    }
  }
}  /* bootweights */


void permute_vec(long *a, long n)
{
  long i, j, k;

  for (i = 1; i < n; i++) {
    k = (long)((i+1) * randum(seed_boot));
    j = a[i];
    a[i] = a[k];
    a[k] = j;
  }
}


void sppermute(long n)
{ /* permute the species order as given in array sppord */
  permute_vec(sppord[n-1], spp);
}  /* sppermute */


void charpermute(long m, long n)
{ /* permute the n+1 characters of species m+1 */
  permute_vec(charorder[m], n);
} /* charpermute */


void writedata( QVector<U2::MultipleSequenceAlignment>& mavect, int rep, const U2::MultipleSequenceAlignment& ma)
{
    

  /* write out one set of bootstrapped sequences */
  long i, j, k, l, m, n, n2;
  double x;
  Phylip_Char charstate;

  sppord = (long **)Malloc(newergroups*sizeof(long *));
  for (i = 0; i < (newergroups); i++)
    sppord[i] = (long *)Malloc(spp*sizeof(long));
  for (j = 1; j <= spp; j++)
    sppord[0][j - 1] = j;
  for (i = 1; i < newergroups; i++) {
    for (j = 1; j <= (spp); j++)
      sppord[i][j - 1] = sppord[i - 1][j - 1];
  }
  if (!justwts || permute) {
    if (data == restsites && enzymes)
      printf("%5ld %5ld% 4ld\n", spp, newergroups, nenzymes);
    else if (data == genefreqs)
      printf("%5ld %5ld\n", spp, newergroups);
    else {
      if ((data == seqs) && rewrite && xml)
        printf("<alignment>\n");
      else 
        if (rewrite && nexus) {
          printf( "#NEXUS\n");
          printf("BEGIN DATA;\n");
          printf( "  DIMENSIONS NTAX=%ld NCHAR=%ld;\n",
                    spp, newersites);
          printf( "  FORMAT");
          if (interleaved)
            printf( " interleave=yes");
          else
            printf( " interleave=no");
          printf( " DATATYPE=");
          if (data == seqs) {
            switch (seq) { 
              case (dnaSeq): printf( "DNA missing=N gap=-"); break;
              case (rna): printf( "RNA missing=N gap=-"); break;
              case (protein):
                printf( "protein missing=? gap=-");
                break;
              }
          }
          if (data == morphology)
            printf( "STANDARD");
          printf( ";\n  MATRIX\n");
          }
        //else printf("%5ld %5ld\n", spp, newersites);
    }
    if (data == genefreqs) {
      for (i = 0; i < (newergroups); i++)
        printf(" %3ld", alleles[factorr[newerwhere[i] - 1] - 1]);
      //putchar('\n');
    }
  }
  l = 1;
  /* When rewriting to PHYLIP, only convert interleaved <-> sequential
   * for molecular and restriction sites. */
  if (( (rewrite && !nexus) )
      && ((data == seqs) || (data == restsites))) {
    interleaved = !interleaved;
    if (rewrite && xml)
      interleaved = false;
  }
  m = interleaved ? 60 : newergroups;
  do {
    if (m > newergroups)
      m = newergroups;
    for (j = 0; j < spp; j++) {
      QByteArray curAr;
      n = 0;
      if ((l == 1) || (interleaved && nexus)) {
        if (rewrite && xml) {
          printf("   <sequence");
          switch (seq) {
            case (dnaSeq): printf(" type=\"dna\""); break;
            case (rna): printf(" type=\"rna\""); break;
            case (protein): printf( " type=\"protein\""); break;
          }
          printf( ">\n");
          printf( "      <name>");
        }
        n2 = nmlngth;
        if (rewrite && (xml || nexus)) {
          while (nayme[j][n2-1] == ' ')
            n2--;
        }
        if (nexus)
          printf( "  ");
        /*for (k = 0; k < n2; k++)
          if (nexus && (nayme[j][k] == ' ') && (k < n2))
            putchar('_');
          else
            putchar(nayme[j][k]);*/
        if (rewrite && xml)
          printf( "</name>\n      <data>");
      } else {
        if (rewrite && xml) {
          printf("      ");
        }
      }
//       if (!xml) {
//         for (k = 0; k < nmlngth-n2; k++)
//           printf( " "); 
//         printf( " "); 
//       }
      for (k = l - 1; k < m; k++) {
        if (permute && j + 1 == 1)
          sppermute(newerfactor[n]);    /* we can assume chars not permuted */
        for (n2 = -1; n2 <= (newerhowmany[charorder[j][k]] - 2); n2++) {
          n++;
          if (data == genefreqs) {
            if (n > 1 && (n & 7) == 1)
              printf( "\n              ");
            x = nodef[sppord[charorder[j][k]][j] - 1]
                     [newerwhere[charorder[j][k]] + n2];
            printf("%8.5f", x);
          } else {
            if (rewrite && xml && (n > 1) && (n % 60 == 1))
              printf( "\n            ");
            else if (!nexus && !interleaved && (n > 1) && (n % 60 == 1))
                printf("\n           ");
            charstate = nodep_boot[sppord[charorder[j][k]][j] - 1]
                             [newerwhere[charorder[j][k]] + n2];
            curAr.append(charstate);
            //putCharToAl(mavect, rep, j, n, charstate);
            //putchar(charstate);
            if (n % 10 == 0 && n % 60 != 0)
              //putchar(' ');
              ;
          }
        }
      }
      if (rewrite && xml) {
        printf("</data>\n   </sequence>\n");
      }
      //putchar('\n');
      if (j >= mavect[rep]->getRowCount()) {
          mavect[rep]->addRow(ma->getMsaRow(j)->getName(), curAr);
      }
      else {
        mavect[rep]->appendChars(j,curAr.data(), curAr.length());
      }
      
    }
    if (interleaved) {
//       if ((m <= newersites) && (newersites > 60))
//         putchar('\n');
      l += 60;
      m += 60;
    }
  } while (interleaved && l <= newersites);
  if ((data == seqs) &&
      (rewrite && xml))
    printf("</alignment>\n");
  if (rewrite && nexus)
    printf("  ;\nEND;\n");
  for (i = 0; i < (newergroups); i++)
    free(sppord[i]);
  free(sppord);
  sppord = NULL;
}  /* writedata */


void writeweights()
{ /* write out one set of post-bootstrapping weights */
  long j, k, l, m, n, o;

  j = 0;
  l = 1;
  if (interleaved)
    m = 60;
  else
    m = sites;
  do {
    if(m > sites)
      m = sites;
    n = 0;
    for (k = l - 1; k < m; k++) {
      for(o = 0 ; o < how_many[k] ; o++){
        if(oldweight[k]==0){
          fprintf(outweightfile, "0");
          j++;
        }
        else{
          if (weight[k-j] < 10)
            fprintf(outweightfile, "%c", (char)('0'+weight[k-j]));
          else
            fprintf(outweightfile, "%c", (char)('A'+weight[k-j]-10));
          n++;
          if (!interleaved && n > 1 && n % 60 == 1) {
            fprintf(outweightfile, "\n");
            if (n % 10 == 0 && n % 60 != 0)
              putc(' ', outweightfile);
          }
        }
      }
    }
    putc('\n', outweightfile);
    if (interleaved) {
      l += 60;
      m += 60;
    }
  } while (interleaved && l <= sites);
}  /* writeweights */


void writecategories()
{
  /* write out categories for the bootstrapped sequences */
  long k, l, m, n, n2;
  Phylip_Char charstate;
  if(justwts){
    if (interleaved)
      m = 60;
    else
      m = sites;
    l=1;
    do {
      if(m > sites)
        m = sites;
      n=0;
      for(k=l-1 ; k < m ; k++){
        n++;
        if (!interleaved && n > 1 && n % 60 == 1)
          fprintf(outcatfile, "\n ");
        charstate =  '0' + category[k];
        putc(charstate, outcatfile);
      }
      if (interleaved) {
        l += 60;
        m += 60;
      }
    }while(interleaved && l <= sites);
    fprintf(outcatfile, "\n");
    return;
  }

  l = 1;
  if (interleaved)
    m = 60;
  else
    m = newergroups;
  do {
    if (m > newergroups)
      m = newergroups;
    n = 0;
    for (k = l - 1; k < m; k++) {
      for (n2 = -1; n2 <= (newerhowmany[k] - 2); n2++) {
        n++;
        if (!interleaved && n > 1 && n % 60 == 1)
          fprintf(outcatfile, "\n ");
        charstate = '0' + category[newerwhere[k] + n2];
        putc(charstate, outcatfile);
        if (n % 10 == 0 && n % 60 != 0)
          putc(' ', outcatfile);
      }
    }
    if (interleaved) {
      l += 60;
      m += 60;
    }
  } while (interleaved && l <= newersites);
  fprintf(outcatfile, "\n");
}  /* writecategories */


void writeauxdata(steptr auxdata, FILE *outauxfile)
{
  /* write out auxiliary option data (mixtures, ancestors, etc.) to
     appropriate file.  Samples parralel to data, or just gives one
     output entry if justwts is true */
  long k, l, m, n, n2;
  Phylip_Char charstate;

  /* if we just output weights (justwts), and this is first set
     just output the data unsampled */
  if(justwts){
    if(firstrep){
      if (interleaved)
        m = 60;
      else
        m = sites;
      l=1;
      do {
        if(m > sites)
          m = sites;
        n = 0;
        for(k=l-1 ; k < m ; k++){
        n++;
        if (!interleaved && n > 1 && n % 60 == 1)
          fprintf(outauxfile, "\n ");
          charstate = auxdata[k];
          putc(charstate, outauxfile);
        }
        if (interleaved) {
          l += 60;
          m += 60;
        }
      }while(interleaved && l <= sites);
      fprintf(outauxfile, "\n");
    }
    return;
  }

  l = 1;
  if (interleaved)
    m = 60;
  else
    m = newergroups;
  do {
    if (m > newergroups)
      m = newergroups;
    n = 0;
    for (k = l - 1; k < m; k++) {
      for (n2 = -1; n2 <= (newerhowmany[k] - 2); n2++) {
        n++;
        if (!interleaved && n > 1 && n % 60 == 1)
          fprintf(outauxfile, "\n ");
        charstate = auxdata[newerwhere[k] + n2];
        putc(charstate, outauxfile);
        if (n % 10 == 0 && n % 60 != 0)
          putc(' ', outauxfile);
      }
    }
    if (interleaved) {
      l += 60;
      m += 60;
    }
  } while (interleaved && l <= newersites);
  fprintf(outauxfile, "\n");
}  /* writeauxdata */

void writefactors(void)
{
  long i, k, l, m, n, writesites;
  char symbol;
  steptr wfactor;
  long grp;

  if(!justwts || firstrep){
    if(justwts){
      writesites = sites;
      wfactor = factorr;
    } else {
      writesites = newergroups;
      wfactor = newerfactor;
    }
    symbol = '+';
    if (interleaved)
      m = 60;
    else
      m = writesites;
    l=1;
    do {
      if(m > writesites)
        m = writesites;
      n = 0;
      for(k=l-1 ; k < m ; k++){
        grp = charorder[0][k];
        for(i = 0; i < newerhowmany[grp]; i++) {
          putc(symbol, outfactfile);
          n++;
          if (!interleaved && n > 1 && n % 60 == 1)
            fprintf(outfactfile, "\n ");
          if (n % 10 == 0 && n % 60 != 0)
            putc(' ', outfactfile);
        }
        symbol = (symbol == '+') ? '-' : '+';
      }
      if (interleaved) {
        l += 60;
        m += 60;
      }
    }while(interleaved && l <= writesites);
    fprintf(outfactfile, "\n");
  }
} /* writefactors */


void bootwrite( QVector<U2::MultipleSequenceAlignment>& mavect, const U2::MultipleSequenceAlignment& ma)
{ /* does bootstrapping and writes out data sets */
  long i, j, rr, repdiv10;

  if (rewrite)
    reps = 1;
  repdiv10 = reps / 10;
  if (repdiv10 < 1)
    repdiv10 = 1;
  if (progress)
    putchar('\n');
  firstrep = true;
  for (rr = 1; rr <= (reps); rr++) {
    bootweights();
    for (i = 0; i < spp; i++)
      for (j = 0; j < newergroups; j++)
        charorder[i][j] = j;
    if (ild) {
      charpermute(0, newergroups);
      for (i = 1; i < spp; i++)
        for (j = 0; j < newergroups; j++)
          charorder[i][j] = charorder[0][j];
    }
    if (lockhart)
      for (i = 0; i < spp; i++)
        charpermute(i, newergroups);
    if (!justwts || permute || ild || lockhart)
      writedata(mavect, rr-1, ma);
    
    if (progress && !rewrite && ((reps < 10) || rr % repdiv10 == 0)) {
      printf("completed replicate number %4ld\n", rr);
#ifdef WIN32
      phyFillScreenColor();
#endif
      firstrep = false;
    }
  }
  
}  /* bootwrite */




/*int main(int argc, Phylip_Char *argv[])
{  // Read in sequences or frequencies and bootstrap or jackknife them 
#ifdef MAC
  argc = 1;                // macsetup("SeqBoot","");                
  argv[0] = "SeqBoot";
#endif
  init(argc,argv);
  openfile(&infile, INFILE, "input file", "r", argv[0], infilename);
  ibmpc = IBMCRT;
  ansi = ANSICRT;
  doinput(argc, argv);
  bootwrite();
  
  freenewer();
  freenew();
  freerest();

  if (nodep)
    matrix_char_delete(nodep, spp);
  if (nodef)
    matrix_double_delete(nodef, spp);

  FClose(infile);
  if (factors) {
    FClose(factfile);
    FClose(outfactfile);
  }
  if (weights)
    FClose(weightfile);
  if (categories) {
    FClose(catfile);
    FClose(outcatfile);
  }
  if(mixture)
    FClose(outmixfile);
  if(ancvar)
    FClose(outancfile);
  if (justwts && !permute) {
    FClose(outweightfile);
  }
  else
    FClose(outfile);
#ifdef MAC
  fixmacfile(outfilename);
  if (justwts && !permute)
    fixmacfile(outweightfilename);
  if (categories)
    fixmacfile(outcatfilename);
  if (mixture)
    fixmacfile(outmixfilename);
#endif
  printf("Done.\n\n");
#ifdef WIN32
  phyRestoreConsoleAttributes();
#endif
  return 0;
}*/
