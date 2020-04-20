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

#include "PluginDescriptor.h"

#include <U2Core/L10n.h>
#include <U2Core/GAutoDeleteList.h>

#include <QDir>
#include <QDomDocument>


namespace U2 {

static PlatformName platformFromText(const QString& text) {
    QString trimmed = text.trimmed();
    if (trimmed == "win") {
        return PlatformName_Win;
    }
    if (trimmed == "unix") {
        return PlatformName_UnixNotMac;
    }
    if (trimmed == "macx") {
        return PlatformName_Mac;
    }
    return PlatformName_Unknown;
}

static PlatformArch archFromText(const QString& text) {
    QString trimmed = text.trimmed();
    if (trimmed == "32") {
        return PlatformArch_32;
    }
    if (trimmed == "64") {
        return PlatformArch_64;
    }
    return PlatformArch_Unknown;
}

static PluginMode modeFromText(const QString& text) {
    QString trimmed = text.trimmed().toLower();
    QStringList tokens = trimmed.split(QRegExp("[\\s,]"), QString::SkipEmptyParts);
    PluginMode result;
    if (tokens.isEmpty()) {
        result|=PluginMode_Malformed;
        return result;
    }
    foreach(const QString& token, tokens) {
        if (token == "ui") {
            result|=PluginMode_UI;
        } else if (token == "console") {
            result|=PluginMode_Console;
        } else {
            result|=PluginMode_Malformed;
            return result;
        }
    }
    return result;
}

PluginDesc PluginDescriptorHelper::readPluginDescriptor(const QString& descUrl, QString& error) {
    PluginDesc result;
    PluginDesc failResult; //empty one, used if parsing is failed

    QFile f(descUrl);
    if (!f.open(QIODevice::ReadOnly)) {
        error = L10N::errorOpeningFileRead(descUrl);
        return failResult;
    }

    result.descriptorUrl = descUrl;

    QByteArray  xmlData = f.readAll();
    f.close();

    QDomDocument doc;
    bool res = doc.setContent(xmlData);
    if (!res) {
        error = L10N::notValidFileFormat("XML", descUrl);
        return failResult;
    }

    QDomElement pluginElement = doc.documentElement();
    QString pluginElementName = pluginElement.tagName();
    if (pluginElementName != "ugene-plugin") {
        error = L10N::notValidFileFormat("UGENE plugin", descUrl);
        return failResult;
    }

    result.id = pluginElement.attribute("id");
    if (result.id.isEmpty()) {
        error = tr("Required attribute not found %1").arg("id");
        return failResult;
    }

    result.pluginVersion = Version::parseVersion(pluginElement.attribute("version"));
    if (result.pluginVersion.text.isEmpty()) {
        error = tr("Required attribute not found %1").arg("version");
        return failResult;
    }

    result.ugeneVersion= Version::parseVersion(pluginElement.attribute("ugene-version"));
    if (result.ugeneVersion.text.isEmpty()) {
        error = tr("Required attribute not found %1").arg("ugene-version");
        return failResult;
    }

    result.qtVersion = Version::parseVersion(pluginElement.attribute("qt-version"));
    if (result.qtVersion.text.isEmpty()) {
        error = tr("Required attribute not found %1").arg("qt-version");
        return failResult;
    }

    QDomElement libraryElement = pluginElement.firstChildElement("library");
    QString libraryUrlText = libraryElement.text();
    if (!libraryUrlText.isEmpty() && QFileInfo(libraryUrlText).isRelative()) { //if path is relative, use descriptor dir as 'current folder'
        libraryUrlText = QFileInfo(descUrl).absoluteDir().canonicalPath() + "/" + libraryUrlText;
    }
    result.libraryUrl = libraryUrlText;
    if (result.libraryUrl.isEmpty()) {
        error = tr("Required element not found %1").arg("library");
        return failResult;
    }
    QString licenseUrl = QString(result.id+".license");
    if (QFileInfo(licenseUrl).isRelative()) { //if path is relative, use descriptor dir as 'current folder'
        licenseUrl = QFileInfo(descUrl).absoluteDir().canonicalPath() + "/" + licenseUrl;
    }
    result.licenseUrl = licenseUrl;

    result.name = pluginElement.firstChildElement("name").text();
    if (result.name.isNull()) {
        error = tr("Required element not found %1").arg("name");
        return failResult;
    }

    result.pluginVendor= pluginElement.firstChildElement("plugin-vendor").text();
    if (result.pluginVendor.isNull()) {
        error = tr("Required element not found %1").arg("plugin-vendor");
        return failResult;
    }

    QString pluginModeText = pluginElement.firstChildElement("plugin-mode").text();
    result.mode = modeFromText(pluginModeText);
    if (result.mode.testFlag(PluginMode_Malformed)) {
        error = tr("Not valid value: '%1', plugin: %2").arg(pluginModeText).arg("plugin-mode");
        return failResult;
    }

    QDomElement platformElement = pluginElement.firstChildElement("platform");
    QString platformNameText = platformElement.attribute("name");
    result.platform.name= platformFromText(platformNameText);
    if (result.platform.name == PlatformName_Unknown) {
        error = tr("Platform arch is unknown: %1").arg(platformNameText);
        return failResult;
    }

    QString platformArchText= platformElement.attribute("arch");
    result.platform.arch = archFromText(platformArchText);
    if (result.platform.arch == PlatformArch_Unknown) {
        error = tr("Platform bits is unknown: %1").arg(platformArchText);
        return failResult;
    }

    QString debugText = pluginElement.firstChildElement("debug-build").text();
    bool debug = debugText == "true" || debugText == "yes" || debugText.toInt() == 1;
    result.qtVersion.debug = result.ugeneVersion.debug = result.pluginVersion.debug = debug;

    QDomNodeList dependsElements = pluginElement.elementsByTagName("depends");
    for(int i = 0; i < dependsElements.size(); i++) {
        QDomNode dn = dependsElements.item(i);
        if (!dn.isElement()) {
            continue;
        }
        QString dependsText = dn.toElement().text();
        QStringList dependsTokes = dependsText.split(QChar(';'), QString::SkipEmptyParts);
        foreach (const QString& token, dependsTokes) {
            QStringList plugAndVersion = token.split(QChar(':'), QString::KeepEmptyParts);
            if (plugAndVersion.size()!=2) {
                error = tr("Invalid depends token: %1").arg(token);
                return failResult;
            }
            DependsInfo di;
            di.id = plugAndVersion.at(0);
            di.version = Version::parseVersion(plugAndVersion.at(1));
            result.dependsList.append(di);
        }
    }

    return result;
}


PluginDesc::PluginDesc()
    : mode(PluginMode_Malformed)
{

}

bool PluginDesc::operator == (const PluginDesc& pd) const  {
    return id == pd.id
        && pluginVersion == pd.pluginVersion
        && ugeneVersion == pd.ugeneVersion
        && qtVersion == pd.qtVersion
        && libraryUrl == pd.libraryUrl
        && licenseUrl == pd.licenseUrl
        && platform == pd.platform
        && mode == pd.mode;
}

//////////////////////////////////////////////////////////////////////////
// ordering

//states set used for DFS graph traversal
enum DepNodeState {
    DS_Clean,
    DS_InProcess,
    DS_Done
};

class DepNode {
public:
    DepNode() {state = DS_Clean; root = false;}
    QList<DepNode*>     parentNodes; //nodes this node depends on
    QList<DepNode*>     childNodes; //nodes that depends on this node
    PluginDesc          desc;

