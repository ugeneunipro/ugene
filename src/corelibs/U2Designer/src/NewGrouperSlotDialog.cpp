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

#include <QMessageBox>
#include <QPushButton>

#include <U2Lang/BaseTypes.h>

#include "GrouperEditorWidget.h"

#include "NewGrouperSlotDialog.h"
#include <U2Gui/HelpButton.h>

namespace U2 {

NewGrouperSlotDialog::NewGrouperSlotDialog(QWidget *parent, QList<Descriptor> &inSlots, QStringList &names)
: QDialog(parent), inSlots(inSlots), names(names)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "28967130");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    foreach (const Descriptor &d, inSlots) {
        slotBox->addItem(d.getDisplayName(), d.getId());
    }
}

void NewGrouperSlotDialog::accept() {
    QString name = slotNameEdit->text();

    static const QRegExp invalidSymbols("[\\.,:;\\?@]");
    QString error;
    if (name.isEmpty()) {
        error = tr("Empty output slot name.");
    } else if (name.contains(invalidSymbols)) {
        error = tr("Invalid symbols in the output slot name. Use letters and digits only.");
    } else if (names.contains(name)) {
        error = tr("This output out slot already exists.");
    }

    if (error.isEmpty()) {
        QDialog::accept();
    } else {
        QMessageBox::critical(this, tr("Error"), error);
    }
}

QString NewGrouperSlotDialog::getInSlotId() const {
    int idx = slotBox->currentIndex();
    QVariant data = slotBox->itemData(idx);

    return data.toString();
}

QString NewGrouperSlotDialog::getOutSlotName() const {
    return slotNameEdit->text();
}

/************************************************************************/
/* Action dialogs */
/************************************************************************/
ActionDialog::ActionDialog(QWidget *parent)
: QDialog(parent)
{

}

ActionDialog *ActionDialog::getActionDialog(QWidget *parent, GrouperSlotAction *action, DataTypePtr type, QAbstractTableModel *grouperModel) {
    if (BaseTypes::DNA_SEQUENCE_TYPE() == type) {
        return new SequeceActionDialog(parent, action);
    } else if (BaseTypes::MULTIPLE_ALIGNMENT_TYPE() == type) {
        return new MsaActionDialog(parent, action);
    } else if (BaseTypes::STRING_TYPE() == type) {
        return new StringActionDialog(parent, action);
    } else if (BaseTypes::ANNOTATION_TABLE_LIST_TYPE() == type ||
        BaseTypes::ANNOTATION_TABLE_TYPE() == type) {
        GrouperSlotsCfgModel *m = dynamic_cast<GrouperSlotsCfgModel*>(grouperModel);
        assert(NULL != m);
        QStringList mergeSeqSlots = m->getMergeSeqSlotsNames();
        return new AnnsActionDialog(parent, action, mergeSeqSlots);
    }
    return NULL;
}

AnnsActionDialog::AnnsActionDialog(QWidget *parent, GrouperSlotAction *action, QStringList mergeSeqSlots)
: ActionDialog(parent)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "28967130");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    slotBox->addItem(tr("Don't shift"));
    foreach (const QString &slot, mergeSeqSlots) {
        slotBox->addItem(slot);
    }

    int idx = 0;
    if (NULL != action) {
        assert(ActionTypes::MERGE_ANNS == action->getType());
        if (action->hasParameter(ActionParameters::SEQ_SLOT)) {
            QString offsetSlot = action->getParameterValue(ActionParameters::SEQ_SLOT).toString();
            idx = mergeSeqSlots.indexOf(offsetSlot);
            if (-1 == idx) {
                idx = 0;
            } else {
                idx++;
            }
        }
        if (action->hasParameter(ActionParameters::UNIQUE)) {
            bool unique = action->getParameterValue(ActionParameters::UNIQUE).toBool();
            filterBox->setChecked(unique);
        }
    }
    slotBox->setCurrentIndex(idx);
}

GrouperSlotAction AnnsActionDialog::getAction() const {
    GrouperSlotAction action(ActionTypes::MERGE_ANNS);

    int idx = slotBox->currentIndex();
    if (idx > 0) {
        QString seqSlot = slotBox->currentText();
        action.setParameterValue(ActionParameters::SEQ_SLOT, seqSlot);
    }
    bool unique = filterBox->isChecked();
    action.setParameterValue(ActionParameters::UNIQUE, unique);

    return action;
}

