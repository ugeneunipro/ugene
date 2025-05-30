/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/Task.h>

namespace U2 {

class BedGraphToBigWigSetting {
public:
    BedGraphToBigWigSetting()
        : outDir(""), outName(""), inputUrl(""), genomePath(""), blockSize(256), itemsPerSlot(1024), uncompressed(false) {
    }

    QString outDir;
    QString outName;
    QString inputUrl;
    QString genomePath;
    int blockSize;
    int itemsPerSlot;
    bool uncompressed;
};

class BedGraphToBigWigTask : public ExternalToolSupportTask {
    Q_OBJECT
public:
    BedGraphToBigWigTask(const BedGraphToBigWigSetting& settings);

    void prepare() override;
    void run() override;

    QString getResult() {
        return resultUrl;
    }

protected:
    QStringList getParameters(U2OpStatus& os);

protected:
    BedGraphToBigWigSetting settings;
    QString resultUrl;
};

class BedGraphToBigWigParser : public ExternalToolLogParser {
public:
    BedGraphToBigWigParser();

    void parseOutput(const QString& partOfLog) override;
    void parseErrOutput(const QString& partOfLog) override;

private:
    QString lastErrLine;
};

}  // namespace U2