    DepNodeState        state;
    bool                root;
};

static void resetState(const QList<DepNode*>& nodes) {
    foreach (DepNode* node, nodes) {
        node->state = DS_Clean;
    }
}

static void findParentNodes(DepNode* node, const PluginDesc& desc, QString & err, QList<DepNode*>& result) {
    assert(node->state == DS_Clean);
    node->state = DS_InProcess;
    foreach (DepNode* childNode, node->childNodes) {
        if (childNode->state == DS_Done) { //check if node is already processed
            continue;
        }
        if (childNode->state == DS_InProcess) { // circular dependency between plugins
            err = PluginDescriptorHelper::tr("Plugin circular dependency detected: %1 <-> %2").arg(desc.id).arg(node->desc.id);
            return;
        }
        findParentNodes(childNode, desc, err, result);
    }
    foreach(const DependsInfo& di, desc.dependsList) {
        if ( di.id == node->desc.id && di.version <= node->desc.pluginVersion ) {
            result.append(node);
            break;
        }
    }
    node->state = DS_Done;
}

static void orderPostorder(DepNode* node, QList<PluginDesc>& result) {
    assert(node->state == DS_Clean);
    node->state = DS_InProcess;
    foreach (DepNode* childNode, node->childNodes) {
        if (childNode->state != DS_Clean) {
            continue;
        }
        orderPostorder(childNode, result);
    }
    if (!node->root) {
        result.append(node->desc);
    }
    node->state = DS_Done;
}

static void orderTopological(DepNode* node, QList<PluginDesc>& result) {
    orderPostorder(node, result);
    QList<PluginDesc> topologicalResult;
    QListIterator<PluginDesc> it(result);
    it.toBack();
    while( it.hasPrevious()) {
        topologicalResult.append(it.previous());
    }
    result = topologicalResult;
}

QList<PluginDesc> PluginDescriptorHelper::orderPlugins(const QList<PluginDesc>& unordered, QString& err) {
    // Sort plugin using dependency graph.
    // Root node has no dependencies. All child nodes depends on all parents.
    QList<PluginDesc> result;
    if (unordered.isEmpty()) {
        return unordered;
    }

    GAutoDeleteList<DepNode> allNodes;
    DepNode* rootNode = new DepNode();
    rootNode->root = true;
    allNodes.qlist.append(rootNode);

    QList<PluginDesc> queue = unordered;

    int iterations = 0;
    int maxIterations = queue.size();

    do  {
        maxIterations = queue.size();
        PluginDesc desc = queue.takeFirst();
        QList<DepNode*> nodes;
        int nDeps = desc.dependsList.size();
        if (nDeps == 0) {
            nodes.append(rootNode);
        } else {
            resetState(allNodes.qlist);
            findParentNodes(rootNode, desc, err, nodes);
        }
        if (!err.isEmpty()) {
            return unordered;
        }
        if (nDeps == 0 || nodes.size() == nDeps) {
            DepNode* descNode = new DepNode();
            descNode->desc = desc;
            allNodes.qlist.append(descNode);
            // now add this node as a child to all nodes it depends on
            foreach(DepNode* node, nodes) {
                node->childNodes.append(descNode);
                descNode->parentNodes.append(node);
            }
            iterations = 0;
            continue;
        }
        queue.append(desc);
        iterations++;
    } while (!queue.isEmpty() && iterations <= maxIterations);

    if (!queue.isEmpty()) {
        err = tr("Can't satisfy dependencies for %1 !").arg(queue.first().id);
        return unordered;
    }

    //traverse graph and add nodes in topological (reverse postorder) mode
    resetState(allNodes.qlist);
    orderTopological(rootNode, result);

    foreach (const PluginDesc &desc, result) {
        if (desc.id.contains("pcr", Qt::CaseInsensitive)) {
            result.removeAll(desc);
            result.prepend(desc);
        }
    }

#ifdef _DEBUG
    assert(result.size() == unordered.size());
    foreach (const PluginDesc& desc, unordered) {
        int idx = result.indexOf(desc);
        assert(idx >= 0);
    }
#endif

    return result;
}


} //namespace
