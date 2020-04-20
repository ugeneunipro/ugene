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

#ifndef GENOMEASSEMBLYREGISTRY_H
#define GENOMEASSEMBLYREGISTRY_H

#include <QList>
#include <QString>
#include <QStringList>
#include <QMutex>
#include <QObject>

#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/global.h>
#include <U2Core/GUrl.h>
#include <U2Core/Task.h>

class QWidget;

namespace U2 {

#define LIB_PAIR_DEFAULT                "paired-end"
#define LIB_PAIR_MATE                   "mate-pairs"
#define LIB_PAIR_MATE_HQ                "hq-mate-pairs"

#define LIB_SINGLE_UNPAIRED             "single"
#define LIB_SINGLE_CSS                  "single"
#define LIB_SINGLE_CLR                  "pacbio"
#define LIB_SINGLE_NANOPORE             "nanopore"
#define LIB_SINGLE_SANGER               "sanger"
#define LIB_SINGLE_TRUSTED              "trusted-contigs"
#define LIB_SINGLE_UNTRUSTED            "untrusted-contigs"

#define ORIENTATION_FR                  "fr"
#define ORIENTATION_RF                  "rf"
#define ORIENTATION_FF                  "ff"

#define PLATFORM_ILLUMINA               "illumina"
#define PLATFORM_ION_TORRENT            "ion torrent"

#define TYPE_SINGLE                     "single reads"
#define TYPE_INTERLACED                 "interlaced reads"

class GenomeAssemblyAlgorithmMainWidget;

class U2ALGORITHM_EXPORT GenomeAssemblyGUIExtensionsFactory {
public:
    virtual ~GenomeAssemblyGUIExtensionsFactory() {}
    virtual GenomeAssemblyAlgorithmMainWidget* createMainWidget(QWidget* parent) = 0;
    virtual bool hasMainWidget() = 0;
};

class U2ALGORITHM_EXPORT GenomeAssemblyUtils {
public:
    static QStringList getOrientationTypes();
    static bool isLibraryPaired(const QString& libName);
};

/////////////////////////////////////////////////////////////
//Task

class U2ALGORITHM_EXPORT AssemblyReads {
public:
    AssemblyReads(const QList<GUrl>& left = QList<GUrl>(),
                  const QList<GUrl>& right = QList<GUrl>(),
                  const QString& orientation = ORIENTATION_FR,
                  const QString& libName = LIB_SINGLE_UNPAIRED,
                  const QString& readType = TYPE_SINGLE)
        :left(left)
        ,right(right)
        ,orientation(orientation)
        ,libName(libName)
        ,readType(readType)
        {}

        QList<GUrl> left;
        QList<GUrl> right;
        QString     orientation;
        QString     libName;
        QString     readType;
};

class U2ALGORITHM_EXPORT GenomeAssemblyTaskSettings {
public:
    GenomeAssemblyTaskSettings() : openView(false) {}

    void setCustomSettings(const QMap<QString, QVariant>& settings);
    QVariant getCustomValue(const QString& optionName, const QVariant& defaultVal) const;
    bool hasCustomValue(const QString & name) const;
    void setCustomValue(const QString& optionName, const QVariant& val);

public:
    QList<AssemblyReads> reads;
    GUrl outDir;
    QString algName;
    bool openView;
    QList<ExternalToolListener*> listeners;

private:
    QMap<QString, QVariant> customSettings;
};

class U2ALGORITHM_EXPORT GenomeAssemblyTask : public Task {
    Q_OBJECT
public:
    GenomeAssemblyTask(const GenomeAssemblyTaskSettings& settings, TaskFlags flags = TaskFlags_FOSCOE);
    virtual ~GenomeAssemblyTask() {}
    bool hasResult() const {return !resultUrl.isEmpty();}
    QString getResultUrl() const;
    const GenomeAssemblyTaskSettings& getSettings() const{return settings;}

protected:
    GenomeAssemblyTaskSettings settings;
    QString resultUrl;
};

class U2ALGORITHM_EXPORT GenomeAssemblyTaskFactory {
public:
    virtual GenomeAssemblyTask* createTaskInstance(const GenomeAssemblyTaskSettings& settings) = 0;
    virtual ~GenomeAssemblyTaskFactory() {}
};

#define GENOME_ASSEMBLEY_TASK_FACTORY(c) \
public: \
    static const QString taskName; \
class Factory : public GenomeAssemblyTaskFactory { \
public: \
    Factory() { } \
    GenomeAssemblyTask* createTaskInstance(const GenomeAssemblyTaskSettings& s) { return new c(s); } \
};


///////////////////////////////////////////////////////////////
//Registry
class U2ALGORITHM_EXPORT GenomeAssemblyAlgorithmEnv {
public:
    GenomeAssemblyAlgorithmEnv(const QString &id,
        GenomeAssemblyTaskFactory *tf ,
        GenomeAssemblyGUIExtensionsFactory *guiExt,
        const QStringList &readsFormats);

    virtual ~GenomeAssemblyAlgorithmEnv();

    const QString& getId()  const {return id;}
    QStringList getReadsFormats() const { return readsFormats; }

    GenomeAssemblyTaskFactory* getTaskFactory() const {return taskFactory;}
    GenomeAssemblyGUIExtensionsFactory* getGUIExtFactory() const {return guiExtFactory;}

private:
    Q_DISABLE_COPY(GenomeAssemblyAlgorithmEnv)

protected:
    QString id;
    GenomeAssemblyTaskFactory* taskFactory;
    GenomeAssemblyGUIExtensionsFactory* guiExtFactory;
    QStringList readsFormats;
};
class U2ALGORITHM_EXPORT GenomeAssemblyAlgRegistry : public QObject {
    Q_OBJECT
public:
    GenomeAssemblyAlgRegistry(QObject* pOwn = 0);
    ~GenomeAssemblyAlgRegistry();

    bool registerAlgorithm(GenomeAssemblyAlgorithmEnv* env);
    GenomeAssemblyAlgorithmEnv* unregisterAlgorithm(const QString& id);
    GenomeAssemblyAlgorithmEnv* getAlgorithm(const QString& id) const;

    QStringList getRegisteredAlgorithmIds() const;
private:
    mutable QMutex mutex;
    QMap<QString, GenomeAssemblyAlgorithmEnv*> algorithms;

    Q_DISABLE_COPY(GenomeAssemblyAlgRegistry)
};

} // namespace

#endif // GENOMEASSEMBLYREGISTRY_H
