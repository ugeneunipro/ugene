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

#include <QMap>

#include <U2Core/AnnotationData.h>
#include <U2Core/Task.h>

namespace U2 {

class AnnotationTableObject;
class CreateAnnotationsTask;
class Document;
class LoadDocumentTask;
class SaveDocumentTask;
class VariantTrackObject;

class ConvertSnpeffVariationsToAnnotationsTask : public Task {
    Q_OBJECT
public:
    ConvertSnpeffVariationsToAnnotationsTask(const QList<VariantTrackObject*>& variantTrackObjects);

    const QMap<QString, QList<SharedAnnotationData>>& getAnnotationsData() const;

private:
    void run() override;

    const QList<VariantTrackObject*> variantTrackObjects;
    QMap<QString, QList<SharedAnnotationData>> annotationTablesData;

    static const QString CHROM_QUALIFIER_NAME;
    static const QString LOCATION_QUALIFIER_NAME;
    static const QString REFERENCE_QUALIFIER_NAME;
    static const QString ALTERNATE_QUALIFIER_NAME;
    static const QString ALLELE_QUALIFIER_NAME;
    static const QString ID_QUALIFIER_NAME;
};

class U2FORMATS_EXPORT LoadConvertAndSaveSnpeffVariationsToAnnotationsTask : public Task {
    Q_OBJECT
public:
    LoadConvertAndSaveSnpeffVariationsToAnnotationsTask(const QString& variationsUrl, const U2DbiRef& dstDbiRef, const QString& dstUrl, const QString& formatId);
    ~LoadConvertAndSaveSnpeffVariationsToAnnotationsTask();

    const QString& getResultUrl() const;

private:
    void prepare() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;

    Document* prepareDocument();
    void prepareSaveTask();

    const QString variationsUrl;
    const U2DbiRef dstDbiRef;
    const QString dstUrl;
    const QString formatId;

    LoadDocumentTask* loadTask;
    ConvertSnpeffVariationsToAnnotationsTask* convertTask;
    QList<Task*> createAnnotationsTasks;
    SaveDocumentTask* saveTask;

    Document* loadedVariationsDocument;
    Document* annotationsDocument;
    QList<AnnotationTableObject*> annotationTableObjects;
};

}  // namespace U2
