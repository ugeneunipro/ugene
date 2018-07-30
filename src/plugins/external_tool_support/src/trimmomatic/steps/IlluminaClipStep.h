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

#ifndef _U2_ILLUMINA_CLIP_STEP_H_
#define _U2_ILLUMINA_CLIP_STEP_H_

#include "ui_IlluminaClipAdditionalSettingsDialog.h"
#include "ui_IlluminaClipSettingsWidget.h"
#include "trimmomatic/TrimmomaticStep.h"

namespace U2 {
namespace LocalWorkflow {

class IlluminaClipStep : public TrimmomaticStep {
    Q_OBJECT
public:
    IlluminaClipStep();

    TrimmomaticStepSettingsWidget* createWidget() const;

private:
    QString serializeState(const QVariantMap &widgetState) const;
    QVariantMap parseState(const QString &command) const;
};

class IlluminaClipSettingsWidget : public TrimmomaticStepSettingsWidget, private Ui_IlluminaClipSettingsWidget {
    Q_OBJECT
public:
    IlluminaClipSettingsWidget();
    ~IlluminaClipSettingsWidget();

    bool validate() const;
    QVariantMap getState() const;
    void setState(const QVariantMap &state);

    static const QString FASTA_WITH_ADAPTERS_ETC;
    static const QString SEED_MISMATCHES;
    static const QString PALINDROME_CLIP_THRESHOLD;
    static const QString SIMPLE_CLIP_THRESHOLD;

private slots:
    void sl_browseButtonClicked();
    void sl_optionalButtonClicked();

private:
    QVariantMap additionalOptions;

    static const QString DEFAULT_SE_ADAPTERS;
    static const QString DEFAULT_PE_ADAPTERS;
};

class IlluminaClipAdditionalSettingsDialog : public QDialog, public Ui_IlluminaClipAdditionalSettingsDialog {
    Q_OBJECT
public:
    IlluminaClipAdditionalSettingsDialog(const QVariantMap &widgetState, QWidget* parent = NULL);

    static QVariantMap extractState(const QVariantMap &fromState);
    QVariantMap getState() const;

    static const QString ADDITIONAL_SETTINGS_ENABLED;
    static const QString MIN_ADAPTER_LENGTH;
    static const QString KEEP_BOTH_READS;
};

class IlluminaClipStepFactory : public TrimmomaticStepFactory {
public:
    static const QString ID;

    IlluminaClipStepFactory();

    IlluminaClipStep *createStep() const;
};

}   // namespace LocalWorkflow
}   // namespace U2

#endif // _U2_ILLUMINA_CLIP_STEP_H_
