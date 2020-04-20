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

#ifndef GENOMEASSEMBLYDIALOG_H
#define GENOMEASSEMBLYDIALOG_H

#include <U2Core/GUrl.h>

#include <QVariant>
#include <QStringList>
#include <QWidget>

#include <ui_GenomeAssemblyDialog.h>

namespace U2 {

class GenomeAssemblyDialog;
class GenomeAssemblyTaskSettings;
class GenomeAssemblyAlgRegistry;
class GenomeAssemblyAlgorithmMainWidget;
class AssemblyReads;

class ReadPropertiesItem : public QTreeWidgetItem {
public:
    ReadPropertiesItem(QTreeWidget* widget);
    QString getNumber() const;
    QString getType() const;
    QString getOrientation() const;
    void setLibraryType( const QString& libraryType );

    static void addItemToTable(ReadPropertiesItem* item, QTreeWidget* treeWidget);

private:
    QComboBox* typeBox;
    QComboBox* orientationBox;
};

class GenomeAssemblyDialog : public QDialog, public Ui_GenomeAssemblyDialog {
    Q_OBJECT

public:
    GenomeAssemblyDialog(QWidget* p = NULL);
    const QString getAlgorithmName();
    const QString getOutDir();
    QList<AssemblyReads> getReads();
    QMap<QString,QVariant> getCustomSettings();

    void updateProperties();

    void addReads(QStringList fileNames, QTreeWidget* readsWidget);
private slots:
    void sl_onAddShortReadsButtonClicked();
    void sl_onRemoveShortReadsButtonClicked();
    void sl_onOutDirButtonClicked();
    void sl_onAlgorithmChanged(const QString &text);
    void sl_onLibraryTypeChanged();

private:
    void updateState();
    void addGuiExtension();
    void accept();

    const GenomeAssemblyAlgRegistry*   assemblyRegistry;
    GenomeAssemblyAlgorithmMainWidget* customGUI;
    static QString                     methodName;
    static QString                     library;
};

} // namespace


#endif // GENOMEASSEMBLYDIALOG_H
