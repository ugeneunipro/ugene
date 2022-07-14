/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "./ParametersDashboardWidget.h"

#include <QApplication>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>

#include <U2Core/AppContext.h>

#include <U2Gui/HoverQLabel.h>

#include <U2Lang/Dataset.h>
#include <U2Lang/URLContainer.h>
#include <U2Lang/WorkflowUtils.h>

#include "./DomUtils.h"

namespace U2 {

#define URL_MARKER_CLASS "url-marker"
#define DIR_MARKER_CLASS "dir-marker"
#define DATASET_MARKER_CLASS "dataset-marker"

static QList<WorkerParametersInfo> dom2WorkerParametersInfo(const QDomElement& dom) {
    QList<WorkerParametersInfo> workers;
    QDomElement navTabsEl = DomUtils::findChildElementByClass(dom, "params-nav-tabs", 2);
    QDomNodeList workerNameList = navTabsEl.elementsByTagName("li");
    for (int i = 0; i < workerNameList.size(); i++) {
        QDomElement workerNameLi = workerNameList.at(i).toElement();
        QList<WorkerParameterInfo> parameters;
        workers << WorkerParametersInfo(workerNameLi.text().simplified(), parameters);
    }
    QDomElement paramTablesRoot = DomUtils::findChildElementByClass(dom, "params-tab-content", 2);
    QList<QDomElement> paramTableList = DomUtils::findChildElementsByClass(paramTablesRoot, "param-value-column");
    for (int i = 0; i < paramTableList.size() && i < workers.size(); i++) {
        QDomElement table = paramTableList[i];
        QDomElement tr = table.firstChildElement("tr");
        if (tr.isNull()) {
            tr = table.firstChildElement("tbody").firstChildElement("tr");
            if (tr.isNull()) {
                tr = table.firstChildElement("thead").firstChildElement("tr");  // UGENE <= v35 has all <tr>s within <thead>.
            }
        }
        int rowSpan = 0;
        for (; !tr.isNull(); tr = tr.nextSiblingElement("tr"), rowSpan--) {
            QDomElement td1 = tr.firstChildElement("td");
            if (td1.isNull()) {
                continue;
            }
            QString name = rowSpan > 0 ? "" : td1.text().simplified();
            QDomElement td2 = rowSpan > 0 ? td1 : td1.nextSiblingElement("td");
            QString value = td2.text().trimmed();
            bool isUrl = DomUtils::hasClass(td2, URL_MARKER_CLASS);
            bool isDir = DomUtils::hasClass(td2, DIR_MARKER_CLASS);
            bool isDataset = DomUtils::hasClass(td2, DATASET_MARKER_CLASS);
            if (!isUrl) {  // Check if it is old-style URL (UGENE <= v35).
                QDomElement button = td2.firstChildElement("div").firstChildElement("div").firstChildElement("button");
                if (!button.isNull()) {
                    isDir = button.hasAttribute("disabled");
                    QString valueFromOnClick = DashboardWidgetUtils::parseOpenUrlValueFromOnClick(button.attribute("onclick"));
                    if (valueFromOnClick.length() > 0) {
                        value = valueFromOnClick;
                        isUrl = true;
                    }
                }
            }
            if (rowSpan > 0) {
                workers[i].parameters.last().isDataset = true;
                workers[i].parameters.last().value += "\n" + value;
            } else {
                workers[i].parameters << WorkerParameterInfo(name, value, isUrl, isDir, isDataset);
            }
            if (td1.hasAttribute("rowspan")) {
                rowSpan = td1.attribute("rowspan").toInt();
            }
        }
    }
    return workers;
}

static QList<WorkerParametersInfo> params2WorkerInfos(const QList<Workflow::Monitor::WorkerParamsInfo>& workerInfoList) {
    QList<WorkerParametersInfo> result;
    for (auto workerInfo : qAsConst(workerInfoList)) {
        QList<WorkerParameterInfo> parameters;
        for (auto p : qAsConst(workerInfo.parameters)) {
            QString name = p->getDisplayName();
            QString value;
            bool isUrl = false;
            bool isDir = false;
            bool isDataset = false;
            QVariant valueVariant = p->getAttributePureValue();
            if (valueVariant.canConvert<QList<Dataset>>()) {
                QList<Dataset> sets = valueVariant.value<QList<Dataset>>();
                for (const Dataset& set : qAsConst(sets)) {
                    if (sets.size() > 1) {
                        name += ": " + set.getName();
                    }
                    QStringList urls;
                    const QList<URLContainer*> urlContainers = set.getUrls();
                    for (const auto& urlContainer : qAsConst(urlContainers)) {
                        urls << urlContainer->getUrl();
                    }
                    value = urls.join("\n");
                    isDataset = true;
                }
            } else {
                value = WorkflowUtils::getStringForParameterDisplayRole(valueVariant);
                UrlAttributeType type = WorkflowUtils::isUrlAttribute(p, workerInfo.actor);
                if (type != NotAnUrl && QString::compare(valueVariant.toString(), "default", Qt::CaseInsensitive) != 0) {
                    isUrl = true;
                    isDir = type == InputDir || type == OutputDir;
                }
            }

            parameters << WorkerParameterInfo(name, value, isUrl, isDir, isDataset);
        }
        result << WorkerParametersInfo(workerInfo.workerName, parameters);
    }
    return result;
}

ParametersDashboardWidget::ParametersDashboardWidget(const QString& _dashboardDir, const QDomElement& dom, const WorkflowMonitor* _monitor)
    : dashboardDir(_dashboardDir), monitor(_monitor) {
    setObjectName("ParametersDashboardWidget");
    setMinimumWidth(1100);

    layout = new QHBoxLayout();
    setLayout(layout);

    auto nameListWidget = new QWidget();
    nameListWidget->setFixedWidth(300);
    auto nameListWidgetLayout = new QVBoxLayout();
    nameListWidget->setLayout(nameListWidgetLayout);
    nameListWidgetLayout->setSpacing(0);
    layout->addWidget(nameListWidget, 1);

    auto rightWidget = new QWidget();
    auto rightWidgetLayout = new QVBoxLayout();
    rightWidget->setLayout(rightWidgetLayout);
    layout->addWidget(rightWidget);

    parametersGridLayout = new QGridLayout();
    parametersGridLayout->setSpacing(0);
    rightWidgetLayout->addLayout(parametersGridLayout);
    rightWidgetLayout->addStretch(1000);

    workers << dom2WorkerParametersInfo(dom);
    if (monitor != nullptr) {
        workers << params2WorkerInfos(monitor->getWorkersParameters());
    }
    for (int i = 0; i < workers.size(); i++) {
        auto workerNameLabel = new HoverQLabel(workers[i].workerName, "", "");
        workerNameLabel->setWordWrap(true);
        workerNameLabel->setObjectName("worker-" + QString::number(i));
        workerNameLabel->setProperty("worker-index", i);
        nameListWidgetLayout->addWidget(workerNameLabel);
        connect(workerNameLabel, SIGNAL(clicked()), SLOT(sl_workerLabelClicked()));
        workerNameLabels << workerNameLabel;
    }
    nameListWidgetLayout->addStretch();
    showWorkerParameters(0);

    if (monitor != nullptr) {
        connect(monitor, &WorkflowMonitor::si_dirSet, this, [&](const QString& newDashboardDir) { dashboardDir = newDashboardDir; });
    }
}

void ParametersDashboardWidget::sl_workerLabelClicked() {
    auto label = qobject_cast<HoverQLabel*>(sender());
    CHECK(label != nullptr, );
    bool ok = false;
    int index = label->property("worker-index").toInt(&ok);
    CHECK(ok && index >= 0 && index <= workers.size(), );
    showWorkerParameters(index);
}

void ParametersDashboardWidget::showWorkerParameters(int workerIndex) {
    CHECK(workerIndex >= 0 && workerIndex <= workers.size(), );
    QLayoutItem* item;
    while ((item = parametersGridLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    if (workerIndex >= workers.size()) {
        return;
    }
    QString commonWorkerNameLabelStyle("padding: 0.7em;");
    QString activeWorkerNameLabelStyle = commonWorkerNameLabelStyle + "border-top: 1px solid #ddd; border-bottom: 1px solid #ddd; border-left: 1px solid #ddd; border-top-left-radius: 6px; border-bottom-left-radius: 6px;";
    QString notActiveWorkerNameLabelStyle = commonWorkerNameLabelStyle + "border-right: 1px solid #ddd;";
    for (int i = 0; i < workerNameLabels.size(); i++) {
        if (i == workerIndex) {
            workerNameLabels[i]->updateStyles(activeWorkerNameLabelStyle, activeWorkerNameLabelStyle);
        } else {
            workerNameLabels[i]->updateStyles(notActiveWorkerNameLabelStyle + "background: white;", notActiveWorkerNameLabelStyle + "background: #eee;");
        }
    }

    // Parameters table.
    addTableHeadersRow(parametersGridLayout, QStringList() << tr("Parameter") << tr("Value"));
    QList<WorkerParameterInfo>& parameters = workers[workerIndex].parameters;
    for (int parameterIndex = 0; parameterIndex < parameters.size(); parameterIndex++) {
        auto parameter = parameters[parameterIndex];
        bool isLastRow = parameterIndex == parameters.size() - 1;
        int rowIndex = parameterIndex + 1;
        addTableCell(parametersGridLayout, parameter.name, parameter.name, rowIndex, 0, isLastRow, false);

        bool renderValueAsFileButton = !parameter.value.isEmpty() && (parameter.isUrl || parameter.isDir || parameter.isDataset);
        if (!renderValueAsFileButton) {
            addTableCell(parametersGridLayout, parameter.name, parameter.value, rowIndex, 1, isLastRow, true);
            continue;
        }

        auto valueWidget = new QWidget();
        valueWidget->setObjectName("valueWidget");
        auto valueWidgetLayout = new QVBoxLayout();
        valueWidgetLayout->setContentsMargins(0, 0, 0, 0);
        valueWidget->setLayout(valueWidgetLayout);

        QStringList urlList = parameter.value.split("\n");
        for (auto url : qAsConst(urlList)) {
            QFileInfo fileInfo(url);
            if (!fileInfo.isAbsolute()) {
                fileInfo = QFileInfo(QDir(dashboardDir), url).absoluteFilePath();
            }
            auto button = new DashboardFileButton(QStringList() << fileInfo.absoluteFilePath(), dashboardDir, monitor, parameter.isDir);
            valueWidgetLayout->addWidget(button);
        }
        valueWidgetLayout->addStretch();
        addTableCell(parametersGridLayout, parameter.name, valueWidget, rowIndex, 1, isLastRow, true);
    }
}

bool ParametersDashboardWidget::isValidDom(const QDomElement& dom) {
    return !DomUtils::findChildElementByClass(dom, "params-nav-tabs", 2).isNull() &&
           !DomUtils::findChildElementByClass(dom, "params-tab-content", 2).isNull();
}

static QString getParameterTypeClass(const WorkerParameterInfo& parameter) {
    QString result = QString() + ((parameter.isUrl) ? URL_MARKER_CLASS : "") + " " + ((parameter.isDir) ? DIR_MARKER_CLASS : "") + " " + ((parameter.isDataset) ? DATASET_MARKER_CLASS : "");
    return result.isEmpty() ? QString() : " class=\"" + result.trimmed() + "\"";
}

QString ParametersDashboardWidget::toHtml() const {
    QString urlMarkerPart = QString(" class=\"%1\"").arg(URL_MARKER_CLASS);
    QString tabIdPrefix = "params_tab_id_";

    QString html = "<div class=\"tabbable tabs-left\">\n";

    // List with worker names.
    html += "<ul class=\"nav nav-tabs params-nav-tabs\">\n";
    for (int i = 0; i < workers.size(); i++) {
        auto worker = workers[i];
        html += "<li" + QString(i == 0 ? " class=\"active\"" : "") + "><a href=\"#" + tabIdPrefix + QString::number(i) + "\" data-toggle=\"tab\">";
        html += worker.workerName.toHtmlEscaped();
        html += "</a></li>\n";
    }
    html += "</ul>\n";

    // List with parameter tables.
    html += "<div class=\"tab-content params-tab-content\">\n";
    for (int i = 0; i < workers.size(); i++) {
        auto worker = workers[i];
        html += "<div class=\"" + QString(i == 0 ? "tab-pane active" : "tab-pane") + "\" id=\"" + tabIdPrefix + QString::number(i) + "\">\n";
        html += "<table class=\"table table-bordered table-fixed param-value-column\">\n";
        html += "<thead><tr><th>" + tr("Parameter").toHtmlEscaped() + "</th><th>" + tr("Value").toHtmlEscaped() + "</th></tr></thead>\n";
        html += "<tbody>\n";
        for (auto parameter : qAsConst(worker.parameters)) {
            html += "<tr>";
            html += "<td>" + parameter.name.toHtmlEscaped() + "</td>";
            html += "<td" + getParameterTypeClass(parameter) + ">" + parameter.value.toHtmlEscaped() + "</td>";
            html += "</tr>\n";
        }
        html += "</tbody>\n";
        html += "</table>\n";
        html += "</div>\n";
    }
    html += "</div>\n";  // param tables.

    html += "</div>\n";
    return html;
}

WorkerParametersInfo::WorkerParametersInfo(const QString& workerName, const QList<WorkerParameterInfo>& parameters)
    : workerName(workerName), parameters(parameters) {
}

WorkerParameterInfo::WorkerParameterInfo(const QString& name, const QString& value, bool isUrl, bool isDir, bool isDataset)
    : name(name), value(value), isUrl(isUrl), isDir(isDir), isDataset(isDataset) {
}

}  // namespace U2
