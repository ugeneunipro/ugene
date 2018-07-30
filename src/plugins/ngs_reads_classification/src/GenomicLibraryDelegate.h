/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#ifndef _U2_GENOMIC_LIBRARY_DELEGATE_H_
#define _U2_GENOMIC_LIBRARY_DELEGATE_H_

#include <U2Designer/DelegateEditors.h>

#include "NgsReadsClassificationPlugin.h"

namespace U2 {
namespace LocalWorkflow {

class U2NGS_READS_CLASSIFICATION_EXPORT GenomicLibraryDelegate : public PropertyDelegate {
    Q_OBJECT
public:
    GenomicLibraryDelegate(QObject *parent = 0);

    QVariant getDisplayValue(const QVariant &value) const;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    PropertyWidget *createWizardWidget(U2OpStatus &os, QWidget *parent) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    PropertyDelegate *clone();

private slots:
    void sl_commit();
};

}   // namespace LocalWorkflow
}   // namespace U2

#endif // _U2_GENOMIC_LIBRARY_DELEGATE_H_
