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

#ifndef _U2_FAST_TREE_WIDGET_H_
#define _U2_FAST_TREE_WIDGET_H_

#include <QPlainTextEdit>

#include <U2Gui/InputWidgetsControllers.h>

#include <U2View/CreatePhyTreeWidget.h>
namespace U2 {

class PhyTreeDisplayOptionsWidget;

class FastTreeWidget : public CreatePhyTreeWidget {
    Q_OBJECT
public:
    FastTreeWidget(const MultipleSequenceAlignment& msa, QWidget* parent);

    void fillSettings(CreatePhyTreeSettings& settings) override;

    void storeSettings() override;

    void restoreDefault() override;

    bool checkSettings(QString& message, const CreatePhyTreeSettings& settings) override;

private:
    void propagateWidgetValuesToTextParameters();

    void propagateTextParametersToWidgetValues();

    QPlainTextEdit* extraParametersTextEdit = nullptr;
    PhyTreeDisplayOptionsWidget* displayOptionsWidget = nullptr;

    QCheckBox* usePseudoCountsCheckBox = nullptr;
    QCheckBox* useFastestCheckBox = nullptr;
    bool isNucleotideAlignment = false;

    bool isInsideChangeCallback = false;
};

}  // namespace U2

#endif  // _U2_FAST_TREE_WIDGET_H_
