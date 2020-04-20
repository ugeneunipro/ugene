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

var needCreateWidgets = false;

function wrapLongText(text) {
    return '<div class="long-text" title="' + text + '">' + text + '</div>';
}

function wrapLongText25Symbols(text) { // a workaround for a dropdown submenu
    if (text.length > 28) {
        return text.substr(0, 25) + '&#133;';
    } else {
        return text;
    }
}

/**
 * Hides the hint of the load scheme button and
 * specifies not to show it anymore.
 */
function hideLoadBtnHint() {
    var hint = document.getElementById("load-btn-hint-container");
    if (null !== hint) {
        hint.parentNode.removeChild(hint);
    }
}

/**
 * Shows a button to load the original scheme.
 * If "showHint" is "true", shows a hint about the button usage.
 */
function showLoadButton(showHint) {
    var menuLine = document.getElementsByClassName("dash-menu-line")[0];
    var btnDef = "<button class='btn load-btn' onclick='agent.loadSchema()' title='Load dashboard workflow'><div /></button>";
    menuLine.insertAdjacentHTML('beforeend', btnDef);

    if (showHint === true) {
        var hintDef =
                "<div id='load-btn-hint-container'>" +
                "<div id='load-btn-hint' class='popover fade bottom in' style='display: block'>" +
                "<div class='arrow' style='left: 91%'></div>" +
                "<div class='popover-content'>" +
                "<span lang=\"en\" class=\"translatable\">You can always open the original workflow for your results by clicking on this button.</span>" +
                "<span lang=\"ru\" class=\"translatable\">Вы всегда можете открыть исходную вычислительную схему для ваших результатов, нажав на эту кнопку.</span>" +
                "<div style='text-align: center;'>" +
                "<button class='btn' onclick='agent.hideLoadButtonHint()' style='margin-bottom: 4px; margin-top: 6px;'><span lang=\"en\" class=\"translatable\">OK, got it!</span><span lang=\"ru\" class=\"translatable\">Хорошо!</span></button>" +
                "</div>" +
                "</div>" +
                "</div>" +
                "</div>";
        menuLine.insertAdjacentHTML('beforeend', hintDef);
    }
}

function showFileButton(url, disabled, notOpenedByUgene) {
    if (disabled === true) {
        disabled = 'disabled';
    } else {
        disabled = '';
    }

    if (url.length === 0)
        return "";
    var fileName = url.slice(url.lastIndexOf('/') + 1, url.length);
    var path = url.slice(0, url.lastIndexOf('/') + 1);
    var button;
    if (notOpenedByUgene) {
        button =
                '<div class="file-button-ctn">' +
                '<div class="btn-group full-width file-btn-group">' +
                '<button class="btn full-width long-text" onclick="agent.openByOS(\'' + path + fileName + '\')"' +
                disabled + '>' + fileName +
                '</button>' +
                '<button class="btn dropdown-toggle" data-toggle="dropdown">' +
                '<span class="caret"></span>' +
                '</button>' +
                '<ul class="dropdown-menu full-width">' +
                '<li><a style="white-space: normal;" onclick="agent.openByOS(\'' + path + '\')"><span lang=\"en\" class=\"translatable\">Open containing folder</span><span lang=\"ru\" class=\"translatable\">Открыть директорию, содержащую файл</span></a></li>' +
                '</ul>' +
                '</div>' +
                '</div>';
    } else {
        button =
                '<div class="file-button-ctn">' +
                '<div class="btn-group full-width file-btn-group">' +
                '<button class="btn full-width long-text" onclick="agent.openUrl(\'' + url + '\')"' +
                disabled + '>' + fileName +
                '</button>' +
                '<button class="btn dropdown-toggle" data-toggle="dropdown">' +
                '<span class="caret"></span>' +
                '</button>' +
                '<ul class="dropdown-menu full-width">' +
                '<li><a style="white-space: normal;" onclick="agent.openByOS(\'' + path + '\')"><span lang=\"en\" class=\"translatable\">Open containing folder</span><span lang=\"ru\" class=\"translatable\">Открыть директорию, содержащую файл</span></a></li>' +
                '<li><a style="white-space: normal;" onclick="agent.openByOS(\'' + path + fileName + '\')"><span lang=\"en\" class=\"translatable\">Open by operating system</span><span lang=\"ru\" class=\"translatable\">Открыть при помощи операционной системы</span></a></li>' +
                '</ul>' +
                '</div>' +
                '</div>';
    }
    return button;
}

function showFileMenu(url) {
    if (url.length === 0)
        return "";
    var fileName = url.slice(url.lastIndexOf('/') + 1, url.length);
    var path = url.slice(0, url.lastIndexOf('/') + 1);
    var li =
            '<li class="file-sub-menu dropdown-submenu left-align">' +
            '<a tabindex="-1" href="#" onclick="agent.openUrl(\'' + url + '\')" title="' + url + '">' + wrapLongText25Symbols(fileName) + '</a>' +
            '<ul class="dropdown-menu ">' +
            '<li><a href="#" onclick="agent.openByOS(\'' + path + '\')"><span lang=\"en\" class=\"translatable\">Open containing folder</span><span lang=\"ru\" class=\"translatable\">Открыть директорию, содержащую файл</span></a></li>' +
            '<li><a href="#" onclick="agent.openByOS(\'' + path + fileName + '\')"><span lang=\"en\" class=\"translatable\">Open by operating system</span><span lang=\"ru\" class=\"translatable\">Открыть при помощи операционной системы</span></a></li>' +
            '</ul></li>';
    return li;
}

