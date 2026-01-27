/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include <QDialog>

#include <U2Core/GUrl.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2Region.h>

#include <U2Gui/SeqPasterWidgetController.h>

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
class U2GUI_EXPORT SeqPasterEventFilter : public QObject {
    Q_OBJECT
public:
    SeqPasterEventFilter(QObject* parent);

signals:
    void si_enterPressed();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
};

struct U2GUI_EXPORT EditSequencDialogConfig {
    EditSequenceDialogMode mode;
    U2Region source;
    const DNAAlphabet* alphabet;
    QByteArray initialText;
    QVector<U2Region> selectionRegions;
    int position;
};

/**
 * This is an abstract class for dialog to edit the sequence.
 * It has setting about "where to insert", "how handle the annotations", etc.
 * In the inherited class you need to add widget, which allows you to add new data.
 * For example, it could be the field for the new sequence (EditSequenceDialogController)
 * or popup menu, where you can choose a restriction enzyme to paste (InsertEnzymeDialog)
 */
class U2GUI_EXPORT EditSequenceDialogVirtualController : public QDialog  {
    Q_OBJECT
public:
    EditSequenceDialogVirtualController(const EditSequencDialogConfig& cfg, QWidget* p = nullptr, const QString& helpId = "65929426");

    void accept() override;

    /**
     * Here you need to create a new DNASequence to paste.
     */
    virtual DNASequence getNewSequence() const = 0;
    GUrl getDocumentPath() const;
    qint64 getPosToInsert() const;
    U1AnnotationUtils::AnnotationStrategyForResize getAnnotationStrategy() const;
    bool mergeAnnotations() const;
    bool recalculateQualifiers() const;
    DocumentFormatId getDocumentFormatId() const;

protected slots:
    void sl_enterPressed();

protected:
    void addInputDataWidgetToLayout(QWidget* w);

    EditSequencDialogConfig config;

private slots:
    void sl_mergeAnnotationsToggled();
    void sl_startPositionliClicked();
    void sl_endPositionliClicked();
    void sl_beforeSlectionClicked();
    void sl_afterSlectionClicked();

private:
    bool modifyCurrentDocument() const;
    void initSaveController();

    QString filter;
    qint64 pos = 1;
    Ui_EditSequenceDialog* ui;
    SaveDocumentController* saveController;

    qint64 seqEndPos = 0;
};

/**
 * Paste or replace some sequence.
 */
class U2GUI_EXPORT EditSequenceDialogController : public EditSequenceDialogVirtualController {
    Q_OBJECT
public:
    EditSequenceDialogController(const EditSequencDialogConfig& cfg, QWidget* p, const QString& helpId);

    void accept() override;

    DNASequence getNewSequence() const override;

private:
    SeqPasterWidgetController* seqPasterWidgetController = nullptr;
};


}  // namespace U2
