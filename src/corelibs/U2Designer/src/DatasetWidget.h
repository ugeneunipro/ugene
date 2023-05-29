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

#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2OpStatus.h>

#include "UrlItem.h"

class QVBoxLayout;
class Ui_DatasetWidget;

namespace U2 {

class OptionsPopup;
class URLListController;

class U2DESIGNER_EXPORT URLListWidget : public QWidget {
    Q_OBJECT
public:
    URLListWidget(URLListController* ctrl);
    ~URLListWidget() override;

    void addUrlItem(UrlItem* urlItem);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void sl_addFileButton();
    void sl_addDirButton();
    void sl_downButton();
    void sl_upButton();
    void sl_deleteButton();
    void sl_itemChecked();
    void sl_selectAll();
    void sl_dataChanged();

private:
    void addUrl(const QString& url);
    void reset();

private:
    Ui_DatasetWidget* ui;
    URLListController* ctrl;
    OptionsPopup* popup;
};

class OptionsPopup : public QFrame {
public:
    OptionsPopup(QWidget* parent);

    void showOptions(QWidget* options, const QPoint& pos);
    void hideOptions();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void removeOptions();

private:
    QVBoxLayout* l;
};

}  // namespace U2
