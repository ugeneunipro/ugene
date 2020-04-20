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

#ifndef _U2_BWA_SETTINGS_WIDGET_H_
#define _U2_BWA_SETTINGS_WIDGET_H_

#include "ui_BwaSettings.h"
#include "ui_BwaMemSettings.h"
#include "ui_BwaSwSettings.h"
#include "ui_BwaBuildSettings.h"
#include "U2View/DnaAssemblyGUIExtension.h"

class BwaIndexAlgorithmWarningReporter : public QObject {
    Q_OBJECT
public:
    BwaIndexAlgorithmWarningReporter( QObject *parent );
    void setReportingLabel( QLabel *reportLabel );
    void setRefSequencePath( const U2::GUrl &path );

public slots:
    void sl_IndexAlgorithmChanged( int index );

private:
    void setReportLabelStyle( );

    QLabel *reportLabel;
    U2::GUrl referenceSequencePath;
};

namespace U2 {

class BwaSettingsWidget : public DnaAssemblyAlgorithmMainWidget, Ui_BwaSettings {
    Q_OBJECT
public:
    BwaSettingsWidget(QWidget *parent);
    QMap<QString, QVariant> getDnaAssemblyCustomSettings() const;
    void validateReferenceSequence(const GUrl &url) const;

private:
    BwaIndexAlgorithmWarningReporter *warningReporter;
};

class BwaSwSettingsWidget : public DnaAssemblyAlgorithmMainWidget, Ui_BwaSwSettings {
    Q_OBJECT
public:
    BwaSwSettingsWidget(QWidget *parent);
    QMap<QString, QVariant> getDnaAssemblyCustomSettings() const;
    void validateReferenceSequence(const GUrl &url) const;
private:
    BwaIndexAlgorithmWarningReporter *warningReporter;
};

class BwaMemSettingsWidget : public DnaAssemblyAlgorithmMainWidget, Ui_BwaMemSettings {
    Q_OBJECT
public:
    BwaMemSettingsWidget(QWidget *parent);
    QMap<QString, QVariant> getDnaAssemblyCustomSettings() const;
    void validateReferenceSequence(const GUrl &url) const;
private:
    BwaIndexAlgorithmWarningReporter *warningReporter;
};

class BwaBuildSettingsWidget : public DnaAssemblyAlgorithmBuildIndexWidget, Ui_BwaBuildSettings {
    Q_OBJECT
public:
    BwaBuildSettingsWidget(QWidget *parent);
    virtual QMap<QString,QVariant> getBuildIndexCustomSettings();
    virtual QString getIndexFileExtension();
    virtual GUrl buildIndexUrl(const GUrl& url);
    void validateReferenceSequence(const GUrl &url) const;

private:
    BwaIndexAlgorithmWarningReporter *warningReporter;
};

class BwaGUIExtensionsFactory : public DnaAssemblyGUIExtensionsFactory {
    DnaAssemblyAlgorithmMainWidget *createMainWidget(QWidget *parent);
    DnaAssemblyAlgorithmBuildIndexWidget *createBuildIndexWidget(QWidget *parent);
    bool hasMainWidget();
    bool hasBuildIndexWidget();
};

class BwaSwGUIExtensionsFactory : public DnaAssemblyGUIExtensionsFactory {
    DnaAssemblyAlgorithmMainWidget *createMainWidget(QWidget *parent);
    DnaAssemblyAlgorithmBuildIndexWidget *createBuildIndexWidget(QWidget *parent);
    bool hasMainWidget();
    bool hasBuildIndexWidget();
};

class BwaMemGUIExtensionsFactory : public DnaAssemblyGUIExtensionsFactory {
    DnaAssemblyAlgorithmMainWidget *createMainWidget(QWidget *parent);
    DnaAssemblyAlgorithmBuildIndexWidget *createBuildIndexWidget(QWidget *parent);
    bool hasMainWidget();
    bool hasBuildIndexWidget();
};



} // namespace U2

#endif // _U2_BWA_SETTINGS_WIDGET_H_
