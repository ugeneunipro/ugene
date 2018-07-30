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

#ifndef _U2_DNA_ASSEMBLY_TASK_H_
#define _U2_DNA_ASSEMBLY_TASK_H_

#include <U2Core/DNASequence.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/GUrl.h>
#include <U2Core/MultipleSequenceAlignment.h>

namespace U2 {

class Document;

class U2ALGORITHM_EXPORT ShortReadSet {
public:
    enum LibraryType {
        SingleEndReads, PairedEndReads
    };

    enum MateOrder {
        UpstreamMate, DownstreamMate
    };

    GUrl url;
    LibraryType type;
    MateOrder order;
    ShortReadSet(const GUrl& _url) : url(_url), type(SingleEndReads), order(UpstreamMate) {}
    ShortReadSet(const GUrl& _url, LibraryType t, MateOrder m) : url(_url), type(t), order(m) {}
};

class U2ALGORITHM_EXPORT DnaAssemblyToRefTaskSettings {
public:
    DnaAssemblyToRefTaskSettings();

    void setCustomSettings(const QMap<QString, QVariant>& settings);
    QVariant getCustomValue(const QString& optionName, const QVariant& defaultVal) const;
    bool hasCustomValue(const QString & name) const;
    void setCustomValue(const QString& optionName, const QVariant& val);
    QList<GUrl> getShortReadUrls() const;

public:
    QString indexDir;       // if prebuiltIndex is true
    QString indexBasename;  // if prebuiltIndex is true
    QList<ShortReadSet> shortReadSets;
    GUrl refSeqUrl;         // if prebuiltIndex is false
    GUrl resultFileName;
    QString indexFileName;
    QString algName;
    bool pairedReads;
    bool filterUnpaired;
    QString tmpDirectoryForFilteredFiles;
    bool prebuiltIndex;
    bool openView;
    bool samOutput;
    QString tmpDirPath;
    bool cleanTmpDir;

    static const QString INDEX;
    static const QString SEQUENCE;

private:
    QMap<QString, QVariant> customSettings;
};

class U2ALGORITHM_EXPORT DnaAssemblyToReferenceTask : public ExternalToolSupportTask {
    Q_OBJECT
public:
    DnaAssemblyToReferenceTask(const DnaAssemblyToRefTaskSettings &settings, TaskFlags flags = TaskFlags_FOSCOE, bool justBuildIndex = false);

    bool hasResult() const {return hasResults;}
    const DnaAssemblyToRefTaskSettings& getSettings() const{return settings;}

    static bool isIndexUrl(const QString &url, const QStringList &indexSuffixes);
    static QString getBaseUrl(const QString &url, const QStringList &indexSuffixes);
    static bool isPrebuiltIndex(const QString &baseFileName, const QStringList& indexExtensions);

protected:
    void setUpIndexBuilding(const QStringList& indexExtensions);

    DnaAssemblyToRefTaskSettings settings;
    bool justBuildIndex;
    bool hasResults;
};

class U2ALGORITHM_EXPORT DnaAssemblyToRefTaskFactory {
public:
    virtual DnaAssemblyToReferenceTask* createTaskInstance(const DnaAssemblyToRefTaskSettings& settings, bool justBuildIndex = false) = 0;
    virtual ~DnaAssemblyToRefTaskFactory() {}
};

#define DNA_ASSEMBLEY_TO_REF_TASK_FACTORY(c) \
public: \
    static const QString taskName; \
class Factory : public DnaAssemblyToRefTaskFactory { \
public: \
    Factory() { } \
    DnaAssemblyToReferenceTask* createTaskInstance(const DnaAssemblyToRefTaskSettings& s, bool justBuildIndex = false) { return new c(s, justBuildIndex); } \
};
} // U2


#endif // _U2_DNA_ASSEMBLEY_TASK_H_
