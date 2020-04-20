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

#ifndef _U2_EXTERNAL_TOOL_SUPPORT_SETTINGS_CONTROLLER_H
#define _U2_EXTERNAL_TOOL_SUPPORT_SETTINGS_CONTROLLER_H

#include <U2Gui/AppSettingsGUI.h>
#include <U2Core/ExternalToolRegistry.h>
#include <ui_ETSSettingsWidget.h>

#include <QLineEdit>

namespace U2 {

class ShowHideSubgroupWidget;

#define ExternalToolSupportSettingsPageId QString("ets")
struct ExternalToolInfo {
    QString id;
    QString name;
    QString path;
    QString description;
    QString version;
    bool    valid;
    bool    isModule;
};

class ExternalToolSupportSettingsPageController : public AppSettingsGUIPageController {
    Q_OBJECT
public:
    ExternalToolSupportSettingsPageController(QObject* p = NULL);

    AppSettingsGUIPageState* getSavedState();
    void saveState(AppSettingsGUIPageState* s);
    AppSettingsGUIPageWidget* createWidget(AppSettingsGUIPageState* state);
    const QString& getHelpPageId() const;

private:
    static const QString helpPageId;
};

class ExternalToolSupportSettingsPageState : public AppSettingsGUIPageState {
    Q_OBJECT
public:
    ExternalToolSupportSettingsPageState(const QList<ExternalTool*>& ets);

    QList<ExternalTool*> getExternalTools() const;

private:
    QList<ExternalTool *> externalTools;
};

class ExternalToolSupportSettingsPageWidget : public AppSettingsGUIPageWidget, public Ui_ETSSettingsWidget {
    Q_OBJECT
public:
    ExternalToolSupportSettingsPageWidget(ExternalToolSupportSettingsPageController* ctrl);
    ~ExternalToolSupportSettingsPageWidget() override;

    virtual void setState(AppSettingsGUIPageState* state) override;

    virtual AppSettingsGUIPageState* getState(QString& err) const override;

private:
    QWidget* createPathEditor(QWidget *parent, const QString& path) const;
    QTreeWidgetItem *findToolkitItem(QTreeWidget *treeWidget, const QString &toolkitName);
    QTreeWidgetItem *createToolkitItem(QTreeWidget *treeWidget, const QString &toolkitName, const QIcon &icon);
    QTreeWidgetItem* insertChild(QTreeWidgetItem* rootItem, const QString& id, int pos, bool isModule = false);
    static ExternalTool* isMasterWithModules(const QList<ExternalTool*>& toolsList);
    void setToolState(ExternalTool* tool);
    QString getToolStateDescription(ExternalTool* tool) const;
    void resetDescription();
    void setDescription(ExternalTool* tool);
    QString warn(const QString& text) const;
    bool eventFilter(QObject *watched, QEvent *event) override;
    void saveShowHideSubgroupsState() const;

private slots:
    void sl_toolPathChanged();
    void sl_itemSelectionChanged();
    void sl_onPathEditWidgetClick();
    void sl_onBrowseToolKitPath();
    void sl_onBrowseToolPackPath();
    void sl_linkActivated(const QString& url);
    void sl_toolValidationStatusChanged(bool isValid);
    void sl_validationComplete();
    void sl_onClickLink(const QUrl& url);
    void sl_importCustomToolButtonClicked();
    void sl_deleteCustomToolButtonClicked();
    void sl_externalToolAdded(const QString &id);
    void sl_externalToolIsAboutToBeRemoved(const QString &id);

private:
    QMap<QString, ExternalToolInfo> externalToolsInfo;
    QMap<QString, QTreeWidgetItem *> externalToolsItems;
    QString getToolLink(const QString &toolName) const;
    mutable int buttonsWidth;
    QString defaultDescriptionText;
    ShowHideSubgroupWidget* supportedToolsShowHideWidget;
    ShowHideSubgroupWidget* customToolsShowHideWidget;
    ShowHideSubgroupWidget* infoShowHideWidget;

    static const QString INSTALLED;
    static const QString NOT_INSTALLED;
    static const QString ET_DOWNLOAD_INFO;
    static const QString SUPPORTED_ID;
    static const QString CUSTOM_ID;
    static const QString INFORMATION_ID;
};

class PathLineEdit : public QLineEdit {
    Q_OBJECT
public:
    PathLineEdit(const QString& filter, const QString& type, bool multi, QWidget *parent)
        : QLineEdit(parent), FileFilter(filter), type(type), multi(multi) {}

signals:
    void si_focusIn();

private slots:
    void sl_onBrowse();
    void sl_clear();

private:
    void focusInEvent(QFocusEvent *event) override;

    QString FileFilter;
    QString type;
    bool    multi;
    QString path;
};

}//namespace

#endif // _U2_EXTERNAL_TOOL_SUPPORT_SETTINGS_CONTROLLER_H
