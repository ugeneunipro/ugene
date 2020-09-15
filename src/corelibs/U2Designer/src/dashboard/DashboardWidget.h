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

#ifndef _U2_DASHBOARD_WIDGET_H_
#define _U2_DASHBOARD_WIDGET_H_

#include <QFileInfo>
#include <QGridLayout>
#include <QMenu>
#include <QToolButton>
#include <QWidget>

#include <U2Core/global.h>

namespace U2 {

/**
 * Container class for all Dashboard widgets.
 *
 * Adds title and bounding box around widget content.
 */
class U2DESIGNER_EXPORT DashboardWidget : public QWidget {
    Q_OBJECT
public:
    DashboardWidget(const QString &title, QWidget *contentWidget);
};

class U2DESIGNER_EXPORT DashboardFileButton : public QToolButton {
    Q_OBJECT
public:
    DashboardFileButton(const QStringList &urlList, const QString &dashboardDir, bool isFolderMode = false);

private slots:
    void sl_openFileClicked();

private:
    void addUrlActionsToMenu(QMenu *menu, const QString &url, bool addOpenByUgeneAction = false);

    /** List of urls to open. */
    QStringList urlList;

    /** Path to dashboard dir. */
    QFileInfo dashboardDirInfo;

    /** The must process the URL as a folder. */
    bool isFolderMode;
};

/** Styled dashboard menu used with file buttons. */
class U2DESIGNER_EXPORT DashboardPopupMenu : public QMenu {
    Q_OBJECT
public:
    explicit DashboardPopupMenu(QAbstractButton *button, QWidget *parent = 0);
    void showEvent(QShowEvent *event);

private:
    QAbstractButton *button;
};

/** Various Dashboard widget helpers. */
class U2DESIGNER_EXPORT DashboardWidgetUtils {
public:
    static void addTableHeadersRow(QGridLayout *gridLayout, const QStringList &headerNameList);

    static void addTableRow(QGridLayout *gridLayout, const QString &rowId, const QStringList &valueList);

    static void addTableCell(QGridLayout *gridLayout, const QString &rowId, QWidget *widget, int row, int column, bool isLastRow, bool isLastColumn);

    static void addTableCell(QGridLayout *gridLayout, const QString &rowId, const QString &text, int row, int column, bool isLastRow, bool isLastColumn);

    /**
     * Adds new row or updates existing row in the table. Uses rowId to compare rows.
     * Returns true if a new row was added.
     */
    static bool addOrUpdateTableRow(QGridLayout *gridLayout, const QString &rowId, const QStringList &valueList);

    static QString parseOpenUrlValueFromOnClick(const QString &onclickValue);
};

}    // namespace U2
#endif
