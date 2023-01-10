/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#ifndef _U2_WELCOME_PAGE_WIDGET_H_
#define _U2_WELCOME_PAGE_WIDGET_H_

#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

namespace U2 {

class WelcomePageWidget : public QScrollArea {
    Q_OBJECT
public:
    WelcomePageWidget(QWidget* parent);

    void updateRecent(const QStringList& recentProjects, const QStringList& recentFiles);

    bool eventFilter(QObject* watched, QEvent* event) override;

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;

private slots:
    void sl_openFiles();
    void sl_createSequence();
    void sl_createWorkflow();
    void sl_openQuickStart();

    void sl_openRecentFile();

private:
    void runAction(const QString& actionId);

    QWidget* createHeaderWidget();
    QWidget* createMiddleWidget();
    QWidget* createFooterWidget();

    QVBoxLayout* recentFilesLayout = nullptr;
    QVBoxLayout* recentProjectsLayout = nullptr;
};

}  // namespace U2

#endif  // _U2_WELCOME_PAGE_WIDGET_H_
