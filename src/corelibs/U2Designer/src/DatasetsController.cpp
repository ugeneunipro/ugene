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

#include "DatasetsController.h"

#include <QFileInfo>
#include <QGroupBox>
#include <QMessageBox>
#include <QSplitter>

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/URLContainer.h>

#include "DatasetWidget.h"
#include "DatasetsListWidget.h"
#include "DbFolderItem.h"
#include "DbObjectItem.h"
#include "DelegateEditors.h"
#include "DirectoryItem.h"
#include "FileItem.h"
#include "UrlItem.h"

namespace U2 {

/************************************************************************/
/* Utilities */
/************************************************************************/
class ItemWidgetCreator : public URLContainerVisitor {
public:
    ItemWidgetCreator()
        : urlItem(nullptr) {
    }

    void visit(FileUrlContainer* url) override {
        urlItem = new FileItem(url->getUrl());
    }

    void visit(DirUrlContainer* url) override {
        auto dItem = new DirectoryItem(url->getUrl());
        dItem->setIncludeFilter(url->getIncludeFilter());
        dItem->setExcludeFilter(url->getExcludeFilter());
        dItem->setRecursive(url->isRecursive());
        urlItem = dItem;
    }

    void visit(DbObjUrlContainer* url) override {
        urlItem = new DbObjectItem(url->getUrl());
    }

    void visit(DbFolderUrlContainer* url) override {
        auto dItem = new DbFolderItem(url->getUrl());
        dItem->setRecursive(url->isRecursive());
        urlItem = dItem;
    }

    UrlItem* getWidget() {
        return urlItem;
    }

private:
    UrlItem* urlItem;
};

class URLContainerUpdateHelper : public UrlItemVisitor {
public:
    URLContainerUpdateHelper(FileUrlContainer* /*url*/)
        : dirUrl(nullptr), dbFolderUrl(nullptr) {
    }
    URLContainerUpdateHelper(DirUrlContainer* url)
        : dirUrl(url), dbFolderUrl(nullptr) {
    }
    URLContainerUpdateHelper(DbObjUrlContainer* /*url*/)
        : dirUrl(nullptr), dbFolderUrl(nullptr) {
    }
    URLContainerUpdateHelper(DbFolderUrlContainer* url)
        : dirUrl(nullptr), dbFolderUrl(url) {
    }

    void visit(DirectoryItem* item) override {
        SAFE_POINT(dirUrl != nullptr, "NULL folder url", );
        dirUrl->setIncludeFilter(item->getIncludeFilter());
        dirUrl->setExcludeFilter(item->getExcludeFilter());
        dirUrl->setRecursive(item->isRecursive());
    }

    void visit(FileItem* /*item*/) override {
    }

    void visit(DbObjectItem* /*item*/) override {
    }

    void visit(DbFolderItem* item) override {
        SAFE_POINT(dbFolderUrl != nullptr, "NULL folder url", );
        dbFolderUrl->setRecursive(item->isRecursive());
    }

private:
    DirUrlContainer* dirUrl;
    DbFolderUrlContainer* dbFolderUrl;
};

class URLContainerUpdater : public URLContainerVisitor {
public:
    URLContainerUpdater(UrlItem* _item)
        : item(_item) {
    }

    void visit(FileUrlContainer* url) override {
        URLContainerUpdateHelper helper(url);
        item->accept(&helper);
    }

    void visit(DirUrlContainer* url) override {
        URLContainerUpdateHelper helper(url);
        item->accept(&helper);
    }

    void visit(DbObjUrlContainer* url) override {
        URLContainerUpdateHelper helper(url);
        item->accept(&helper);
    }

