/*  rand.c -- drand48 implementation from the FreeBSD source tree. */

// This file is an amalgamation of the many small files in FreeBSD to do with
// drand48 and friends implementations.
// It comprises _rand48.c, rand48.h, srand48.c, drand48.c, erand48.c, lrand48.c

/*
 * Copyright (c) 1993 Martin Birgmeier
 * All rights reserved.
 *
 * You may redistribute unmodified or modified versions of this source
 * code provided that the above copyright notice and this and the
 * following conditions are retained.
 *
 * This software is provided ``as is'', and comes with no warranties
 * of any kind. I shall in no event be liable for anything that happens
 * to anyone/anything when using this software.
 */

//#include <sys/cdefs.h>
//__FBSDID("$FreeBSD: src/lib/libc/gen/_rand48.c,v 1.2 2002/03/22 21:52:05 obrien Exp $");

#include <math.h>

#define	RAND48_SEED_0	(0x330e)
#define	RAND48_SEED_1	(0xabcd)
#define	RAND48_SEED_2	(0x1234)
#define	RAND48_MULT_0	(0xe66d)
#define	RAND48_MULT_1	(0xdeec)
#define	RAND48_MULT_2	(0x0005)
#define	RAND48_ADD	(0x000b)

static void
    _dorand48(unsigned short xseed[3]);

static void hts_srand48(long seed);

static double hts_erand48(unsigned short xseed[3]);

static double hts_drand48(void);

static long hts_lrand48(void);
