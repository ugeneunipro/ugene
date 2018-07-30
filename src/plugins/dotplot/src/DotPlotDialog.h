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

#ifndef _U2_DOT_PLOT_DIALOG_H_
#define _U2_DOT_PLOT_DIALOG_H_

#include <ui_DotPlotDialog.h>

#include <U2Algorithm/RepeatFinderSettings.h>
#include <QDialog>

namespace U2 {

class ADVSequenceObjectContext;
class GObject;
class Document;
class AnnotatedDNAView;
class U2SequenceObject;

class DotPlotDialog : public QDialog, public Ui_DotPlotDialog{
    Q_OBJECT
public:
    DotPlotDialog(QWidget *parent, AnnotatedDNAView* currentADV, int minLen, int identity,
        ADVSequenceObjectContext *seqX, ADVSequenceObjectContext *seqY, bool dir, bool inv,
        const QColor &dColor = QColor(), const QColor &iColor = QColor(), bool hideLoadSequences = false);

    virtual void accept();

    ADVSequenceObjectContext    *getXSeq() const {return xSeq;}
    ADVSequenceObjectContext    *getYSeq() const {return ySeq;}

    int getMinLen() const;
    int getMismatches() const;
    RFAlgorithm getAlgo() const;

    bool isDirect() const;
    bool isInverted() const;

    const QColor& getDirectColor() const {return directColor;}
    const QColor& getInvertedColor() const {return invertedColor;}

protected slots:
    void sl_minLenHeuristics();

    void sl_hundredPercent();

    void sl_directInvertedCheckBox();

    void sl_directColorButton();
    void sl_invertedColorButton();

    void sl_directDefaultColorButton();
    void sl_invertedDefaultColorButton();

    void sl_loadSequenceButton();

    void sl_loadTaskStateChanged(Task* t);

    void sl_documentAddedOrRemoved();
    void sl_objectAddedOrRemoved();
    void sl_loadedStateChanged();

private:
    void reconnectAllProjectDocuments();
    void updateSequenceSelectors();

    ADVSequenceObjectContext    *xSeq, *ySeq;
    AnnotatedDNAView*           adv;

    QColor directColor, invertedColor;

    void updateColors();

    bool isObjectInADV(GObject* obj);
    GObject* getGObjectByName(const QString& gObjectName);

    Task* openSequenceTask;
};

} //namespace

#endif
