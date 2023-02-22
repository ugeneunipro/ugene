#include <math.h>
#include <stdint.h>
#include "bam.h"
#include "kstring.h"
#include "bam2bcf.h"
#include "errmod.h"

extern	void ks_introsort_uint32_t(size_t n, uint32_t a[]);

#define CALL_DEFTHETA 0.83f
#define DEF_MAPQ 20

#define CAP_DIST 25

char bam_nt16_nt4_table[] = { 4, 0, 1, 4, 2, 4, 4, 4, 3, 4, 4, 4, 4, 4, 4, 4 };

bcf_callaux_t *bcf_call_init(double theta, int min_baseQ)
{
	bcf_callaux_t *bca;
	if (theta <= 0.) theta = CALL_DEFTHETA;
	bca = calloc(1, sizeof(bcf_callaux_t));
	bca->capQ = 60;
	bca->openQ = 40; bca->extQ = 20; bca->tandemQ = 100;
	bca->min_baseQ = min_baseQ;
	bca->e = errmod_init(1. - theta);
	bca->min_frac = 0.002;
	bca->min_support = 1;
	return bca;
}

void bcf_call_destroy(bcf_callaux_t *bca)
{
	if (bca == 0) return;
	errmod_destroy(bca->e);
	free(bca->bases); free(bca->inscns); free(bca);
}
/* ref_base is the 4-bit representation of the reference base. It is
 * negative if we are looking at an indel. */
int bcf_call_glfgen(int _n, const bam_pileup1_t *pl, int ref_base, bcf_callaux_t *bca, bcf_callret1_t *r)
{
    static int *var_pos = NULL, nvar_pos = 0;
	int i, n, ref4, is_indel, ori_depth = 0;
	memset(r, 0, sizeof(bcf_callret1_t));
	if (ref_base >= 0) {
		ref4 = bam_nt16_nt4_table[ref_base];
		is_indel = 0;
	} else ref4 = 4, is_indel = 1;
	if (_n == 0) return -1;
	// enlarge the bases array if necessary
	if (bca->max_bases < _n) {
		bca->max_bases = _n;
		kroundup32(bca->max_bases);
		bca->bases = (uint16_t*)realloc(bca->bases, 2 * bca->max_bases);
	}
	// fill the bases array
	memset(r, 0, sizeof(bcf_callret1_t));
	for (i = n = 0; i < _n; ++i) {
		const bam_pileup1_t *p = pl + i;
		int q, b, mapQ, baseQ, is_diff, min_dist, seqQ;
		// set base
		if (p->is_del || p->is_refskip || (p->b->core.flag&BAM_FUNMAP)) continue;
		++ori_depth;
		baseQ = q = is_indel? p->aux&0xff : (int)bam1_qual(p->b)[p->qpos]; // base/indel quality
		seqQ = is_indel? (p->aux>>8&0xff) : 99;
		if (q < bca->min_baseQ) continue;
		if (q > seqQ) q = seqQ;
		mapQ = p->b->core.qual < 255? p->b->core.qual : DEF_MAPQ; // special case for mapQ==255
		mapQ = mapQ < bca->capQ? mapQ : bca->capQ;
		if (q > mapQ) q = mapQ;
		if (q > 63) q = 63;
		if (q < 4) q = 4;
		if (!is_indel) {
			b = bam1_seqi(bam1_seq(p->b), p->qpos); // base
			b = bam_nt16_nt4_table[b? b : ref_base]; // b is the 2-bit base
			is_diff = (ref4 < 4 && b == ref4)? 0 : 1;
		} else {
			b = p->aux>>16&0x3f;
			is_diff = (b != 0);
		}
		bca->bases[n++] = q<<5 | (int)bam1_strand(p->b)<<4 | b;
		// collect annotations
		if (b < 4) r->qsum[b] += q;
		++r->anno[0<<2|is_diff<<1|bam1_strand(p->b)];
		min_dist = p->b->core.l_qseq - 1 - p->qpos;
		if (min_dist > p->qpos) min_dist = p->qpos;
		if (min_dist > CAP_DIST) min_dist = CAP_DIST;
		r->anno[1<<2|is_diff<<1|0] += baseQ;
		r->anno[1<<2|is_diff<<1|1] += baseQ * baseQ;
		r->anno[2<<2|is_diff<<1|0] += mapQ;
		r->anno[2<<2|is_diff<<1|1] += mapQ * mapQ;
		r->anno[3<<2|is_diff<<1|0] += min_dist;
		r->anno[3<<2|is_diff<<1|1] += min_dist * min_dist;
	}
	r->depth = n; r->ori_depth = ori_depth;
	// glfgen
	errmod_cal(bca->e, n, 5, bca->bases, r->p);

    // Calculate the Variant Distance Bias (make it optional?)
    if ( nvar_pos < _n ) {
        nvar_pos = _n;
        var_pos = realloc(var_pos,sizeof(int)*nvar_pos);
    }
    {
    int alt_dp=0, read_len=0;
    for (i=0; i<_n; i++) {
        const bam_pileup1_t *p = pl + i;
        if ( bam1_seqi(bam1_seq(p->b),p->qpos) == ref_base )
            continue;

        var_pos[alt_dp] = p->qpos;
        if ( (bam1_cigar(p->b)[0]&BAM_CIGAR_MASK)==4 )
            var_pos[alt_dp] -= bam1_cigar(p->b)[0]>>BAM_CIGAR_SHIFT;

        alt_dp++;
        read_len += p->b->core.l_qseq;
    }
    {
    float mvd=0;
    int j;
    n=0;
    for (i=0; i<alt_dp; i++) {
        for (j=0; j<i; j++) {
            mvd += abs(var_pos[i] - var_pos[j]);
            n++;
        }
    }
    r->mvd[0] = n ? mvd/n : 0;
    r->mvd[1] = alt_dp;
    r->mvd[2] = alt_dp ? read_len/alt_dp : 0;

	return r->depth;
	}}
}


