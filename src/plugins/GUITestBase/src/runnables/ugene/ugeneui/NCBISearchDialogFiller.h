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

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

class NcbiSearchDialogFiller : public Filler {
public:
    enum ActionType {  // An appropriate action data (as QVariant)
        SetField,  // QPair<int, QString> with a number of block and the combobox visible text
        SetTerm,  // QPair<int, QString> with a number of block and a query term
        AddTerm,  // ignored
        RemoveTerm,  // int with a 0-based number of term (including the primary term, the primary term cannot be removed)
        SetDatabase,  // QString with the combobox value
        CheckQuery,  // QString with the expected query
        ClickResultByNum,  // int with a result's number (0-based)
        ClickResultById,  // QString with a result's ID
        ClickResultByDesc,  // QString with a result's description (exact case-sensitive comparison)
        ClickResultBySize,  // int with a result's size (the first results with this size will be clicked)
        SelectResultsByNumbers,  // QList<int> with results numbers (0-based)
        SelectResultsByIds,  // QStringList with results IDs
        SelectResultsByDescs,  // QStringList with results descriptions (exact case sensitive comparison)
        SelectResultsBySizes,  // QList<int> with results sizes
        SetResultLimit,  // int with the limit
        ClickSearch,  // ignored
        ClickDownload,  // QList with actions for the "Fetch from remote database" dialog filler
        ClickClose,  // ignored
        WaitTasksFinish  // ignored
    };
    typedef QPair<ActionType, QVariant> Action;

    NcbiSearchDialogFiller(const QList<Action>& actions);

    void commonScenario() override;

private:
    void setField(const QVariant& actionData);
    void setTerm(const QVariant& actionData);
    void addTerm();
    void removeTerm(const QVariant& actionData);
    void setDatabase(const QVariant& actionData);
    void checkQuery(const QVariant& actionData);
    void clickResultByNum(const QVariant& actionData);
    void clickResultById(const QVariant& actionData);
    void clickResultByDesc(const QVariant& actionData);
    void clickResultBySize(const QVariant& actionData);
    void selectResultsByNumbers(const QVariant& actionData);
    void selectResultsByIds(const QVariant& actionData);
    void selectResultsByDescs(const QVariant& actionData);
    void selectResultsBySizes(const QVariant& actionData);
    void setResultLimit(const QVariant& actionData);
    void clickSearch();
    void clickDownload(const QVariant& actionData);
    void clickClose();
    void waitTasksFinish();

    QWidget* dialog = nullptr;
    QList<Action> actions;
};

class NCBISearchDialogSimpleFiller : public Filler {
public:
    NCBISearchDialogSimpleFiller(
        const QString& query,
        bool doubleEnter = false,
        int _resultLimit = -1,
        const QString& term = "",
        const QString& resultToLoad = "");

    void commonScenario() override;

private:
    QString query;

    bool doubleEnter = false;
    int resultLimit = -1;
    QString term;

    /** If not empty the filler will look for this result (Qt::MatchContains) and load it. */
    QString resultToLoad;

    int getResultNumber();
};

}  // namespace U2

typedef QPair<int, QString> intStrStrPair;
Q_DECLARE_METATYPE(intStrStrPair)
Q_DECLARE_METATYPE(QList<int>)
