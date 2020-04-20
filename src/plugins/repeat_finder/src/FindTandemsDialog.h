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

#ifndef _U2_FIND_TANDEMS_DIALOG_H_
#define _U2_FIND_TANDEMS_DIALOG_H_

#include <U2Core/U2Region.h>

#include "RF_SArray_TandemFinder.h"

#include "ui_FindTandemsDialog.h"

class QToolButton;

namespace U2 {

class CreateAnnotationWidgetController;
class ADVSequenceObjectContext;
class RegionSelector;

class FindTandemsDialog : public QDialog, public Ui_FindTandemsDialog {
    Q_OBJECT
public:
    FindTandemsDialog(ADVSequenceObjectContext* seq);

    static FindTandemsTaskSettings defaultSettings();

    virtual void accept();

protected slots:
    void minPeriodChanged(int min);
    void maxPeriodChanged(int max);
    void presetSelected(int preset);
    void customization();

private:
    U2Region getActiveRange(bool *ok = NULL) const;
    void saveState();
    QStringList getAvailableAnnotationNames() const;
    bool getRegions(QCheckBox* cb, QLineEdit* le, QVector<U2Region>& res);
    void prepareAMenu(QToolButton* tb, QLineEdit* le, const QStringList& names);

    //methods for results count estimation
    quint64 areaSize() const ;
    int estimateResultsCount() const;

    ADVSequenceObjectContext*           sc;
    CreateAnnotationWidgetController*   ac;
    RegionSelector*                      rs;
};

} //namespace

#endif
