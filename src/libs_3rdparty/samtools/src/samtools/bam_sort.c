#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "bam.h"
#include "ksort.h"

static int g_is_by_qname = 0;

static inline int strnum_cmp(const char *a, const char *b)
{
	char *pa, *pb;
	pa = (char*)a; pb = (char*)b;
	while (*pa && *pb) {
		if (isdigit(*pa) && isdigit(*pb)) {
			long ai, bi;
			ai = strtol(pa, &pa, 10);
			bi = strtol(pb, &pb, 10);
			if (ai != bi) return ai<bi? -1 : ai>bi? 1 : 0;
		} else {
			if (*pa != *pb) break;
			++pa; ++pb;
		}
	}
	if (*pa == *pb)
		return (pa-a) < (pb-b)? -1 : (pa-a) > (pb-b)? 1 : 0;
	return *pa<*pb? -1 : *pa>*pb? 1 : 0;
}

#define HEAP_EMPTY 0xffffffffffffffffull

typedef struct {
	int i;
	uint64_t pos, idx;
	bam1_t *b;
} heap1_t;

#define __pos_cmp(a, b) ((a).pos > (b).pos || ((a).pos == (b).pos && ((a).i > (b).i || ((a).i == (b).i && (a).idx > (b).idx))))

static inline int heap_lt(const heap1_t a, const heap1_t b)
{
	if (g_is_by_qname) {
		int t;
		if (a.b == 0 || b.b == 0) return a.b == 0? 1 : 0;
		t = strnum_cmp(bam1_qname(a.b), bam1_qname(b.b));
		return (t > 0 || (t == 0 && __pos_cmp(a, b)));
	} else return __pos_cmp(a, b);
}

KSORT_INIT(heap, heap1_t, heap_lt)

static void swap_header_targets(bam_header_t *h1, bam_header_t *h2)
{
	bam_header_t t;
	t.n_targets = h1->n_targets, h1->n_targets = h2->n_targets, h2->n_targets = t.n_targets;
	t.target_name = h1->target_name, h1->target_name = h2->target_name, h2->target_name = t.target_name;
	t.target_len = h1->target_len, h1->target_len = h2->target_len, h2->target_len = t.target_len;
}

typedef bam1_t *bam1_p;

static int change_SO(bam_header_t *h, const char *so)
{
    char *p, *q, *beg = 0, *end = 0, *newtext;
    if (h->l_text > 3) {
        if (strncmp(h->text, "@HD", 3) == 0) {
            if ((p = strchr(h->text, '\n')) == 0) return -1;
            *p = '\0';
            if ((q = strstr(h->text, "\tSO:")) != 0) {
                *p = '\n'; // change back
                if (strncmp(q + 4, so, p - q - 4) != 0) {
                    beg = q;
                    for (q += 4; *q != '\n' && *q != '\t'; ++q) {}
                    end = q;
                } else return 0; // no need to change
            } else beg = end = p, *p = '\n';
        }
    }
    if (beg == 0) { // no @HD
        h->l_text += strlen(so) + 15;
        newtext = (char*)malloc(h->l_text + 1);
        sprintf(newtext, "@HD\tVN:1.3\tSO:%s\n", so);
        strcat(newtext, h->text);
    } else { // has @HD but different or no SO
        h->l_text = (beg - h->text) + (4 + strlen(so)) + (h->text + h->l_text - end);
        newtext = (char*)malloc(h->l_text + 1);
        strncpy(newtext, h->text, beg - h->text);
        sprintf(newtext + (beg - h->text), "\tSO:%s", so);
        strcat(newtext, end);
    }
    free(h->text);
    h->text = newtext;
    return 0;
}

static inline int bam1_lt(const bam1_p a, const bam1_p b)
{
	if (g_is_by_qname) {
		int t = strnum_cmp(bam1_qname(a), bam1_qname(b));
		return (t < 0 || (t == 0 && (((uint64_t)a->core.tid<<32|(a->core.pos+1)) < ((uint64_t)b->core.tid<<32|(b->core.pos+1)))));
	} else return (((uint64_t)a->core.tid<<32|(a->core.pos+1)) < ((uint64_t)b->core.tid<<32|(b->core.pos+1)));
}
KSORT_INIT(sort, bam1_p, bam1_lt)
