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

#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QVBoxLayout>

#include <U2Core/U2SafePoints.h>

#include <U2Gui/U2FileDialog.h>

#include <U2Lang/WorkflowSettings.h>

#include "OutputDirectoryWidget.h"

namespace U2 {

OutputDirectoryWidget::OutputDirectoryWidget(QWidget *parent, bool commitOnHide)
: QWidget(parent), commitOnHide(commitOnHide)
{
    QVBoxLayout *l = new QVBoxLayout(this);
    l->setContentsMargins(3, 3, 3, 3);
    label = new QLabel(tr(
                           "The Workflow Output Folder is a common folder that is used to store all output files in the Workflow Designer."
                           " A separate subdirectory of the folder is created for each run of a workflow."
                           "\n\nSet up the folder:"
                           ), this);
    label->setAlignment(Qt::AlignJustify | Qt::AlignVCenter);
    label->setWordWrap(true);
    label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    l->addWidget(label);
    QHBoxLayout *hl = new QHBoxLayout();
    hl->setContentsMargins(0, 0, 0, 0);
    pathEdit = new QLineEdit(this);
    pathEdit->setObjectName("pathEdit");
    browseButton = new QToolButton(this);
    browseButton->setText("...");
    hl->addWidget(pathEdit);
    hl->addWidget(browseButton);

    l->addLayout(hl);

    connect(browseButton, SIGNAL(clicked()), SLOT(sl_browse()));
    pathEdit->setText(WorkflowSettings::getWorkflowOutputDirectory());
}

void OutputDirectoryWidget::sl_browse() {
    QString dir = U2FileDialog::getExistingDirectory(this, tr("Select a folder"), pathEdit->text());

    if(!dir.isEmpty()) {
        dir = QDir::toNativeSeparators(dir);
        if (!dir.endsWith(QDir::separator())) {
            dir += QDir::separator();
        }
        pathEdit->setText(dir);
        WorkflowSettings::setWorkflowOutputDirectory(dir);
    }
    emit si_browsed();
}

void OutputDirectoryWidget::commit() {
    WorkflowSettings::setWorkflowOutputDirectory(pathEdit->text());
}

void OutputDirectoryWidget::hideEvent(QHideEvent *event) {
    if (commitOnHide) {
        commit();
    }
    QWidget::hideEvent(event);
}

} // U2
