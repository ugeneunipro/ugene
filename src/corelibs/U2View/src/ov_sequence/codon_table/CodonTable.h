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

#ifndef _U2_CODON_TABLE_H_
#define _U2_CODON_TABLE_H_

#include <U2Core/DNATranslation.h>

#include <U2View/GSequenceLineViewAnnotated.h>
#include <U2View/ADVSplitWidget.h>
#include <U2View/ADVSequenceWidget.h>

#include <QLabel>
#include <QVBoxLayout>
#include <QTableWidget>

namespace U2 {

class U2VIEW_EXPORT CodonTableView : public ADVSplitWidget {
    Q_OBJECT
public:
    CodonTableView(AnnotatedDNAView *view);

    virtual bool acceptsGObject(GObject*) {return false;}
    virtual void updateState(const QVariantMap&) {}
    virtual void saveState(QVariantMap&){}

    static const QColor NONPOLAR_COLOR;
    static const QColor POLAR_COLOR;
    static const QColor BASIC_COLOR;
    static const QColor ACIDIC_COLOR;
    static const QColor STOP_CODON_COLOR;

public slots:
    void sl_setVisible();
    void sl_setAminoTranslation();
    void sl_onSequenceFocusChanged(ADVSequenceWidget* from, ADVSequenceWidget* to);
private:
    QTableWidget *table;

    void addItemToTable(int row, int column,
                        const QString& text, const QColor& backgroundColor = QColor(0, 0, 0, 0),
                        int rowSpan = 1, int columnSpan = 1);
    void addItemToTable(int row, int column,
                        const QString& text,
                        int rowSpan = 1, int columnSpan = 1);
    void addItemToTable(int row, int column,
                        const QString& text, const QColor& backgroundColor,
                        const QString& link,
                        int rowSpan = 1, int columnSpan = 1);
    void addItemToTable(int row, int column, DNACodon *codon);

    void setAminoTranslation(const QString &trId);
    void spanEqualCells();
    QColor getColor(DNACodonGroup gr);
};

} // namespace

#endif // _U2_CODON_TABLE_H_
