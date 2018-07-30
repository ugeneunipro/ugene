/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#ifndef _U2_DIAMOND_CLASSIFY_TASK_H_
#define _U2_DIAMOND_CLASSIFY_TASK_H_

#include <U2Core/ExternalToolRunTask.h>

namespace U2 {

struct DiamondClassifyTaskSettings {
    DiamondClassifyTaskSettings();

    QString databaseUrl;
    QString readsUrl;
    QString pairedReadsUrl;
    QString taxonMapUrl;
    QString taxonNodesUrl;

    QString classificationUrl;

    QString sensitive;
    int topAlignmentsPercentage;
    QString matrix;
    double max_evalue;
    double block_size;
    unsigned gencode;
    unsigned frame_shift;
    int gap_open;
    int gap_extend;
    int index_chunks;
    int num_threads;

    static const QString SENSITIVE_DEFAULT;
    static const QString SENSITIVE_ULTRA;
    static const QString SENSITIVE_HIGH;

    static const QString BLOSUM45;
    static const QString BLOSUM50;
    static const QString BLOSUM62;
    static const QString BLOSUM80;
    static const QString BLOSUM90;
    static const QString PAM250;
    static const QString PAM70;
    static const QString PAM30;
};

class DiamondClassifyTask : public ExternalToolSupportTask {
    Q_OBJECT
public:
    DiamondClassifyTask(const DiamondClassifyTaskSettings &settings);

    const QString &getClassificationUrl() const;

private:
    void prepare();

    void checkSettings();
    QStringList getArguments() const;

    const DiamondClassifyTaskSettings settings;

    static const QString TAXONOMIC_CLASSIFICATION_OUTPUT_FORMAT;
};

}   // namespace U2

#endif // _U2_DIAMOND_CLASSIFY_TASK_H_
