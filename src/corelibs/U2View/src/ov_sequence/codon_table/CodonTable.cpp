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

#include "CodonTable.h"

#include <QApplication>
#include <QHeaderView>
#include <QScrollArea>
#include <QScrollBar>
#include <QSizePolicy>
#include <QTableWidget>
#include <QUrl>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/U2SafePoints.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>
namespace U2 {

const QColor CodonTableView::NONPOLAR_COLOR = QColor("#FFEE00").lighter();
const QColor CodonTableView::POLAR_COLOR = QColor("#3DF490").lighter();
const QColor CodonTableView::BASIC_COLOR = QColor("#FF5082").lighter();
const QColor CodonTableView::ACIDIC_COLOR = QColor("#00ABED").lighter();
const QColor CodonTableView::STOP_CODON_COLOR = QColor(Qt::gray);

// CodonTableView
CodonTableView::CodonTableView(AnnotatedDNAView* view)
    : ADVSplitWidget(view) {
    table = new QTableWidget(18, 10);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setObjectName("Codon table widget");

    table->horizontalHeader()->hide();
    table->verticalHeader()->hide();

    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    int rowHeight = 18;
    table->verticalHeader()->setDefaultSectionSize(rowHeight);

    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(9, QHeaderView::ResizeToContents);

    // Fill the table
    QStringList nucleicBases = {"U", "C", "A", "G"};
    addItemToTable(0, 0, tr("1st base"), 2, 1);
    addItemToTable(0, 1, tr("2nd base"), 1, 8);
    addItemToTable(0, 9, tr("3rd base"), 2, 1);
    for (int i = 0; i < 4; i++) {
        // 1 column
        addItemToTable(2 + i * 4, 0, nucleicBases[i], 4, 1);
        // 2 row
        addItemToTable(1, 1 + 2 * i, nucleicBases[i], 1, 2);
        for (int j = 0; j < 4; j++) {
            // last column
            addItemToTable(2 + i * 4 + j, 9, nucleicBases[j], 1, 1);
            for (int k = 0; k < 4; k++) {
                // codon variations
                addItemToTable(2 + i * 4 + k, 1 + j * 2, nucleicBases[i] + nucleicBases[j] + nucleicBases[k], 1, 1);
            }
        }
    }

    auto l = new QVBoxLayout(this);
    l->addWidget(table);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);

    setMaximumHeight(rowHeight * 18 + 10);
    setMinimumHeight(70);

    setLayout(l);
    setVisible(false);

    QList<ADVSequenceObjectContext*> list = view->getSequenceContexts();
    foreach (ADVSequenceObjectContext* ctx, list) {
        // find first with translation table
        if (ctx->getAminoTT() != nullptr) {
            setAminoTranslation(ctx->getAminoTT()->getTranslationId());
            return;
        }
    }
    // set standard genetic code table
    setAminoTranslation(DNATranslationID(1));
}

void CodonTableView::sl_setVisible() {
    GCOUNTER(cvar, "CodonTableView");
    setVisible(!isVisible());
}

void CodonTableView::sl_setAminoTranslation() {
    auto a = qobject_cast<QAction*>(sender());
    if (a != nullptr) {
        QString tid = a->data().toString();
        setAminoTranslation(tid);
    }
}

void CodonTableView::sl_onActiveSequenceChanged(ADVSequenceWidget* /*from*/, ADVSequenceWidget* to) {
    CHECK(to != nullptr, );

    const QList<ADVSequenceObjectContext*> ctx = to->getSequenceContexts();
    CHECK(ctx.first() != nullptr, );
    CHECK(ctx.first()->getAminoTT() != nullptr, );

    setAminoTranslation(ctx.first()->getAminoTT()->getTranslationId());
}

void CodonTableView::setAminoTranslation(const QString& trId) {
    DNATranslationRegistry* trReg = AppContext::getDNATranslationRegistry();
    SAFE_POINT(trReg != nullptr, "DNATranslationRegistry is NULL!", );

    DNAAlphabetRegistry* alphReg = AppContext::getDNAAlphabetRegistry();
    SAFE_POINT(alphReg != nullptr, "DNAAlphabetRegistry is NULL!", );
    const DNAAlphabet* alph = alphReg->findById(BaseDNAAlphabetIds::NUCL_RNA_DEFAULT());
    SAFE_POINT(alph != nullptr, "Standard RNA alphabet not found!", );

    DNATranslation* tr = trReg->lookupTranslation(alph, trId);
    SAFE_POINT(tr != nullptr, "No translation found!", );

    QList<char> nucleobases;
    nucleobases << 'U' << 'C' << 'A' << 'G';
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                char codon = tr->translate3to1(nucleobases[i], nucleobases[j], nucleobases[k]);
                addItemToTable(2 + i * 4 + k, 2 + j * 2, trReg->lookupCodon(codon));
            }
        }
    }
    spanEqualCells();
    table->resize(table->width() - 1, table->height());
    table->updateGeometry();
};

