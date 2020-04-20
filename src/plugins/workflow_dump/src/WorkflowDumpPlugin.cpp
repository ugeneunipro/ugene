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

#include "WorkflowDumpPlugin.h"

#include <U2Lang/WorkflowEnv.h>

#include <U2Core/AppContext.h>
#include <U2Gui/MainWindow.h>
#include <U2Core/Settings.h>

#include <QtGui/QMenu>
#include <QtCore/QStringList>

#include <U2Lang/ActorModel.h>
#include <U2Lang/ActorPrototypeRegistry.h>

/* TRANSLATOR U2::LocalWorkflow::WorkflowView */
/* TRANSLATOR U2::LocalWorkflow::WorkflowDumpPlugin */


namespace U2 {

using namespace Workflow;

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    WorkflowDumpPlugin * plug = new WorkflowDumpPlugin();
    return plug;
}

WorkflowDumpPlugin::WorkflowDumpPlugin()
: Plugin(tr("Workflow Dump"), tr("Workflow Dump exports workflow data.")){
#ifdef WORKFLOW_DUMP
    if (AppContext::getMainWindow()) {
        QAction* dumpAction = new QAction(tr("Dump workers"), this);
        QMenu* tools = AppContext::getMainWindow()->getTopLevelMenu(MWMENU_TOOLS);
        tools->addAction(dumpAction);
        connect(dumpAction,SIGNAL(triggered()),SLOT(sl_dumpWorkers()));

    }
#endif /*WORKFLOW_DUMP*/
}

class JsonWriter {
public:

    JsonWriter() : separator(":"), lenient(false), indent("    ") {
        stack.append(EMPTY_DOCUMENT);
    }

    JsonWriter& name(QString name) {
        //if (name == null) {
        //    throw new NullPointerException("name == null");
        //}
        beforeName();
        string(name);
        return *this;
    }
    JsonWriter& value(QString value) {
        beforeValue(false);
        string(value);
        return *this;
    }
    JsonWriter& boolValue(bool value) {
        beforeValue(false);
        out.append(value ? "true" : "false");
        return *this;
    }
    JsonWriter& intValue(int value) {
        beforeValue(false);
        out.append(QString::number(value));
        return *this;
    }
    JsonWriter& doubleValue(double value) {
        beforeValue(false);
        out.append(QString::number(value));
        return *this;
    }
    JsonWriter& longValue(long value) {
        beforeValue(false);
        out.append(QString::number(value));
        return *this;
    }


    JsonWriter& plainValue(QByteArray value) {
        /*if (value == null) {
            return nullValue();
        }*/
        beforeValue(false);
        out.append(value);
        return *this;
    }

    JsonWriter& beginArray() {
        return open(EMPTY_ARRAY, "[");
    }
    JsonWriter& beginObject() {
        return open(EMPTY_OBJECT, "{");
    }
    JsonWriter& endArray() {
        return close(EMPTY_ARRAY, NONEMPTY_ARRAY, "]");
    }
    JsonWriter& endObject() {
        return close(EMPTY_OBJECT, NONEMPTY_OBJECT, "}");
    }

    QByteArray& getBytes() {
        return out;
    }

private:
    enum JsonScope {
        EMPTY_ARRAY,
        NONEMPTY_ARRAY,
        EMPTY_OBJECT,
        DANGLING_NAME,
        NONEMPTY_OBJECT,
        EMPTY_DOCUMENT,
        NONEMPTY_DOCUMENT,
        CLOSED
    };
    const QString separator;
    bool lenient;
    QString indent;
    QList<JsonScope> stack;
    QByteArray out;

    void string(QString value) {
        out.append("\"");
        for (int i = 0, length = value.length(); i < length; i++) {
          char c = value[i].toAscii();

          /*
           * From RFC 4627, "All Unicode characters may be placed within the
           * quotation marks except for the characters that must be escaped:
           * quotation mark, reverse solidus, and the control characters
           * (U+0000 through U+001F)."
           */
          switch (c) {
          case '"':
          case '\\':
            out.append('\\');
            out.append(c);
            break;

          case '\t':
            out.append("\\t");
            break;

          case '\b':
            out.append("\\b");
            break;

          case '\n':
            out.append("\\n");
            break;

          case '\r':
            out.append("\\r");
            break;

          case '\f':
            out.append("\\f");
            break;

          case '<':
          case '>':
          case '&':
          case '=':
          case '\'':
              out.append(c);
            break;

          default:
            /*if (c <= 0x1F) {
              out.append(String.format("\\u%04x", (int) c));
            } else {*/
              out.append(c);
            //}
            break;
          }
        }
        out.append("\"");
    }

    JsonWriter& open(JsonScope empty, QString openBracket) {
        beforeValue(true);
        stack.append(empty);
        out.append(openBracket);
        return *this;
    }
    JsonWriter& close(JsonScope empty, JsonScope nonempty, QString closeBracket) {
        JsonScope context = peek();
        if (context != nonempty && context != empty) {
            throw QString("Nesting problem:");
        }

        stack.takeAt(stack.size() - 1);
        if (context == nonempty) {
            newline();
        }
        out.append(closeBracket);
        return *this;
    }

