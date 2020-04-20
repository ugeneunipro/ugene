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

function showOnlyLang(lang) {
    var elements = document.getElementsByClassName("translatable");
    for (i = 0; i < elements.length; i++){
        attr = elements[i].getAttribute("lang");
        if (attr != lang){
            elements[i].style.display = "none";
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

function initializeWebkitPage() {
    //document.getElementById("log_messages").innerHTML += "initializeWebkitPage! <br/>";  // sample of debug message
    showOnlyLang(ugene.lang);
    ugene.sl_pageInitialized();
}

var createAgent = function(channel) {
    window.ugene = channel.objects.ugene;

    showOnlyLang(ugene.lang);
    ugene.sl_pageInitialized();
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
