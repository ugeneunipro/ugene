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

//'Copy' menu definition
function initializeCopyMenu() {
    var items = [{
                     isDivider: false,
                     tag: "a",
                     id: "copy_selected_action",
                     innerText: "Copy selected text",
                     command: "copySelected(event, this)",
                     active: true
                 }, {
                     isDivider: false,
                     tag: "a",
                     id: "copy_all_action",
                     innerText: "Copy element content",
                     command: "copyElementContent(event, this)",
                     active: true
                 }];
    initializeMenu("ext_menu", items);
}

function addEventListeners() {
    document.documentElement.addEventListener("mouseup", hideMenu);
}

function copySelected(e, element) {
    agent.setClipboardText(document.getSelection() + "");
    e.stopPropagation();
}

function copyElementContent(e, element) {
    var menuObj = document.getElementById(element.getAttribute("menuId"));
    var targetObj = document.getElementById(menuObj.getAttribute("target"));
    agent.setClipboardText(targetObj.innerText);
    e.stopPropagation();
}

function contextmenu(e, element) {
    //Check that right button is pressed
    if (e.which === 3 || e.button === 2) {
        return onContextMenuTriggered(e, "ext_menu", element.id);
    } else {
        return true;
    }
}

//Common context menu functions 
function initializeMenu(menuId, menuItems) {
    var container = document.body;
    var menu = createContextMenu(menuId, menuItems);
    $("#"+menuId).hide();
}

function onContextMenuTriggered(e, menuId, element) { 
    var menu = document.getElementById(menuId);
    if (menu === null) {
        return false;
    }
    $("#" + menuId).show();
    setCoordsForMenu(menu, e);
    menu.setAttribute("target", element);

    var copySelAction = document.getElementById("li_copy_selected_action");
    if (copySelAction !== null) {
        if (document.getSelection() && document.getSelection() != "") {
            copySelAction.className = "";
        } else {
            copySelAction.className = "disabled";
        }
    }
    e.stopPropagation();
    return false;
}

function createContextMenu(menuId, menuElements) {
    var newMenu = document.createElement("ul");
    newMenu.className = "dropdown-menu context";
    newMenu.id = menuId;
    document.body.appendChild(newMenu);
    
    for (var i = 0; i < menuElements.length; i++) {
        var currentElement = menuElements[i];
        var listElement = document.createElement("li");
        listElement.id = "li_" + currentElement.id;
        newMenu.appendChild(listElement);
        if (currentElement.isDivider) {
            listElement.className = "divider";
            continue;
        }
        
        var docElement = document.createElement(currentElement.tag);
        docElement.innerHTML = currentElement.innerText;
        docElement.id = currentElement.id;
        docElement.setAttribute("menuId", menuId);
        docElement.setAttribute("onmousedown", currentElement.command);
        listElement.appendChild(docElement);
    }
    return newMenu;
}

//Place the menu in the mouse click position
function setCoordsForMenu(menuObj, event) {
    var winWidth = $(window).width();
    var winHeight = $(window).height();

    if ((event.pageX + menuObj.offsetWidth) > winWidth) {
        menuObj.style.left = (event.pageX - menuObj.offsetWidth.toString()) + 'px';
    } else {
        menuObj.style.left = event.pageX.toString() + 'px';
    }

    if ((event.pageY + menuObj.offsetHeight) > winHeight) {
        menuObj.style.top = (event.pageY - menuObj.offsetHeight).toString() + 'px';
    } else {
        menuObj.style.top = event.pageY.toString() + 'px';
    }
}

function hideMenu() {
    $(".dropdown-menu.context").hide();
}
