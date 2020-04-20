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

#ifndef _U2_SYNC_SORT_H_
#define _U2_SYNC_SORT_H_

#include <U2Core/global.h>

namespace U2 {

template<class T, class S>
class SyncSort {
public:
    SyncSort(QVector<T> &arr, QVector<S> &ind);
    SyncSort(T *arr, S *ind, int first, int length);
    void sort();
private:
    void sort(T *x, int off, int len);
    qint64 compare(const T *x1, const T *x2) const;
    void swap(T *x1, T *x2) const;
    quint32 med3(T *x, quint32 a, quint32 b, quint32 c);
    void vecswap(T *x1, T *x2, quint32 n);

    int len;
    T* start;
    S* indexes;
};

template<class T, class S>
void SyncSort<T,S>::sort(T *x, int off, int len) {
    // Insertion sort on smallest arrays
    if (len < 7) {
        for (int i=off; i<len+off; i++){
            for (int j=i; j > off && compare(x+j-1,x+j)>0; j--) {
                swap(x+j, x+j-1);
            }
        }
        return;
    }

    // Choose a partition element, v
    quint32 m = off + len / 2;       // Small arrays, middle element
    if (len > 7) {
        quint32 l = off;
        quint32 n = off + len - 1;
        if (len > 40) {        // Big arrays, pseudo median of 9
            quint32 s = len / 8;
            l = med3(x, l,     l+s, l+2*s);
            m = med3(x, m-s,   m,   m+s);
            n = med3(x, n-2*s, n-s, n);
        }
        m = med3(x, l, m, n); // Mid-size, med of 3
    }
    T  *v = x + m;

    // Establish Invariant: v* (<v)* (>v)* v*
    int a = off, b = a, c = off + len - 1, d = c;
    while(true) {
        qint64 cr;
        while (b <= c && (cr = compare(v, x+b)) >=0 ) {
            if (cr == 0) {
                (x+b==v) && (v=x+a);//save middle pos value
                swap(x+a++,x+b);
            }
            b++;
        }
        while (c >= b && (cr = compare(x+c, v)) >=0 ) {
            if (cr == 0) {
                (x+c==v) && (v=x+d);//save middle pos value
                swap(x+c, x+d--);
            }
            c--;
        }
        if (b > c) {
            break;
        }
        swap(x+b++, x+c--);
    }

    // Swap partition elements back to middle
    int s, n = off + len;
    s = qMin(a-off, b-a  ); vecswap(x+off, x+b-s, s);
    s = qMin(d-c,   n-d-1); vecswap(x+b,   x+n-s, s);

    // Recursively sort non-partition-elements
    if ((s = b-a) > 1) {
        sort(x, off, s);
    }
    if ((s = d-c) > 1) {
        sort(x, n-s, s);
    }
}

template<class T, class S>
qint64 SyncSort<T,S>::compare(const T *x1, const T *x2) const {
    return *x1-*x2;
}

template<class T, class S>
void SyncSort<T,S>::swap(T *x1, T *x2) const {
    int ind1 = x1-start;
    int ind2 = x2-start;

    qSwap(*(indexes + ind1), *(indexes + ind2));
    qSwap(*x1, *x2);
}

template<class T, class S>
quint32 SyncSort<T,S>::med3(T *x, quint32 a, quint32 b, quint32 c) {
    qint64 bc = compare(x+b, x+c);
    qint64 ac = compare(x+a, x+c);
    return compare(x+a, x+b) < 0 ?
        (bc < 0 ? b : ac < 0 ? c : a) :
        (bc > 0 ? b : ac > 0 ? c : a);
}

template<class T, class S>
void SyncSort<T,S>::vecswap(T *x1, T *x2, quint32 n) {
    for (quint32 i=0; i<n; i++) {
        swap(x1+i, x2+i);
    }
}

template<class T, class S>
SyncSort<T,S>::SyncSort(QVector<T> &arr, QVector<S> &ind) {
    len = arr.size();
    start = arr.data();
    indexes = ind.data();
}

template<class T, class S>
SyncSort<T,S>::SyncSort(T *arr, S *ind, int _start, int _len) {
    len = _len;
    start = arr + _start;
    indexes = ind;
}

template<class T, class S>
void SyncSort<T,S>::sort() {
    if (len <= 0 || !start || !indexes) {
        return;
    }
    sort(start, 0, len);
}



} //namespace

#endif
