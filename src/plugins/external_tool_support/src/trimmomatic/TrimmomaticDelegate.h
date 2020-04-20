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

#ifndef _U2_TRIMMOMATIC_DELEGATE_H_
#define _U2_TRIMMOMATIC_DELEGATE_H_

#include <QStringListModel>

#include <U2Designer/DelegateEditors.h>

#include <U2Gui/SaveDocumentController.h>

#include "TrimmomaticStep.h"
#include "ui_TrimmomaticPropertyDialog.h"

namespace U2 {
namespace LocalWorkflow {

class TrimmomaticDelegate : public PropertyDelegate {
    Q_OBJECT
public:
    TrimmomaticDelegate(QObject *parent = 0);

    QVariant getDisplayValue(const QVariant &value) const;
    PropertyDelegate* clone();
    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    PropertyWidget *createWizardWidget(U2OpStatus &os,
                                                  QWidget *parent) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                                       const QModelIndex &index) const;

private slots:
    void sl_commit();
};

class TrimmomaticPropertyWidget : public PropertyWidget {
    Q_OBJECT
public:
    TrimmomaticPropertyWidget(QWidget* parent = NULL, DelegateTags* tags = NULL);

    QVariant value();

public slots:
    void setValue(const QVariant& value);

private slots:
    void sl_textEdited();
    void sl_showDialog();

private:
    QLineEdit *lineEdit;
    QToolButton *toolButton;
};

class TrimmomaticPropertyDialog : public QDialog, private Ui_TrimmomaticPropertyDialog {
    Q_OBJECT
public:
    TrimmomaticPropertyDialog(const QString &value, QWidget *parent);

    QString getValue() const;

private slots:
    void sl_currentRowChanged();
    void sl_addStep(QAction* a);
    void sl_moveStepUp();
    void sl_moveStepDown();
    void sl_removeStep();
    void sl_valuesChanged();

private:
    void emptySelection();
    void enableButtons(bool setEnabled);
    static QString defaultDir();

    void addStep(TrimmomaticStep *step);
    void parseCommand(const QString &command);

    QList<TrimmomaticStep *> steps;
    QWidget* currentWidget;
    QWidget *defaultSettingsWidget;
    QMenu *menu;

    static const QString DEFAULT_DESCRIPTION;
    static const QString DEFAULT_SETTINGS_TEXT;
};

}// namespace LocalWorkflow
}// namespace U2

#endif // _U2_TRIMMOMATIC_DELEGATE_H_
