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

#include <U2Core/DocumentProviderTask.h>
#include <U2Core/GObjectReference.h>

#include <U2Gui/ObjectViewTasks.h>
namespace U2 {

class MaEditor;
class MaEditorFactory;
class MsaEditor;
class MsaObject;
class UnloadedObject;
class MsaConsensusAlgorithm;

/*!
 * \brief The OpenMaEditorTask class
 */
class OpenMaEditorTask : public ObjectViewTask {
    Q_OBJECT
public:
    OpenMaEditorTask(MsaObject* obj, GObjectViewFactoryId fid, GObjectType type);
    OpenMaEditorTask(UnloadedObject* obj, GObjectViewFactoryId fid, GObjectType type);
    OpenMaEditorTask(Document* doc, GObjectViewFactoryId fid, GObjectType type);

    void loadCache() override;

    void open() override;

    static void updateTitle(MsaEditor* msaEd);

    virtual MaEditor* getEditor(const QString& viewName, GObject* obj) = 0;

protected:
    GObjectType type;
    QPointer<MsaObject> maObject;
    GObjectReference unloadedReference;
};

/*!
 * \brief The OpenMsaEditorTaskOpenMsaEditorTask class
 */
class OpenMsaEditorTask : public OpenMaEditorTask {
    Q_OBJECT
public:
    OpenMsaEditorTask(MsaObject* obj);
    OpenMsaEditorTask(UnloadedObject* obj);
    OpenMsaEditorTask(Document* doc);

    MaEditor* getEditor(const QString& viewName, GObject* obj) override;
};

/*!
 * \brief The OpenMcaEditorTask class
 */
class OpenMcaEditorTask : public OpenMaEditorTask {
    Q_OBJECT
public:
    OpenMcaEditorTask(MsaObject* obj);
    OpenMcaEditorTask(UnloadedObject* obj);
    OpenMcaEditorTask(Document* doc);

    MaEditor* getEditor(const QString& viewName, GObject* obj) override;
};

class OpenSavedMaEditorTask : public ObjectViewTask {
    Q_OBJECT
public:
    OpenSavedMaEditorTask(GObjectType type, MaEditorFactory* factory, const QString& viewName, const QVariantMap& stateData);
    void open() override;

    static void updateRanges(const QVariantMap& stateData, MaEditor* ctx);

private:
    GObjectType type;
    MaEditorFactory* factory;
};

class UpdateMaEditorTask : public ObjectViewTask {
public:
    UpdateMaEditorTask(GObjectViewController* v, const QString& stateName, const QVariantMap& stateData);

    void update() override;
};

class ExportMaConsensusTaskSettings {
public:
    ExportMaConsensusTaskSettings();

    bool keepGaps;
    MaEditor* ma;
    QString url;
    DocumentFormatId format;
    QString name;
    MsaConsensusAlgorithm* algorithm;
};

class ExtractConsensusTask : public Task {
    Q_OBJECT
public:
    ExtractConsensusTask(bool keepGaps, MaEditor* ma, MsaConsensusAlgorithm* algorithm);
    ~ExtractConsensusTask();
    void run() override;
    const QByteArray& getExtractedConsensus() const;

private:
    bool keepGaps;
    MaEditor* ma;
    QByteArray filteredConsensus;
    MsaConsensusAlgorithm* algorithm;
};

class ExportMaConsensusTask : public DocumentProviderTask {
    Q_OBJECT
public:
    ExportMaConsensusTask(const ExportMaConsensusTaskSettings& s);

    void prepare() override;

    const QString& getConsensusUrl() const;

protected:
    QList<Task*> onSubTaskFinished(Task* subTask) override;

private:
    Document* createDocument();

    ExportMaConsensusTaskSettings settings;
    ExtractConsensusTask* extractConsensus;
    QByteArray filteredConsensus;
};

}  // namespace U2
