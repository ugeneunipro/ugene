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

#ifndef _U2_SEQUENCE_EXPORT_SETTINGS_WIDGET_H_
#define _U2_SEQUENCE_EXPORT_SETTINGS_WIDGET_H_

#include "ui_SequenceImageExportSettingsWidget.h"
#include "SequencePainter.h"


namespace U2 {

class DNASequenceSelection;
class RegionSelector;
class U2SequenceObject;
class U2Region;

class SequenceExportSettingsWidget : public QWidget, public Ui_SequenceExportSettingsWidget {
    Q_OBJECT
public:
    SequenceExportSettingsWidget(U2SequenceObject* seqObj, QSharedPointer<CustomExportSettings> settings, DNASequenceSelection* selection = NULL);

signals:
    void si_regionChanged(U2Region region);

private slots:
    void sl_areaChanged();
    void sl_regionChanged(const U2Region& r);

private:
    SequenceExportType  getExportType() const;

    QSharedPointer<SequenceExportSettings>  settings;

    U2SequenceObject*       seqObject;
    RegionSelector*         regionSelector;
};

} // namespace

#endif // _U2_SEQUENCE_EXPORT_SETTINGS_WIDGET_H_
