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


typedef long NumberType;

__kernel void
        binarySearch_classic(__const __global NumberType* haystack,
                             __const int haystackSize,
                             __global NumberType* needlesArr,
                             __const int needlesArrSize,
                             __const __global int *windowSizes
                             )
{
    size_t global_id = get_global_id(0);
    if (global_id >= needlesArrSize) {
        return;
    }

    NumberType needle = 0, // the number to search for, with the filter applied
        curValue = 0, // the current value from the haystack
        low = 0, high = haystackSize - 1, mid = 0, // indices used when searching in both mini and full-sized arrays
        firstOccurrenceOffset; // used to search the first occurrence of a needle when a row of identical ones is found

        long filter = ((long)0 - 1) << (62 - windowSizes[global_id] * 2);
        needle = needlesArr[global_id] & filter;

        // needle < min(haystack)
//      if (needle < (miniHaystack[0] & filter)) {
        if (needle < (haystack[low] & filter)) {
            needlesArr[global_id] = -1;
            return;
        }

        // needle > max(haystack)
//         if (needle > (miniHaystack[miniHaystackSize - 1] & filter)) {
        if (needle > (haystack[high] & filter)) {
            needlesArr[global_id] = -1;
            return;
        }

        // needle == haystack[0]
//      if(needle == (miniHaystack[0] & filter)) {
        if(needle == (haystack[low] & filter)) {
            needlesArr[global_id] = 0;
            return;
        }

        // search in the big haystack now
        mid = haystackSize / 2;
        curValue = haystack[mid] & filter;

        for (int j = 0; low <= high && curValue != needle; j++){
            if (needle > curValue) {
                low = mid + 1;
            }
            else {
                high = mid - 1;
            }
            mid = low + (high - low) / 2;
            curValue = haystack[mid] & filter;
        }

        if (curValue == needle) {
            for(firstOccurrenceOffset = mid;
                firstOccurrenceOffset >= 0 && (haystack[firstOccurrenceOffset] & filter) == needle;
                firstOccurrenceOffset--) {};

            needlesArr[global_id] = firstOccurrenceOffset + 1;
        } else {
            needlesArr[global_id] = -1;
        }
}
