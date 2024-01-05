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

#include <QSet>

#include <U2Core/DNASequence.h>
#include <U2Core/Msa.h>
#include <U2Core/Task.h>

namespace U2 {

class U2CORE_EXPORT ExportSequencesTask : public Task {
    Q_OBJECT
public:
    ExportSequencesTask(const Msa& msa,
                        const QSet<qint64>& rowIds,
                        bool trimGaps,
                        bool addToProjectFlag,
                        const QString& dirUrl,
                        const DocumentFormatId& format,
                        const QString& extension,
                        const QString& customFileName = QString());

protected:
    void prepare() override;

private:
    QList<DNASequence> sequences;
    bool addToProjectFlag;
    QString dirUrl;
    DocumentFormatId format;
    QString extension;
    QString customFileName;
};

}  // namespace U2
