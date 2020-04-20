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

#ifndef _U2_DNA_ASSEMBLEY_GUI_EXTENSION_H_
#define _U2_DNA_ASSEMBLEY_GUI_EXTENSION_H_

#include <U2Core/global.h>
#include <U2Core/GUrl.h>
#include <U2Core/Task.h>

#include <QWidget>

#include <U2Algorithm/DnaAssemblyTask.h>

namespace U2 {

class U2VIEW_EXPORT DnaAssemblyAlgorithmBaseWidget : public QWidget {
public:
    DnaAssemblyAlgorithmBaseWidget(QWidget *parent);
    virtual void validateReferenceSequence(const GUrl &url) const;
};

// These classes are intended for extending standard Dna Assembly dialog GUI
// with options specific to the assembly algorithm

class U2VIEW_EXPORT DnaAssemblyAlgorithmMainWidget : public DnaAssemblyAlgorithmBaseWidget {
    Q_OBJECT
public:
    DnaAssemblyAlgorithmMainWidget(QWidget *parent);

    virtual QMap<QString,QVariant> getDnaAssemblyCustomSettings() const = 0;

    virtual bool isParametersOk(QString &error) const;
    virtual bool buildIndexUrl(const GUrl &url, bool prebuiltIndex, QString &error) const;
    virtual bool isIndexOk(const GUrl &url, QString &error) const;
    virtual bool isIndex(const QString &url) const;
    virtual bool isValidIndex(const QString &oneIndexFileUrl) const;

protected:
    bool requiredToolsAreOk() const;

    QStringList indexSuffixes;
    QStringList requiredExtToolIds;
};

class U2VIEW_EXPORT DnaAssemblyAlgorithmBuildIndexWidget : public DnaAssemblyAlgorithmBaseWidget {
public:
    DnaAssemblyAlgorithmBuildIndexWidget(QWidget *parent);

    virtual QMap<QString,QVariant> getBuildIndexCustomSettings() = 0;
    virtual QString getIndexFileExtension() = 0;
    virtual GUrl buildIndexUrl(const GUrl& url) = 0;
};

class U2VIEW_EXPORT DnaAssemblyGUIExtensionsFactory {
public:
    virtual ~DnaAssemblyGUIExtensionsFactory();
    virtual DnaAssemblyAlgorithmMainWidget* createMainWidget(QWidget* parent) = 0;
    virtual DnaAssemblyAlgorithmBuildIndexWidget* createBuildIndexWidget(QWidget* parent) = 0;
    virtual bool hasMainWidget() = 0;
    virtual bool hasBuildIndexWidget() = 0;
};

class DnaAssemblyDialog;
class U2VIEW_EXPORT DnaAssemblyGUIUtils : public QObject {
public:
    static void runAssembly2ReferenceDialog(const QStringList& shortReadUrls = QStringList(), const QString& refSeqUrl = QString());
    static DnaAssemblyToRefTaskSettings getSettings(DnaAssemblyDialog *dialog);
};


// These classes are intended for extending standard Genome Assembly dialog GUI
// with options specific to the assembly algorithm

class U2VIEW_EXPORT GenomeAssemblyAlgorithmMainWidget : public QWidget {
public:
    GenomeAssemblyAlgorithmMainWidget(QWidget *parent);
    virtual QMap<QString,QVariant> getGenomeAssemblyCustomSettings() = 0;
    virtual bool isParametersOk(QString &error) = 0;
};

} // U2


#endif // _U2_DNA_ASSEMBLEY_GUI_EXTENSION_H_
