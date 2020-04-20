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

#ifndef _U2_SNPEFF_DATABASE_DELEGATE_H_
#define _U2_SNPEFF_DATABASE_DELEGATE_H_

#include <U2Lang/ConfigurationEditor.h>

#include <QDialog>
#include <QLineEdit>
#include <QToolButton>

#include "ui_SnpEffDatabaseDialog.h"

class QSortFilterProxyModel;

namespace U2 {
namespace LocalWorkflow {

/************************************************************************/
/* SnpEffDatabaseDialog */
/************************************************************************/
class SnpEffDatabaseDialog : public QDialog, public Ui_SnpEffDatabaseDialog {
    Q_OBJECT
public:
    SnpEffDatabaseDialog(QWidget* parent = 0);
    QString getDatabase() const;
private slots:
    void sl_selectionChanged();
private:
    QSortFilterProxyModel* proxyModel;
};

/************************************************************************/
/* SnpEffDatabasePropertyWidget */
/************************************************************************/
class SnpEffDatabasePropertyWidget : public PropertyWidget {
    Q_OBJECT
public:
    SnpEffDatabasePropertyWidget(QWidget *parent = NULL, DelegateTags *tags = NULL);
    virtual QVariant value();

public slots:
    virtual void setValue(const QVariant &value);

    void sl_showDialog();

private:
    QLineEdit*      lineEdit;
    QToolButton*    toolButton;
};

/************************************************************************/
/* SnpEffDatabaseDelegate */
/************************************************************************/
class SnpEffDatabaseDelegate : public PropertyDelegate {
    Q_OBJECT
public:
    SnpEffDatabaseDelegate(QObject *parent = 0);

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const;
    virtual PropertyWidget * createWizardWidget(U2OpStatus &os, QWidget *parent) const;

    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model,
        const QModelIndex &index) const;

    virtual PropertyDelegate *clone();

private slots:
    void sl_commit();
};

} // namespace LocalWorkflow
} // namespace U2

#endif // _U2_SNPEFF_DATABASE_DELEGATE_H_
