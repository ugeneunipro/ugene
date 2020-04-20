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

#ifndef _U2_SEQUENCE_INFO_H_
#define _U2_SEQUENCE_INFO_H_

#include <QWidget>

#include <U2Core/BackgroundTaskRunner.h>
#include <U2Core/U2Region.h>

#include <U2Gui/U2SavableWidget.h>

#include "CharOccurTask.h"
#include "DinuclOccurTask.h"
#include "DNAStatisticsTask.h"
#include "StatisticsCache.h"

class QLabel;

namespace U2 {

class ADVSequenceObjectContext;
class ADVSequenceWidget;
class AnnotatedDNAView;
class LRegionsSelection;
class ShowHideSubgroupWidget;

class U2VIEW_EXPORT SequenceInfo : public QWidget {
    Q_OBJECT
public:
    SequenceInfo(AnnotatedDNAView*);

private slots:
    void sl_onSelectionChanged(LRegionsSelection*, const QVector<U2Region>& , const QVector<U2Region>&);

    /**
    * Focus is changed e.g. when a user selects another sequence or deletes the sequence in focus
    * Verifies either a region is selected on the sequence in focus.
    */
    void sl_onFocusChanged(ADVSequenceWidget *from, ADVSequenceWidget *to);

    /** A sequence part was added, removed or replaced */
    void sl_onSequenceModified();

    /** A sequence object has been added */
    void sl_onSequenceAdded(ADVSequenceObjectContext*);

    /** Update calculated info */
    void sl_updateCharOccurData();
    void sl_updateDinuclData();
    void sl_updateStatData();

    /** A subgroup (e.g. characters occurrence subgroup) has been opened/closed */
    void sl_subgroupStateChanged(QString subgroupId);

    bool eventFilter(QObject *object, QEvent *event);

private:
    /** Initializes the whole layout of the widget */
    void initLayout();

    /** Show or hide widgets depending on the alphabet of the sequence in focus */
    void updateLayout(); // calls the following update functions
    void updateCharOccurLayout();
    void updateDinuclLayout();

    void updateData();
    void updateCommonStatisticsData();
    void updateCommonStatisticsData(const DNAStatistics &commonStatistics);
    void updateCharactersOccurrenceData();
    void updateCharactersOccurrenceData(const CharactersOccurrence &charactersOccurrence);
    void updateDinucleotidesOccurrenceData();
    void updateDinucleotidesOccurrenceData(const DinucleotidesOccurrence &dinucleotidesOccurrence);

    /**  Listen when something has been changed in the AnnotatedDNAView or in the Options Panel */
    void connectSlotsForSeqContext(ADVSequenceObjectContext*);
    void connectSlots();

    /**
     * Updates current regions to the selection. If selection is empty the whole sequence is used.
     */
    void updateCurrentRegions();

    /**
     * Calculates the sequence (or region) length and launches other tasks (like characters occurrence).
     * The tasks are launched if:
     * 1) The corresponding widget is shown (this depends on the sequence alphabet)
     * 2) The corresponding subgroup is opened
     * The subgroupId parameter is used to skip unnecessary calculation when a subgroup signal has come.
     * Empty subgroupId means that the signal has come from other place and all required calculation should be re-done.
     */
    void launchCalculations(QString subgroupId = QString(""));

    int getAvailableSpace(DNAAlphabetType alphabetType) const;

    QString formBoldTableRow(const QString& caption, const QString &value, int availableSpace) const;
    QString formTableRow(const QString& caption, const QString &value, int availableSpace) const;

    StatisticsCache<DNAStatistics> *getCommonStatisticsCache() const;
    StatisticsCache<CharactersOccurrence> *getCharactersOccurrenceCache() const;
    StatisticsCache<DinucleotidesOccurrence> *getDinucleotidesOccurrenceCache() const;

    AnnotatedDNAView* annotatedDnaView;

    ShowHideSubgroupWidget* statsWidget;
    QLabel* statisticLabel;
    BackgroundTaskRunner<DNAStatistics> dnaStatisticsTaskRunner;
    DNAStatistics currentCommonStatistics;

    ShowHideSubgroupWidget* charOccurWidget;
    QLabel* charOccurLabel;
    BackgroundTaskRunner<CharactersOccurrence> charOccurTaskRunner;

    ShowHideSubgroupWidget* dinuclWidget;
    QLabel* dinuclLabel;
    BackgroundTaskRunner<DinucleotidesOccurrence> dinuclTaskRunner;

    QVector<U2Region> currentRegions;

    U2SavableWidget savableWidget;

    static const int COMMON_STATISTICS_TABLE_CELLSPACING;
    static const QString CAPTION_SEQ_REGION_LENGTH;

    static const QString CAPTION_SUFFIX_DS_DNA;
    static const QString CAPTION_SUFFIX_SS_DNA;
    static const QString CAPTION_SUFFIX_DS_RNA;
    static const QString CAPTION_SUFFIX_SS_RNA;

    //nucl
    static const QString CAPTION_SEQ_GC_CONTENT;
    static const QString CAPTION_SEQ_NUCL_MOLECULAR_WEIGHT;
    static const QString CAPTION_SEQ_EXTINCTION_COEFFICIENT;
    static const QString CAPTION_SEQ_MELTING_TEMPERATURE;

    static const QString CAPTION_SEQ_NMOLE_OD;
    static const QString CAPTION_SEQ_MG_OD;

    //amino
    static const QString CAPTION_SEQ_AMINO_MOLECULAR_WEIGHT;
    static const QString CAPTION_SEQ_ISOELECTIC_POINT;

    static const QString CHAR_OCCUR_GROUP_ID;
    static const QString DINUCL_OCCUR_GROUP_ID;
    static const QString STAT_GROUP_ID;
};

} // namespace U2

#endif
