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

#ifndef _U2_LOGVIEW_IMPL_
#define _U2_LOGVIEW_IMPL_

#include <LogSettings.h>

#include <QHash>
#include <QRegExp>
#include <QTimer>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QShortcut>
#include <QSyntaxHighlighter>

namespace U2 {

class LogCache;
class LogMessage;
class LogFilter;

class SearchHighlighter: public QSyntaxHighlighter {
public:
    SearchHighlighter(QTextDocument *doc): QSyntaxHighlighter(doc) {}

    QRegExp reg_exp;
    void highlightBlock(const QString &text);
};


enum LogViewSearchBoxMode {
    LogViewSearchBox_Visible,
    LogViewSearchBox_Auto, //TODO: not supported today
    LogViewSearchBox_Hidden
};

class U2GUI_EXPORT LogViewWidget : public QWidget, public LogListener, public LogSettingsHolder {
    Q_OBJECT
public:
    /** If categoriesFilter is not-empty LogViewWidget shows log messages 
        only from categories listed in categoriesFilter
        LogSettings are ignored in this case
     */
    LogViewWidget(LogCache* c);
    LogViewWidget(const LogFilter& filter);
    ~LogViewWidget();

    bool isShown(const LogMessage& msg);
    bool isShown(const QString& txt);
    /** returns first category in the msg.categories that match 'show-filter' criteria*/
    QString getEffectiveCategory(const LogMessage& msg) const;

    virtual void setSettings(const LogSettings& s); 

    void setSearchBoxMode(LogViewSearchBoxMode mode);

    virtual void onMessage(const LogMessage& msg);

protected:
    void addMessage(const LogMessage& msg);
    void addText(const QString& text);
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);
    void resetView();

private slots:
    void sl_showNewMessages();
    void sl_onTextEdited(const QString& text);
    void popupMenu(const QPoint &pos);
    void sl_openSettingsDialog();
    void sl_dumpCounters();
    void sl_clear();
    void sl_addSeparator();
    void sl_showHideEdit();
    void searchPopupMenu(const QPoint &pos);
    void setSearchCaseSensitive();
    void useRegExp();

private:
    QString prepareText(const LogMessage& msg) const;
    void init();
    bool eventFilter(QObject *object, QEvent *event);

    QTimer                   updateViewTimer;
    QPlainTextEdit*          edit;
    QLineEdit*               searchEdit;
    QShortcut*               shortcut;
    SearchHighlighter*       highlighter;
    bool                     caseSensitive, useRegexp;
    
	int                 messageCounter;
    LogCache*           cache;
    QAction*            showViewAction;
    QAction*            showSettingsAction;
    QAction*            dumpCountersAction;
    QAction*            clearAction;
    QAction*            addSeparatorAction;
    bool                connected;
};

} //namespace

#endif
