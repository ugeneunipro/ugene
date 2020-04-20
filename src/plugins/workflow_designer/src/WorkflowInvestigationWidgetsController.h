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

#ifndef _U2_WORKFLOW_INVESTIGATION_WIDGETS_CONTROLLER_H_
#define _U2_WORKFLOW_INVESTIGATION_WIDGETS_CONTROLLER_H_

#include <QObject>
#include <QMap>

#include <U2Lang/ActorModel.h>
#include <U2Lang/WorkflowInvestigationData.h>

class QWidget;
class QTableView;

namespace U2 {

typedef QTableView WorkflowInvestigationWidget;

class InvestigationDataModel;

class WorkflowInvestigationWidgetsController : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(WorkflowInvestigationWidgetsController)
public:
    explicit WorkflowInvestigationWidgetsController(QWidget *parent = NULL);
    ~WorkflowInvestigationWidgetsController();

    void deleteBusInvestigations();
    void resetInvestigations();
    void setInvestigationWidgetsVisible(bool visible);
    void setCurrentInvestigation(const Workflow::Link *bus);
    bool eventFilter(QObject *watched, QEvent *event);

public slots:
    void sl_currentInvestigationUpdateResponse(const WorkflowInvestigationData &investigationInfo,
        const Workflow::Link *bus);
    void sl_countOfMessagesResponse(const Workflow::Link *bus, int countOfMessages);

signals:
    void si_updateCurrentInvestigation(const Workflow::Link *bus);
    void si_updateCurrentInvestigation(const Workflow::Link *bus, int messageNumber);
    void si_countOfMessagesRequested(const Workflow::Link *bus);
    void si_convertionMessages2DocumentsIsRequested(const Workflow::Link *bus,
        const QString &messageType, int messageNumber);

private slots:
    void sl_contextMenuRequested(const QPoint &cursorPosition);
    void sl_hotizontalHeaderContextMenuRequested(const QPoint &cursorPosition);
    void sl_exportInvestigation();
    void sl_copyToClipboard() const;
    void sl_hideSelectedColumn();
    void sl_hideAllColumnsButSelected();
    void sl_showAllColumns();
    void sl_columnsVisibilityResponse();

private:
    void createNewInvestigation();
    void createInvestigationModel();
    void adjustInvestigationColumnWidth(WorkflowInvestigationWidget *investigator);

    WorkflowInvestigationWidget *investigationView;
    InvestigationDataModel *investigationModel;
    const Workflow::Link *investigatedLink;
    QString investigatorName;
    // the variable signals if investigation widget was visible when
    // debugging finished. If true, then it will become visible when
    // debugging starts next time
    bool wasDisplayed;
    QAction *exportInvestigationAction;
    QAction *copyToClipboardAction;

    QAction *hideThisColumnAction;
    QAction *hideAllColumnsButThisAction;
    QAction *showAllColumnsAction;
    int selectedColumn;
    QMap<const Workflow::Link *, QVector<int> > columnWidths;
};

} // namespace U2

#endif // _U2_WORKFLOW_INVESTIGATION_WIDGETS_CONTROLLER_H_
