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

#ifndef _U2_PCR_GROUP_BOX_H_
#define _U2_PCR_GROUP_BOX_H_

#include "ui_PrimerGroupBox.h"

namespace U2 {

class ADVSequenceObjectContext;
class AnnotatedDNAView;
class FindAlgorithmTask;
class U2SequenceObject;

class PrimerGroupBox : public QWidget, public Ui_PrimerGroupBox {
    Q_OBJECT
public:
    PrimerGroupBox(QWidget *parent);
    void setAnnotatedDnaView(AnnotatedDNAView* dnaView);

    QByteArray getPrimer() const;
    uint getMismatches() const;

signals:
    void si_primerChanged();

private slots:
    void sl_onPrimerChanged(const QString &primer);
    void sl_translate();
    void sl_browse();

    void sl_findPrimerTaskStateChanged();
    void sl_focusChanged();

private:
    static QString getTmString(const QString &sequence);

    void findPrimerAlternatives(const QString& primer);
    void cancelFindPrimerTask();

    void updateStatistics(const QString &primer);

    FindAlgorithmTask* findPrimerTask;
    AnnotatedDNAView*   annotatedDnaView;
};

} // U2

#endif // _U2_PCR_GROUP_BOX_H_
