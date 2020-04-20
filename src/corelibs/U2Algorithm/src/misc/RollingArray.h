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

#ifndef _U2_ROLLING_ARRAY_H_
#define _U2_ROLLING_ARRAY_H_

#include <assert.h>

#include <QVector>

namespace U2 {

//todo: not tested!

template <class T>
class RollingArray {
public:
    RollingArray(int maxSize);
    RollingArray( const T * data, int size );

    inline RollingArray() : startRollPos(0), endRollPos(0), maxSize(0){};

    inline int getMaxSize() const;
    inline int size() const;

    inline void set(int pos, T val);
    inline T get(int pos) const;
    inline T& get(int pos);

    inline void push_back_pop_front(T val);
    inline void push_front_pop_back(T val);

private:
    inline void push(T val);
    inline void push_front(T val);
    inline T pop();
    inline T pop_front();

    inline int toRollPos(int pos) const;
    inline int toPos(int rollPos) const;

    QVector<T> buf;
    int startRollPos;
    int endRollPos;
    int maxSize;
};


template<class T> inline RollingArray<T>::RollingArray(int ms) {
    assert(ms > 0);
    maxSize = ms;
    buf.resize(maxSize);
    startRollPos = 0;
    endRollPos = 0;
}

template<class T> inline RollingArray<T>::RollingArray( const T * data, int size ) :
buf(size), startRollPos(0), endRollPos(size-1), maxSize(size)
{
    for (int i = 0; i < buf.size(); ++i)    buf[i] = *(data + i);
}


template<class T> inline int RollingArray<T>::getMaxSize() const {
    return maxSize;
}

template<class T> inline int RollingArray<T>::size() const {
    int res = toPos(endRollPos);
    return res + 1;
}



template<class T> void RollingArray<T>::set(int pos, T val) {
    int rollPos = toRollPos(pos);
    buf[rollPos] = val;
}

template<class T> T RollingArray<T>::get(int pos) const {
    int rollPos = toRollPos(pos);
    return buf[rollPos];
}

template<class T> T& RollingArray<T>::get(int pos) {
    int rollPos = toRollPos(pos);
    return buf[rollPos];
}


template<class T> void RollingArray<T>::push(T val) {
//    int pos = size();
//    assert(pos < maxSize);
    endRollPos++;
    if (endRollPos >= maxSize) {
        endRollPos = 0;
    }
    buf[endRollPos] = val;
}


template<class T> inline void RollingArray<T>::push_front(T val)
{
    --startRollPos;
    if( startRollPos < 0 ) {
        startRollPos = maxSize - 1;
    }
    buf[startRollPos] = val;
}

template<class T> T RollingArray<T>::pop() {
    T res = buf[endRollPos];
    endRollPos--;
    if (endRollPos < 0) {
        endRollPos = maxSize-1;
    }
    return res;
}


template<class T> inline T RollingArray<T>::pop_front()
{
    T res = buf[startRollPos];
    ++startRollPos;
    if( startRollPos >= maxSize )
    {
        startRollPos = 0;
    }
    return res;
}


template<class T> inline void RollingArray<T>::push_back_pop_front( T val )
{
    if( size() == getMaxSize() ) {
        pop_front();
    }
    push( val );
}


template<class T> inline void RollingArray<T>::push_front_pop_back(T val)
{
    if( size() == getMaxSize() ) {
        pop();
    }
    push_front(val);
}

template<class T> int RollingArray<T>::toRollPos(int pos) const {
    assert(pos < size() && pos >= 0);
    int rollPos = pos + startRollPos;
    if (rollPos >= maxSize) {
        rollPos-=maxSize;
    }
    return rollPos;
}

template<class T> int RollingArray<T>::toPos(int rollPos) const {
    int pos = rollPos - startRollPos;
    if (pos < 0) {
        pos += maxSize;
    }
    return pos;
}

} //namespace

#endif
