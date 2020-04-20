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

#ifndef _U2_BUILD_INDEX_DIALOG_H_
#define _U2_BUILD_INDEX_DIALOG_H_

#include <U2Core/GUrl.h>
#include <ui_BuildIndexFromRefDialog.h>

#include <QVariant>

namespace U2 {

class DnaAssemblyAlgRegistry;
class DnaAssemblyAlgorithmBuildIndexWidget;

class BuildIndexDialog : public QDialog, private Ui_BuildIndexFromRefDialog    {
    Q_OBJECT

public:
    BuildIndexDialog(const DnaAssemblyAlgRegistry* registry, QWidget* p = NULL);
    const GUrl getRefSeqUrl();
    const QString getAlgorithmName();
    const QString getIndexFileName();
    QMap<QString,QVariant> getCustomSettings();

private:
    const DnaAssemblyAlgRegistry *assemblyRegistry;
    DnaAssemblyAlgorithmBuildIndexWidget *customGUI;
    static QString genomePath;
    void buildIndexUrl(const GUrl& url);
    void updateState();
    void addGuiExtension();
    void accept();

private slots:
    void sl_onAddRefButtonClicked();
    void sl_onSetIndexFileNameButtonClicked();
    void sl_onAlgorithmChanged(const QString &text);
};

} // namespace

#endif //  _U2_BUILD_INDEX_DIALOG_H_
