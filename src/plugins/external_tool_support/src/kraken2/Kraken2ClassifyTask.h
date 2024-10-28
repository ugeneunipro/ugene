/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "NgsReadsClassificationUtils.h"

namespace U2 {

struct Kraken2ClassifyTaskSettings {
    QString databaseUrl;
    QString readsUrl;
    QString pairedReadsUrl;
    bool quickOperation = false;
    int numberOfThreads = 1;
    bool pairedReads = false;

    QString classificationUrl;

    static const QString SINGLE_END;
    static const QString PAIRED_END;
};

class Kraken2ClassifyTask : public ExternalToolSupportTask {
    Q_OBJECT
public:
    Kraken2ClassifyTask(const Kraken2ClassifyTaskSettings &settings);

    void prepare() override;
    void run() override;

    const QString &getClassificationUrl() const;
    const LocalWorkflow::TaxonomyClassificationResult &getParsedReport() const;

private:
    QStringList getArguments();

    const Kraken2ClassifyTaskSettings settings;
    ExternalToolRunTask *classifyTask = nullptr;
    LocalWorkflow::TaxonomyClassificationResult parsedReport;
};

}  // namespace U2
