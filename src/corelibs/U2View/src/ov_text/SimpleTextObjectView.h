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

#pragma once

#include <QPlainTextEdit>

#include <U2Core/TextSelection.h>
#include <U2Core/global.h>

#include <U2Gui/ObjectViewModel.h>

namespace U2 {

class OpenSimpleTextObjectViewTask;

class U2VIEW_EXPORT SimpleTextObjectViewFactory : public GObjectViewFactory {
    Q_OBJECT
public:
    SimpleTextObjectViewFactory(QObject* p = nullptr);

    static const GObjectViewFactoryId ID;

    bool canCreateView(const MultiGSelection& multiSelection) override;

    bool isStateInSelection(const MultiGSelection& multiSelection, const QVariantMap& stateData) override;

    Task* createViewTask(const MultiGSelection& multiSelection, bool single = false) override;

    Task* createViewTask(const QString& viewName, const QVariantMap& state) override;

    bool supportsSavedStates() const override {
        return true;
    }
};

class U2VIEW_EXPORT SimpleTextObjectView : public GObjectViewController {
    Q_OBJECT

    friend class SimpleTextObjectViewFactory;

public:
    SimpleTextObjectView(const QString& name, TextObject* to, const QVariantMap& state);

    QVariantMap saveState() override;

    Task* updateViewTask(const QString& stateName, const QVariantMap& stateData) override;

    virtual const TextSelection& getSelectedText() {
        return selection;
    }

    QWidget* createViewWidget(QWidget* parent) override;

    void updateView(const QVariantMap& stateData);

    // saved state accessors -> todo: extract into separate model class
    static QString getDocumentUrl(const QVariantMap& savedState);
    static QString getObjectName(const QVariantMap& savedState);

    static void setDocumentUrl(QVariantMap& savedState, const QString& url);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void sl_onTextEditTextChanged();
    void sl_onTextObjStateLockChanged();

private:
    TextObject* textObject;
    QVariantMap openState;
    TextSelection selection;
    QPlainTextEdit* textEdit;
    bool firstShow;
};

}  // namespace U2
