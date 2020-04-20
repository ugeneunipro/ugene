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

#ifndef _U2_SEARCH_QUALIFIER_DIALOG_H_
#define _U2_SEARCH_QUALIFIER_DIALOG_H_

#include <QDialog>

#include <U2Core/global.h>

class Ui_SearchQualifierDialog;

namespace U2 {

class AnnotationsTreeView;
class AVItem;

class U2VIEW_EXPORT SearchQualifierDialog: public QDialog {
    Q_OBJECT
public:
    SearchQualifierDialog(QWidget* p, AnnotationsTreeView *treeView);
    ~SearchQualifierDialog();

protected:
    bool eventFilter(QObject *obj, QEvent *e);
    void clearPrevResults();

protected slots:
    void sl_searchTaskStateChanged();
    void sl_searchNext();
    void sl_searchAll();

private slots:
    void sl_onSearchSettingsChanged();

private:
    void search(bool searchAll = false);

    AnnotationsTreeView * treeView;
    Ui_SearchQualifierDialog* ui;
    AVItem* groupToSearchIn;
    AVItem * parentAnnotationofPrevResult;
    int indexOfPrevResult;
};

}   // namespace U2

#endif //_U2_SEARCH_QUALIFIER_DIALOG_H_
