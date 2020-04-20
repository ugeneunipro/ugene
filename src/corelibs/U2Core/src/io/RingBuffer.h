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

#ifndef _U2_RING_BUF_H_
#define _U2_RING_BUF_H_

namespace U2 {

	class RingBuffer {
	public:
		RingBuffer(char* buf, int size):data(buf),size(size),len(0),start(0){}
		inline int read(char* dest, int n, int index = 0) const;
		inline void append(const char* src, int n);
		int length() const {return len;}
		char* rawData() const {return data;}
	private:
		char* data; // buffer area
		int size; // buffer size
		int len; // length of buffered data
		int start; // start offset
	};

	int RingBuffer::read(char *dest, int n, int index) const {
		assert (index < len);
		if (n > len - index) {
			n = len - index;
		}
		int begin = (start + index) % size;
		int end = (start + index + n) % size;
		if (begin < end) {
			memcpy(dest, data + begin, n);
		} else {
			assert(end + size - begin >= n);
			int l1 = qMin(n, size - begin);
			memcpy(dest, data + begin, l1);
			if (l1 < n) {
				memcpy(dest + l1, data, n - l1);
			}
		}
		return n;
	}

	void RingBuffer::append(const char *src, int n) {
		if (n >= size) {
			start = 0;
			len = size;
			memcpy(data, src + n - size, size);
		} else {
			int trail_len = qMin(len, size - n);
			int trail_start = (start + len - trail_len) % size;
			int trail_end = (start + len) % size;
			if (trail_start > trail_end) {
				assert(trail_start - trail_end >= n);
				memcpy(data + trail_end, src, n);
			} else {
				assert(trail_start + size - trail_end >= n);
				int l1 = qMin(size - trail_end, n);
				memcpy(data + trail_end, src, l1);
				if (l1 < n){
					memcpy(data, src + l1, n - l1);
				}
			}
			start = trail_start;
			len += n;
			if (len > size) len = size;
		}
	}



} //namespace

#endif
