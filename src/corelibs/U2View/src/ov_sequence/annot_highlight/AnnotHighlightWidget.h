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

#ifndef _U2_ANNOT_HIGHLIGHT_WIDGET_H_
#define _U2_ANNOT_HIGHLIGHT_WIDGET_H_

#include "AnnotHighlightSettings.h"
#include "AnnotHighlightTree.h"

class QPushButton;

namespace U2 {

class ADVSequenceObjectContext;
class ADVSequenceWidget;
class AnnotatedDNAView;
class Annotation;
class AnnotationModification;
class AnnotationTableObject;
class AnnotatedRegion;
class U2Region;

/** Label that can be pressed with two states: show all types or types for sequence only */
class ShowAllAnnotTypesLabel : public QLabel
{
    Q_OBJECT
public:
    ShowAllAnnotTypesLabel();
    inline bool isShowAllSelected() { return showAllIsSelected; }

signals:
    void si_showAllStateChanged();

private:
    virtual void mousePressEvent(QMouseEvent*);

    bool showAllIsSelected;
};

class AnnotHighlightWidget : public QWidget
{
    Q_OBJECT
public:
    AnnotHighlightWidget(AnnotatedDNAView *);

private slots:
    void sl_onShowAllStateChanged();
    void sl_onSelectedItemChanged(const QString &annotName);
    void sl_storeNewColor(const QString &annotName, const QColor &newColor);
    void sl_storeNewSettings(AnnotationSettings *annotSettings);
    void sl_onSequenceModified(ADVSequenceObjectContext *);
    void sl_onAnnotationsAdded(const QList<Annotation *> &);
    void sl_onAnnotationsRemoved(const QList<Annotation *> &);
    void sl_onAnnotationsModified();
    void sl_onAnnotationObjectAdded(AnnotationTableObject *);
    void sl_onAnnotationObjectRemoved(AnnotationTableObject *);
    void sl_onNextAnnotationClick();
    void sl_onPrevAnnotationClick();
    void sl_onAnnotationSelectionChanged();

private:
    void initLayout();
    /**
     * Annotation types tree and settings are not shown.
     * A label describes that there are no annotations.
     */
    void setNoAnnotsLayout();

    /**
     * The tree and the settings are shown, the "no annotation"
     * label is not visible.
     */
    void setLayoutWithAnnotsSelection();

    /** The label value depends on the number of sequences (one or more) */
    void setNoAnnotTypesLabelValue();

    void connectSlots();
    void connectSlotsForAnnotTableObj(const AnnotationTableObject *annotTableObj);
    void disconnectSlotsForAnnotTableObj(const AnnotationTableObject *annotTableObj);

    /**
     * Depending on the showAllLabel loads either annotations
     * for the sequence or all annotations.
     * The tree is cleared before adding the items.
     */
    void loadAnnotTypes();

    /**
     * Searches for all annotation types IN the current Sequence View and
     * saves the results into the annotNamesWithAminoInfo map.
     * "annotIsOnAminoSeq" = true, when annotations of this type were found
     * on amino acid sequences only, otherwise it is "true"
     */
    void findAllAnnotationsNamesForSequence();

    /**
     * Searches for ALL registered annotation types and
     * saves the results into the annotNamesWithAminoInfo map.
     * "annotIsOnAminoSeq" = false in this case (i.e. enable the "Show on translation" option).
     */
    void findAllAnnotationsNamesInSettings();

    void selectNextAnnotation(bool isForward) const;

    /**
     * Returns true if provided @region of @annotation is:
     *  - the first one in case @fromTheBeginning is true,
     *  - the last one in case @fromTheBeginning is false.
     */
    bool isFirstAnnotatedRegion(Annotation *annotation, const U2Region& region, bool fromTheBeginning = true) const;

    bool noAnnotatedRegions() const;

    bool findFirstAnnotatedRegion(AnnotatedRegion &annRegion, bool fromTheBeginning = true) const;

    bool findFirstAnnotatedRegionAfterPos(AnnotatedRegion &annRegion, qint64 startPos, bool isForward) const;

    bool findNextUnselectedAnnotatedRegion(AnnotatedRegion &annRegion, bool isForward) const;

    QList<AnnotatedRegion> getAllAnnotatedRegionsByStartPos(qint64 startPos) const;

    void updateAnnotationNames();

    AnnotatedDNAView* annotatedDnaView;

    /**
     * For each annotation type specifies whether the "Show on translation" option
     * should be enabled or disabled.
     */
    QMap<QString, bool> annotNamesWithAminoInfo;

    QLabel* noAnnotTypesLabel;
    QLabel* annotTreeTitle;
    AnnotHighlightTree* annotTree;
    ShowAllAnnotTypesLabel* showAllLabel;
    QLabel* settingsTitle;
    AnnotHighlightSettingsWidget* annotSettingsWidget;
    QPushButton* nextAnnotationButton;
    QPushButton* prevAnnotationButton;
};

} // namespace

#endif
