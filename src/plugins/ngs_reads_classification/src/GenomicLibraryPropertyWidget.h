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

#ifndef _U2_GENOMIC_LIBRARY_PROPERTY_WIDGET_H_
#define _U2_GENOMIC_LIBRARY_PROPERTY_WIDGET_H_

#include <U2Designer/PropertyWidget.h>

#include <U2Lang/Dataset.h>

namespace U2 {
namespace LocalWorkflow {

class GenomicLibraryPropertyWidget : public PropertyWidget {
    Q_OBJECT
public:
    GenomicLibraryPropertyWidget(QWidget *parent = NULL, DelegateTags *tags = NULL);

    QVariant value();

    static const QString PLACEHOLDER;
    static const QString FILLED_VALUE;

public slots:
    void setValue(const QVariant &value);

private slots:
    void sl_showDialog();

private:
    QLineEdit *lineEdit;
    QToolButton *toolButton;
    Dataset dataset;
};

}   // namespace LocalWorkflow
}   // namespace U2

#endif // _U2_GENOMIC_LIBRARY_PROPERTY_WIDGET_H_
