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

#include <QScopedPointer>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2SafePoints.h>

#include "ExportAnnotations2CSVTask.h"

namespace U2 {

const QString ExportAnnotations2CSVTask::SEQUENCE_NAME = "sequence_name";

ExportAnnotations2CSVTask::ExportAnnotations2CSVTask(const QList<Annotation *> &annotations, const QByteArray &sequence, const QString &_seqName,
    const DNATranslation *complementTranslation, bool exportSequence, bool _exportSeqName, const QString &url, bool apnd, const QString &sep)
    : Task(tr("Export annotations to CSV format"), TaskFlag_None), annotations(annotations), sequence(sequence), seqName(_seqName),
    complementTranslation(complementTranslation), exportSequence(exportSequence), exportSequenceName(_exportSeqName), url(url),
    append(apnd), separator(sep)
{
    GCOUNTER(cvar, tvar, "ExportAnnotattions2CSVTask");
}

static void writeCSVLine(const QStringList &container, IOAdapter *ioAdapter, const QString &separator, U2OpStatus &os) {
    bool first = true;
    foreach (const QString &value, container) {
        if (!first) {
            if (0 == ioAdapter->writeBlock(separator.toLatin1())) {
                os.setError(L10N::errorWritingFile(ioAdapter->getURL()));
                return;
            }
        }
        QString preparedStr = value;
        preparedStr.replace("\"", "\"\"");
        preparedStr = "\"" + preparedStr + "\"";

        if (0 == ioAdapter->writeBlock(preparedStr.toLocal8Bit())) {
            os.setError(L10N::errorWritingFile(ioAdapter->getURL()));
            return;
        }
        first = false;
    }
    if (0 == ioAdapter->writeBlock("\n")) {
        os.setError(L10N::errorWritingFile(ioAdapter->getURL()));
        return;
    }
}

void ExportAnnotations2CSVTask::run() {
    QScopedPointer<IOAdapter> ioAdapter;

    IOAdapterId ioAdapterId = IOAdapterUtils::url2io(url);
    IOAdapterRegistry *ioRegistry = AppContext::getIOAdapterRegistry();
    CHECK_EXT(NULL != ioRegistry,
        stateInfo.setError(tr("Invalid I/O environment!").arg(url)),);
    IOAdapterFactory *ioAdapterFactory = ioRegistry->getIOAdapterFactoryById(ioAdapterId);
    CHECK_EXT(NULL != ioAdapterFactory,
        stateInfo.setError(tr("No IO adapter found for URL: %1").arg(url)),);
    ioAdapter.reset(ioAdapterFactory->createIOAdapter());

    if (!ioAdapter->open(url, append ? IOAdapterMode_Append : IOAdapterMode_Write)) {
        stateInfo.setError(L10N::errorOpeningFileWrite(url));
        return;
    }

    QHash<QString, int> columnIndices;
    QStringList columnNames;
    columnNames << tr("Group") << tr("Name") << tr("Start") <<  tr("End") << tr("Length") << tr("Complementary");
    if (exportSequenceName) {
        columnNames << tr("Sequence name");
    }
    if (exportSequence) {
        columnNames << tr("Sequence");
    }

    bool hasSequenceNameQualifier = false;
    foreach (Annotation *annotation, annotations) {
        foreach(const U2Qualifier &qualifier, annotation->getQualifiers()) {
            const QString &qName = qualifier.name;
            if (qName == SEQUENCE_NAME) {
                hasSequenceNameQualifier = true;
                continue;
            }
            if (!columnIndices.contains(qName)) {
                columnIndices.insert(qName, columnNames.size());
                columnNames.append(qName);
            }
        }
    }
    writeCSVLine(columnNames, ioAdapter.data(), separator, stateInfo);
    CHECK_OP(stateInfo,);

    bool noComplementarySequence = false;
    foreach (Annotation *annotation, annotations) {
        foreach(const U2Region &region, annotation->getRegions()) {
            QStringList values;
            values << annotation->getGroup()->getGroupPath();
            values << annotation->getName();
            values << QString::number(region.startPos + 1);
            values << QString::number(region.startPos + region.length);
            values << QString::number(region.length);

            const bool isComplementary = annotation->getStrand().isCompementary();
            values << ((isComplementary) ? tr("yes") : tr("no"));

            if (exportSequenceName) {
                if (!seqName.isEmpty()) {
                    values << seqName.toLatin1();
                } else if (hasSequenceNameQualifier) {
                    foreach(const U2Qualifier& qf, annotation->getQualifiers()) {
                        if (qf.name == SEQUENCE_NAME) {
                            values << qf.value;
                        }
                    }
                }
            }
            if (exportSequence) {
                QByteArray sequencePart = sequence.mid(region.startPos, region.length);
                if (isComplementary) {
                    if (complementTranslation != NULL) {
                        complementTranslation->translate(sequencePart.data(), sequencePart.size());
                        TextUtils::reverse(sequencePart.data(), sequencePart.size());
                    } else {
                        noComplementarySequence = true;
                        sequencePart.clear();
                    }
                }
                values << sequencePart;
            }

            //add empty strings as default qualifier values
            while (values.size() < columnNames.size()) {
                values << QString();
            }

            foreach (const U2Qualifier& qualifier, annotation->getQualifiers()) {
                if (qualifier.name == SEQUENCE_NAME) {
                    continue;
                }

                int qualifiedIndex = columnIndices[qualifier.name];
                SAFE_POINT(qualifiedIndex > 0 && qualifiedIndex < values.length(), "Invalid qualifier index",);
                values[qualifiedIndex] = qualifier.value;
            }
            writeCSVLine(values, ioAdapter.data(), separator, stateInfo);
            CHECK_OP(stateInfo,);
        }
    }
    if (noComplementarySequence) {
        taskLog.error(tr("Attaching a sequence to an annotation was ignored. The annotation is on the complementary strand. Can not generate a complementary sequence for a non-nucleic alphabet."));
    }
}

} // namespace U2
