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

#ifndef _U2_DATASETS_CONTROLLER_H_
#define _U2_DATASETS_CONTROLLER_H_

#include <U2Core/U2OpStatus.h>

#include <U2Lang/URLAttribute.h>

namespace U2 {

class DatasetsListWidget;
class URLDelegate;
class URLListController;
class URLListWidget;
class UrlItem;

/************************************************************************/
/* DatasetsController */
/************************************************************************/
class U2DESIGNER_EXPORT DatasetsController : public QObject {
    Q_OBJECT
public:
    DatasetsController(const QSet<GObjectType> &compatibleObjTypes = QSet<GObjectType>(), QObject *parent = NULL);
    virtual ~DatasetsController();

    virtual void update();
    virtual void renameDataset(int dsNum, const QString &newName, U2OpStatus &os) = 0;
    virtual void deleteDataset(int dsNum) = 0;
    virtual void addDataset(const QString &name, U2OpStatus &os) = 0;
    virtual void onUrlAdded(URLListController *ctrl, URLContainer *url) = 0;

    virtual const QSet<GObjectType> & getCompatibleObjTypes() const;

signals:
    void si_attributeChanged();

protected:
    virtual QStringList names() const = 0;
    void checkName(const QString &name, U2OpStatus &os, const QString &exception = "");

private:
    QSet<GObjectType> compatibleObjTypes;
};

class U2DESIGNER_EXPORT AttributeDatasetsController : public DatasetsController {
public:
    AttributeDatasetsController(QList<Dataset> &sets, const QSet<GObjectType> &compatibleObjTypes);
    virtual ~AttributeDatasetsController();

    virtual void renameDataset(int dsNum, const QString &newName, U2OpStatus &os);
    virtual void deleteDataset(int dsNum);
    virtual void addDataset(const QString &name, U2OpStatus &os);
    virtual void onUrlAdded(URLListController *ctrl, URLContainer *url);

    QWidget * getWigdet();
    QList<Dataset> getDatasets();

protected:
    virtual QStringList names() const;

private:
    DatasetsListWidget *datasetsWidget;
    QList<Dataset*> sets;

private:
    void initSets(const QList<Dataset> &s);
    void initialize();
    URLListWidget * createDatasetWidget(Dataset *dSet);
};

class U2DESIGNER_EXPORT PairedReadsController : public DatasetsController {
public:
    PairedReadsController(const QList<Dataset> &sets1, const QList<Dataset> &sets2, const QString &label1, const QString &label2);
    virtual ~PairedReadsController();

    virtual void renameDataset(int dsNum, const QString &newName, U2OpStatus &os);
    virtual void deleteDataset(int dsNum);
    virtual void addDataset(const QString &name, U2OpStatus &os);
    virtual void onUrlAdded(URLListController *ctrl, URLContainer *url);

    QWidget * getWigdet();
    /** num == 0 || num == 1*/
    QList<Dataset> getDatasets(int num);

protected:
    virtual QStringList names() const;

private:
    QString label1;
    QString label2;
    typedef QPair<Dataset*, Dataset*> SetsPair;
    typedef QPair<URLListController*, URLListController*> CtrlsPair;
    QList<SetsPair> sets;
    QList<CtrlsPair> ctrls;
    DatasetsListWidget *datasetsWidget;

private:
    void initSets(const QList<Dataset> &sets1, const QList<Dataset> &sets2);
    void initialize();
    QWidget * createDatasetWidget(const SetsPair &pair);
    int pairNumByCtrl(URLListController *ctrl) const;
    URLListController * pairedCtrl(URLListController *ctrl) const;
};

class U2DESIGNER_EXPORT UrlAndDatasetController : public DatasetsController {
    Q_OBJECT
public:
    UrlAndDatasetController(const QList<Dataset>& urls, const QList<Dataset> &sets, const QString &_urlLabel, const QString &_datasetLabel);
    virtual ~UrlAndDatasetController();

    virtual void renameDataset(int dsNum, const QString &newName, U2OpStatus &os);
    virtual void deleteDataset(int dsNum);
    virtual void addDataset(const QString &name, U2OpStatus &os);
    virtual void onUrlAdded(URLListController * /*ctrl*/, URLContainer * /*url*/) {}

    QWidget * getWigdet();
    QList<Dataset> getDatasets() const;
    QList<Dataset> getUrls() const;

protected:
    virtual QStringList names() const;

private slots:
    void sl_urlChanged(QWidget *widget);

private:
    QString urlLabel;
    QString datasetLabel;
    typedef QPair<URLDelegate*, URLListController*> CtrlsPair;
    QStringList urls;
    QList<Dataset*> sets;
    QList<CtrlsPair> ctrls;
    DatasetsListWidget *datasetsWidget;

private:
    void initSets(const QList<Dataset> &_urls, const QList<Dataset> &sets);
    void initialize();
    QWidget *createDatasetPageWidget(Dataset *set);
    QWidget *createUrlWidget(URLDelegate* ctrl, const QString &value);
    QWidget *createDatasetWidget(URLListController* ctrl);
    QString getUrlByDataset(Dataset *set) const;
};

/************************************************************************/
/* URLListController */
/************************************************************************/
class U2DESIGNER_EXPORT URLListController : public QObject {
    Q_OBJECT
public:
    URLListController(DatasetsController *parent, Dataset *set);

    void addUrl(const QString &url, U2OpStatus &os);
    void replaceUrl(int pos, int newPos);
    void deleteUrl(int pos);
    void updateUrl(UrlItem *item);

    URLListWidget * getWidget();
    Dataset * dataset();
    const QSet<GObjectType> & getCompatibleObjTypes() const;

private:
    URLListWidget *widget;
    DatasetsController *controller;
    Dataset *set;
    QMap<UrlItem*, URLContainer*> urlMap;

private:
    void addItemWidget(URLContainer *url);
    void createWidget();
    URLContainer * getUrl(int pos);
};

} // U2

#endif // _U2_DATASETS_CONTROLLER_H_