    void visit(DbFolderUrlContainer* url) override {
        URLContainerUpdateHelper helper(url);
        item->accept(&helper);
    }

private:
    UrlItem* item;
};

/************************************************************************/
/* Controller */
/************************************************************************/
DatasetsController::DatasetsController(const QSet<GObjectType>& compatibleObjTypes, QObject* parent)
    : QObject(parent),
      compatibleObjTypes(compatibleObjTypes) {
}

void DatasetsController::update() {
    emit si_attributeChanged();
}

void DatasetsController::checkName(const QString& newName, U2OpStatus& os, const QString& exception) {
    if (newName.isEmpty()) {
        os.setError(tr("Dataset name is empty"));
        return;
    }
    if (newName.contains(";") || newName.contains(":")) {
        os.setError(tr("Dataset name can not contain ':' and ';' symbols"));
        return;
    }
    foreach (const QString& name, names()) {
        if (name != exception && name == newName) {
            os.setError(tr("This dataset name already exists"));
            return;
        }
    }
}

const QSet<GObjectType>& DatasetsController::getCompatibleObjTypes() const {
    return compatibleObjTypes;
}

/************************************************************************/
/* AttributeDatasetsController */
/************************************************************************/
AttributeDatasetsController::AttributeDatasetsController(QList<Dataset>& _sets, const QSet<GObjectType>& compatibleObjTypes)
    : DatasetsController(compatibleObjTypes), datasetsWidget(nullptr) {
    initSets(_sets);
    initialize();
}

void AttributeDatasetsController::initSets(const QList<Dataset>& s) {
    foreach (const Dataset& d, s) {
        sets << new Dataset(d);
    }
}

void AttributeDatasetsController::initialize() {
    SAFE_POINT(!sets.empty(), "0 datasets count", );
    datasetsWidget = new DatasetsListWidget(this);

    foreach (Dataset* dSet, sets) {
        datasetsWidget->appendPage(dSet->getName(), createDatasetWidget(dSet));
    }
}

AttributeDatasetsController::~AttributeDatasetsController() {
    datasetsWidget->setParent(nullptr);
    delete datasetsWidget;
    qDeleteAll(sets);
    sets.clear();
}

QWidget* AttributeDatasetsController::getWidget() {
    return datasetsWidget;
}

QList<Dataset> AttributeDatasetsController::getDatasets() {
    QList<Dataset> result;
    foreach (Dataset* d, sets) {
        result << Dataset(*d);
    }
    return result;
}

URLListWidget* AttributeDatasetsController::createDatasetWidget(Dataset* dSet) {
    auto ctrl = new URLListController(this, dSet);
    return ctrl->getWidget();
}

void AttributeDatasetsController::deleteDataset(int dsNum) {
    SAFE_POINT(dsNum < sets.size(), "Datasets: out of range", );

    Dataset* dSet = sets.at(dsNum);
    SAFE_POINT(dSet != nullptr, "NULL dataset", );

    sets.removeOne(dSet);

    delete dSet;

    // add empty default dataset is the last one is deleted
    if (sets.isEmpty()) {
        sets << new Dataset();
        datasetsWidget->appendPage(sets.first()->getName(),
                                   createDatasetWidget(sets.first()));
    }
    update();
}

void AttributeDatasetsController::addDataset(const QString& name, U2OpStatus& os) {
    checkName(name, os);
    CHECK_OP(os, );
    sets << new Dataset(name);
    datasetsWidget->appendPage(sets.last()->getName(),
                               createDatasetWidget(sets.last()));
    update();
}

void AttributeDatasetsController::renameDataset(int dsNum, const QString& newName, U2OpStatus& os) {
    SAFE_POINT(dsNum < sets.size(), "Datasets: out of range", );

    Dataset* dSet = sets.at(dsNum);
    SAFE_POINT(dSet != nullptr, "NULL dataset", );
    QPair<QString, QString> oldNewNames(dSet->getName(), newName);
    checkName(newName, os, dSet->getName());
    CHECK_OP(os, );

    dSet->setName(newName);
    emit si_datasetRenamed(oldNewNames);
}

QStringList AttributeDatasetsController::names() const {
    QStringList result;
    foreach (Dataset* dSet, sets) {
        result << dSet->getName();
    }
    return result;
}

void AttributeDatasetsController::onUrlAdded(URLListController* /*ctrl*/, URLContainer* /*url*/) {
}

/************************************************************************/
/* PairedController */
/************************************************************************/
PairedReadsController::PairedReadsController(const QList<Dataset>& sets1, const QList<Dataset>& sets2, const QString& _label1, const QString& _label2)
    : DatasetsController(), label1(_label1), label2(_label2), datasetsWidget(nullptr) {
    initSets(sets1, sets2);
    initialize();
}

void PairedReadsController::initSets(const QList<Dataset>& s1, const QList<Dataset>& s2) {
    QList<Dataset>::ConstIterator it1 = s1.constBegin();
    QList<Dataset>::ConstIterator it2 = s2.constBegin();
    for (; it1 != s1.end() && it2 != s2.end(); it1++, it2++) {
        SetsPair pair;
        pair.first = new Dataset(*it1);
        pair.second = new Dataset(*it2);
        pair.second->setName(it1->getName());
        sets << pair;
    }
}

void PairedReadsController::initialize() {
    SAFE_POINT(!sets.empty(), "0 datasets count", );
    datasetsWidget = new DatasetsListWidget(this);

    foreach (const SetsPair& p, sets) {
        datasetsWidget->appendPage(p.first->getName(), createDatasetWidget(p));
    }
}

static QWidget* getLayout(QWidget* widget, const QString& labelText) {
    if (labelText.isEmpty()) {
        return widget;
    }

    auto label = new QLabel(labelText);
    label->setStyleSheet(
        "background: palette(midlight);"
        "border-style: solid;"
        "border-width: 1px;"
        "border-color: palette(mid);"
        "text-align: center;"
        "padding: 2px;");

    auto result = new QWidget();
    auto l = new QVBoxLayout(result);
    l->setContentsMargins(0, 3, 0, 0);
    l->addWidget(label);
    l->addWidget(widget);
    return result;
}

QWidget* PairedReadsController::createDatasetWidget(const SetsPair& pair) {
    CtrlsPair cp;
    cp.first = new URLListController(this, pair.first);
    cp.second = new URLListController(this, pair.second);
    ctrls << cp;

    auto splitter = new QSplitter();
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(getLayout(cp.first->getWidget(), label1));
    splitter->addWidget(getLayout(cp.second->getWidget(), label2));
    return splitter;
}

PairedReadsController::~PairedReadsController() {
    datasetsWidget->setParent(nullptr);
    delete datasetsWidget;
    foreach (const SetsPair& p, sets) {
        delete p.first;
        delete p.second;
    }
    sets.clear();
}

QWidget* PairedReadsController::getWidget() {
    return datasetsWidget;
}

QList<Dataset> PairedReadsController::getDatasets(int num) {
    QList<Dataset> result;
    foreach (const SetsPair& p, sets) {
        Dataset* d = num == 0 ? p.first : p.second;
        result << Dataset(*d);
    }
    return result;
}

void PairedReadsController::renameDataset(int dsNum, const QString& newName, U2OpStatus& os) {
    SAFE_POINT(dsNum < sets.size(), "Datasets: out of range", );

    SetsPair& pair = sets[dsNum];

    checkName(newName, os, pair.first->getName());
    CHECK_OP(os, );

    pair.first->setName(newName);
    pair.second->setName(newName);
    update();
}

void PairedReadsController::deleteDataset(int dsNum) {
    SAFE_POINT(dsNum < sets.size(), "Datasets: out of range", );
    SAFE_POINT(dsNum < ctrls.size(), "Datasets ctrl: out of range", );

    SetsPair dp = sets[dsNum];
    CtrlsPair cp = ctrls[dsNum];
    sets.removeOne(dp);
    ctrls.removeOne(cp);
    delete dp.first;
    delete dp.second;

    // add empty default dataset is the last one is deleted
    if (sets.isEmpty()) {
        SetsPair pair(new Dataset(), new Dataset());
        sets << pair;
        datasetsWidget->appendPage(pair.first->getName(),
                                   createDatasetWidget(pair));
    }
    update();
}

void PairedReadsController::addDataset(const QString& name, U2OpStatus& os) {
    checkName(name, os);
    CHECK_OP(os, );
    sets << SetsPair(new Dataset(name), new Dataset(name));
    datasetsWidget->appendPage(sets.last().first->getName(),
                               createDatasetWidget(sets.last()));
    update();
}

QStringList PairedReadsController::names() const {
    QStringList result;
    foreach (const SetsPair& p, sets) {
        result << p.first->getName();
    }
    return result;
}

static QString replaceNum(const QString& name) {
    int onePos = name.lastIndexOf("1");
    int twoPos = name.lastIndexOf("2");

    QString srcNum = "1";
    QString dstNum = "2";
    int pos = onePos;
    if (onePos < twoPos) {
        srcNum = "2";
        dstNum = "1";
        pos = twoPos;
    }
    return name.left(pos) + dstNum + name.mid(pos + 1);
}

static QString getMateUrl(const QString& url) {
    QFileInfo info(url);
    QString dir = info.dir().absolutePath();
    QString name = info.baseName();
    QString ext = info.fileName().mid(name.size());
    if (name.lastIndexOf("1") > 0 || name.lastIndexOf("2") > 0) {
        return dir + "/" + replaceNum(info.baseName()) + ext;
    }
    return "";
}

static bool askAboutAdding(const QString& mateUrl) {
    QString title = QObject::tr("Paired reads");
    QString file = QFileInfo(mateUrl).fileName();
    QString src = QFileInfo(mateUrl).isFile() ? QObject::tr("file") : QObject::tr("folder");
    QString message = QObject::tr("UGENE has found \"%1\" on the file system. Do you want to add it as a second %2 with paired reads?").arg(file).arg(src);

    int answer = QMessageBox::question(QApplication::activeWindow(), title, message, QMessageBox::Yes, QMessageBox::No);
    return (QMessageBox::Yes == answer);
}

void PairedReadsController::onUrlAdded(URLListController* ctrl, URLContainer* url) {
    QString mateUrl = getMateUrl(url->getUrl());
    CHECK(!mateUrl.isEmpty(), );
    CHECK(QFile::exists(mateUrl), );

    URLListController* targetCtrl = pairedCtrl(ctrl);
    CHECK(targetCtrl != nullptr, );
    CHECK(!targetCtrl->dataset()->contains(mateUrl), );

    bool add = askAboutAdding(mateUrl);
    if (add) {
        U2OpStatusImpl os;
        targetCtrl->addUrl(mateUrl, os);
    }
}

int PairedReadsController::pairNumByCtrl(URLListController* ctrl) const {
    int result = 0;
    foreach (const CtrlsPair& cp, ctrls) {
        if (cp.first == ctrl || cp.second == ctrl) {
            return result;
        }
        result++;
    }
    return -1;
}

URLListController* PairedReadsController::pairedCtrl(URLListController* ctrl) const {
    int dsNum = pairNumByCtrl(ctrl);
    SAFE_POINT(-1 != dsNum, "Unregistered url list controller", nullptr);

    CtrlsPair cp = ctrls[dsNum];
    if (cp.first == ctrl) {
        return cp.second;
    } else {
        return cp.first;
    }
}

/************************************************************************/
/* UrlAndDatasetController */
/************************************************************************/
UrlAndDatasetController::UrlAndDatasetController(const QList<Dataset>& _urls, const QList<Dataset>& _sets, const QString& _urlLabel, const QString& _datasetLabel)
    : DatasetsController(), urlLabel(_urlLabel), datasetLabel(_datasetLabel), datasetsWidget(nullptr) {
    initSets(_urls, _sets);
    initialize();
    update();
}

void UrlAndDatasetController::initSets(const QList<Dataset>& _urls, const QList<Dataset>& s) {
    for (Dataset urlSet : qAsConst(_urls)) {
        foreach (URLContainer* urlCon, urlSet.getUrls()) {
            urls << urlCon->getUrl();
        }
    }

    QList<Dataset>::ConstIterator it = s.constBegin();
    for (; it != s.end(); it++) {
        sets << new Dataset(*it);
    }

    while (sets.count() < urls.count()) {
        sets << new Dataset();
    }

    while (urls.count() < sets.count()) {
        urls << "";
    }

    // Set datasets names
    for (int i = 0; i < urls.count(); ++i) {
        if (sets[i]->getName().isEmpty()) {
            sets[i]->setName(QFileInfo(urls[i]).fileName());
        }
    }
}

void UrlAndDatasetController::initialize() {
    SAFE_POINT(!sets.empty(), "0 datasets count", );
    datasetsWidget = new DatasetsListWidget(this);

    foreach (Dataset* set, sets) {
        datasetsWidget->appendPage(set->getName(), createDatasetPageWidget(set));
    }
}

QWidget* UrlAndDatasetController::createDatasetPageWidget(Dataset* set) {
    CtrlsPair cp;

    auto urlDelegate = new URLDelegate("", "", false, false, false);
    connect(urlDelegate, &URLDelegate::commitData, this, &UrlAndDatasetController::sl_urlChanged);

    cp.first = urlDelegate;
    cp.second = new URLListController(this, set);
    ctrls << cp;

    auto widget = new QWidget();
    auto layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setContentsMargins(0, 4, 0, 4);
    layout->setSpacing(6);
    widget->setLayout(layout);

    layout->addWidget(createUrlWidget(urlDelegate, getUrlByDataset(set)));
    layout->addWidget(getLayout(cp.second->getWidget(), datasetLabel));
    return widget;
}

QWidget* UrlAndDatasetController::createUrlWidget(URLDelegate* ctrl, const QString& value) {
    auto urlWidget = qobject_cast<URLWidget*>(ctrl->createEditor(nullptr, QStyleOptionViewItem(), QModelIndex()));
    urlWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    urlWidget->setValue(value);

    auto labeledWidget = new QWidget();
    auto labeledLayout = new QHBoxLayout;
    labeledLayout->setContentsMargins(6, 8, 0, 7);

    auto urlLabelWidget = new QLabel(urlLabel);
    urlLabelWidget->setObjectName(urlLabel + " label");
    labeledLayout->addWidget(urlLabelWidget);
    labeledLayout->addWidget(urlWidget);
    labeledWidget->setLayout(labeledLayout);

    labeledWidget->setObjectName(urlLabel + " labeledWidget");
    return labeledWidget;
}

QWidget* UrlAndDatasetController::createDatasetWidget(URLListController* ctrl) {
    auto result = new QGroupBox(datasetLabel);
    auto layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(ctrl->getWidget());
    result->setLayout(layout);

    return result;
}

UrlAndDatasetController::~UrlAndDatasetController() {
    datasetsWidget->setParent(nullptr);
    delete datasetsWidget;

    qDeleteAll(sets);
}

QWidget* UrlAndDatasetController::getWidget() {
    return datasetsWidget;
}

QList<Dataset> UrlAndDatasetController::getDatasets() const {
    QList<Dataset> result;
    foreach (Dataset* set, sets) {
        result << *set;
    }

    return result;
}

QList<Dataset> UrlAndDatasetController::getUrls() const {
    QList<Dataset> result;
    for (int i = 0; i < urls.count(); ++i) {
        QString a = sets[i]->getName();
        Dataset set(sets[i]->getName());
        set.addUrl(URLContainerFactory::createUrlContainer(urls[i]));
        result << set;
    }
    return result;
}

void UrlAndDatasetController::sl_urlChanged(QWidget* widget) {
    auto urlDelegate = qobject_cast<URLDelegate*>(sender());
    SAFE_POINT(urlDelegate, "URL delegate is NULL", );

    auto editor = qobject_cast<URLWidget*>(widget);
    SAFE_POINT(editor, "Unexpected widget", );

    for (int i = 0; i < ctrls.count(); i++) {
        if (ctrls[i].first == urlDelegate) {
            urls[i] = editor->value().toString();
            update();
            break;
        }
    }
}

void UrlAndDatasetController::renameDataset(int dsNum, const QString& newName, U2OpStatus& os) {
    SAFE_POINT(dsNum < sets.size(), "Datasets: out of range", );

    checkName(newName, os, sets[dsNum]->getName());
    CHECK_OP(os, );

    sets[dsNum]->setName(newName);
    update();
}

void UrlAndDatasetController::deleteDataset(int dsNum) {
    SAFE_POINT(dsNum < sets.size(), "Datasets: out of range", );
    SAFE_POINT(dsNum < ctrls.size(), "Datasets ctrl: out of range", );

    delete sets[dsNum];
    sets.removeAt(dsNum);

    urls.removeAt(dsNum);
    ctrls.removeAt(dsNum);

    // add empty default dataset is the last one is deleted
    if (sets.isEmpty()) {
        sets << new Dataset();
        urls << "";
        datasetsWidget->appendPage(sets.last()->getName(),
                                   createDatasetPageWidget(sets.last()));
    }
    update();
}

void UrlAndDatasetController::addDataset(const QString& name, U2OpStatus& os) {
    checkName(name, os);
    CHECK_OP(os, );
    sets << new Dataset(name);
    urls << "";
    datasetsWidget->appendPage(name,
                               createDatasetPageWidget(sets.last()));
    update();
}

QStringList UrlAndDatasetController::names() const {
    QStringList result;
    foreach (Dataset* set, sets) {
        result << set->getName();
    }
    return result;
}

QString UrlAndDatasetController::getUrlByDataset(Dataset* set) const {
    return urls.at(sets.indexOf(set));
}

/************************************************************************/
/* URLListController */
/************************************************************************/
URLListController::URLListController(DatasetsController* parent, Dataset* _set)
    : QObject(parent), widget(nullptr), controller(parent), set(_set) {
}

URLListWidget* URLListController::getWidget() {
    if (widget == nullptr) {
        createWidget();
    }
    return widget;
}

Dataset* URLListController::dataset() {
    return set;
}

void URLListController::createWidget() {
    widget = new URLListWidget(this);

    foreach (URLContainer* url, set->getUrls()) {
        addItemWidget(url);
    }
}

void URLListController::addItemWidget(URLContainer* url) {
    SAFE_POINT(widget != nullptr, "NULL url list widget", );
    ItemWidgetCreator wc;
    url->accept(&wc);
    urlMap[wc.getWidget()] = url;

    widget->addUrlItem(wc.getWidget());
}

void URLListController::updateUrl(UrlItem* item) {
    URLContainer* url = urlMap[item];
    SAFE_POINT(url != nullptr, "NULL url container", );

    URLContainerUpdater updater(item);
    url->accept(&updater);
    controller->update();
}

void URLListController::replaceUrl(int pos, int newPos) {
    URLContainer* url = getUrl(pos);
    CHECK(url != nullptr, );

    SAFE_POINT(newPos >= 0 && newPos < set->getUrls().size(),
               "New url position is out of range", );

    set->getUrls().removeAt(pos);
    set->getUrls().insert(newPos, url);
    controller->update();
}

void URLListController::addUrl(const QString& url, U2OpStatus& os) {
    URLContainer* urlCont = URLContainerFactory::createUrlContainer(url);
    if (urlCont == nullptr) {
        os.setError(tr("This file or folder does not exist: %1").arg(url));
        return;
    }

    set->addUrl(urlCont);
    addItemWidget(urlCont);

    controller->onUrlAdded(this, urlCont);
    controller->update();
}

void URLListController::deleteUrl(int pos) {
    URLContainer* url = getUrl(pos);
    CHECK(url != nullptr, );

    set->removeUrl(url);
    delete url;
    controller->update();
}

URLContainer* URLListController::getUrl(int pos) {
    SAFE_POINT(pos < set->getUrls().size(), "Urls: out of range", nullptr);
    URLContainer* url = set->getUrls().at(pos);
    SAFE_POINT(url != nullptr, "NULL url container", nullptr);
    return url;
}

const QSet<GObjectType>& URLListController::getCompatibleObjTypes() const {
    return controller->getCompatibleObjTypes();
}

}  // namespace U2