void CodonTableView::spanEqualCells() {
    table->clearSpans();
    table->setSpan(0, 0, 2, 1);
    table->setSpan(0, 1, 1, 8);
    table->setSpan(0, 9, 2, 1);
    for (int i = 0; i < 4; i++) {
        table->setSpan(2 + i * 4, 0, 4, 1);
        table->setSpan(1, 1 + 2 * i, 1, 2);
        int rowSpan = 1;
        for (int j = 2; j < table->rowCount() - 1; j++) {
            if ((table->cellWidget(j, 2 + i * 2) == nullptr) || (table->cellWidget(j + 1, 2 + i * 2) == nullptr)) {
                continue;
            }

            if (table->cellWidget(j, 2 + i * 2)->objectName() == table->cellWidget(j + 1, 2 + i * 2)->objectName()) {
                rowSpan++;
            } else {
                if (rowSpan != 1) {
                    table->setSpan(j - rowSpan + 1, 2 + i * 2, rowSpan, 1);
                }
                rowSpan = 1;
            }
        }
        if (rowSpan != 1) {
            table->setSpan(17 - rowSpan + 1, 2 + i * 2, rowSpan, 1);
        }
    }
}

void CodonTableView::addItemToTable(int row, int column, const QString& text, const QColor& backgroundColor, int rowSpan, int columnSpan) {
    auto item = new QTableWidgetItem(text);
    QFont font = item->font();
    font.setPointSize(10);
    item->setFont(font);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    item->setBackground(backgroundColor);
    item->setTextAlignment(Qt::AlignCenter);
    if (rowSpan != 1 || columnSpan != 1) {
        table->setSpan(row, column, rowSpan, columnSpan);
    }
    table->setItem(row, column, item);
}

void CodonTableView::addItemToTable(int row, int column, const QString& text, int rowSpan, int columnSpan) {
    addItemToTable(row, column, text, QColor(0, 0, 0, 0), rowSpan, columnSpan);
}

void CodonTableView::addItemToTable(int row, int column, const QString& text, const QColor& backgroundColor, const QString& link, int rowSpan, int columnSpan) {
    table->removeCellWidget(row, column);

    QColor appTextColor = QApplication::palette().text().color();
    auto label = new QLabel("<a href=\"" + link + "\" style=\"color: " + appTextColor.name() + "\">" + text + "</a>");
    label->setObjectName("row_" + QString::number(row) + "_column_" + QString::number(column));
    label->setAlignment(Qt::AlignCenter);
    label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);

    QFont font = label->font();
    font.setPointSize(10);
    label->setFont(font);
    label->setStyleSheet("QLabel {background-color: " + backgroundColor.name() + ";}");

    label->setOpenExternalLinks(true);
    label->setTextFormat(Qt::RichText);
    label->setTextInteractionFlags(Qt::TextBrowserInteraction);

    if (rowSpan != 1 || columnSpan != 1) {
        table->setSpan(row, column, rowSpan, columnSpan);
    }

    if (backgroundColor == POLAR_COLOR) {
        label->setToolTip("Polar Codon");
    } else if (backgroundColor == NONPOLAR_COLOR) {
        label->setToolTip("Nonpolar Codon");
    } else if (backgroundColor == BASIC_COLOR) {
        label->setToolTip("Basic Codon");
    } else if (backgroundColor == ACIDIC_COLOR) {
        label->setToolTip("Acidic Codon");
    } else if (backgroundColor == STOP_CODON_COLOR) {
        label->setToolTip("Stop Codon");
    }

    table->setCellWidget(row, column, label);
}

void CodonTableView::addItemToTable(int row, int column, DNACodon* codon) {
    CHECK(codon != nullptr, )
    addItemToTable(row, column, codon->getFullName() + " (" + codon->getTreeLetterCode() + (codon->getFullName() != "Stop codon" ? QString(", ") + QChar(codon->getSymbol()) : "") + ")", getColor(codon->getCodonGroup()), codon->getLink());
}

QColor CodonTableView::getColor(DNACodonGroup gr) {
    if (gr == DNACodonGroup_POLAR) {
        return POLAR_COLOR;
    }
    if (gr == DNACodonGroup_NONPOLAR) {
        return NONPOLAR_COLOR;
    }
    if (gr == DNACodonGroup_BASIC) {
        return BASIC_COLOR;
    }
    if (gr == DNACodonGroup_ACIDIC) {
        return ACIDIC_COLOR;
    }
    if (gr == DNACodonGroup_STOP) {
        return STOP_CODON_COLOR;
    }
    return QColor();
}

}  // namespace U2
