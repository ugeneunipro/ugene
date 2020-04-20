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

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QFile>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLinearGradient>
#include <QTextBrowser>
#include <QMessageBox>
#include <QDesktopServices>

#include <U2Core/AppContext.h>
#include <U2Core/IdRegistry.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/Settings.h>
#include <U2Core/Counter.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/Task.h>

#include <U2Gui/MainWindow.h>
#include <U2Gui/WelcomePageAction.h>

#include "WelcomePageWidget.h"
#include "main_window/MainWindowImpl.h"

namespace U2 {

static QString newImageAndTextHtml(const QString& image, const QString& text) {
    return QString("<center>") +
           "<img src=':/ugene/images/welcome_page/" + image + "'>" +
           "<br>" + text +
           "</center>";
}

WelcomePageWidget::WelcomePageWidget(QWidget* parent)
        : QWidget(parent) {

    auto layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);

    auto headerWidget = createHeaderWidget();
    auto middleWidget = createMiddleWidget();
    auto footerWidget = createFooterWidget();

    layout->addWidget(headerWidget);
    layout->addWidget(middleWidget);
    layout->addWidget(footerWidget);

    installEventFilter(this);
}

QWidget* WelcomePageWidget::createHeaderWidget() {
    auto headerWidget = new QWidget();
    headerWidget->setContentsMargins(0, 0, 0, 0);
    headerWidget->setStyleSheet("background: qlineargradient(x1:0 y1:0, x2:1 y2:0, stop:0 #E0E7E9, stop:1 white);");
    headerWidget->setFixedHeight(110);
    auto headerWidgetLayout = new QHBoxLayout();
    headerWidgetLayout->setMargin(0);
    headerWidget->setLayout(headerWidgetLayout);
    auto topLevelWidgetLabel = new QLabel(tr("Welcome to UGENE"));
    topLevelWidgetLabel->setStyleSheet("padding-left: 25px; color: #145774; font-size: 34px;");
    headerWidgetLayout->addWidget(topLevelWidgetLabel);
    return headerWidget;
}

QWidget* WelcomePageWidget::createMiddleWidget() {
    auto middleWidget = new QWidget();
    middleWidget->setStyleSheet("background: white;");
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
    QString normalStyle = "QLabel {text-decoration: none; color: #145774; font-size: 18px;}";
    QString hoveredStyle = "QLabel {text-decoration: underline; color: #145774; font-size: 18px;}";

    auto openFilesButton = new HoverQLabel(newImageAndTextHtml("welcome_btn_open.png", openFilesText), normalStyle, hoveredStyle);
    openFilesButton->setObjectName("openFilesButton");
    connect(openFilesButton, SIGNAL(clicked()), SLOT(sl_openFiles()));
    buttonsGridLayout->addWidget(openFilesButton, 0, 0);

    auto createSequenceButton = new HoverQLabel(newImageAndTextHtml("welcome_btn_create_seq.png", createSequenceText), normalStyle, hoveredStyle);
    createSequenceButton->setObjectName("createSequenceButton");
    connect(createSequenceButton, SIGNAL(clicked()), SLOT(sl_createSequence()));
    buttonsGridLayout->addWidget(createSequenceButton, 0, 1);

    auto createWorkflowButton = new HoverQLabel(newImageAndTextHtml("welcome_btn_workflow.png", createWorkflowText), normalStyle, hoveredStyle);
    createWorkflowButton->setObjectName("createWorkflowButton");
    connect(createWorkflowButton, SIGNAL(clicked()), SLOT(sl_createWorkflow()));
    buttonsGridLayout->addWidget(createWorkflowButton, 1, 0);

    auto quickStartButton = new HoverQLabel(newImageAndTextHtml("welcome_btn_help.png", quickStartText), normalStyle, hoveredStyle);
    connect(quickStartButton, SIGNAL(clicked()), SLOT(sl_openQuickStart()));
    buttonsGridLayout->addWidget(quickStartButton, 1, 1);

    middleLeftWidgetLayout->addStretch();
    middleLeftWidgetLayout->addLayout(buttonsGridLayout);
    middleLeftWidgetLayout->addStretch();

    middleWidgetLayout->addWidget(middleLeftWidget, 3);

    auto middleRightWidget = new QWidget();
    middleRightWidget->setObjectName("middleRightWidget");
    middleRightWidget->setStyleSheet("#middleRightWidget {background: qlineargradient(x1:0 y1:0, x2:1 y2:0, stop:0 #E0E7E9, stop:1 white); border-radius: 25px;}");
    middleRightWidget->setContentsMargins(8, 8, 8, 8);
    middleWidgetLayout->addWidget(middleRightWidget, 2);

    auto middleRightWidgetLayout = new QVBoxLayout();
    middleRightWidget->setLayout(middleRightWidgetLayout);
    QString recentHeaderStyle = "color: #145774; font-size: 20px; background: transparent;";
    auto recentHeaderLabel = new QLabel(tr("Recent files"));
    recentHeaderLabel->setStyleSheet(recentHeaderStyle);
    middleRightWidgetLayout->addWidget(recentHeaderLabel);
    middleRightWidgetLayout->setSpacing(0);
    recentFilesLayout = new QVBoxLayout();
    middleRightWidgetLayout->addLayout(recentFilesLayout);
    auto recentProjectsLabel = new QLabel(tr("Recent projects"));
    recentProjectsLabel->setStyleSheet(recentHeaderStyle);
    middleRightWidgetLayout->addSpacing(15);
    middleRightWidgetLayout->addWidget(recentProjectsLabel);
    recentProjectsLayout = new QVBoxLayout();
    middleRightWidgetLayout->addLayout(recentProjectsLayout);
    middleRightWidgetLayout->addStretch();

    return middleWidget;
}

