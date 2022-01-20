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
#include "PCRPrimerDesignForDNAAssemblyTaskSettings.h"
#include "ui_PCRPrimerDesignForDNAAssemblyOPWidget.h"

namespace U2 {

class AnnotatedDNAView;
class PCRPrimerDesignForDNAAssemblyTask;

class PCRPrimerDesignForDNAAssemblyOPWidget : public QWidget, private Ui_PCRPrimerDesignForDNAAssemblyOPWidget {
    Q_OBJECT
public:
    PCRPrimerDesignForDNAAssemblyOPWidget(AnnotatedDNAView* annDnaView);

private slots:
    /**
     * Updates state of the widget to match the new active sequence.
     */
    void sl_activeSequenceChanged();
    /**
     * Update regions when sequence length changes.
     */
    void sl_sequenceModified();
    /**
     * Start calcultaion process.
     * This slot is called when a user clicks on the "Start" button.
     */
    void sl_start();
    /**
     * Activate "Select region manually" mode.
     * When this mode is active, user may choose left or right search regin by selecting region manyally on sequence.
     */
    void sl_selectManually();
    /**
     * When @DNASequenceSelection::si_onSelectionChanged is send, the region fields are about to be updated in this slot.
     */
    void sl_updateRegion();
    /**
     * This slot is used to update ranges of fields, which has a "pair".
     * E.g. "Required delta G" has minimum and maximum, and minimum should not lesser, then maximum.
     */
    void sl_updateParametersRanges();
    /**
     * Update range of minimum limit of parameter, which have maximum.
     */
    void sl_minValueChanged();
    /**
     * Update range of maximum limit of parameter, which have minimum.
     */
    void sl_maxValueChanged();
    /**
     * Show the "Open file" dialog, choose a file with backbones and set path to the corresponding line edit.
     */
    void sl_loadBackbone();
    /**
     * Show the "Open file" dialog, choose a file with other PCR sequences and set path to the corresponding line edit.
     */
    void sl_loadOtherSequenceInPcr();
    /*
    * Associate created result annotations with result table.
    */
    void sl_onFindTaskFinished();
    /*
    * Extract result product to file.
    */
    void sl_extractProduct();
    /*
     * Create genbank file with result annotations/
     */
    void sl_annotationCreationTaskFinished();
    /**
     * Select the sequence in the @twGeneratedSequences, which is reverse-complement to the selected one
     */
    void sl_selectReverseComplementInTable();
    /**
     * Add the selected in @twGeneratedSequences to the forvard user primer as 5' end
     */
    void sl_add5ForwardSequence();
    /**
     * Add the selected in @twGeneratedSequences to the forward user primer as 3' end
     */
    void sl_add3ForwardSequence();
    /**
     * Add the selected in @twGeneratedSequences to the reverse user primer as 5' end
     */
    void sl_add5ReverseSequence();
    /**
     * Add the selected in @twGeneratedSequences to the reverse user primer as 3' end
     */
    void sl_add3ReverseSequence();
    /**
     * Update sequences in @twGeneratedSequences, corresponding to the filter from @leFilter
     */
    void sl_updateSequenceList(const QString&);

private:
    void createResultAnnotations();
    /**
     * Check that active sequence is DNA and enable all widgets except warning label. Otherwise, disable widgets except
     * warning label.
     */
    void makeWarningInvisibleIfDna();
    /**
     * Sets region to area spinboxes; updates the spinbox ranges. If region is beyond the sequence length, limits it.
     * Does not check the validity of passed arguments.
     */
    void setRegion(QSpinBox *start, QSpinBox *end, const U2Region &region) const;
    void setRegion(QSpinBox *start, QSpinBox *end, const U2Region &region, qint64 seqLength) const;
    /**
     * Adjusts maximum and minimum of area spinboxes according to the current values of these spinboxes and the length
     * of the active sequence (start from [1, end.value], end from [start.value, seqLength]).
     * Does not check the validity of passed arguments.
     */
    void updateAreaSpinboxRanges(QSpinBox *start, QSpinBox *end, qint64 seqLength) const;
    /**
     * Updates ranges of the area spinboxes, connects signals.
     */
    void initAreaSpinboxes() const;

    /**
     * Return the selected in @twGeneratedSequences sequence
     */
    QString getSelectedSequence() const;

    AnnotatedDNAView *annDnaView = nullptr;

    QToolButton* smButton = nullptr;
    QSpinBox* sbStartRegion = nullptr;
    QSpinBox* sbEndRegion = nullptr;
    QMetaObject::Connection updateRegionConnection;

    PCRPrimerDesignForDNAAssemblyTask *pcrTask = nullptr;

    PCRPrimerDesignForDNAAssemblyOPSavableTab savableWidget;
    //"Parameters of priming sequences" and "Parameters to exclude in whole primers" has set of parameters.
    //This map contains spinboxes with minimum values as keys and spinboxes with maximum values as values.
    QMap<QSpinBox*, QSpinBox*> parametersMinMaxSpinBoxes;
    PCRPrimerDesignForDNAAssemblyTaskSettings lastRunSettings;
    QByteArray backboneSequence;

    static const QString USER_PRIMERS_SHOW_HIDE_ID;
    static const QString PARAMETERS_OF_PRIMING_SEQUENCES_SHOW_HIDE_ID;
    static const QString PARAMETERS_2_EXCLUDE_IN_WHOLE_PRIMERS_SHOW_HIDE_ID;
    static const QString SELECT_AREAS_FOR_PRIMING_SHOW_HIDE_ID;
    static const QString OPEN_BACKBONE_SEQUENCE_SHOW_HIDE_ID;
    static const QString GENERATE_SEQUENCE_SHOW_HIDE_ID;
    static const QString OTHER_SEQUENCES_IN_PCR_REACTION_SHOW_HIDE_ID;
    static const QString PCR_TABLE_OBJECT_NAME;
};


}    // namespace U2

#endif    // _U2_PCR_PRIMER_DESIGN_FOR_DNA_ASSEMBLY_OP_WIDGET_H_
