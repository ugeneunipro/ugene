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

function DashboardWidget(containerId) {
    //private
    var self = this;
    this._container = document.getElementById(containerId);
    this._isContainerExists = function (){
        self._container = document.getElementById(containerId);
        if (self._container === null) {
            return false;
        }
        return true;
    };
}

function TableWidget(containerId) {
    DashboardWidget.apply(this, arguments); //inheritance
    var MIN_ROW_COUNT = 3;
    //private
    var self = this;
    //protected
    this._containerId = containerId;
    this._useEmptyRows = true;
    this._rows = Object.create(null); //hash-map

    //public
    this.widths = [];
    this.headers = [];
    this.data = [
             []
         ];
    this.wrapLongText = function(text) {
        return "<div class=\"long-text\" title=\"" + text + "\">" + text + "</div>";
    };

    //protected functions
    this._addContent = function(content) {
        if (!self._isContainerExists()) {
            agent.sl_onJsError("Can't find container by id = " + self._containerId + "!");
            return;
        }
        self._container.innerHTML = self._container.innerHTML + content;
    };

    this._addRow = function(rowId, rowData) {
        if (!self._isContainerExists()) {
            agent.sl_onJsError("Can't find container by id = " + self._containerId + "!");
            return;
        }

        var row = "<tr class=\"filled-row\" id=\"" + rowId + "\">";
        row += self._createRow(rowData);
        row += "</tr>";
        var body = self._container.getElementsByTagName('tbody')[0];
        //alert(body.id);
        var emptyRows = body.getElementsByClassName("empty-row");
        if (emptyRows.length === 0) {
            body.innerHTML += row;
            self._rows[rowId] = row;
        } else {
            emptyRows[0].id=rowId;
            emptyRows[0].outerHTML = row;

            self._rows[rowId] = row;
        }
    };

    this._createRow = function(rowData) {
        var row = "";
        rowData.forEach(function(item) {
            row += "<td>" + item + "</td>";
        });
        return row;
    };

    this._createTable = function() {
        var content = "<table class=\"table table-bordered table-fixed\">";
        self.widths.forEach(function(item) {
            content += "<col width=\"" + item + "%\" />";
        });
        content += "<thead><tr>";
        self.headers.forEach(function(item) {
            content += "<th><span class=\"text\">" + item + "</span></th>";
        });
        content += "<thead><tr>";
        content += "<tbody scroll=\"yes\" id=\""+self._containerId+"123\"/>";
        content += "</table>";
        self._addContent(content);
        if (self._useEmptyRows) {
            addEmptyRows();
        }
    };

    this._fillTable = function() {
        self.data.forEach(function(row) {
            self._addRow(row[1], row.slice(1));
        });
    };

    this._updateRow = function(rowId, rowData) {
        var row = document.getElementById(rowId); //TODO container?
        if (row === null) {
            self._addRow(rowId, rowData);
        } else {
            row.innerHTML = self._createRow(rowData);
        }
    };

    //private functions
    var addEmptyRows = function() {
        if (!self._isContainerExists()) {
            agent.sl_onJsError("Can't find container by id = " + self._containerId + "!");
            return;
        }

        var body = self._container.getElementsByTagName('tbody')[0];

        var rowIdx = 0;
        for (var key in self._rows) {
            rowIdx++;
        }
        while (rowIdx < MIN_ROW_COUNT) {
            var row = "<tr class=\"empty-row\">";
            for (i = 0; i < self.headers.length; i++) {
                row += "<td>&nbsp;</td>";
            }
            row += "</tr>";
            body.innerHTML += row;
            rowIdx++;
        }
    };
}

