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

#ifndef _U2_REGION_SELECTOR_H_
#define _U2_REGION_SELECTOR_H_

#include <U2Core/DNASequenceSelection.h>
#include <U2Core/U2Region.h>
#include <U2Core/global.h>

#include <QDialog>
#include <QLineEdit>

class QComboBox;

#include "RegionSelectorController.h"


namespace U2 {

class RegionLineEdit : public QLineEdit {
    Q_OBJECT
public:
    RegionLineEdit(QWidget* p, QString actionName, qint64 defVal)
        : QLineEdit(p),
          actionName(actionName),
          defaultValue(defVal) {}

protected:
    void focusOutEvent ( QFocusEvent * event );
    void contextMenuEvent(QContextMenuEvent *);

private slots:
    void sl_onSetMinMaxValue();

private:
    const QString actionName;
    qint64 defaultValue;
};

class U2GUI_EXPORT RegionSelector : public QWidget {
    Q_OBJECT
public:
    RegionSelector(QWidget* p, qint64 len = 0, bool isVertical = false,
                   DNASequenceSelection* selection = NULL,
                   bool isCircularSelectionAvailable = false,
                   QList<RegionPreset> presetRegions = QList<RegionPreset>());

    U2Region getRegion(bool *ok = NULL) const;
    bool isWholeSequenceSelected() const;

    void setCustomRegion(const U2Region& value);
    void setWholeRegionSelected();
    void setCurrentPreset(const QString &presetName);
    void reset();
    void removePreset(const QString &itemName);

    void showErrorMessage();

    static const QString WHOLE_SEQUENCE;
    static const QString SELECTED_REGION;
    static const QString CUSTOM_REGION;

signals:
    void si_regionChanged(const U2Region& newRegion);

private:
    void initLayout();

    RegionSelectorController* controller;

    qint64                maxLen;
    RegionLineEdit *      startEdit;
    RegionLineEdit *      endEdit;
    QComboBox *           comboBox;
    bool                  isVertical;
};

}//namespace

#endif
