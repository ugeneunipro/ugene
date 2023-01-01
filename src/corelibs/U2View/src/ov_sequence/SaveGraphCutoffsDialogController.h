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

#ifndef _U2_SAVE_GRAPH_CUTOFFS_DIALOG_H_
#define _U2_SAVE_GRAPH_CUTOFFS_DIALOG_H_

#include <U2Gui/CreateAnnotationWidgetController.h>

#include <U2View/ADVGraphModel.h>

#include <ui_SaveGraphCutoffsDialog.h>

namespace U2 {

class SequenceObjectContext;
class SaveGraphCutoffsDialogController : public QDialog, Ui_SaveGraphCutoffsDialog {
    Q_OBJECT
public:
    SaveGraphCutoffsDialogController(QSharedPointer<GSequenceGraphData>& graph,
                                     const GSequenceGraphMinMaxCutOffState& cutOffState,
                                     QWidget* parent,
                                     SequenceObjectContext* ctx);

    void accept() override;

private:
    bool isAcceptableValue(float val) const;
    bool validate();
    void tryAddObject(AnnotationTableObject* annotationTableObject);

    CreateAnnotationWidgetController* createAnnotationController;
    SequenceObjectContext* ctx;
    QSharedPointer<GSequenceGraphData> graph;
};

class SaveGraphCutoffsSettings {
public:
    float minCutoff, maxCutoff;
    bool isBetween;
};

}  // namespace U2
#endif  //_U2_SAVE_GRAPH_CUTOFFS_DIALOG_H_
