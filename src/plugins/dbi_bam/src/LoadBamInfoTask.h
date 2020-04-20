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

#ifndef _U2_BAM_LOAD_BAM_INFO_TASK_H_
#define _U2_BAM_LOAD_BAM_INFO_TASK_H_

#include <U2Core/Task.h>
#include <U2Core/GUrl.h>

#include "Index.h"
#include "Header.h"


namespace U2 {
namespace BAM {

class BAMInfo {
public:

    BAMInfo() : _hasIndex(false), unmappedSelected(false) {}
    BAMInfo(const BAMInfo& src) : header(src.header), selected(src.selected), index(src.index), _hasIndex(src._hasIndex), unmappedSelected(src.unmappedSelected) {}

    inline QList<bool>& getSelected() { return selected; }
    inline bool hasIndex() const { return _hasIndex; }
    inline bool isReferenceSelected(int id) { if(id == -1) return unmappedSelected; else return selected.at(id); }
    inline Index& getIndex() { return index; }
    inline const Header& getHeader() { return header; }
    inline bool isUnmappedSelected() { return unmappedSelected; }
    void setIndex(Index& index) { this->index = index; _hasIndex = true; }
    void setHeader(const Header& header) { this->header = header; selected.clear(); for(int i=0; i< header.getReferences().count(); i++) { selected.append(true); } }
    void setUnmappedSelected(bool unmappedSelected) { this->unmappedSelected = unmappedSelected; }
private:
    Header header;
    QList<bool> selected;
    Index index;
    bool _hasIndex;
    bool unmappedSelected;
};

class LoadInfoTask : public Task {
    Q_OBJECT
public:
    LoadInfoTask(const GUrl& sourceUrl, bool sam);
    void run();
    inline BAMInfo& getInfo() { return bamInfo; }
    const GUrl& getSourceUrl() const;
    bool isSam() const { return sam; }
private:
    const GUrl sourceUrl;
    BAMInfo bamInfo;
    bool sam;
};

} // namespace BAM
} // namespace U2

#endif // _U2_BAM_LOAD_BAM_INFO_TASK_H_
