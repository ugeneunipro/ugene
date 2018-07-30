/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#ifndef _U2_FIND_REPEATS_DIALOG_H_
#define _U2_FIND_REPEATS_DIALOG_H_

#include <ui_FindRepeatsDialog.h>
#include <QAction>
#include <U2Core/U2Region.h>

#include "FindRepeatsTask.h"

namespace U2 {

class CreateAnnotationWidgetController;
class ADVSequenceObjectContext;
class RegionSelector;

class FindRepeatsDialog : public QDialog, public Ui_FindRepeatsDialog {
    Q_OBJECT
public:
    FindRepeatsDialog(ADVSequenceObjectContext* seq);

    static FindRepeatsTaskSettings defaultSettings();

    virtual void accept();

protected slots:
    void sl_setPredefinedAnnotationName();
    void sl_minDistChanged(int i);
    void sl_maxDistChanged(int i);
    void sl_onRegionChanged(const U2Region&);
    void sl_minLenHeuristics();
    void sl_hundredPercent();
    void sl_repeatParamsChanged(int);
    void sl_minMaxToggle(bool);

private:
    void saveState();
    QStringList getAvailableAnnotationNames() const;
    bool getRegions(QCheckBox* cb, QLineEdit* le, QVector<U2Region>& res);
    void prepareAMenu(QToolButton* tb, QLineEdit* le, const QStringList& names);
    void updateStatus();

    //methods for results count estimation
    quint64 areaSize() const ;
    int estimateResultsCount() const;

    ADVSequenceObjectContext*           sc;
    CreateAnnotationWidgetController*   ac;
    RegionSelector* rs;
};

class SetAnnotationNameAction : public QAction {
    Q_OBJECT
public:
    SetAnnotationNameAction(const QString& txt, QObject* p, QLineEdit*  _le) : QAction(txt, p), le(_le) {}
    QLineEdit* le;
};

} //namespace

#endif
