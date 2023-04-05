/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#pragma once

#include <U2Core/GUrl.h>
#include <U2Core/Task.h>
#include <U2Core/U2OpStatusUtils.h>

namespace U2 {
namespace BAM {

class PrepareToImportTask : public Task {
public:
    PrepareToImportTask(const GUrl& assemblyUrl, bool sam, const QString& refUrl, const QString& _workDir);
    void run() override;
    const GUrl& getSourceUrl() const;
    bool isNewURL() const;

private:
    GUrl sourceURL;
    QString refUrl;
    QString workDir;
    bool samFormat;
    bool newURL;

private:
    void checkReferenceFile();
    QString getIndexedBamUrl(const QString& sortedBamUrl) const;
    QString getCopyError(const QString& url1, const QString& url2) const;
    bool needToCopyBam(const QString& sortedBamUrl) const;
};

}  // namespace BAM
}  // namespace U2
