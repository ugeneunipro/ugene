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

#include <U2Core/AppContext.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/Task.h>

#include <U2Formats/BAMUtils.h>

#include "BaiReader.h"
#include "Exception.h"
#include "Index.h"
#include "LoadBamInfoTask.h"
#include "Reader.h"
#include "SamReader.h"

namespace U2 {
namespace BAM {

LoadInfoTask::LoadInfoTask(const GUrl& _sourceUrl, bool _sam)
    : Task(tr("Load BAM info"), TaskFlag_None), sourceUrl(_sourceUrl), sam(_sam)
{
    setSubtaskProgressWeight(0);
}

const GUrl& LoadInfoTask::getSourceUrl() const {
    return sourceUrl;
}

void LoadInfoTask::run() {
    try {
        QScopedPointer<IOAdapter> ioAdapter;
        {
            IOAdapterFactory *factory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(sourceUrl));
            ioAdapter.reset(factory->createIOAdapter());
        }

        GUrl baiUrl = BAMUtils::getBamIndexUrl(sourceUrl);

        QScopedPointer<IOAdapter> ioIndexAdapter;
        IOAdapterFactory *factory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(baiUrl));
        ioIndexAdapter.reset(factory->createIOAdapter());

        if(!ioAdapter->open(sourceUrl, IOAdapterMode_Read)) {
            stateInfo.setError(LoadInfoTask::tr("Can't open file '%1'").arg(sourceUrl.getURLString()));
            return;
        }

        bool hasIndex = false;
        if (sam) {
            hasIndex = false;
        } else {
            hasIndex = ioIndexAdapter->open(baiUrl, IOAdapterMode_Read);
        }

        QScopedPointer<Reader> reader(NULL);
        if (sam) {
            reader.reset(new SamReader(*ioAdapter));
        } else {
            reader.reset(new BamReader(*ioAdapter));
        }
        bamInfo.setHeader(reader->getHeader());

        if (!sam) {
            QScopedPointer<BaiReader> baiReader(new BaiReader(*ioIndexAdapter));
            Index index;
            if(hasIndex) {
                index = baiReader->readIndex();
                if(index.getReferenceIndices().count() != reader->getHeader().getReferences().size()) {
                    throw Exception("Invalid index");
                }
                bamInfo.setIndex(index);
            }
        }
    } catch(const Exception &ex) {
        stateInfo.setError(tr("Loading BAM info for file '%1' failed: %2")
                           .arg(sourceUrl.getURLString())
                           .arg(ex.getMessage()));
    }
}

} // namespace BAM
} // namespace U2
