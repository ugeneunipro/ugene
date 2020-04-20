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

#ifndef _U2_EDIT_SEQUENCE_CONTROLLER_H_
#define _U2_EDIT_SEQUENCE_CONTROLLER_H_

#include <QDialog>

#include <U2Core/U2Region.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/GUrl.h>

#include <U2Gui/SeqPasterWidgetController.h>
#include <U2Gui/DialogUtils.h>

class Ui_EditSequenceDialog;

namespace U2 {

class SaveDocumentController;

enum EditSequenceDialogMode {
    EditSequenceMode_Replace,
    EditSequenceMode_Insert
};

/**
 * A workaround to listen to enter in the pattern field and
 * make a correct (almost) tab order.
 */
class U2GUI_EXPORT SeqPasterEventFilter : public QObject
{
    Q_OBJECT
public:
    SeqPasterEventFilter(QObject* parent);

signals:
    void si_enterPressed();

protected:
    bool eventFilter(QObject* obj, QEvent *event);
};

struct U2GUI_EXPORT EditSequencDialogConfig {
    EditSequenceDialogMode mode;
    U2Region source;
    const DNAAlphabet* alphabet;
    QByteArray initialText;
    QVector<U2Region> selectionRegions; 
    int position;
};

class U2GUI_EXPORT EditSequenceDialogController : public QDialog {
    Q_OBJECT
public:
    EditSequenceDialogController(const EditSequencDialogConfig &cfg, QWidget *p = NULL);
    ~EditSequenceDialogController();

    void accept();

    DNASequence getNewSequence() const;
    GUrl getDocumentPath() const;
    int getPosToInsert() const;
    U1AnnotationUtils::AnnotationStrategyForResize getAnnotationStrategy() const;
    bool mergeAnnotations() const;
    bool recalculateQualifiers() const;
    DocumentFormatId getDocumentFormatId() const;

private slots:
    void sl_mergeAnnotationsToggled();
    void sl_startPositionliClicked();
    void sl_endPositionliClicked();
    void sl_beforeSlectionClicked();
    void sl_afterSlectionClicked();
    void sl_enterPressed();

private:
    void addSeqpasterWidget();
    bool modifyCurrentDocument() const;
    void initSaveController();

    QString filter;
    int pos;
    SeqPasterWidgetController *w;
    SaveDocumentController *saveController;
    EditSequencDialogConfig config;
    Ui_EditSequenceDialog* ui;

    int seqEndPos;
};

}//ns

#endif

