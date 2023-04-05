/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "SimpleTextObjectView.h"

#include <QScrollBar>
#include <QVBoxLayout>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/SelectionUtils.h>
#include <U2Core/TextObject.h>

#include "SimpleTextObjectViewTasks.h"

namespace U2 {

/* TRANSLATOR U2::SimpleTextObjectViewFactory */

//////////////////////////////////////////////////////////////////////////
/// Factory
const GObjectViewFactoryId SimpleTextObjectViewFactory::ID(GObjectViewFactory::SIMPLE_TEXT_FACTORY);

SimpleTextObjectViewFactory::SimpleTextObjectViewFactory(QObject* p)
    : GObjectViewFactory(ID, tr("Text editor"), p) {
}

bool SimpleTextObjectViewFactory::canCreateView(const MultiGSelection& multiSelection) {
    bool hasTextDocuments = !SelectionUtils::findDocumentsWithObjects(GObjectTypes::TEXT, &multiSelection, UOF_LoadedAndUnloaded, true).isEmpty();
    return hasTextDocuments;
}

bool SimpleTextObjectViewFactory::isStateInSelection(const MultiGSelection& multiSelection, const QVariantMap& stateData) {
    QString documentURL = SimpleTextObjectView::getDocumentUrl(stateData);
    QSet<Document*> documents = SelectionUtils::findDocumentsWithObjects(GObjectTypes::TEXT, &multiSelection, UOF_LoadedAndUnloaded, true);
    foreach (Document* doc, documents) {
        if (doc->getURL() == documentURL) {
            return true;
        }
    }
    return false;
}

Task* SimpleTextObjectViewFactory::createViewTask(const MultiGSelection& multiSelection, bool single) {
    const QList<GObject*> objects = SelectionUtils::findObjects(GObjectTypes::TEXT, &multiSelection, UOF_LoadedAndUnloaded);
    if (objects.isEmpty()) {
        return nullptr;
    }

    Task* result = (single || objects.size() == 1) ? nullptr : new Task(tr("Open multiple views task"), TaskFlag_NoRun);
    Task* t = new OpenSimpleTextObjectViewTask(objects);

    if (result == nullptr) {
        return t;
    } else {
        // todo: limit number of views?
        result->addSubTask(t);
    }

    return result;
}

Task* SimpleTextObjectViewFactory::createViewTask(const QString& viewName, const QVariantMap& state) {
    return new OpenSavedTextObjectViewTask(viewName, state);
}

//////////////////////////////////////////////////////////////////////////
/// Simple Text View

SimpleTextObjectView::SimpleTextObjectView(const QString& name, TextObject* to, const QVariantMap& _state)
    : GObjectViewController(SimpleTextObjectViewFactory::ID, name), textObject(to), openState(_state), selection(to) {
    GCOUNTER(cvar, "SimpleTextView");
    textEdit = nullptr;
    firstShow = true;
    assert(to);
    objects.append(to);
    requiredObjects.append(to);
}

QWidget* SimpleTextObjectView::createViewWidget(QWidget* parent) {
    SAFE_POINT(textEdit == nullptr, "Widget is already created", textEdit);
    textEdit = new QPlainTextEdit(parent);
    textEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
    textEdit->setWordWrapMode(QTextOption::NoWrap);
    try {
        textEdit->setPlainText(textObject->getText());
    } catch (std::bad_alloc&) {
        coreLog.error("Not enough memory for loading text data");
        return nullptr;
    }
    if (textObject->isStateLocked()) {
        textEdit->setReadOnly(true);
    }
    connect(textEdit, SIGNAL(textChanged()), SLOT(sl_onTextEditTextChanged()));
    connect(textObject, SIGNAL(si_lockedStateChanged()), SLOT(sl_onTextObjStateLockChanged()));
    textEdit->installEventFilter(this);
    textEdit->setWindowIcon(GObjectTypes::getTypeInfo(GObjectTypes::TEXT).icon);
    return textEdit;
}

bool SimpleTextObjectView::eventFilter(QObject* o, QEvent* e) {
    if (firstShow && o == textEdit && e->type() == QEvent::ShowToParent) {
        firstShow = false;
        updateView(openState);
    }
    return false;
}

void SimpleTextObjectView::sl_onTextEditTextChanged() {
    textObject->setText(textEdit->toPlainText());
}

void SimpleTextObjectView::sl_onTextObjStateLockChanged() {
    textEdit->setReadOnly(textObject->isStateLocked());
}

Task* SimpleTextObjectView::updateViewTask(const QString& stateName, const QVariantMap& state) {
    return new UpdateSimpleTextObjectViewTask(this, stateName, state);
}

#define URL_KEY "url"
#define OBJ_KEY "obj"
#define CURS_POS_KEY "cursor_pos"
#define HBAR_POS_KEY "hbar_pos"
#define VBAR_POS_KEY "vbar_pos"

void SimpleTextObjectView::updateView(const QVariantMap& data) {
    int cursPos = data.value(CURS_POS_KEY, 0).toInt();
    int hScrollPos = data.value(HBAR_POS_KEY, 0).toInt();
    int vScrollPos = data.value(VBAR_POS_KEY, 0).toInt();

    QTextCursor c = textEdit->textCursor();
    c.setPosition(cursPos);  //, QTextCursor::KeepAnchor
    textEdit->setTextCursor(c);
    textEdit->verticalScrollBar()->setSliderPosition(vScrollPos);
    textEdit->horizontalScrollBar()->setSliderPosition(hScrollPos);
}

QVariantMap SimpleTextObjectView::saveState() {
    QVariantMap data;
    // Content:
    // file url
    data[URL_KEY] = textObject->getDocument()->getURLString();
    data[OBJ_KEY] = textObject->getGObjectName();
    // GUI:
    // start vis pos
    data[CURS_POS_KEY] = textEdit->textCursor().position();
    data[HBAR_POS_KEY] = textEdit->horizontalScrollBar()->sliderPosition();
    data[VBAR_POS_KEY] = textEdit->verticalScrollBar()->sliderPosition();
    return data;
}

QString SimpleTextObjectView::getDocumentUrl(const QVariantMap& savedState) {
    return savedState.value(URL_KEY).toString();
}

void SimpleTextObjectView::setDocumentUrl(QVariantMap& savedState, const QString& url) {
    savedState[URL_KEY] = url;
}

QString SimpleTextObjectView::getObjectName(const QVariantMap& savedState) {
    return savedState.value(OBJ_KEY).toString();
}

}  // namespace U2
