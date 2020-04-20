/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#ifndef _U2_IO_LIB_UTILS_H_
#define _U2_IO_LIB_UTILS_H_

#include <stdio.h>

namespace U2 {

#define BUFF_SIZE 8196
#define CHECK_MB 1024*1024

class SeekableBuf
{
public:
    const char*    head;
    int             pos;
    int             size;
    const uchar* ubuf() const {return (const uchar*)(head + pos);}
    const char* buf() const {return (head + pos);}
    uchar next() {return *(head + pos++);}
    int read(char* buf, int len) {
        if (pos + len <= size) {
            memcpy(buf, head + pos, len);
            pos += len;
            return len;
        }
        return 0;
    }
};

inline int SeekBuf(SeekableBuf* fp, int offset, int origin) {
    if (0 == origin && offset >= 0 && offset < fp->size) {
        fp->pos = offset;
        return 0;
    }
    return 1;
}

inline uint be_int4(const uchar* buf) {
    uchar c1 = *(buf++);
    uchar c2 = *(buf++);
    uchar c3 = *(buf++);
    uchar c4 = *(buf++);
    return (c1<<24) + (c2<<16) + (c3<<8) + c4;
}

inline ushort be_int2(const uchar* buf) {
    uchar c1 = *(buf++);
    uchar c2 = *(buf++);
    return (c1<<8) + c2;
}


inline int be_read_int_1(SeekableBuf* fp, uchar *i1) {
    if ((fp->pos + 1) > fp->size) {
        return 0;
    }
    *i1 = fp->next();
    return 1;
}

inline int be_read_int_2(SeekableBuf* fp, ushort *i2) {
    if ((fp->pos + 2) > fp->size) {
        return 0;
    }
    *i2 = be_int2(fp->ubuf());
    fp->pos += 2;
    return 1;
}


inline int be_read_int_4(SeekableBuf* fp, uint *i4) {
    if ((fp->pos + 4) > fp->size) {
        return 0;
    }
    *i4 = be_int4(fp->ubuf());
    fp->pos += 4;
    return 1;
}


/*
* Write a big-endian int1
*/
inline int be_write_int_1(FILE *fp, uchar *i1)
{
    if (fwrite(i1, sizeof(uchar), 1, fp) != 1) return (0);
    return (1);
}


/*
* Write a big-endian int2
*/
inline int be_write_int_2(FILE *fp, ushort *i2)
{
    ushort i = be_int2( reinterpret_cast<const uchar*>(i2) );

    if (fwrite(&i, 2, 1, fp) != 1) return (0);
    return (1);
}

/*
* Write a big-endian int4
*/
inline int be_write_int_4(FILE *fp, uint *i4)
{
    uint i = be_int4(reinterpret_cast<const uchar*> (i4));

    if (fwrite(&i, 4, 1, fp) != 1) return (0);

    return (1);
}


/*
* Copyright (c) Medical Research Council 1994. All rights reserved.
*
* Permission to use, copy, modify and distribute this software and its
* documentation for any purpose is hereby granted without fee, provided that
* this copyright and notice appears in all copies.
*
* MRC disclaims all warranties with regard to this software.
*/
#define READ_BASES	(1<<0)
#define READ_SAMPLES	(1<<1)
#define READ_COMMENTS	(1<<2)
#define READ_ALL	(READ_BASES | READ_SAMPLES | READ_COMMENTS)

#define IEEE

inline float int_to_float(int in)
/*
** interpret the integer in as a
** floating point number in IEEE format
*/
{
    /*
    Assume `in' is stored as a float according to the
    ANSI IEEE 754-1985 standard. See the tables below:

    s = sign ( 1 bit)
    e = biased exponent (8 bits)
    f = fraction (23 bits)

    floating point number =  (-1)^s 2^(e-127) 1.f

    Bits  Name      Content
    31   Sign      1 iff number is negative
    23-30  Exponent  Eight-Bit exponent, biased by 127
    0-22  Fraction  23-bit fraction component of normalised significant.
    The "one" bit is "hidden"

    If IEEE floating point format is supported on your machine...
    ensure there is a #define IEEE somewhere.
    */

#ifdef IEEE
    union {
        int i;
        float f;
    } cvt;
    cvt.i = in;
    return cvt.f;
#else
    int fraction;
    int exponent;
    int sign;

    fraction = in & ( (1<<23)-1 );
    exponent = (in >> 23) & ( (1<<8)-1 );
    sign = (in >> 31);

    return
        (float) (
        (sign?-1.0:1.0) *
        exp ( log ( (double) 2.0) * (double) (exponent - 127 - 23) ) *
        (double) ((1<<23)+fraction)) ;
#endif
}

} //namespace
#endif