QWidget* WelcomePageWidget::createFooterWidget() {
    auto footerWidget = new QWidget();
    footerWidget->setStyleSheet("background-color: #B2C4C9;");
    footerWidget->setFixedHeight(150);
    auto footerWidgetLayout = new QVBoxLayout();
    footerWidgetLayout->setMargin(0);
    footerWidget->setLayout(footerWidgetLayout);

    auto footerStrippedLineWidget = new QWidget();
    footerStrippedLineWidget->setFixedHeight(31);
    footerStrippedLineWidget->setStyleSheet("background: url(':/ugene/images/welcome_page/line.png')");
    footerWidgetLayout->addWidget(footerStrippedLineWidget);

    auto footerBottomWidget = new QWidget();
    footerBottomWidget->setFixedHeight(footerWidget->height() - footerStrippedLineWidget->height());
    footerWidgetLayout->addWidget(footerBottomWidget);
    footerBottomWidget->setStyleSheet("color: #145774; font-size: 16px;");
    auto footerBottomWidgetLayout = new QHBoxLayout();
    footerBottomWidget->setLayout(footerBottomWidgetLayout);
    footerBottomWidgetLayout->setContentsMargins(25, 0, 0, 0);


    auto footerCiteBlock = new QTextBrowser();
    footerCiteBlock->setFrameShape(QFrame::NoFrame);
    footerCiteBlock->setHtml("<b>" + tr("Cite UGENE:") + "</b>"
                                                         "<table><tr><td width=40></td><td>"
                                                         "\"Unipro UGENE: a unified bioinformatics toolkit\"<br>"
                                                         "Okonechnikov; Golosova; Fursov; the UGENE team<br>"
                                                         "Bioinformatics 2012 28: 1166-1167"
                                                         "</td></tr></table>"
    );
    footerBottomWidgetLayout->addWidget(footerCiteBlock);
    footerBottomWidgetLayout->addStretch();

    auto footerFollowBlock = new QTextBrowser();
    footerFollowBlock->setFrameShape(QFrame::NoFrame);
    footerFollowBlock->setOpenExternalLinks(true);
    footerFollowBlock->setHtml(
            "<b>" + tr("Follow UGENE:") + "</b>"
                                          "<table cellspacing=7><tr>"
                                          "<td width=33></td>"
                                          "<td><a href='https://www.facebook.com/groups/ugene'><img src=':/ugene/images/welcome_page/social_icon_facebook.png'></a></td>"
                                          "<td><a href='https://twitter.com/uniprougene'><img src=':/ugene/images/welcome_page/social_icon_twitter.png'></a></td>"
                                          "<td><a href='https://www.linkedin.com/profile/view?id=200543736'><img src=':/ugene/images/welcome_page/social_icon_linkedin.png'></a></td>"
                                          "<td><a href='http://www.youtube.com/user/UniproUGENE'><img src=':/ugene/images/welcome_page/social_icon_youtube.png'></a></td>"
                                          "<td><a href='http://vk.com/uniprougene'><img src=':/ugene/images/welcome_page/social_icon_vkontakte.png'></a></td>"
                                          "<td><a href='http://feeds2.feedburner.com/NewsOfUgeneProject'><img src=':/ugene/images/welcome_page/social_icon_rss.png'></a></td>"
                                          "</tr></table>"
    );
    footerBottomWidgetLayout->addWidget(footerFollowBlock);
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
    QString recentItemStyle = "color: #1B769D; font-size: 18px; padding-top: 0; padding-bottom: 0; padding-left: 5px; background: transparent;";
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
    HoverQLabel* label = qobject_cast<HoverQLabel*>(sender());
    QString path = label == nullptr ? QString() : label->property(PATH_PROPERTY).toString();
    if (!path.isEmpty()) {
        Task* openWithProjectTask = AppContext::getProjectLoader()->openWithProjectTask(QList<GUrl>() << path);
        if (openWithProjectTask != nullptr) { // The task may be null if another open project task is in progress.
            AppContext::getTaskScheduler()->registerTopLevelTask(openWithProjectTask);
        }
    }
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
        GRUNTIME_NAMED_COUNTER(cvar, tvar, "Welcome Page: " + actionId, "");
        action->perform();
    } else if (actionId == BaseWelcomePageActions::CREATE_WORKFLOW) {
        QMessageBox::warning(AppContext::getMainWindow()->getQMainWindow(), L10N::warningTitle(),
                             tr("The Workflow Designer plugin is not loaded. You can add it using the menu Settings -> Plugins. Then you need to restart UGENE."));
    } else if (actionId == BaseWelcomePageActions::QUICK_START) {
        QDesktopServices::openUrl(QUrl("https://ugene.net/wiki/display/QSG/Quick+Start+Guide"));
    }
}

HoverQLabel::HoverQLabel(const QString& html, const QString& _normalStyle, const QString& _hoveredStyle, const QString& objectName)
        : QLabel(html), normalStyle(_normalStyle), hoveredStyle(_hoveredStyle) {
    setCursor(Qt::PointingHandCursor);
    setObjectName(objectName);
    if (!objectName.isEmpty()) {
        normalStyle = "#" + objectName + " {" + normalStyle + "}";
        hoveredStyle = "#" + objectName + " {" + hoveredStyle + "}";
    }
    setStyleSheet(normalStyle);
}

void HoverQLabel::enterEvent(QEvent* event) {
    Q_UNUSED(event);
    setStyleSheet(hoveredStyle);
}

void HoverQLabel::leaveEvent(QEvent* event) {
    Q_UNUSED(event);
    setStyleSheet(normalStyle);
}

void HoverQLabel::mousePressEvent(QMouseEvent* event) {
    Q_UNUSED(event);
    emit clicked();
}

} // U2
