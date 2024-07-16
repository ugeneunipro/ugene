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

#include <QStringList>

#include <U2Core/AppResources.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/Msa.h>
#include <U2Core/U2Region.h>

namespace U2 {

class AnnotationSettings;
class AnnotationTableObject;
class DNAAlphabet;
class DNASequence;
class Document;
class GObject;
class GObjectReference;
class GUrl;
class U2DbiRef;
class U2OpStatus;
class U2Sequence;
class U2SequenceObject;

class U2FORMATS_EXPORT DocumentFormatUtils : public QObject {
    Q_OBJECT
public:
    static QList<DocumentFormatId> toIds(const QList<DocumentFormat*>& formats);

    static QList<AnnotationSettings*> predefinedSettings();

    /** Extracts sequences either from Sequence or MultipleAlignment object */
    static QList<DNASequence> toSequences(const GObject* obj);

    static int getMergeGap(const QVariantMap& hints);

    static int getMergedSize(const QVariantMap& hints, int defaultVal);

    static void updateFormatHints(QList<GObject*>& objects, QVariantMap& fs);

    using Bytes = int;
    /** The approximate amount of memory required for one merged sequence annotation, in bytes. Doesn't depend on the
     * sequence name length.
     */
    static constexpr Bytes DocumentFormatUtils::memPerMergedAnnot();

    /** Doc URL here is used to set up sequence<->annotation relations */
    static AnnotationTableObject* addAnnotationsForMergedU2Sequence(const GObjectReference& mergedSequenceRef,
                                                                    const U2DbiRef& dbiRef,
                                                                    const QStringList& contigs,
                                                                    const QVector<U2Region>& mergedMapping,
                                                                    const QVariantMap& hints);
    /** Function overload above. In addition to creating the "Contigs" annotation table, the function accepts an already
     * acquired "memory" resource that will be moved to the annotation table itself.
     */
    static AnnotationTableObject* addAnnotationsForMergedU2Sequence(const GObjectReference& mergedSequenceRef,
                                                                    const U2DbiRef& dbiRef,
                                                                    const QStringList& contigs,
                                                                    const QVector<U2Region>& mergedMapping,
                                                                    const QVariantMap& hints,
                                                                    MemoryLocker memLocker);

    static QString getFormatNameById(const DocumentFormatId& formatId);
};

}  // namespace U2
