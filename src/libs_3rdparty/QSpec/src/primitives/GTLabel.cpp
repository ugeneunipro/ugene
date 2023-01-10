/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
 * https://ugene.net
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

#include "primitives/GTLabel.h"

#include "primitives/GTWidget.h"

namespace HI {
QString GTLabel::getText(GUITestOpStatus& os, const QString& labelName, QWidget* parent) {
    QString text;
    class GetLabelTextScenario : public CustomScenario {
        QLabel* label;
        QString& text;

    public:
        GetLabelTextScenario(QLabel* label, QString& text)
            : label(label), text(text) {
        }
        void run(GUITestOpStatus&) override {
            text = label->text();
        }
    };
    GTThread::runInMainThread(os, new GetLabelTextScenario(GTWidget::findLabel(os, labelName, parent), text));
    return text;
}
}  // namespace HI
