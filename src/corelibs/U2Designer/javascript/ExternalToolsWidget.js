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

/** Creates the ParametersWidget layout and the first active tab (without parameters). */

/**
The tree will look like this:

root node (invisible)
|- Actor_1 node (actorNode)
|  |- Actor_1 run node  (actorTickNode)
|  |  |- Tool run node  (toolRunNode)
|  |     |- Command
|  |     |- Stdout
|  |     |- Stderr
|  |- Actor_1 run node  (actorTickNode)
|     |- Tool run node  (toolRunNode)
|        |- Command
|        |- Stdout
|        |- Stderr
|- Actor_2 node (actorNode)
   |- Actor_2 run node  (actorTickNode)
   |  |- Tool run node  (toolRunNode)
   |     |- Command
   |     |- Stdout
   |     |- Stderr
   |- Actor_2 run node  (actorTickNode)
      |- Tool run node  (toolRunNode)
         |- Command
         |- Stdout
         |- Stderr

There could be several actor nodes.
Each 'actor' node can have several 'actor tick' nodes - it depends on how many external tools based tasks the element has created.
Each 'actor tick' node can have several 'tool run' node - it depends on how many external tools were launched with listeners.
Each 'tool run' node have one 'command' node and two 'output' nodes, which are shown if there are any messages in the appropriate output.

Count of nodes is limited:
  MAXIMUM_TOOL_RUN_NODES_PER_ACTOR 'tool run' nodes per one actor;
  MAXIMUM_TOOL_RUN_NODES_TOTAL 'tool run' nodes per one workflow launch;
  MAXIMUM_ACTOR_TICK_NODES 'actor tick' nodes per one workflow launch;
  MAXIMUM_ACTOR_NODES 'actor' nodes per one workflow launch.
A node with message will appear in case of limit exeeded.

Size of each 'output' node is limited with MAXIMUM_TOOL_RUN_CONTENT_SIZE. A message will appear at the end of the 'output' node in case of limit exeeded.
*/

