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

#pragma once

#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

namespace U2 {

class HoverQLabel;

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
    void sl_colorModeSwitched();

private:
    void runAction(const QString& actionId);

    QWidget* createHeaderWidget();
    QWidget* createMiddleWidget();
    QWidget* createFooterWidget();

    QWidget* headerWidget = nullptr;
    QLabel* topLevelWidgetLabel = nullptr;

    QWidget* middleWidget = nullptr;
    QWidget* middleRightWidget = nullptr;
    HoverQLabel* openFilesButton = nullptr;
    HoverQLabel* createSequenceButton = nullptr;
    HoverQLabel* createWorkflowButton = nullptr;
    HoverQLabel* quickStartButton = nullptr;
    QLabel* recentHeaderLabel = nullptr;
    QLabel* recentProjectsLabel = nullptr;

    QWidget* footerWidget = nullptr;
    QWidget* footerStrippedLineWidget = nullptr;
    QWidget* footerBottomWidget = nullptr;
    QLabel* footerSupportLabel = nullptr;

    QVBoxLayout* recentFilesLayout = nullptr;
    QVBoxLayout* recentProjectsLayout = nullptr;

    static const QString BACKGROUND_QLINEARGRADIENT;
    static const QString PADDING_LEFT;
    static const QString NORMAL_STYLE;
    static const QString HOVERED_STYLE;
    static const QString MIDDLE_RIGHT_WIDGET;
    static const QString RECENT_HEADER_STYLE;
    static const QString BACKGROUND_COLOR;
    static const QString BACKGROUND_URL;
    static const QString COLOR_FONT_SIZE;
    static const QString FOOTER_SUPPORT_LABEL_STRING;
    static const QString RECENT_ITEM_STYLE;
};

}  // namespace U2

