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

#include "InsertEnzymeWidget.h"

#include "EnzymesIO.h"

#include <U2Core/U2SafePoints.h>

#include <QKeyEvent>

#include <algorithm>

namespace U2 {

InsertEnzymeWidget::InsertEnzymeWidget(QWidget* parent, const DNAAlphabet* _alphabet)
    : QWidget(parent), alphabet(_alphabet) {
    setupUi(this);

    updateEnzymesList(false);

    connect(cbShowEnzymesWithUndefinedShuppliers, &QCheckBox::stateChanged, this, [this](bool state) {
        updateEnzymesList(state);
    });

    connect(cbChooseEnzyme, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int /*index*/) {
        auto enzyme = cbChooseEnzyme->currentData().value<SEnzymeData>();
        CHECK(enzyme != nullptr, );

        teChoosedEnzymeInfo->setText(enzyme->generateEnzymeTooltip());
    });

    cbChooseEnzyme->setCurrentIndex(-1);

}

QString InsertEnzymeWidget::getEnzymeSequence() const {
    auto text = cbChooseEnzyme->currentText();
    CHECK(items.contains(text), QString());

    auto enzyme = cbChooseEnzyme->currentData().value<SEnzymeData>();
    SAFE_POINT_NN(enzyme, QString());

    return enzyme->seq;
}

void InsertEnzymeWidget::updateEnzymesList(bool showEnzymesWithUndefinedSuppliers) {
    auto enzymeList = EnzymesIO::getDefaultEnzymesList();
    cbChooseEnzyme->clear();
    items.clear();
    for (const auto& enzyme : qAsConst(enzymeList)) {
        static const QString notDefinedTr = EnzymesIO::tr(EnzymesIO::NOT_DEFINED_SIGN);
        CHECK_CONTINUE(enzyme->suppliers.contains(notDefinedTr) == showEnzymesWithUndefinedSuppliers);
        CHECK_CONTINUE(U2AlphabetUtils::matches(alphabet, enzyme->seq, enzyme->seq.size()));

        cbChooseEnzyme->addItem(enzyme->id, QVariant::fromValue(enzyme));
        items << enzyme->id;
    }

    lbEnzymesNumber->setText(tr("%n enzyme(s)", "", items.size()));
}

}  // namespace U2
