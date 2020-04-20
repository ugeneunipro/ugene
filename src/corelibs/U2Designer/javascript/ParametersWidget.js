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

function ParametersWidget(containerId) {
    DashboardWidget.apply(this, arguments); //inheritance

    //private
    var self = this;

    //public
    this.createWidget = function(workersParamsInfo) {
        var i = 0;
        workersParamsInfo.forEach(function(info) {
            //create tab
            var tabId = "params_tab_id_" + i;
            if (i === 0) {
                pwInitAndActiveTab(self._container, info.workerName, tabId);
            } else {
                pwAddTab(self._container, info.workerName, tabId);
            }
            //create parameters table
            info.parameters.forEach(function(parameter) {
                if (parameter.isDataset) {
                    pwAddFilesParameter(tabId, parameter.name, parameter.value, false);
                } else {
                    if (parameter.isUrl) {
                        pwAddFilesParameter(tabId, parameter.name, parameter.value, (parameter.type === "Dir"));
                    } else {
                        pwAddCommonParameter(tabId, parameter.name, parameter.value);
                    }
                }
            });
            i++;
        });
    };

    /** Creates the ParametersWidget layout and the first active tab (without parameters). */
    function pwInitAndActiveTab(container, activeTabName, activeTabId) {
        var mainHtml =
                '<div class="tabbable tabs-left">' +
                '<ul class="nav nav-tabs params-nav-tabs">' +
                '<li class="active"><a href="#' + activeTabId + '" data-toggle="tab">' + activeTabName + '</a></li>' +
                '</ul>' +
                '<div class="tab-content params-tab-content">' +
                '<div class="tab-pane active" id="' + activeTabId + '">' +
                '<table class="table table-bordered table-fixed param-value-column">' +
                '<col width="45%">' +
                '<col width="55%">' +
                '<thead>' +
                '<tr>' +
                '<th><span lang=\"en\" class=\"translatable text\">Parameter</span><span lang=\"ru\" class=\"translatable text\">Параметр</span></th>' +
                '<th><span lang=\"en\" class=\"translatable text\">Value</span><span lang=\"ru\" class=\"translatable text\">Значение</span></th>' +
                '</tr>' +
                '</thead>' +
                '<tbody scroll="yes">' +
                '</tbody>' +
                '</table>' +
                '</div>' +
                '</div>' +
                '</div>';

        container.innerHTML = mainHtml;
    }

    /** Add a parameter with the specified name and value to the specified parameters tab. */
    function pwAddCommonParameter(paramsTabId, paramName, paramValue) {
        var paramsTab = document.getElementById(paramsTabId);
        var paramsTable = paramsTab.getElementsByTagName('table')[0];
        var rowCount = paramsTable.rows.length;
        var row = paramsTable.insertRow(rowCount);
        var cell1 = row.insertCell(0);
        var cell2 = row.insertCell(1);
        cell1.innerHTML = wrapLongText(paramName);
        cell2.innerHTML = wrapLongText(paramValue);
    }

    /**
   * Add a parameter with the specified name and several values.
   * The values should be joined by ';' and
   * input to the method as a single string ('paramValue').
   */
    function pwAddFilesParameter(paramsTabId, paramName, paramValue, disableOpeningByUgene) {
        var paramsTab = document.getElementById(paramsTabId);
        var paramsTable = paramsTab.getElementsByTagName('table')[0];

        var urls = paramValue.split(';');

        var rowCount = paramsTable.rows.length;
        var row1 = paramsTable.insertRow(rowCount);

        var cell1 = row1.insertCell(0);
        cell1.innerHTML = wrapLongText(paramName);
        cell1.rowSpan = urls.length;

        var cell2 = row1.insertCell(1);
        cell2.innerHTML = showFileButton(urls[0], disableOpeningByUgene);

        for (var i = 1; i < urls.length; i++) {
            var row = paramsTable.insertRow(rowCount + i);
            var cell = row.insertCell(0);
            cell.innerHTML = showFileButton(urls[i], disableOpeningByUgene);
        }
    }

    /**
   * Searches for the ParametersWidget initial layout in the container and
   * appends a new common (non-active) tab without parameters.
   */
    function pwAddTab(container, tabName, tabId) {
        var tabsList = container.getElementsByClassName("params-nav-tabs")[0];
        var tabsContent = container.getElementsByClassName("params-tab-content")[0];

        var newTabsListEntry = '<li><a href="#' + tabId + '" data-toggle="tab">' + tabName + '</a></li>';
        var newTabsContentEntry =
                '<div class="tab-pane" id="' + tabId + '">' +
                '<table class="table table-bordered table-fixed param-value-column">' +
                '<col width="45%">' +
                '<col width="55%">' +
                '<thead>' +
                '<tr>' +
                '<th><span class="text">Parameter</span></th>' +
                '<th><span class="text">Value</span></th>' +
                '</tr>' +
                '</thead>' +
                '<tbody scroll="yes">' +
                '</tbody>' +
                '</table>' +
                '</div>';

        tabsList.insertAdjacentHTML('beforeend', newTabsListEntry);
        tabsContent.insertAdjacentHTML('beforeend', newTabsContentEntry);
    }

    //constructor code
    var infos = JSON.parse(agent.workersParamsInfo);
    self.createWidget(infos);
    showOnlyLang(agent.lang); //translate labels
}