function ExternalToolsWidget(containerId) {
    document.getElementById("ext_tools_tab_menu").style.display = ""; // set visible menu
    DashboardWidget.apply(this, arguments); //inheritance
    var LINE_BREAK = "break_line";
    var BACK_SLASH = "s_quote";
    var SINGLE_QUOTE = "b_slash";

    var TREE_ROOT_DIV_ID = 'treeRootDiv';
    var TREE_ROOT_ID = 'treeRoot';

    var LIMIT_EXEEDED_ATTRIBUTE = 'limitExeeded';
    var TEXT_UPDATED_ATTRIBUTE = 'textUpdated';

    var MAXIMUM_TOOL_RUN_CONTENT_SIZE = 100000;
    var MAXIMUM_TOOL_RUN_NODES_PER_ACTOR = 3;
    var MAXIMUM_TOOL_RUN_NODES_TOTAL = 100;
    var MAXIMUM_ACTOR_TICK_NODES = 1000;
    var MAXIMUM_ACTOR_NODES = 1000;

    // see enum initialization in ExternalToolRunTask.h
    var ERROR_LOG = 0;
    var OUTPUT_LOG = 1;
    var PROGRAM_WITH_ARGUMENTS = 2;

    //private
    var self = this;

    //public
    this.sl_onLogChanged = function(entry) {
        addInfoToWidget(entry);
    };

    this.sl_onLogUpdate = function(extToolsLog) {
        if (lastEntryIndex === extToolsLog.length - 1)
            return;
        var logEntries = extToolsLog;
        for (var i = lastEntryIndex + 1; i < logEntries.length; i++) {
            var entry = logEntries[i];
            addInfoToWidget(entry);
        }
        lastEntryIndex = extToolsLog.length - 1;
    };

    function lwInitContainer(container) {
        var mainHtml =
                '<div class="tree" id="' + TREE_ROOT_DIV_ID + '">' +
                '<ul id="' + TREE_ROOT_ID + '">' +
                '</ul>' +
                '</div>';

        container.innerHTML = mainHtml;
        initializeCopyMenu();
    }

    //constructor
    lwInitContainer(self._container);
    showOnlyLang(agent.lang); //translate labels

    //private
    var lastEntryIndex = 0;

    function getActorNodeId(entry) {
        return "log_tab_id_" + entry.actorId;
    }

    function getActorTickNodeId(entry) {
        return "actor_" + entry.actorId + "_run_" + entry.actorRunNumber;
    }

    function getToolRunNodeId(entry) {
        return getActorTickNodeId(entry) + "_tool_" + entry.toolName + "_run_" + entry.toolRunNumber;
    }

    function getToolRunCommandLabelNodeId(entry) {
        return getToolRunNodeId(entry) + '_command';
    }

    function getToolRunStdoutLabelNodeId(entry) {
        return getToolRunNodeId(entry) + '_stdout';
    }

    function getToolRunStderrLabelNodeId(entry) {
        return getToolRunNodeId(entry) + '_stderr';
    }

    function getToolRunContentLabelNodeId(entry) {
        switch (entry.contentType) {
        case ERROR_LOG:
            return getToolRunStderrLabelNodeId(entry);
        case OUTPUT_LOG:
            return getToolRunStdoutLabelNodeId(entry);
        case PROGRAM_WITH_ARGUMENTS:
            return getToolRunCommandLabelNodeId(entry);
        }
    }

    function getToolRunContentLabelNodeText(contentType) {
        switch (contentType) {
        case ERROR_LOG:
            return 'Output log (stderr)';
        case OUTPUT_LOG:
            return 'Output log (stdout)';
        case PROGRAM_WITH_ARGUMENTS:
            return 'Command';
        }
    }

    function getToolRunContentLabelNodeClass(contentType) {
        switch (contentType) {
        case ERROR_LOG:
            return 'badge badge-important';
        case OUTPUT_LOG:
            return 'badge badge-info';
        case PROGRAM_WITH_ARGUMENTS:
            return 'badge command';
        }
    }

    function getAppropriateSpanId(nodeId) {
        return nodeId + '_span';
    }

    function addInfoToWidget(entry) {
        var content = entry.lastLine;
        content = content.replace(new RegExp("\n", 'g'), "break_line");
        content = content.replace(new RegExp("\r", 'g'), "");
        content = content.replace("'", "s_quote");
        lwAddTreeNode(entry, content);
    }

    function addChildrenElement(parentObject, elemTag, elemHTML) {
        var newElem = document.createElement(elemTag);
        newElem.innerHTML = elemHTML;
        parentObject.appendChild(newElem);
        return newElem;
    }

    function addLimitationMessageNode(parentNode) {
        var newListElem = addChildrenElement(parentNode, 'LI', '');
        newListElem.className = 'parent_li';

        var logsFolderUrl = agent.getLogsFolderUrl();
        var linkClass = 'log-folder-link';
        var messageNodeText = 'Messages limit on the dashboard exceeded. See <a onclick=\"openLog(\'' + logsFolderUrl + '\')\" class=\'' + linkClass + '\'>log files</a>, if required.';
        var messageNode = addChildrenElement(newListElem, 'span', messageNodeText);
        messageNode.className = 'badge limitation-message';
    }

    function addNoncollapsibleChildrenNode(parentNode, nodeContent, nodeId, nodeClass) {
        var newListElem = addChildrenElement(parentNode, 'LI', '');
        newListElem.className = 'parent_li';
        var span = addChildrenElement(newListElem, 'span', nodeContent);

        span.setAttribute('onmouseover', 'highlightElement(this, event, true)');
        span.setAttribute('onmouseout', 'highlightElement(this, event, false)');
        span.setAttribute('onmouseup', 'return contextmenu(event, this);');

        span.id = getAppropriateSpanId(nodeId);
        span.className = nodeClass;
        var newList = addChildrenElement(newListElem, 'UL', '');
        newList.id = nodeId;

        return newList;
    }

    function addChildrenNode(parentNode, nodeContent, nodeId, nodeClass) {
        var newList = addNoncollapsibleChildrenNode(parentNode, nodeContent, nodeId, nodeClass);

        var span = document.getElementById(getAppropriateSpanId(nodeId));
        span.setAttribute('title', 'Collapse this branch');
        span.setAttribute('onclick', 'collapseNode(this)');

        return newList;
    }

    function getActorNode(entry) {
        var actorNodeId = getActorNodeId(entry);
        var actorNode = document.getElementById(actorNodeId);

        if (actorNode === null) {
            var rootList = document.getElementById(TREE_ROOT_ID);

            if (rootList.hasAttribute(LIMIT_EXEEDED_ATTRIBUTE)) {
                return;
            }

            if (rootList.childElementCount >= MAXIMUM_ACTOR_TICK_NODES) {
                addLimitationMessageNode(rootList);
                rootList.setAttribute(LIMIT_EXEEDED_ATTRIBUTE, '');
                return;
            }

            actorNode = addChildrenNode(rootList, entry.actorName, actorNodeId, 'badge actor-node');
        }

        return actorNode;
    }

    function getActorTickNode(entry) {
        var actorNode = getActorNode(entry);

        var actorTickNodeId = getActorTickNodeId(entry);
        var actorTickNode = document.getElementById(actorTickNodeId);

        if (actorTickNode === null) {
            if (actorNode.hasAttribute(LIMIT_EXEEDED_ATTRIBUTE)) {
                return;
            }

            var rootList = document.getElementById(TREE_ROOT_ID);
            if (rootList.hasAttribute(LIMIT_EXEEDED_ATTRIBUTE)) {
                return;
            }

            if (actorNode.childElementCount >= MAXIMUM_ACTOR_TICK_NODES) {
                addLimitationMessageNode(actorNode);
                actorNode.setAttribute(LIMIT_EXEEDED_ATTRIBUTE, '');
                return;
            }

            var actorTickNodeText = entry.actorName + ' run ' + entry.actorRunNumber;
            actorTickNode = addChildrenNode(actorNode, actorTickNodeText, actorTickNodeId, 'badge actor-tick-node');
        }

        return actorTickNode;
    }

    function updateFirstToolRunNode(entry) {
        var toolRunNumber = entry.toolRunNumber;
        entry.toolRunNumber = 1;
        var toolRunNodeId = getToolRunNodeId(entry);
        var toolRunNode = document.getElementById(toolRunNodeId);

        if (!toolRunNode.hasAttribute(TEXT_UPDATED_ATTRIBUTE)) {
            var toolRunNodeText = entry.toolName + ' run ' + entry.toolRunNumber;
            var toolRunSpan = document.getElementById(getAppropriateSpanId(toolRunNodeId));
            agent.setClipboardText(toolRunSpan.innerHTML);
            var originalText = entry.toolName + ' run ';
            toolRunSpan.innerHTML = toolRunSpan.innerHTML.replace(originalText, originalText + entry.toolRunNumber + ' ');
            toolRunNode.setAttribute(TEXT_UPDATED_ATTRIBUTE, '');
        }

        entry.toolRunNumber = toolRunNumber;
    }

    function getTotalToolRunNodesConut() {
        return document.querySelectorAll('.tool-run-node').length;
    }

    function getToolRunNode(entry) {
        var actorTickNode = getActorTickNode(entry);

        var toolRunNodeId = getToolRunNodeId(entry);
        var toolRunNode = document.getElementById(toolRunNodeId);

        if (toolRunNode === null) {
            if (actorTickNode.hasAttribute(LIMIT_EXEEDED_ATTRIBUTE)) {
                return;
            }

            if (actorTickNode.childElementCount >= MAXIMUM_TOOL_RUN_NODES_PER_ACTOR) {
                addLimitationMessageNode(actorTickNode);
                actorTickNode.setAttribute(LIMIT_EXEEDED_ATTRIBUTE, '');
                return;
            }

            if (getTotalToolRunNodesConut() >= MAXIMUM_TOOL_RUN_NODES_TOTAL) {
                var rootList = document.getElementById(TREE_ROOT_ID);
                if (rootList.hasAttribute(LIMIT_EXEEDED_ATTRIBUTE)) {
                    return;
                }

                addLimitationMessageNode(rootList);
                rootList.setAttribute(LIMIT_EXEEDED_ATTRIBUTE, '');
                return;
            }

            var toolRunNodeText = entry.toolName + ' run ';
            if (entry.toolRunNumber > 1) {
                updateFirstToolRunNode(entry);
                toolRunNodeText += entry.toolRunNumber + ' ';
            }
            toolRunNode = addChildrenNode(actorTickNode, toolRunNodeText, toolRunNodeId, 'badge badge-success tool-run-node');

            var toolRunSpan = document.getElementById(getAppropriateSpanId(toolRunNodeId));

            var copyRunInfoButton = document.createElement('button');
            copyRunInfoButton.className = "copyRunInfo";
            copyRunInfoButton.setAttribute("title", "Copy external tool run string");
            copyRunInfoButton.setAttribute("onclick", "copyRunInfo(event, \'" + toolRunNodeId + "\'); return false;");

            copyRunInfoButton.setAttribute('onmousedown', 'return onButtonPressed(this, event);');
            copyRunInfoButton.setAttribute('onmouseup',   'return onButtonReleased(this, event);');

            copyRunInfoButton.setAttribute('onmouseover', 'highlightElement(this, event, true)');
            copyRunInfoButton.setAttribute('onmouseout', 'highlightElement(this, event, false)');

            toolRunSpan.appendChild(copyRunInfoButton);

            if (!isNodeCollapsed(toolRunSpan)) {
                collapseNode(toolRunSpan);
            }
        }

        return toolRunNode;
    }

    function getToolRunContentNode(entry) {
        var toolRunNode = getToolRunNode(entry);
        if (!toolRunNode) {
            return;
        }

        var toolRunContentLabelNodeId = getToolRunContentLabelNodeId(entry);
        var toolRunContentLabelNode = document.getElementById(toolRunContentLabelNodeId);

        var toolRunContentNodeId = toolRunContentLabelNodeId + '_content';
        var toolRunContentNode = null;

        if (toolRunContentLabelNode === null) {
            var toolRunContentLabelNodeText = getToolRunContentLabelNodeText(entry.contentType);
            var labelNodeClass = getToolRunContentLabelNodeClass(entry.contentType);
            toolRunContentLabelNode = addChildrenNode(toolRunNode, toolRunContentLabelNodeText, toolRunContentLabelNodeId, labelNodeClass);
            toolRunContentNode = addNoncollapsibleChildrenNode(toolRunContentLabelNode, '', toolRunContentNodeId, 'content');
        } else {
            toolRunContentNode = document.getElementById(toolRunContentNodeId);
        }

        return toolRunContentNode;
    }

    function lwAddTreeNode(entry, content) {
        if (content) {
            content = content.replace(/break_line/g, '<br>');
            content = content.replace(/(<br>){3,}/g, '<br><br>');
            content = content.replace(/s_quote/g, '\'');
            content = content.replace(/b_slash/g, '\\');
        } else {
            return;
        }

        var toolRunContentNode = getToolRunContentNode(entry);
        if (!toolRunContentNode) {
            return;
        }

        if (!toolRunContentNode.innerHTML) {
            content = content.replace(/^(<br>)+/, "");
        }

        if (ERROR_LOG === entry.contentType) {
            var toolRunNodeSpan = document.getElementById(getAppropriateSpanId(getToolRunNodeId(entry)));
            toolRunNodeSpan.className = 'badge badge-important tool-run-node';
        }

        var toolRunContentNodeSpan = document.getElementById(getAppropriateSpanId(toolRunContentNode.id));
        if (toolRunContentNodeSpan.hasAttribute(LIMIT_EXEEDED_ATTRIBUTE)) {
            return;
        }

        if (toolRunContentNodeSpan.innerHTML.length >= MAXIMUM_TOOL_RUN_CONTENT_SIZE) {
            var logUrl = agent.getLogUrl(entry.actorId, entry.actorRunNumber, entry.toolName, entry.toolRunNumber, entry.contentType);
            var linkClass = 'log-file-link';
            content = '<br/><br/>The external tools output is too large and can\'t be visualized on the dashboard. Find full output in <a onclick=\"openLog(\'' + logUrl + '\')\" class=\'' + linkClass + '\'>this file</a>.'
            toolRunContentNodeSpan.setAttribute(LIMIT_EXEEDED_ATTRIBUTE, '');
        }

        toolRunContentNodeSpan.innerHTML += content;
    }

    function setElementBackground(element, backgroundColor) {
        element.style.backgroundColor = backgroundColor;
    }
}

function onButtonPressed(element, event) {
    if(1 === event.which) {
        $(element).addClass('pressed');
    }
    event.stopPropagation();
    return false;
}

function onButtonReleased(element, event) {
    $(element).removeClass('pressed');
    event.stopPropagation();
    return false;
}

function copyRunInfo(event, toolRunNodeId) {
    agent.setClipboardText(document.getElementById(toolRunNodeId + '_command_content_span').innerHTML);
    event.stopPropagation();
}

function isNodeCollapsed(node) {
    return node.getAttribute('title') === 'Expand this branch';
}

function collapseNode(element) {
    var children = $(element).parent('li.parent_li').find('ul:first');
    if (children.is(":visible") === $(element).is(":visible")) {
        children.hide(0);
        $(element).attr('title', 'Expand this branch');
    } else {
        children.show(0);
        $(element).attr('title', 'Collapse this branch');
    }
}

function highlightElement(element, e, isHighlighted)  {
    if(true === isHighlighted) {
        $('li span').removeClass('hoverIntent');
        $(element).addClass('hoverIntent');
        e.stopPropagation();
    }
    else {
        $(element).removeClass('hoverIntent');
    }
}

function openLog(logUrl) {
    agent.openByOS(logUrl);
}
