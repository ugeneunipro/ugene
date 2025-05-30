/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <QLineEdit>

#include <U2Core/global.h>

class QTreeWidget;

namespace U2 {

class U2GUI_EXPORT CompletionFiller {
public:
    virtual ~CompletionFiller() = default;
    virtual QStringList getSuggestions(const QString& str) = 0;
    virtual QString handleNewUrlInput(const QString& editorText, const QString& suggestion) = 0;
};

class U2GUI_EXPORT MSACompletionFiller : public CompletionFiller {
public:
    MSACompletionFiller()
        : CompletionFiller(), seqNameList(QStringList()), defaultValue("") {};
    MSACompletionFiller(QStringList& _seqNameList, const QString& defVal = "")
        : CompletionFiller(), seqNameList(_seqNameList), defaultValue(defVal) {};

    QStringList getSuggestions(const QString& str) override;
    void updateSeqList(const QStringList& list) {
        seqNameList = list;
    };
    QString handleNewUrlInput(const QString& editorText, const QString& suggestion) override;

private:
    QStringList seqNameList;
    const QString defaultValue;
};

class U2GUI_EXPORT BaseCompleter : public QObject {
    Q_OBJECT
public:
    BaseCompleter(CompletionFiller* filler, QLineEdit* parent = nullptr);
    ~BaseCompleter() override;
    bool eventFilter(QObject* obj, QEvent* ev) override;
    void showCompletion(const QStringList& choices);
    int getLastChosenItemIndex() const;

signals:
    void si_editingFinished();
    void si_completerClosed();

protected slots:
    void doneCompletion();
    void sl_textChanged(const QString&);

private:
    CompletionFiller* filler;
    QLineEdit* editor;
    QTreeWidget* popup;
    int lastChosenItemIndex;
};

}  // namespace U2
