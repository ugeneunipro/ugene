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

#include <QDir>
#include <QFileInfo>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/UserApplicationsSettings.h>

#include "DnaAssemblyTask.h"

namespace U2 {

const QString DnaAssemblyToRefTaskSettings::INDEX = "Index";
const QString DnaAssemblyToRefTaskSettings::SEQUENCE = "Sequence";

DnaAssemblyToReferenceTask::DnaAssemblyToReferenceTask(const DnaAssemblyToRefTaskSettings &settings, TaskFlags flags, bool justBuildIndex)
    : ExternalToolSupportTask(tr("Align short reads"), flags),
      settings(settings),
      justBuildIndex(justBuildIndex),
      hasResults(false)
{
}

void DnaAssemblyToReferenceTask::setUpIndexBuilding(const QStringList &indexSuffixes) {
    if (settings.prebuiltIndex) {
        return;
    }

    if (isIndexUrl(settings.refSeqUrl.getURLString(), indexSuffixes)) {
        settings.prebuiltIndex = true;
        settings.refSeqUrl = getBaseUrl(settings.refSeqUrl.getURLString(), indexSuffixes);
        settings.indexFileName = settings.refSeqUrl.getURLString();
        return;
    }

    settings.prebuiltIndex = isPrebuiltIndex(settings.refSeqUrl.getURLString(), indexSuffixes);
    if (settings.prebuiltIndex) {
        settings.indexFileName = settings.refSeqUrl.getURLString();
    } else {
        QString baseUrl = QDir(settings.refSeqUrl.dirPath()).filePath(settings.refSeqUrl.baseFileName());
        settings.prebuiltIndex = isPrebuiltIndex(baseUrl, indexSuffixes);
        if (settings.prebuiltIndex) {
            settings.refSeqUrl = baseUrl;
            settings.indexFileName = baseUrl;
        }
    }
}

bool DnaAssemblyToReferenceTask::isIndexUrl(const QString &url, const QStringList &indexSuffixes) {
    foreach (const QString &suffix, indexSuffixes) {
        if (url.endsWith(suffix)) {
            return true;
        }
    }
    return false;
}

QString DnaAssemblyToReferenceTask::getBaseUrl(const QString &url, const QStringList &indexSuffixes) {
    foreach (const QString &suffix, indexSuffixes) {
        if (url.endsWith(suffix)) {
            return url.left(url.length() - suffix.length());
        }
    }
    return url;
}

bool DnaAssemblyToReferenceTask::isPrebuiltIndex(const QString& baseFileName, const QStringList& indexExtensions) {
    foreach(const QString& curIndexExtension, indexExtensions) {
        QString indexFilePath = baseFileName + curIndexExtension;
        QFileInfo fileInfo(indexFilePath);
        if(!fileInfo.exists()) {
            return false;
        }
    }
    return true;
}

QVariant DnaAssemblyToRefTaskSettings::getCustomValue( const QString& optionName, const QVariant& defaultVal ) const {
    if (customSettings.contains(optionName)) {
        return customSettings.value(optionName);
    } else {
        return defaultVal;
    }
}

bool DnaAssemblyToRefTaskSettings::hasCustomValue(const QString & name) const {
    return customSettings.contains(name);
}

void DnaAssemblyToRefTaskSettings::setCustomValue( const QString& optionName, const QVariant& val ) {
    customSettings.insert(optionName,val);
}

QList<GUrl> DnaAssemblyToRefTaskSettings::getShortReadUrls() const
{
    QList<GUrl> res;
    foreach (const ShortReadSet& set, shortReadSets) {
        res.append(set.url);
    }

    return res;
}

DnaAssemblyToRefTaskSettings::DnaAssemblyToRefTaskSettings()
    : pairedReads(false),
    filterUnpaired(false),
    prebuiltIndex(false),
    openView(false),
    samOutput(true),
    tmpDirPath(AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath()),
    cleanTmpDir(true) {
}

void DnaAssemblyToRefTaskSettings::setCustomSettings(const QMap<QString, QVariant>& settings) {
    customSettings = settings;
}
} // U2
