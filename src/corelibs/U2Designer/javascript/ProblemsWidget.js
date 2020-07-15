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

function ProblemsWidget(containerId) { // may be add first problem as second argument function ProblemsWidget(containerId, firstProblemInfo)
    addWidget("<span lang=\"en\" class=\"translatable\">Notifications</span>" + "<span lang=\"ru\" class=\"translatable\">Уведомления</span>", "overview_tab", 0, containerId);
    TableWidget.apply(this, arguments); //inheritance

    //private
    var self = this;

    //public
    this.widths = [10, 30, 60];
    this.headers = ["<span lang=\"en\" class=\"translatable\">Type</span>" + "<span lang=\"ru\" class=\"translatable\">Тип</span>",
                    "<span lang=\"en\" class=\"translatable\">Element</span>" + "<span lang=\"ru\" class=\"translatable\">Элемент</span>",
                    "<span lang=\"en\" class=\"translatable\">Message</span>" + "<span lang=\"ru\" class=\"translatable\">Сообщение</span>"
         ];

    this.sl_newProblem = function (problemInfo, count) {
        if (!self._isContainerExists()) {
            agent.sl_onJsError("Can't find container by id = " + self._containerId + "!");
            return;
        }
        if (id(problemInfo) in self._rows) {
            self._updateRow(id(problemInfo), createRow(problemInfo, /*multi row*/ true, count));
        } else {
            self._addRow(id(problemInfo), createRow(problemInfo));
        }
    };

    this.problemImage = function (problemInfo) {
        var image = "qrc:///U2Lang/images/";
        if ("error" === problemInfo.type) {
            image += "error.png";
            tooltip = (agent.lang !== "ru") ? "Error" : "Ошибка";
        } else if ("warning" === problemInfo.type) {
            image += "warning.png";
            tooltip = (agent.lang !== "ru") ? "Warning" : "Предупреждение";
        } else if ("info" === problemInfo.type) {
            image = "qrc:///core/images/info.png";
            tooltip = (agent.lang !== "ru") ? "Information" : "Информация";
        } else {
            agent.sl_onJsError("Unknown type: " + problemInfo.type, "");
        }
        return "<img src=\"" + image + "\" title=\"" + tooltip + "\" class=\"problem-icon\"/>";
    };

    //protected
    this._createRow = function(rowData) {
        var row = "";
        rowData.forEach(function(item) {
            row += "<td style=\"word-wrap: break-word\">" + item + "</td>";
        });
        return row;
    };

    //private
    function createRow(info, multi, count) {
        multi = multi || false;
        count = count || 1;
        var result = [];
        var prefix = "";
        if (multi) {
            prefix = "(" + count + ") ";
        }

        result.push(self.problemImage(info));
        result.push(self.wrapLongText(info.actorName));
        result.push(getTextWithWordBreaks(prefix + info.message));

        return result;
    }

    function getTextWithWordBreaks(text) {
        var textWithBreaks = text;
        textWithBreaks = textWithBreaks.replace("\\", "\\<wbr>").replace("/", "/<wbr>");
        return textWithBreaks;
    }

    function id(info) {
        return escape(info.actorId + info.message);
    }

    //constructor code
    this._createTable();
    showOnlyLang(agent.lang); //translate labels
}
