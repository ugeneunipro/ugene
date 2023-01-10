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

#include "GraphSettingsDialog.h"

#include <QColorDialog>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QProxyStyle>
#include <QPushButton>
#include <QStyleFactory>

#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>

#include "ADVGraphModel.h"
#include "WindowStepSelectorWidget.h"

namespace U2 {

namespace {

void setButtonColor(QPushButton* button, const QColor& color) {
    QPalette palette = button->palette();
    palette.setColor(button->backgroundRole(), color);
    button->setPalette(palette);
}

}  // namespace

GraphSettingsDialog::GraphSettingsDialog(GSequenceGraphDrawer* d, const U2Region& range, QWidget* parent)
    : QDialog(parent), colorMap(d->getColors()) {
    const GSequenceGraphMinMaxCutOffState& cutOffData = d->getCutOffState();
    wss = new WindowStepSelectorWidget(this, range, d->getWindow(), d->getStep());
    mms = new MinMaxSelectorWidget(this, cutOffData.min, cutOffData.max, cutOffData.isEnabled);

    QFormLayout* form = wss->getFormLayout();
    foreach (const QString& key, colorMap.keys()) {
        QPushButton* colorChangeButton = new QPushButton();
        colorChangeButton->setObjectName(key);
        connect(colorChangeButton, SIGNAL(clicked()), SLOT(sl_onPickColorButtonClicked()));
        QColor color = colorMap.value(key);

        QStyle* buttonStyle = new QProxyStyle(QStyleFactory::create("fusion"));
        buttonStyle->setParent(this);
        colorChangeButton->setStyle(buttonStyle);

        setButtonColor(colorChangeButton, color);

        form->addRow(QString("%1:").arg(key), colorChangeButton);
    }

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    buttonBox->setObjectName("buttonBox");

    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch(10);
    buttonsLayout->addWidget(buttonBox);

    QVBoxLayout* l = new QVBoxLayout();
    l->setSizeConstraint(QLayout::SetFixedSize);
    l->addWidget(wss);
    l->addWidget(mms);
    l->addLayout(buttonsLayout);

    setLayout(l);
    setWindowTitle(tr("Graph Settings"));
    setWindowIcon(QIcon(":core/images/graphs.png"));

    QPushButton* okButton = buttonBox->button(QDialogButtonBox::Ok);
    QPushButton* cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
    new HelpButton(this, buttonBox, "65929576");

    connect(cancelButton, SIGNAL(clicked()), SLOT(sl_onCancelClicked()));
    connect(okButton, SIGNAL(clicked()), SLOT(sl_onOkClicked()));

    okButton->setDefault(true);
    setObjectName("GraphSettingsDialog");
}

void GraphSettingsDialog::sl_onPickColorButtonClicked() {
    QPushButton* colorButton = qobject_cast<QPushButton*>(sender());
    SAFE_POINT(colorButton, "Button for color is NULL", );

    QString colorName = colorButton->objectName();
    QColor initial = colorMap.value(colorName);

    QObjectScopedPointer<QColorDialog> colorDialog = new QColorDialog(initial, this);

    // Disable use of native dialog here.
    // Reason: the native dialog will not be shown (tested on Ubuntu 20.04) if we have at least 1 graph-label visible (QT bug?).
    // The problem is caused by GSequenceGraphDrawer::draw() method that calls graphLabel->setVisible().
    // One possible solution here could be an optimization of GSequenceGraphViewRA::drawAll method: avoid full redraw when it is not needed.
    // Another solution is to make 'TextLabel' not a QLabel (which causes the problem), but a QWidget that draws the label text manually.
    colorDialog->setOption(QColorDialog::DontUseNativeDialog, true);
    colorDialog->exec();
    CHECK(!colorDialog.isNull(), );

    if (colorDialog->result() == QDialog::Accepted) {
        QColor newColor = colorDialog->selectedColor();
        colorMap[colorName] = newColor;
        setButtonColor(colorButton, newColor);
    }
}

void GraphSettingsDialog::sl_onCancelClicked() {
    reject();
}

void GraphSettingsDialog::sl_onOkClicked() {
    QString err = wss->validate();
    QString mmerr = mms->validate();
    if (err.isEmpty() && mmerr.isEmpty()) {
        accept();
        return;
    }
    QMessageBox::critical(this, windowTitle(), err.append(' ').append(mmerr));
}

}  // namespace U2