void calc_vdb(int n, const bcf_callret1_t *calls, bcf_call_t *call)
{
    // Variant distance bias. Samples merged by means of DP-weighted average.

    float weight=0, tot_prob=0;

    int i;
    for (i=0; i<n; i++)
    {
        int mvd      = calls[i].mvd[0];
        int dp       = calls[i].mvd[1];
        int read_len = calls[i].mvd[2];

        if ( dp<2 ) continue;
		{
        float prob = 0;
        if ( dp==2 )
        {
            // Exact formula
            prob = (mvd==0) ? 1.0/read_len : (read_len-mvd)*2.0/read_len/read_len;
        }
        else if ( dp==3 )
        {
            // Sin, quite accurate approximation
            float mu = read_len/2.9;
            prob = mvd>2*mu ? 0 : sin(mvd*3.14/2/mu) / (4*mu/3.14);
        }
        else
        {
            // Scaled gaussian curve, crude approximation, but behaves well. Using fixed depth for bigger depths.
            if ( dp>5 )
                dp = 5;
			{
            float sigma2 = (read_len/1.9/(dp+1)) * (read_len/1.9/(dp+1));
            float norm   = 1.125*sqrt(2*3.14*sigma2);
            float mu     = read_len/2.9;
            if ( mvd < mu )
                prob = exp(-(mvd-mu)*(mvd-mu)/2/sigma2)/norm;
            else
                prob = exp(-(mvd-mu)*(mvd-mu)/3.125/sigma2)/norm;
			}
        }

        //fprintf(stderr,"dp=%d mvd=%d read_len=%d -> prob=%f\n", dp,mvd,read_len,prob);
        tot_prob += prob*dp;
        weight += dp;
		}
    }
    tot_prob = weight ? tot_prob/weight : 1;
    //fprintf(stderr,"prob=%f\n", tot_prob);
    call->vdb = tot_prob;
}

int bcf_call_combine(int n, const bcf_callret1_t *calls, int ref_base /*4-bit*/, bcf_call_t *call)
{
	int ref4, i, j, qsum[4];
	int64_t tmp;
	if (ref_base >= 0) {
		call->ori_ref = ref4 = bam_nt16_nt4_table[ref_base];
		if (ref4 > 4) ref4 = 4;
	} else call->ori_ref = -1, ref4 = 0;
	// calculate qsum
	memset(qsum, 0, 4 * sizeof(int));
	for (i = 0; i < n; ++i)
		for (j = 0; j < 4; ++j)
			qsum[j] += calls[i].qsum[j];
	for (j = 0; j < 4; ++j) qsum[j] = qsum[j] << 2 | j;
	// find the top 2 alleles
	for (i = 1; i < 4; ++i) // insertion sort
		for (j = i; j > 0 && qsum[j] < qsum[j-1]; --j)
			tmp = qsum[j], qsum[j] = qsum[j-1], qsum[j-1] = tmp;
	// set the reference allele and alternative allele(s)
	for (i = 0; i < 5; ++i) call->a[i] = -1;
	call->unseen = -1;
	call->a[0] = ref4;
	for (i = 3, j = 1; i >= 0; --i) {
		if ((qsum[i]&3) != ref4) {
			if (qsum[i]>>2 != 0) call->a[j++] = qsum[i]&3;
			else break;
		}
	}
	if (ref_base >= 0) { // for SNPs, find the "unseen" base
		if (((ref4 < 4 && j < 4) || (ref4 == 4 && j < 5)) && i >= 0)
			call->unseen = j, call->a[j++] = qsum[i]&3;
		call->n_alleles = j;
	} else {
		call->n_alleles = j;
		if (call->n_alleles == 1) return -1; // no reliable supporting read. stop doing anything
	}
	// set the PL array
	if (call->n < n) {
		call->n = n;
		call->PL = realloc(call->PL, 15 * n);
	}
	{
		int x, g[15], z;
		double sum_min = 0.;
		x = call->n_alleles * (call->n_alleles + 1) / 2;
		// get the possible genotypes
		for (i = z = 0; i < call->n_alleles; ++i)
			for (j = 0; j <= i; ++j)
				g[z++] = call->a[j] * 5 + call->a[i];
		for (i = 0; i < n; ++i) {
			uint8_t *PL = call->PL + x * i;
			const bcf_callret1_t *r = calls + i;
			float min = 1e37;
			for (j = 0; j < x; ++j)
				if (min > r->p[g[j]]) min = r->p[g[j]];
			sum_min += min;
			for (j = 0; j < x; ++j) {
				int y;
				y = (int)(r->p[g[j]] - min + .499);
				if (y > 255) y = 255;
				PL[j] = y;
			}
		}
//		if (ref_base < 0) fprintf(stderr, "%d,%d,%f,%d\n", call->n_alleles, x, sum_min, call->unseen);
		call->shift = (int)(sum_min + .499);
	}
	// combine annotations
	memset(call->anno, 0, 16 * sizeof(int));
	for (i = call->depth = call->ori_depth = 0, tmp = 0; i < n; ++i) {
		call->depth += calls[i].depth;
		call->ori_depth += calls[i].ori_depth;
		for (j = 0; j < 16; ++j) call->anno[j] += calls[i].anno[j];
	}

    calc_vdb(n, calls, call);

	return 0;
}
