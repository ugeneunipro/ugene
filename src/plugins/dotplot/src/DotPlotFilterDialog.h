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

#ifndef _U2_DOT_PLOT_FILTER_DIALOG_H_
#define _U2_DOT_PLOT_FILTER_DIALOG_H_

#include <ui_DotPlotFilterDialog.h>

#include <QDialog>

namespace U2 {

class ADVSequenceObjectContext;

enum FilterType{
    All,
    Features
};

enum FilterIntersectionParameter{
    SequenceX,
    SequenceY,
    Both
};

class DotPlotFilterDialog : public QDialog, public Ui_DotPlotFilterDialog {
    Q_OBJECT
public:
    DotPlotFilterDialog(QWidget *parent, ADVSequenceObjectContext *sequenceX, ADVSequenceObjectContext *sequenceY);

    virtual void accept();

    FilterType getFilterType();
    QMultiMap<FilterIntersectionParameter, QString> getFeatureNames(){return featureNames;}
protected slots:
    void sl_filterTypeChanged(int);
    void sl_selectAll();
    void sl_selectDifferent();
    void sl_clearSelection();
    void sl_invertSelection();
private:
    ADVSequenceObjectContext    *xSeq, *ySeq;
    FilterType fType;
    QMultiMap<FilterIntersectionParameter, QString> featureNames;
    QButtonGroup *filterGroup;
    QTreeWidgetItem *seqXItem;
    QTreeWidgetItem *seqYItem;

    QSet<QString> getUniqueAnnotationNames(ADVSequenceObjectContext *seq);
};

} //namespace

#endif