    void newline() {
        if (indent.isEmpty()) {
            return;
        }

        out.append("\r\n");
        for (int i = 1; i < stack.size(); i++) {
            out.append(indent);
        }
    }

    JsonScope peek() {
        return stack.at(stack.size() - 1);
    }

    void replaceTop(JsonScope topOfStack) {
        stack[stack.size() - 1] = topOfStack;
    }

    void beforeName() {
        JsonScope context = peek();
        if (context == NONEMPTY_OBJECT) { // first in object
            out.append(',');
        } else if (context != EMPTY_OBJECT) { // not in an object!
            throw QString("Nesting problem");
        }
        newline();
        replaceTop(DANGLING_NAME);
    }

    void beforeValue(bool root) {
        switch (peek()) {
        case EMPTY_DOCUMENT: // first in document
            if (!lenient && !root) {
                throw QString("JSON must start with an array or an object.");
            }
            replaceTop(NONEMPTY_DOCUMENT);
            break;

        case EMPTY_ARRAY: // first in array
            replaceTop(NONEMPTY_ARRAY);
            newline();
            break;

        case NONEMPTY_ARRAY: // another in array
            out.append(',');
            newline();
            break;

        case DANGLING_NAME: // value for name
            out.append(separator);
            replaceTop(NONEMPTY_OBJECT);
            break;

        case NONEMPTY_DOCUMENT:
            throw QString("JSON must have only one top-level value.");

        default:
            throw QString("Nesting problem");
        }
    }
};

static QByteArray type2json(DataTypePtr type) {
    JsonWriter w;
    w.beginObject()
        .name("id").value(type->getId())
        .name("kind");
    switch(type->kind()) {
        case DataType::Single:
            w.value("Single");
            break;
        case DataType::List:
            w.value("List")
             .name("type").plainValue(type2json(type->getDatatypeByDescriptor()));
            break;
        case DataType::Map:
            w.value("Map").name("type").beginObject();
            foreach(const Descriptor& desc, type->getDatatypesMap().keys()) {
                w.name(desc.getId())
                 .plainValue(type2json(type->getDatatypesMap()[desc]));
            }
            w.endObject();
            break;
    }
    w.endObject();
    return w.getBytes();
}

void WorkflowDumpPlugin::sl_dumpWorkers() {
    const QMap<Descriptor, QList<ActorPrototype*> >& map = WorkflowEnv::getProtoRegistry()->getProtos();
    JsonWriter w;
    w.beginObject();
    w.name("types");
    w.beginArray();
    foreach(const DataTypePtr desc, WorkflowEnv::getDataTypeRegistry()->getAllEntries()) {
        w.beginObject()
            .name("id").value(desc->getId())
            .name("name").value(desc->getDisplayName())
            .name("desc").value(desc->getDocumentation())
        .endObject();
    }
    w.endArray();
    w.name("groups");
    w.beginArray();
    foreach(const Descriptor& desc, WorkflowEnv::getProtoRegistry()->getProtos().keys()) {
        w.beginObject()
            .name("id").value(desc.getId())
            .name("name").value(desc.getDisplayName())
            .name("desc").value(desc.getDocumentation())
        .endObject();
    }
    w.endArray();
    w.name("workers");
    w.beginArray();
    foreach(const Descriptor& desc, map.keys()) {
        foreach(const ActorPrototype* proto, map[desc]) {
            w.beginObject()
                .name("group").value(desc.getId())
                .name("id").value(proto->getId())
                .name("name").value(proto->getDisplayName())
                .name("desc").value(proto->getDocumentation())
                .name("icon").value(proto->getIconPath())
                .name("attrs").beginArray();
            foreach(const Attribute* attr, proto->getAttributes()) {
                w.beginObject()
                    .name("id").value(attr->getId())
                    .name("name").value(attr->getDisplayName())
                    .name("desc").value(attr->getDocumentation())
                    .name("type").plainValue(type2json(attr->getAttributeType()))
                    .name("value").value(attr->getAttributeValue<QString>())
                    .name("required").boolValue(attr->isRequiredAttribute())
                    .endObject();
            }
            w.endArray()
                .name("ports").beginArray();
            foreach(const PortDescriptor* port, proto->getPortDesciptors()) {
                w.beginObject()
                    .name("id").value(port->getId())
                    .name("name").value(port->getDisplayName())
                    .name("desc").value(port->getDocumentation())
                    .name("type").plainValue(type2json(port->getType()))
                    .name("input").boolValue(port->isInput())
                    .name("multi").boolValue(port->isMulti())
                    .name("flags").intValue(port->getFlags())
                    .endObject();
            }
            w.endArray()
                .endObject();
        }
    }
    w.endArray();
    w.endObject();
    QFile file("D:\\workers.dump");
    if(file.open(QFile::WriteOnly)) {
        file.write(w.getBytes());
    } else {
        printf("Can't open file to write: %s", file.fileName().toAscii().constData());
    }
    file.close();
}

}//namespace
