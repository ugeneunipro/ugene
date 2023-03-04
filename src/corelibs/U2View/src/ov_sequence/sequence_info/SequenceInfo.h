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

#pragma once

#include <QSharedPointer>
#include <QWidget>

#include <U2Core/BackgroundTaskRunner.h>
#include <U2Core/U2Region.h>

#include <U2Gui/U2SavableWidget.h>

#include "CharOccurTask.h"
#include "DNAStatisticsTask.h"
#include "DinuclOccurTask.h"
#include "StatisticsCache.h"

class QLabel;

namespace U2 {

class ADVSequenceObjectContext;
class ADVSequenceWidget;
class AnnotatedDNAView;
class TmCalculator;
class LRegionsSelection;
class ShowHideSubgroupWidget;
class AnnotationSelection;
class Annotation;

class U2VIEW_EXPORT SequenceInfo : public QWidget {
    Q_OBJECT
public:
    SequenceInfo(AnnotatedDNAView*);
    ~SequenceInfo();

private slots:
    void sl_onSelectionChanged(LRegionsSelection*, const QVector<U2Region>&, const QVector<U2Region>&);
    void sl_onAnnotationSelectionChanged(AnnotationSelection*, const QList<Annotation*>&, const QList<Annotation*>&);
    void sl_onAminoTranslationChanged();

    /** Updates sequence info to match active sequence. */
    void sl_onActiveSequenceChanged(ADVSequenceWidget* oldSequenceWidget, ADVSequenceWidget* newSequenceWidget);

    /** A sequence part was added, removed or replaced */
    void sl_onSequenceModified();

    /** A sequence object has been added */
    void sl_onSequenceAdded(ADVSequenceObjectContext*);

    /** Update calculated info */
    void sl_updateCharOccurData();
    void sl_updateDinuclData();
    void sl_updateCodonOccurData();
    void sl_updateStatData();

    /** A subgroup (e.g. characters occurrence subgroup) has been opened/closed */
    void sl_subgroupStateChanged(const QString& subgroupId);

    bool eventFilter(QObject* object, QEvent* event);

    void statisticLabelLinkActivated(const QString& link);

private:
    /** Initializes the whole layout of the widget */
    void initLayout();

    /** Show or hide widgets depending on the alphabet of the sequence in focus */
    void updateLayout();  // calls the following update functions
    void updateCharOccurLayout();
    void updateDinuclLayout();
    void updateCodonOccurLayout();

    void updateData();
    void updateCommonStatisticsData(bool forceUpdate = false);
    void updateCommonStatisticsData(const DNAStatistics& commonStatistics);
    void updateCharactersOccurrenceData();
    void updateCharactersOccurrenceData(const CharactersOccurrence& charactersOccurrence);
    void updateDinucleotidesOccurrenceData();
    void updateDinucleotidesOccurrenceData(const DinucleotidesOccurrence& dinucleotidesOccurrence);

    /** Updates codon occurrence data from cache if available or re-launch the update task if the cached data does not match current selection state. */
    void updateCodonsOccurrenceData();

    /** Updates codon occurrence label from the 'codonStatList'. */
    void updateCodonsOccurrenceData(const QMap<QByteArray, qint64>& codonStatsMap);

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
    void launchCalculations(const QString& subgroupId = "");

    int getAvailableSpace(DNAAlphabetType alphabetType) const;

    QString formTableRow(const QString& caption, const QString& value, int availableSpace, bool addHyperlink = false) const;

    StatisticsCache<DNAStatistics>* getCommonStatisticsCache() const;
    StatisticsCache<CharactersOccurrence>* getCharactersOccurrenceCache() const;
    StatisticsCache<DinucleotidesOccurrence>* getDinucleotidesOccurrenceCache() const;
    StatisticsCache<QMap<QByteArray, qint64>>* getCodonsOccurrenceCache() const;

    QPointer<AnnotatedDNAView> annotatedDnaView;
    // This field is required because we need to save some settings in the destructor,
    // but @annotatedDnaView is dead up to this time
    QString annotatedDnaViewName;

    ShowHideSubgroupWidget* statsWidget = nullptr;
    QLabel* statisticLabel = nullptr;
    BackgroundTaskRunner<DNAStatistics> dnaStatisticsTaskRunner;
    DNAStatistics currentCommonStatistics;

    ShowHideSubgroupWidget* charOccurWidget = nullptr;
    QLabel* charOccurLabel = nullptr;
    BackgroundTaskRunner<CharactersOccurrence> charOccurTaskRunner;

    ShowHideSubgroupWidget* dinuclWidget = nullptr;
    QLabel* dinuclLabel = nullptr;
    BackgroundTaskRunner<DinucleotidesOccurrence> dinuclTaskRunner;

    ShowHideSubgroupWidget* codonWidget = nullptr;
    QLabel* codonLabel = nullptr;
    BackgroundTaskRunner<QMap<QByteArray, qint64>> codonTaskRunner;

    ShowHideSubgroupWidget* aminoAcidWidget = nullptr;
    QLabel* aminoAcidLabel = nullptr;

    QVector<U2Region> currentRegions;

    U2SavableWidget savableWidget;

    QSharedPointer<TmCalculator> temperatureCalculator;

};

}  // namespace U2
