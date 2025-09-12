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

#include "WelcomePageWidget.h"

#include <QDesktopServices>
#include <QDropEvent>
#include <QFile>
#include <QFileInfo>
#include <QGridLayout>
#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/IdRegistry.h>
#include <U2Core/L10n.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/Settings.h>
#include <U2Core/Task.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/HoverQLabel.h>
#include <U2Gui/WelcomePageAction.h>

#include "main_window/MainWindowImpl.h"

namespace U2 {

const QString WelcomePageWidget::BACKGROUND_QLINEARGRADIENT = "background: qlineargradient(x1:0 y1:0, x2:1 y2:0, stop:0 %1, stop:1 %2);";
const QString WelcomePageWidget::PADDING_LEFT = "padding-left: 25px; color: %1; font-size: 34px;";
const QString WelcomePageWidget::NORMAL_STYLE = "text-decoration: none; color: %1; font-size: 18px;";
const QString WelcomePageWidget::HOVERED_STYLE = "text-decoration: underline; color: %1; font-size: 18px;";
const QString WelcomePageWidget::MIDDLE_RIGHT_WIDGET = QString("#middleRightWidget {%1 border-radius: 25px;}").arg(BACKGROUND_QLINEARGRADIENT);
const QString WelcomePageWidget::RECENT_HEADER_STYLE = "color: %1; font-size: 20px; background: transparent;";
const QString WelcomePageWidget::BACKGROUND_COLOR = "background-color: %1;";
const QString WelcomePageWidget::BACKGROUND_URL = "background: url('%1')";
const QString WelcomePageWidget::COLOR_FONT_SIZE = "color: %1; font-size: 16px;";
const QString WelcomePageWidget::FOOTER_SUPPORT_LABEL_STRING = "<b>%1</b>"
                                                               "<table cellspacing=7><tr>"
                                                               "<td width=30></td>"
                                                               "<td><a href='https://patreon.com/uniprougene'><img src='%2'></a></td>"
                                                               "</tr></table>";
const QString WelcomePageWidget::RECENT_ITEM_STYLE = "color: %1; font-size: 18px; padding-top: 0; padding-bottom: 0; padding-left: 5px; background: transparent;";

static QString newImageAndTextHtml(const QString& image, const QString& text) {
    return QString("<center>") +
           "<img src=':/ugene/images/welcome_page/" + image + "'>" +
           "<br>" + text +
           "</center>";
}

WelcomePageWidget::WelcomePageWidget(QWidget* parent)
    : QScrollArea(parent) {
    auto widget = new QWidget();
    auto layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(createHeaderWidget());
    layout->addWidget(createMiddleWidget());
    layout->addWidget(createFooterWidget());

    setWidget(widget);
    setWidgetResizable(true);  // make the widget to fill whole available space
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    installEventFilter(this);
    connect(AppContext::getMainWindow(), &MainWindow::si_colorThemeSwitched, this, &WelcomePageWidget::sl_colorThemeSwitched);
}

QWidget* WelcomePageWidget::createHeaderWidget() {
    headerWidget = new QWidget();
    headerWidget->setContentsMargins(0, 0, 0, 0);
    bool isDark = AppContext::getMainWindow()->isDarkTheme();
    headerWidget->setStyleSheet(BACKGROUND_QLINEARGRADIENT.arg(isDark ? "#494561" : "#E0E7E9").arg(QPalette().base().color().name()));
    headerWidget->setFixedHeight(110);
    auto headerWidgetLayout = new QHBoxLayout();
    headerWidgetLayout->setContentsMargins(0, 0, 0, 0);
    headerWidget->setLayout(headerWidgetLayout);
    topLevelWidgetLabel = new QLabel(tr("Welcome to UGENE"));
    topLevelWidgetLabel->setStyleSheet(PADDING_LEFT.arg(isDark ? "#2BB6F2" : "#145774"));
    headerWidgetLayout->addWidget(topLevelWidgetLabel);
    return headerWidget;
}

QWidget* WelcomePageWidget::createMiddleWidget() {
    middleWidget = new QWidget();
    middleWidget->setStyleSheet(QString("background: %1;").arg(QPalette().base().color().name()));
    auto middleWidgetVCenteringLayout = new QVBoxLayout();
    middleWidget->setLayout(middleWidgetVCenteringLayout);

    auto middleWidgetLayout = new QHBoxLayout();
    middleWidgetVCenteringLayout->addStretch(1);
    middleWidgetVCenteringLayout->addLayout(middleWidgetLayout, 2);
    middleWidgetVCenteringLayout->addStretch(2);

    auto middleLeftWidget = new QWidget();
    auto middleLeftWidgetLayout = new QHBoxLayout();
    middleLeftWidget->setLayout(middleLeftWidgetLayout);

    auto buttonsGridLayout = new QGridLayout();
    buttonsGridLayout->setVerticalSpacing(100);
    buttonsGridLayout->setHorizontalSpacing(140);
    QString openFilesText = tr("Open File(s)");

    QString createSequenceText = tr("Create Sequence");
    QString createWorkflowText = tr("Run or Create Workflow");
    QString quickStartText = tr("Quick Start Guide");
    bool isDark = AppContext::getMainWindow()->isDarkTheme();
    QString normalStyle = NORMAL_STYLE.arg(isDark ? "#2BB6F2" : "#145774");
    QString hoveredStyle = HOVERED_STYLE.arg(isDark ? "#2BB6F2" : "#145774");

    openFilesButton = new HoverQLabel(newImageAndTextHtml("welcome_btn_open.png", openFilesText), normalStyle, hoveredStyle);
    openFilesButton->updateStyles(normalStyle, hoveredStyle);
    openFilesButton->setObjectName("openFilesButton");
    connect(openFilesButton, SIGNAL(clicked()), SLOT(sl_openFiles()));
    buttonsGridLayout->addWidget(openFilesButton, 0, 0);

    createSequenceButton = new HoverQLabel(newImageAndTextHtml("welcome_btn_create_seq.png", createSequenceText), normalStyle, hoveredStyle);
    createSequenceButton->setObjectName("createSequenceButton");
    connect(createSequenceButton, SIGNAL(clicked()), SLOT(sl_createSequence()));
    buttonsGridLayout->addWidget(createSequenceButton, 0, 1);

    createWorkflowButton = new HoverQLabel(newImageAndTextHtml("welcome_btn_workflow.png", createWorkflowText), normalStyle, hoveredStyle);
    createWorkflowButton->setObjectName("createWorkflowButton");
    connect(createWorkflowButton, SIGNAL(clicked()), SLOT(sl_createWorkflow()));
    buttonsGridLayout->addWidget(createWorkflowButton, 1, 0);

    quickStartButton = new HoverQLabel(newImageAndTextHtml("welcome_btn_help.png", quickStartText), normalStyle, hoveredStyle);
    quickStartButton->setObjectName("quickStartButton");
    connect(quickStartButton, SIGNAL(clicked()), SLOT(sl_openQuickStart()));
    buttonsGridLayout->addWidget(quickStartButton, 1, 1);

    middleLeftWidgetLayout->addStretch();
    middleLeftWidgetLayout->addLayout(buttonsGridLayout);
    middleLeftWidgetLayout->addStretch();

    middleWidgetLayout->addWidget(middleLeftWidget, 3);

    middleRightWidget = new QWidget();
    middleRightWidget->setObjectName("middleRightWidget");
    middleRightWidget->setStyleSheet(MIDDLE_RIGHT_WIDGET.arg(isDark ? "#494561" : "#E0E7E9").arg(QPalette().base().color().name()));
    middleRightWidget->setContentsMargins(8, 8, 8, 8);
    middleWidgetLayout->addWidget(middleRightWidget, 2);

    auto middleRightWidgetLayout = new QVBoxLayout();
    middleRightWidget->setLayout(middleRightWidgetLayout);
    QString recentHeaderStyle = RECENT_HEADER_STYLE.arg(isDark ? "#2BB6F2" : "#145774");
    recentHeaderLabel = new QLabel(tr("Recent files"));
    recentHeaderLabel->setStyleSheet(recentHeaderStyle);
    middleRightWidgetLayout->addWidget(recentHeaderLabel);
    middleRightWidgetLayout->setSpacing(0);
    recentFilesLayout = new QVBoxLayout();
    middleRightWidgetLayout->addLayout(recentFilesLayout);
    recentProjectsLabel = new QLabel(tr("Recent projects"));
    recentProjectsLabel->setStyleSheet(recentHeaderStyle);
    middleRightWidgetLayout->addSpacing(15);
    middleRightWidgetLayout->addWidget(recentProjectsLabel);
    recentProjectsLayout = new QVBoxLayout();
    middleRightWidgetLayout->addLayout(recentProjectsLayout);
    middleRightWidgetLayout->addStretch();

    return middleWidget;
}

QWidget* WelcomePageWidget::createFooterWidget() {
    footerWidget = new QWidget();
    bool isDark = AppContext::getMainWindow()->isDarkTheme();
    footerWidget->setStyleSheet(BACKGROUND_COLOR.arg(isDark ? "#3F4547" : "#B2C4C9"));
    footerWidget->setFixedHeight(150);
    auto footerWidgetLayout = new QVBoxLayout();
    footerWidgetLayout->setContentsMargins(0, 0, 0, 0);
    footerWidget->setLayout(footerWidgetLayout);

    footerStrippedLineWidget = new QWidget();
    footerStrippedLineWidget->setFixedHeight(31);
    footerStrippedLineWidget->setStyleSheet(BACKGROUND_URL.arg(GUIUtils::getThemedPath(":/ugene/images/welcome_page/line.png")));
    footerWidgetLayout->addWidget(footerStrippedLineWidget);

    footerBottomWidget = new QWidget();
    footerBottomWidget->setFixedHeight(footerWidget->height() - footerStrippedLineWidget->height());
    footerWidgetLayout->addWidget(footerBottomWidget);
    footerBottomWidget->setStyleSheet(COLOR_FONT_SIZE.arg(isDark ? "#2BB6F2" : "#145774"));
    auto footerBottomWidgetLayout = new QHBoxLayout();
    footerBottomWidget->setLayout(footerBottomWidgetLayout);
    footerBottomWidgetLayout->setContentsMargins(25, 10, 25, 0);

    auto footerCiteLabel = new QLabel("<b>" + tr("Cite UGENE:") + "</b>" +
                                      "<table><tr><td width=40></td><td>"
                                      "\"Unipro UGENE: a unified bioinformatics toolkit\"<br>"
                                      "Okonechnikov; Golosova; Fursov; the UGENE team<br>"
                                      "Bioinformatics 2012 28: 1166-1167"
                                      "</td></tr></table>");
    footerCiteLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    footerCiteLabel->setAlignment(Qt::AlignTop);
    footerBottomWidgetLayout->addWidget(footerCiteLabel);
    footerBottomWidgetLayout->addStretch(1);

    auto footerFollowLabel = new QLabel("<b>" + tr("Follow UGENE:") + "</b>" +
                                        "<table cellspacing=7><tr>"
                                        "<td width=33></td>"
                                        "<td><a href='https://www.facebook.com/groups/ugene'><img src=':/ugene/images/welcome_page/social_icon_facebook.png'></a></td>"
                                        "<td><a href='https://www.linkedin.com/in/unipro-ugene-4a01b357/'><img src=':/ugene/images/welcome_page/social_icon_linkedin.png'></a></td>"
                                        "<td><a href='https://www.youtube.com/user/UniproUGENE'><img src=':/ugene/images/welcome_page/social_icon_youtube.png'></a></td>"
                                        "<td><a href='https://vk.com/uniprougene'><img src=':/ugene/images/welcome_page/social_icon_vkontakte.png'></a></td>"
                                        "</tr></table>");
    footerFollowLabel->setOpenExternalLinks(true);
    footerFollowLabel->setAlignment(Qt::AlignTop);
    footerBottomWidgetLayout->addWidget(footerFollowLabel);
    footerBottomWidgetLayout->addStretch(1);

    QString supportUgene = tr("Support UGENE:");
    footerSupportLabel = new QLabel(FOOTER_SUPPORT_LABEL_STRING.arg(supportUgene).arg(GUIUtils::getThemedPath(":/ugene/images/welcome_page/social_icon_patreon.png")));
    footerSupportLabel->setOpenExternalLinks(true);
    footerSupportLabel->setAlignment(Qt::AlignTop);
    footerBottomWidgetLayout->addWidget(footerSupportLabel);

    footerBottomWidgetLayout->addStretch(2);
    return footerWidget;
}

#define PATH_PROPERTY "path"
#define MAX_RECENT 7

void WelcomePageWidget::updateRecent(const QStringList& recentProjects, const QStringList& recentFiles) {
    // Clean lists.
    QLayoutItem* layoutItem;
    while ((layoutItem = recentFilesLayout->takeAt(0)) != nullptr) {
        delete layoutItem->widget();
        delete layoutItem;
    }
    while ((layoutItem = recentProjectsLayout->takeAt(0)) != nullptr) {
        delete layoutItem->widget();
        delete layoutItem;
    }

    // Add new items.
    bool isDark = AppContext::getMainWindow()->isDarkTheme();
    QString recentItemStyle = RECENT_ITEM_STYLE.arg(isDark ? "#2DC0FF" : "#1B769D");
    QString normalStyle = recentItemStyle + " text-decoration: none;";
    QString hoveredStyle = recentItemStyle + " text-decoration: underline;";

    for (int i = 0; i < recentFiles.size() && recentFilesLayout->count() < MAX_RECENT; i++) {
        QString recentFilePath = recentFiles[i];
        QString recentFileName = QFileInfo(recentFilePath).fileName();
        if (!recentFileName.isEmpty()) {
            auto recentFileLabel = new HoverQLabel("- " + recentFileName, normalStyle, hoveredStyle, QString("recent_file_%1").arg(i));
            recentFileLabel->setProperty(PATH_PROPERTY, recentFilePath);
            connect(recentFileLabel, SIGNAL(clicked()), SLOT(sl_openRecentFile()));
            recentFileLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
            recentFileLabel->setToolTip(recentFilePath);
            recentFilesLayout->addWidget(recentFileLabel);
        }
    }
    if (recentFilesLayout->count() == 0) {
        auto noFilesLabel = new QLabel(tr("No recent files"));
        noFilesLabel->setStyleSheet(recentItemStyle);
        recentFilesLayout->addWidget(noFilesLabel);
    }

    for (int i = 0; i < recentProjects.size() && recentProjectsLayout->count() < MAX_RECENT; i++) {
        QString recentProjectPath = recentProjects[i];
        QString recentProjectName = QFileInfo(recentProjectPath).fileName();
        if (!recentProjectName.isEmpty()) {
            auto recentProjectLabel = new HoverQLabel("- " + recentProjectName, normalStyle, hoveredStyle, QString("recent_project_%1").arg(i));
            recentProjectLabel->setProperty(PATH_PROPERTY, recentProjectPath);
            connect(recentProjectLabel, SIGNAL(clicked()), SLOT(sl_openRecentFile()));
            recentProjectLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
            recentProjectLabel->setToolTip(recentProjectPath);
            recentProjectsLayout->addWidget(recentProjectLabel);
        }
    }
    if (recentProjectsLayout->count() == 0) {
        auto noProjectsLabel = new QLabel(tr("No recent projects"));
        noProjectsLabel->setStyleSheet(recentItemStyle);
        recentProjectsLayout->addWidget(noProjectsLabel);
    }
}

void WelcomePageWidget::dragEnterEvent(QDragEnterEvent* event) {
    MainWindowDragNDrop::dragEnterEvent(event);
}

void WelcomePageWidget::dropEvent(QDropEvent* event) {
    MainWindowDragNDrop::dropEvent(event);
}

void WelcomePageWidget::dragMoveEvent(QDragMoveEvent* event) {
    MainWindowDragNDrop::dragMoveEvent(event);
}

void WelcomePageWidget::sl_openFiles() {
    runAction(BaseWelcomePageActions::LOAD_DATA);
}

void WelcomePageWidget::sl_createSequence() {
    runAction(BaseWelcomePageActions::CREATE_SEQUENCE);
}

void WelcomePageWidget::sl_createWorkflow() {
    runAction(BaseWelcomePageActions::CREATE_WORKFLOW);
}

void WelcomePageWidget::sl_openQuickStart() {
    runAction(BaseWelcomePageActions::QUICK_START);
}

void WelcomePageWidget::sl_openRecentFile() {
    auto label = qobject_cast<HoverQLabel*>(sender());
    SAFE_POINT(label != nullptr, "sl_openRecentFile sender is not HoverQLabel", );
    QString url = label->property(PATH_PROPERTY).toString();
    AppContext::getProjectLoader()->runOpenRecentFileOrProjectTask(url);
}

void WelcomePageWidget::sl_colorThemeSwitched() {
    bool isDark = AppContext::getMainWindow()->isDarkTheme();
    headerWidget->setStyleSheet(BACKGROUND_QLINEARGRADIENT.arg(isDark ? "#494561" : "#E0E7E9").arg(QPalette().base().color().name()));
    topLevelWidgetLabel->setStyleSheet(PADDING_LEFT.arg(isDark ? "#2BB6F2" : "#145774"));

    middleWidget->setStyleSheet(QString("background: %1;").arg(QPalette().base().color().name()));
    QString normalStyle = NORMAL_STYLE.arg(isDark ? "#2BB6F2" : "#145774");
    QString hoveredStyle = HOVERED_STYLE.arg(isDark ? "#2BB6F2" : "#145774");
    openFilesButton->updateStyles(normalStyle, hoveredStyle);
    createSequenceButton->updateStyles(normalStyle, hoveredStyle);
    createWorkflowButton->updateStyles(normalStyle, hoveredStyle);
    quickStartButton->updateStyles(normalStyle, hoveredStyle);
    middleRightWidget->setStyleSheet(MIDDLE_RIGHT_WIDGET.arg(isDark ? "#494561" : "#E0E7E9").arg(QPalette().base().color().name()));
    QString recentHeaderStyle = RECENT_HEADER_STYLE.arg(isDark ? "#2BB6F2" : "#145774");
    recentHeaderLabel->setStyleSheet(recentHeaderStyle);
    recentProjectsLabel->setStyleSheet(recentHeaderStyle);

    footerWidget->setStyleSheet(BACKGROUND_COLOR.arg(isDark ? "#3F4547" : "#B2C4C9"));
    footerStrippedLineWidget->setStyleSheet(BACKGROUND_URL.arg(GUIUtils::getThemedPath(":/ugene/images/welcome_page/line.png")));
    footerBottomWidget->setStyleSheet(COLOR_FONT_SIZE.arg(isDark ? "#2BB6F2" : "#145774"));
    QString supportUgene = tr("Support UGENE:");
    footerSupportLabel->setText(FOOTER_SUPPORT_LABEL_STRING.arg(supportUgene).arg(GUIUtils::getThemedPath(":/ugene/images/welcome_page/social_icon_patreon.png")));

    QString recentItemStyle = RECENT_ITEM_STYLE.arg(isDark ? "#2DC0FF" : "#1B769D");
    normalStyle = recentItemStyle + " text-decoration: none;";
    hoveredStyle = recentItemStyle + " text-decoration: underline;";

    auto updateRecent = [&normalStyle, &hoveredStyle, &recentItemStyle](QVBoxLayout* recentLayout) {
        for (int i = 0; i < recentLayout->count(); i++) {
            auto item = recentLayout->itemAt(i);
            auto hoverQlabel = qobject_cast<HoverQLabel*>(item->widget());
            if (hoverQlabel != nullptr) {
                hoverQlabel->updateStyles(normalStyle, hoveredStyle);
                continue;
            }
            auto qlabel = qobject_cast<QLabel*>(item->widget());
            CHECK_CONTINUE(qlabel != nullptr);

            qlabel->setStyleSheet(recentItemStyle);
        }

    };

    updateRecent(recentFilesLayout);
    updateRecent(recentProjectsLayout);
}

bool WelcomePageWidget::eventFilter(QObject* watched, QEvent* event) {
    CHECK(this == watched, false);
    switch (event->type()) {
        case QEvent::DragEnter:
            dragEnterEvent(dynamic_cast<QDragEnterEvent*>(event));
            return true;
        case QEvent::DragMove:
            dragMoveEvent(dynamic_cast<QDragMoveEvent*>(event));
            return true;
        case QEvent::Drop:
            dropEvent(dynamic_cast<QDropEvent*>(event));
            return true;
        case QEvent::FocusIn:
            setFocus();
            return true;
        default:
            break;
    }
    return false;
}

void WelcomePageWidget::runAction(const QString& actionId) {
    auto action = AppContext::getWelcomePageActionRegistry()->getById(actionId);
    if (action != nullptr) {
        GCounter::increment("Welcome Page: " + actionId);
        action->perform();
    } else if (actionId == BaseWelcomePageActions::CREATE_WORKFLOW) {
        QMessageBox::warning(AppContext::getMainWindow()->getQMainWindow(), L10N::warningTitle(), tr("The Workflow Designer plugin is not loaded. You can add it using the menu Settings -> Plugins. Then you need to restart UGENE."));
    } else if (actionId == BaseWelcomePageActions::QUICK_START) {
        QDesktopServices::openUrl(QUrl("https://ugene.net/quick-start-guide"));
    }
}

}  // namespace U2
