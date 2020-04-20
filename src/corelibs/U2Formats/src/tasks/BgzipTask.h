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

#ifndef _U2_BGZIP_TASK_H_
#define _U2_BGZIP_TASK_H_

#include <U2Core/Task.h>
#include <U2Core/GUrl.h>


namespace U2 {

class U2FORMATS_EXPORT BgzipTask : public Task {
    Q_OBJECT
public:
    BgzipTask(const GUrl& fileUrl, const GUrl& bgzfUrl = GUrl());
    void run();
    QString generateReport() const;
    ReportResult report();

    static bool checkBgzf(const GUrl &fileUrl);
private:
    GUrl fileUrl;
    GUrl bgzfUrl;
};

class U2FORMATS_EXPORT GzipDecompressTask : public Task {
    Q_OBJECT
public:
    GzipDecompressTask(const GUrl& zippedUrl, const GUrl& unzippedUrl = GUrl());
    void run();
    QString generateReport() const;
    ReportResult report();

    static bool checkZipped(const GUrl &fileUrl);
private:
    GUrl zippedUrl;
    GUrl unzippedUrl;
};

} // namespace U2

#endif // _U2_BGZIP_TASK_H_
