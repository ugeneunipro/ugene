/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#ifndef _U2_PCR_PRIMER_DESIGN_FOR_DNA_ASSEMBLY_OP_WIDGET_H_
#define _U2_PCR_PRIMER_DESIGN_FOR_DNA_ASSEMBLY_OP_WIDGET_H_

#include <QWidget>

#include "PCRPrimerDesignForDNAAssemblyOPSavableTab.h"
#include "ui_PCRPrimerDesignForDNAAssemblyOPWidget.h"

namespace U2 {

class AnnotatedDNAView;

class PCRPrimerDesignForDNAAssemblyOPWidget : public QWidget, private Ui_PCRPrimerDesignForDNAAssemblyOPWidget {
    Q_OBJECT
public:
    PCRPrimerDesignForDNAAssemblyOPWidget(AnnotatedDNAView* annDnaView);

private slots:
    void sl_start();
    void sl_selectManually();
    void sl_updateRegion();
    void sl_updateMaxValues();
    void sl_minValueChanged();
    void sl_maxValueChanged();
    void sl_loadBackbone();
    void sl_saveRandomSequences();
    void sl_loadOtherSequenceInPcr();

private:
    AnnotatedDNAView* annDnaView = nullptr;

    QToolButton* smButton = nullptr;
    QSpinBox* start = nullptr;
    QSpinBox* end = nullptr;
    QMetaObject::Connection updateRegionConnection;

    PCRPrimerDesignForDNAAssemblyOPSavableTab savableWidget;
    QMap<QSpinBox*, QSpinBox*> minMaxValuesSb;

    static const QString USER_PRIMERS_SHOW_HIDE_ID;
    static const QString PARAMETERS_OF_PRIMING_SEQUENCES_SHOW_HIDE_ID;
    static const QString PARAMETERS_2_EXCLUDE_IN_WHOLE_PRIMERS_SHOW_HIDE_ID;
    static const QString SELECT_AREAS_FOR_PRIMING_SHOW_HIDE_ID;
    static const QString OPEN_BACKBONE_SEQUENCE_SHOW_HIDE_ID;
    static const QString GENERATE_SEQUENCE_SHOW_HIDE_ID;
    static const QString OTHER_SEQUENCES_IN_PCR_REACTION_SHOW_HIDE_ID;

};


}    // namespace U2

#endif    // _U2_PCR_PRIMER_DESIGN_FOR_DNA_ASSEMBLY_OP_WIDGET_H_
