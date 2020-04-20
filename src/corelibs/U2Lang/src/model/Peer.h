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

#ifndef _U2_WORKFLOW_PEER_H_
#define _U2_WORKFLOW_PEER_H_

#include <U2Lang/Descriptor.h>

namespace U2 {

namespace Workflow {

/**
* wrapper throw 'void*'
* use as a base class to store any pointer in your class
*/
class U2LANG_EXPORT Peer {
public:
    Peer() : peer(NULL) {}
    virtual ~Peer() {}

    // casting to whatever you want
    template<typename T> inline T* castPeer() const {
        return (T*)(peer);
    }

    void* getPeer() const {return peer;}
    void setPeer(void* p) { peer = p; }

protected:
    // any pointer
    void* peer;

}; // Peer

} // Workflow

} // U2

#endif // _U2_WORKFLOW_PEER_H_