SequeceActionDialog::SequeceActionDialog(QWidget *parent, GrouperSlotAction *action)
: ActionDialog(parent)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "28967130");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    if (NULL != action) {
        if (ActionTypes::MERGE_SEQUENCE == action->getType()) {
            mergeSeqButton->setChecked(true);

            if (action->hasParameter(ActionParameters::SEQ_NAME)) {
                QString seqName = action->getParameterValue(ActionParameters::SEQ_NAME).toString();
                seqNameEdit->setText(seqName);
            }
            if (action->hasParameter(ActionParameters::GAP)) {
                int gap = action->getParameterValue(ActionParameters::GAP).toInt();
                gapSpinBox->setValue(gap);
            } else {
                gapSpinBox->setValue(0);
            }
        } else if (ActionTypes::SEQUENCE_TO_MSA == action->getType()) {
            intoMsaButton->setChecked(true);

            if (action->hasParameter(ActionParameters::MSA_NAME)) {
                QString msaName = action->getParameterValue(ActionParameters::MSA_NAME).toString();
                msaNameEdit->setText(msaName);
            }
            if (action->hasParameter(ActionParameters::UNIQUE)) {
                bool unique = action->getParameterValue(ActionParameters::UNIQUE).toBool();
                filterBox->setChecked(unique);
            }
        }
    }
}

GrouperSlotAction SequeceActionDialog::getAction() const {
    if (mergeSeqButton->isChecked()) {
        GrouperSlotAction action(ActionTypes::MERGE_SEQUENCE);

        QString seqName = seqNameEdit->text();
        if (!seqName.isEmpty()) {
            action.setParameterValue(ActionParameters::SEQ_NAME, seqName);
        }

        int gap = gapSpinBox->value();
        action.setParameterValue(ActionParameters::GAP, gap);

        return action;
    } else {
        GrouperSlotAction action(ActionTypes::SEQUENCE_TO_MSA);

        QString msaName = msaNameEdit->text();
        if (!msaName.isEmpty()) {
            action.setParameterValue(ActionParameters::MSA_NAME, msaName);
        }

        bool unique = filterBox->isChecked();
        action.setParameterValue(ActionParameters::UNIQUE, unique);

        return action;
    }
}

MsaActionDialog::MsaActionDialog(QWidget *parent, GrouperSlotAction *action)
: ActionDialog(parent)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "28967130");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    if (NULL != action) {
        assert(ActionTypes::MERGE_MSA == action->getType());
        if (action->hasParameter(ActionParameters::MSA_NAME)) {
            QString msaName = action->getParameterValue(ActionParameters::MSA_NAME).toString();
            msaNameEdit->setText(msaName);
        }
        if (action->hasParameter(ActionParameters::UNIQUE)) {
            bool unique = action->getParameterValue(ActionParameters::UNIQUE).toBool();
            filterBox->setChecked(unique);
        }
    }
}

GrouperSlotAction MsaActionDialog::getAction() const {
    GrouperSlotAction action(ActionTypes::MERGE_MSA);

    QString msaName = msaNameEdit->text();
    if (!msaName.isEmpty()) {
        action.setParameterValue(ActionParameters::MSA_NAME, msaName);
    }

    bool unique = filterBox->isChecked();
    action.setParameterValue(ActionParameters::UNIQUE, unique);

    return action;
}

StringActionDialog::StringActionDialog(QWidget *parent, GrouperSlotAction *action)
: ActionDialog(parent)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "28967130");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    if (NULL != action) {
        assert(ActionTypes::MERGE_STRING == action->getType());
        if (action->hasParameter(ActionParameters::SEPARATOR)) {
            QString sep = action->getParameterValue(ActionParameters::SEPARATOR).toString();
            separatorEdit->setText(sep);
        }
    }
}

GrouperSlotAction StringActionDialog::getAction() const {
    GrouperSlotAction action(ActionTypes::MERGE_STRING);

    QString sep = separatorEdit->text();
    if (!sep.isEmpty()) {
        action.setParameterValue(ActionParameters::SEPARATOR, sep);
    }

    return action;
}

} // U2
