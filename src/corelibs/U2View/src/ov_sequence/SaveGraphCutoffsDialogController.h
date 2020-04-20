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

#ifndef _U2_SAVE_GRAPH_CUTOFFS_DIALOG_H_
#define _U2_SAVE_GRAPH_CUTOFFS_DIALOG_H_

#include <ui_SaveGraphCutoffsDialog.h>

#include <U2View/ADVGraphModel.h>

#include <U2Gui/CreateAnnotationWidgetController.h>

namespace U2 {

class SequenceObjectContext;
class SaveGraphCutoffsDialogController : public QDialog, Ui_SaveGraphCutoffsDialog {
    Q_OBJECT
public:
    SaveGraphCutoffsDialogController(GSequenceGraphDrawer *d,
                                     QSharedPointer<GSequenceGraphData>& gd,
                                     QWidget *parent,
                                     SequenceObjectContext* ctx);

    virtual void accept();
private:
    inline bool isAcceptableValue(float val);
    bool validate();
    void tryAddObject(AnnotationTableObject *annotationTableObject);

    CreateAnnotationWidgetController*   ac;
    SequenceObjectContext*              ctx;
    GSequenceGraphDrawer*               d;
    QSharedPointer<GSequenceGraphData>  gd;
    QList<U2Region>                     resultRegions;
};

class SaveGraphCutoffsSettings{
public:
    float minCutoff, maxCutoff;
    bool isBetween;
};

}
#endif //_U2_SAVE_GRAPH_CUTOFFS_DIALOG_H_