function addTab(tabId, tabName) {
    var tabsList = document.getElementsByClassName("nav nav-pills dash-nav")[0];
    var newTab = "<li class=''><a href='" + tabId + "' data-toggle='tab' class='dash-tab-name'>" + tabName + "</a></li>";
    tabsList.insertAdjacentHTML('beforeend', newTab);
}

function addWidget(title, dashTab, cntNum, id) {
    var tabContainer = document.getElementById(dashTab);
    if (tabContainer === null) {
        agent.sl_onJsError("Can't find the tab container!");
        return;
    }
    var hasInnerContainers = true;
    var InputDashTab = "input_tab";
    var ExternalToolsTab = "ext_tools_tab";
    if (InputDashTab == dashTab || ExternalToolsTab == dashTab) {
        hasInnerContainers = false;
    }
    var mainContainer = tabContainer;
    if (hasInnerContainers) {
        var left = true;
        if (0 === cntNum) {
            left = true;
        } else if (1 == cntNum) {
            left = false;
        } else if (containerSize(tabContainer, ".left-container") <= containerSize(tabContainer, ".right-container")) {
            left = true;
        } else {
            left = false;
        }

        var elements = tabContainer.getElementsByClassName(left ? "left-container" : "right-container");
        if (elements[0] === null) {
            agent.sl_onJsError("Can't find a container inside a tab!");
            return;
        }
        mainContainer = elements[0];
        mainContainer.innerHTML = mainContainer.innerHTML +"<div class=\"widget\">" +
                "<div class=\"title\"><div class=\"title-content\">" + title + "</div></div>" +
                "<div class=\"widget-content\" id=\"" + id + "\"></div>" +
                "</div>";
    }
}

function showOnlyLang(lang) {
    var elements = document.getElementsByClassName("translatable");
    for (var i = 0; i<elements.length; i++) {
        var attr = elements[i].getAttribute("lang");
        if (attr !== lang) {
            elements[i].style.display = "none";
        } else {
            elements[i].style.display = "";
        }
    }
}

function loadScript(url, callback) {
    // Adding the script tag to the head as suggested before
    var head = document.getElementsByTagName('head')[0];
    var script = document.createElement('script');
    script.type = 'text/javascript';
    script.src = url;

    // Then bind the event to the callback function.
    // There are several events for cross browser compatibility.
    script.onreadystatechange = callback;
    script.onload = callback;

    // Fire the loading
    head.appendChild(script);
}

function createWidgets() {
    parametersWidget = new ParametersWidget("parametersWidget");
    outputWidget = new OutputFilesWidget("outputWidget");
    statusWidget = new StatusWidget("statusWidget");
    statisticsWidget = new StatisticsWidget("statisticsWidget");
    startTimer();
}

function connect() {
    try {
        agent.si_progressChanged.connect(function(progress) {
            statusWidget.sl_progressChanged(progress);
        });

        window.agent.si_taskStateChanged.connect(function(state) {
            statusWidget.sl_taskStateChanged(state);
        });

        window.agent.si_newProblem.connect(function(problem) {
            problem = JSON.parse(problem);
            if (document.getElementById("problemsWidget") === null) {
                problemWidget = new ProblemsWidget("problemsWidget");
            }
            problemWidget.sl_newProblem(problem, problem.count);
        });

        window.agent.si_workerStatsInfoChanged.connect(function(info) {
            info = JSON.parse(info);
            statisticsWidget.sl_workerStatsInfoChanged(info);
        });

        window.agent.si_workerStatsUpdate.connect(function(workersStatisticsInfo) {
            workersStatisticsInfo = JSON.parse(workersStatisticsInfo);
            statisticsWidget.sl_workerStatsUpdate(workersStatisticsInfo);
        });

        window.agent.si_newOutputFile.connect(function(fileInfo) {
            fileInfo = JSON.parse(fileInfo);
            window.outputWidget.sl_newOutputFile(fileInfo);
        });

        window.agent.si_onLogChanged.connect(function(logEntry) {
            logEntry = JSON.parse(logEntry);
            if (externalToolsWidget === null) {
                externalToolsWidget = new ExternalToolsWidget("externalToolsWidget");
            }
            externalToolsWidget.sl_onLogChanged(logEntry);
        });
    } catch (e) {
        agent.sl_onJsError(e);
    }
}

function initializeWebkitPage() {
    if (needCreateWidgets) {
        createWidgets();
        connect();
    }

    showOnlyLang(agent.lang);
    agent.sl_pageInitialized();
}

var createAgent = function(channel) {
    window.agent = channel.objects.agent;
    if (needCreateWidgets) {
        createWidgets();
        connect();
    }

    showOnlyLang(agent.lang);
    //document.getElementById("log_messages").innerHTML += "Agent created! <br/>";  // sample of debug message
    agent.sl_pageInitialized();
}

function installWebChannel(onSockets, port) {
    if (onSockets) {
        var baseUrl = "ws://127.0.0.1:" + port;
        var socket = new WebSocket(baseUrl);

        socket.onclose = function() {
            console.error("web channel closed");
        };

        socket.onerror = function(error) {
            console.error("web channel error: " + error);
        };

        socket.onopen = function() {
            loadScript("qrc:///qtwebchannel/qwebchannel.js",
                       function() {
                           new QWebChannel(socket, createAgent);
                       });
        }
    } else {
        loadScript("qrc:///qtwebchannel/qwebchannel.js",
                   function() {
                       new QWebChannel(qt.webChannelTransport, createAgent);
                   });
    }
}

function setNeedCreateWidgets(_needCreateWidgets) {
    needCreateWidgets = _needCreateWidgets;
}
