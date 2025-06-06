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

#pragma once

#include <U2Gui/OPWidgetFactory.h>

namespace U2 {

class MsaEditor;
class SequenceSelectorWidgetController;

/** Reference Sequence is a common widget in the Alignment Editor */
class RefSeqCommonWidget : public QWidget {
    Q_OBJECT
public:
    RefSeqCommonWidget(MsaEditor* msaEditor);

private slots:
    void sl_refSeqChanged(qint64);
    void sl_textControllerChanged();

private:
    QWidget* createReferenceGroup();
    MsaEditor* msaEditor;
    SequenceSelectorWidgetController* reSeqSelector;
};

class U2VIEW_EXPORT RefSeqCommonWidgetFactory : public OPCommonWidgetFactory {
    Q_OBJECT
public:
    RefSeqCommonWidgetFactory(QList<QString> groups);
    virtual ~RefSeqCommonWidgetFactory();

    QWidget* createWidget(GObjectViewController* objView, const QVariantMap& options) override;
};

}  // namespace U2
