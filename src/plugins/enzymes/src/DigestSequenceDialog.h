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

#ifndef _U2_DIGEST_SEQUENCE_DIALOG_H_
#define _U2_DIGEST_SEQUENCE_DIALOG_H_

#include <QTimer>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include <U2Algorithm/EnzymeModel.h>

#include <U2Core/U2Region.h>

#include <U2Gui/MainWindow.h>

#include "EnzymesQuery.h"
#include "ui_DigestSequenceDialog.h"

namespace U2 {

class AnnotationTableObject;
class ADVSequenceObjectContext;
class U2SequenceObject;
class CreateAnnotationWidgetController;

class DigestSequenceDialog : public QDialog, public Ui_DigestSequenceDialog {
    Q_OBJECT
public:
    DigestSequenceDialog(ADVSequenceObjectContext* ctx, QWidget* parent);
    virtual void accept();

private slots:
    void sl_addPushButtonClicked();
    void sl_addAllPushButtonClicked();
    void sl_removePushButtonClicked();
    void sl_clearPushButtonClicked();
    void sl_addAnnBtnClicked();
    void sl_removeAnnBtnClicked();
    void sl_removeAllAnnsBtnClicked();
    void sl_timerUpdate();
    void sl_taskStateChanged();

private:
    void addAnnotationWidget();
    void searchForAnnotatedEnzymes(ADVSequenceObjectContext* ctx);
    void updateAvailableEnzymeWidget();
    void updateSelectedEnzymeWidget();
    void setUiEnabled(bool enabled);
    bool loadEnzymesFile();
    QList<SEnzymeData> findEnzymeDataById(const QString& id);
    QDialog* createSelectAnnotationsDialog();

    ADVSequenceObjectContext*           seqCtx;
    U2SequenceObject*                  dnaObj;
    AnnotationTableObject *               sourceObj;
    EnzymesSelectorDialogHandler        enzymesSelectorHandler;
    CreateAnnotationWidgetController*   ac;
    QTimer*                             timer;
    QSet<QString>                       availableEnzymes;
    QSet<QString>                       selectedEnzymes;
    QMap<QString,U2Region>              annotatedEnzymes;
    QList<SEnzymeData>                  enzymesBase;
    int                                 animationCounter;
    static const QString                WAIT_MESSAGE;
    static const QString                HINT_MESSAGE;
    QPushButton* okButton;
};

} //namespace

#endif
