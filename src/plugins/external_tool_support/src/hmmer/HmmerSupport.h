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

#ifndef _U2_HMMER_SUPPORT_H_
#define _U2_HMMER_SUPPORT_H_

#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/ExternalToolRunTask.h>

#include <U2Gui/ObjectViewModel.h>

namespace U2 {

class U2SequenceObject;

class HmmerSupport : public ExternalTool {
    Q_OBJECT
public:
    HmmerSupport(const QString& id, const QString &name);

    static const QString BUILD_TOOL;
    static const QString BUILD_TOOL_ID;
    static const QString SEARCH_TOOL;
    static const QString SEARCH_TOOL_ID;
    static const QString PHMMER_TOOL;
    static const QString PHMMER_TOOL_ID;
private slots:
    void sl_buildProfile();
    void sl_search();
    void sl_phmmerSearch();

private:
    void initBuild();
    void initSearch();
    void initPhmmer();

    bool isToolSet(const QString &id) const;
};

class HmmerMsaEditorContext : public GObjectViewWindowContext {
    Q_OBJECT
public:
    HmmerMsaEditorContext(QObject *parent);

private slots:
    void sl_build();

private:
    void initViewContext(GObjectView *view);
    void buildMenu(GObjectView *view, QMenu *menu);
};

class HmmerAdvContext : public GObjectViewWindowContext {
    Q_OBJECT
public:
    HmmerAdvContext(QObject *parent);

private slots:
    void sl_search();

private:
    void initViewContext(GObjectView *view);

    QWidget * getParentWidget(QObject *sender);
    U2SequenceObject * getSequenceInFocus(QObject *sender);
};

class HmmerContext : public QObject {
public:
    HmmerContext(QObject *parent);

    void init();

private:
    HmmerMsaEditorContext *msaEditorContext;
    HmmerAdvContext *advContext;
};

class Hmmer3LogParser : public ExternalToolLogParser {
    Q_OBJECT
    Q_DISABLE_COPY(Hmmer3LogParser)
public:
    Hmmer3LogParser();
    void parseErrOutput(const QString& partOfLog);
private:
    QString lastErrLine;
};

}   // namespace U2

#endif // _U2_HMMER_SUPPORT_H_