function OutputFilesWidget(containerId) { //TODO
    //create parent widget
    addWidget("<span lang=\"en\" class=\"translatable\">Output files</span>" + "<span lang=\"ru\" class=\"translatable\">Выходные файлы</span>", "overview_tab", 0, containerId);
    TableWidget.apply(this, arguments); //inheritance
    var MAX_FILES_COUNT = 10;
    var MAX_LEN = 25; //for file's name
    //private
    var self = this;
    var collapsed = false;
    var files = [];
    var actors = [];

    //public
    this.widths = [50, 50];
    this.headers = ["<span lang=\"en\" class=\"translatable\">File</span>" + "<span lang=\"ru\" class=\"translatable\">Файл</span>",
                    "<span lang=\"en\" class=\"translatable\">Producer</span>" + "<span lang=\"ru\" class=\"translatable\">Производитель</span>"
         ];

    this.sl_newOutputFile = function (fileInfo) {
        files.push(fileInfo);
        if (files.length > MAX_FILES_COUNT && !collapsed) {
            collapse();
            showOnlyLang(agent.lang);
            return;
        }
        if (collapsed /*&& id(fileInfo) in self._rows*/) {
            addFileMenu(fileInfo);
        } else {
            self._addRow(id(fileInfo), createRowByFile(fileInfo));
        }
        showOnlyLang(agent.lang);
    };

    //private
    function id(fileInfo){
        if (collapsed) {
            return ":;" + fileInfo.actor + ":;";
        }
        return fileInfo.url;
    }

    function createRowByFile(fileInfo){
        var result = [];
        // const WorkflowMonitor *m = dashboard->monitor();
        // CHECK(NULL != m, result);

        //result.push(createFileButton(fileInfo));
        result.push(showFileButton(fileInfo.url, false, fileInfo.openBySystem));
        result.push(self.wrapLongText(fileInfo.actor));// result << wrapLongText(m->actorName(info.actor));
        return result;
    }

    function createFileButton(fileInfo /*relativeURLPath, relativeDirPath, openBySystem, fileName, openByOsTranslation, openContainingDirTranslation*/) {
        var content = "<div class=\"file-button-ctn\">" +
                "<div class=\"btn-group full-width file-btn-group\">" +
                "<button class=\"btn full-width long-text\" onclick=" +
                onClickAction(fileInfo) +
                "onmouseover=\"this.title=agent.absolute('"+
                relativeURLPath  +
                "')\">" +
                fileName +
                "</button><button class=\"btn dropdown-toggle\" data-toggle=\"dropdown\">" +
                "<span class=\"caret\"></span></button>" +
                createActionsSubMenu(relativeURLPath, relativeDirPath, true, openBySystem, openByOsTranslation, openContainingDirTranslation) +
                "</div></div>";
        return content;
    }

    function onClickAction(fileInfo) {
        var content = (fileInfo.openBySystem === true ) ? "\"agent.openByOS('" + relativeURLPath + "')\"":"\"agent.openUrl('" + relativeURLPath +  "')\"";
        return content;
    }

    function addFileMenu(fileInfo) {
        var dropDownMenuId = "drop-down-menu-"+fileInfo.actor;
        var hasActor = (actors.filter(function(actor) {
            return actor === fileInfo.actor;
        }).length !== 0);
        if(hasActor){
            //add to exist dropdown menu
            var menu = document.getElementById(dropDownMenuId);
            if (!menu) {
                agent.sl_onJsError("Can't find container by id = " + self._containerId + "!");
                return;
            }
            var subMenu = menu.getElementsByClassName("files-menu")[0];
            subMenu.innerHTML += showFileMenu(fileInfo.url);
            var counter = menu.getElementsByClassName("counter")[0];
            //alert(counter);
            counter.innerHTML = files.filter(function(info) {
                return info.actor === fileInfo.actor;
            }).length;
            //var result = [];
            // result.push(menu);
            // result.push(self.wrapLongText(fileInfo.actor));// result << wrapLongText(m->actorName(info.actor));
            // self._updateRow(id(fileInfo), result);
        } else {
            //create new dropdown menu
            var filesButton = "<div id=\""+dropDownMenuId+"\" class=\"btn-group full-width\">" +
                    "<button class=\"files-btn btn dropdown-toggle full-width\" data-toggle=\"dropdown\">" +
                    "<span class=\"counter\">1</span> <span lang=\"en\" class=\"translatable\">file(s)</span><span lang=\"ru\" class=\"translatable\">файл(ов)</span>" +
                    "</button>" +
                    "<ul class=\"files-menu dropdown-menu full-width\"/>" +
                    showFileMenu(fileInfo.url)+
                    "</div>";

            var result = [];
            result.push(filesButton);
            result.push(self.wrapLongText(fileInfo.actor));// result << wrapLongText(m->actorName(info.actor));
            self._addRow(id(fileInfo), result);
            actors.push(fileInfo.actor);
        }
    }

    function collapse() {
        collapsed = true;
        self._container.innerHTML = "";
        self._createTable();
        // var actors = files.map(function(fileInfo){
        //   return fileInfo.actor;
        // });
        files.forEach(function(fileInfo){
            addFileMenu(fileInfo);
        });
        console.log("Collapse!!!");
    }

    //constructor code
    this._createTable();
    showOnlyLang(agent.lang); //translate labels
}
