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

#ifndef _U2_TOPHATSAMPLESWIDGETCONTROLLER_H_
#define _U2_TOPHATSAMPLESWIDGETCONTROLLER_H_

#include <U2Lang/SupportStructures.h>
#include <U2Lang/WizardWidget.h>

#include "WidgetController.h"

class QListWidget;
class QScrollArea;
class QToolButton;
class QVBoxLayout;

namespace U2 {

class TophatSamplesWidgetController : public WidgetController {
    Q_OBJECT
public:
    TophatSamplesWidgetController(WizardController *wc, TophatSamplesWidget *tsw);

    virtual QWidget * createGUI(U2OpStatus &os);

    void renameSample(int pos, const QString &newName, U2OpStatus &os);
    bool removeSample(int pos, QStringList &insertToFirst, QList<TophatSample> &append);
    TophatSample insertSample(int pos, U2OpStatus &os);
    void replaceDataset(int oldSamplePos, int oldDatasetPos, int newSamplePos, int newDatasetPos, U2OpStatus &os);

    bool canShowWarning() const;

private:
    enum RangeType {INCLUSIVE, EXCLUSIVE};
    void checkRange(int pos, RangeType rangeType, U2OpStatus &os) const;
    void checkDatasetRange(int samplePos, int datasetPos, RangeType rangeType, U2OpStatus &os) const;
    void initSamplesMap();
    QStringList getAllDatasets() const;
    QStringList getSampledDatasets() const;
    QStringList getUnsampledDatasets(const QStringList &sampledDatasets) const;
    void removeMissedDatasets();
    void commit();

private:
    TophatSamplesWidget *tsw;
    QList<TophatSample> samples;
};

class TophatSamples : public QWidget {
    Q_OBJECT
public:
    TophatSamples(const QList<TophatSample> &samples, TophatSamplesWidgetController *ctrl, QWidget *parent = NULL);

    bool rename(QLineEdit *nameEdit);

private:
    void init(const QList<TophatSample> &samples);
    void appendSample(const TophatSample &sample);
    QWidget * initSample(const QString &sampleName, const QStringList &datasets);
    QListWidget * getListWidget(int pos) const;
    QScrollArea * createScrollArea();
    QToolButton * createButton(QWidget *parent, const QString &icon) const;
    QVBoxLayout * createControlButtonsLayout();
    QVBoxLayout * createControlButtons();
    enum Direction {UP, DOWN};
    void findSelectedDataset(int &samplePos, int &datasetPos) const;
    bool isBorderCase(QListWidget *list, int datasetPos, Direction direction) const;
    void getNewPositions(QListWidget *oldList, int oldSamplePos, int oldDatasetPos, Direction direction, int &newSamplePos, int &newDatasetPos, QListWidget * &newList) const;
    void selectSample(int pos);
    void move(Direction direction);
    void updateArrows();

private slots:
    void sl_remove();
    void sl_add();
    void sl_selectionChanged();
    void sl_up();
    void sl_down();

private:
    TophatSamplesWidgetController *ctrl;
    QList<QWidget*> order;
    QScrollArea *scrollArea;
    QVBoxLayout *listLayout;
    QToolButton *upButton;
    QToolButton *downButton;
};

} // U2

#endif // _U2_TOPHATSAMPLESWIDGETCONTROLLER_H_
