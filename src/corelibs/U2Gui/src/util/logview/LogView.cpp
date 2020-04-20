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

#include "LogView.h"
#include <U2Core/LogCache.h>

#include <U2Gui/MainWindow.h>
#include <U2Core/AppContext.h>
#include <U2Gui/AppSettingsGUI.h>
#include <U2Core/Timer.h>
#include <U2Core/Counter.h>

#include <QDate>
#include <QThread>
#include <QApplication>
#include <QAction>
#include <QMenu>
#include <QVBoxLayout>

namespace U2 {

#define MAX_VISIBLE_MESSAGES 1000

LogViewWidget::LogViewWidget(const LogFilter& filter) : messageCounter(0), connected(false) {
    cache = new LogCache(MAX_VISIBLE_MESSAGES);
    cache->filter = filter;
    cache->setParent(this);
    init();
}

LogViewWidget::LogViewWidget(LogCache* c) : messageCounter(0), connected(false) {
    cache = c;
    init();
}

LogViewWidget::~LogViewWidget() {
    updateViewTimer.stop();
    if (connected) {
        LogServer::getInstance()->removeListener(this);
    }
}

void LogViewWidget::init() {
    caseSensitive = true;
    useRegexp = true;

    setWindowTitle(tr("Log"));
    setWindowIcon(QIcon(":ugene/images/book_open.png"));

    connect(&updateViewTimer, SIGNAL(timeout()), this, SLOT(sl_showNewMessages()));

    settings.reinitAll();

    showSettingsAction = new QAction(tr("Settings..."), this);
    showSettingsAction->setIcon(QIcon(":ugene/images/log_settings.png"));
    connect(showSettingsAction, SIGNAL(triggered()), SLOT(sl_openSettingsDialog()));
    
    dumpCountersAction = new QAction(tr("Dump performance counters"), this);
    connect(dumpCountersAction, SIGNAL(triggered()), SLOT(sl_dumpCounters()));

    addSeparatorAction = new QAction(tr("Append separator"), this);
    connect(addSeparatorAction, SIGNAL(triggered()), SLOT(sl_addSeparator()));
    clearAction = new QAction(tr("Clear log"), this);
    connect(clearAction, SIGNAL(triggered()), SLOT(sl_clear()));

    QVBoxLayout* l = new QVBoxLayout();
    l->setSpacing(0);
    l->setMargin(0);
    l->setContentsMargins(0, 0, 0, 0);
    setLayout(l);

    edit = new QPlainTextEdit(); 
    edit->setUndoRedoEnabled(false);
    edit->setReadOnly(true);
    edit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    edit->setContextMenuPolicy(Qt::CustomContextMenu);
    edit->setTextInteractionFlags(Qt::NoTextInteraction|Qt::TextSelectableByMouse|Qt::TextSelectableByKeyboard);
    edit->setMaximumBlockCount(MAX_VISIBLE_MESSAGES);
    edit->installEventFilter(this);

    searchEdit = new QLineEdit(); 
    searchEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    shortcut = new QShortcut(QString("/"), this, 0, 0, Qt::WidgetWithChildrenShortcut);
    highlighter = new SearchHighlighter(edit->document());

    l->addWidget(edit);
    l->addWidget(searchEdit);

    QObject::connect(edit, SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(popupMenu(const QPoint &)));
    QObject::connect(searchEdit, SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(searchPopupMenu(const QPoint &)));
    QObject::connect(shortcut, SIGNAL(activated()),this,SLOT(sl_showHideEdit()));
    QObject::connect(searchEdit, SIGNAL(textEdited(const QString &)),this,SLOT(sl_onTextEdited(const QString &)));
    resetView();
}

bool LogViewWidget::eventFilter(QObject *object, QEvent *event) {
    if (edit == object && event->type() == QEvent::ShortcutOverride) {
        event->accept();
        return true;
    }
    return false;
}

void LogViewWidget::popupMenu(const QPoint& pos) {
    Q_UNUSED(pos);

    QMenu popup;
    QAction* copyAction = popup.addAction(tr("Copy"), edit, SLOT(copy()));
    copyAction->setShortcut(QKeySequence::Copy);
    copyAction->setEnabled(edit->textCursor().hasSelection());
    popup.addAction(dumpCountersAction);
    popup.addAction(addSeparatorAction);
    popup.addAction(clearAction);
    
    if (cache == LogCache::getAppGlobalInstance()) { //customization is allowed only for global cache today
        popup.addAction(showSettingsAction);
    }

    popup.exec(QCursor::pos());
}

void SearchHighlighter::highlightBlock(const QString &text) {
    if (reg_exp.pattern() == "")
        return;

    QTextCharFormat cf;
    cf.setBackground(Qt::green);

    for (int pos = 0; (pos = reg_exp.indexIn(text, pos)) != -1;) {
        int length = reg_exp.matchedLength();
        if (length == 0) {
            ++pos;
            continue;
        }
        setFormat(pos, length, cf);
        pos += length;
    }
}

void LogViewWidget::sl_showHideEdit() {
    if (searchEdit->isVisible())
        searchEdit->hide();
    else {
        searchEdit->show();
        searchEdit->setFocus();
    }
}

void LogViewWidget::sl_onTextEdited(const QString & text) {
    QRegExp re(text);
    if (highlighter->reg_exp.patternSyntax() == QRegExp::RegExp && !re.isValid())
        return;

    highlighter->reg_exp.setPattern(text);
    resetView();
}

bool LogViewWidget::isShown(const QString & txt) {
    if (highlighter->reg_exp.indexIn(txt, 0) < 0) {
        return false;
    }
    return true;
}

void LogViewWidget::sl_openSettingsDialog() {
    AppContext::getAppSettingsGUI()->showSettingsDialog(APP_SETTINGS_GUI_LOG);
}

void LogViewWidget::searchPopupMenu(const QPoint &pos) {
    Q_UNUSED(pos);

    QMenu popup;
    QAction *action = popup.addAction(tr("logview_set_case"), this, SLOT(setSearchCaseSensitive()));
    action->setCheckable(true);
    action->setChecked(caseSensitive);
    action = popup.addAction(tr("logview_use_regexp"), this, SLOT(useRegExp()));
    action->setCheckable(true);
    action->setChecked(useRegexp);
    popup.exec(QCursor::pos());
}

void LogViewWidget::setSearchCaseSensitive() {
    caseSensitive = !caseSensitive;
    if (caseSensitive) {
        highlighter->reg_exp.setCaseSensitivity(Qt::CaseSensitive);
    } else {
        highlighter->reg_exp.setCaseSensitivity(Qt::CaseInsensitive);
    }
    resetView();
}

void LogViewWidget::useRegExp() {
    useRegexp = !useRegexp;
    if (useRegexp) {
        highlighter->reg_exp.setPatternSyntax(QRegExp::RegExp);
    } else {
        highlighter->reg_exp.setPattern(searchEdit->text());
        highlighter->reg_exp.setPatternSyntax(QRegExp::FixedString);
    }
    resetView();
}

void LogViewWidget::resetView() {
    edit->clear();
    messageCounter = MAX_VISIBLE_MESSAGES;
}

void LogViewWidget::showEvent(QShowEvent *e) {
    Q_UNUSED(e);
    if (!connected) {
        updateViewTimer.start(100);
        LogServer::getInstance()->addListener(this);
        connected = !connected;
    }

    resetView();
}

void LogViewWidget::hideEvent(QHideEvent *e) {
    Q_UNUSED(e);
    if (connected) {
        updateViewTimer.stop();
        LogServer::getInstance()->removeListener(this);
        connected = !connected;
    }

    edit->clear();
}

void LogViewWidget::sl_showNewMessages() {
    QList<LogMessage> newMessagesToShow = cache->getLastMessages(messageCounter);
    messageCounter = 0;

    int count = 0;
    foreach(const LogMessage& m, newMessagesToShow) {
        addMessage(m);
        if (count++ > MAX_VISIBLE_MESSAGES) {
            break;
        }
    }
}

void LogViewWidget::onMessage(const LogMessage& /*msg*/) {
    messageCounter++;
}

bool LogViewWidget::isShown(const LogMessage& msg)  {
    QString category = getEffectiveCategory(msg);
    return !category.isEmpty();
}

QString LogViewWidget::getEffectiveCategory(const LogMessage& msg) const {
    QString result;
    if (cache->filter.isEmpty()) {
        if (!settings.activeLevelGlobalFlag[msg.level]) {
            return QString();
        }

        foreach (const QString& category, msg.categories) {
            const LoggerSettings& cs = settings.getLoggerSettings(category);
            if (cs.activeLevelFlag[msg.level]) {
                result = category;
                break;
            }
        }
    } else {
        result = cache->filter.selectEffectiveCategory(msg);
    }

    return result;
}

void LogViewWidget::setSettings(const LogSettings& s) {
    if (settings == s) {
        return;
    }
    LogSettingsHolder::setSettings(s);

    resetView();
}


QString LogViewWidget::prepareText(const LogMessage& msg) const {
    QString color = settings.enableColor? settings.levelColors[msg.level] : QString();

    QString prefix = "[" + settings.logPattern + "]";
    QString category = settings.showCategory ? "[" + getEffectiveCategory(msg) + "]" : QString();
    QString level = settings.showLevel ? "[" + LogCategories::getLocalizedLevelName(msg.level) + "]" : QString();
    QStringList date =  GTimer::createDateTime(msg.time).toString("yyyy:yy:MM:dd:hh:mm:ss:zz").split(":");
    prefix.replace("YYYY", date[0]);
    prefix.replace("YY", date[1]);
    prefix.replace("MM", date[2]);
    prefix.replace("dd", date[3]);
    prefix.replace("hh", date[4]);
    prefix.replace("mm", date[5]);
    prefix.replace("ss", date[6]);
    prefix.replace("zzz", date[7]);
    prefix.prepend(level);
    prefix.prepend(category);
    QString spacing = prefix.isEmpty() ? QString() : QString(" ");
    QString text = "<font color="+color+">" + prefix + spacing + msg.text +"</font><br/>";

    return text;
}


void LogViewWidget::addMessage(const LogMessage& msg) {
    if (!isShown(msg)) {
        return;
    }
    addText(prepareText(msg));
}

void LogViewWidget::addText(const QString& txt) {
    if (!isShown(txt)) {
        return;
    }
    edit->appendHtml(txt);
}

void LogViewWidget::sl_dumpCounters() {
    QString text = "Counters report start ***********************\n";
    addText(text);
    foreach(GCounter* c, GCounter::allCounters()) {
        double val = c->scaledTotal();
        text = c->name + " " + QString::number(val) + " " + c->suffix;
        addText(text);
    }
    text = "Counters report end ***********************\n";
    addText(text);
}

void LogViewWidget::sl_addSeparator() {
    QString text = "\n==================================================\n";
    addText(text);
}

void LogViewWidget::sl_clear() {
    cache->messages.clear();
    edit->clear(); 
}

void LogViewWidget::setSearchBoxMode(LogViewSearchBoxMode mode) {
    if (mode == LogViewSearchBox_Visible) {
        searchEdit->setVisible(true);
    } else {
        searchEdit->setVisible(false);
    }
}
}//namespace

