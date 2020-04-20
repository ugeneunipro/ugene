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

#ifndef _U2_DNA_ASSEMBLEY_DIALOG_H_
#define _U2_DNA_ASSEMBLEY_DIALOG_H_

#include <QStringList>
#include <QVariant>

#include <ui_AssemblyToRefDialog.h>

#include <U2Algorithm/DnaAssemblyMultiTask.h>

#include <U2Core/GUrl.h>


namespace U2 {

class DnaAssemblyAlgRegistry;
class DnaAssemblyAlgorithmMainWidget;
class SaveDocumentController;

class ShortReadsTableItem : public QTreeWidgetItem {

    QComboBox* mateTypeBox;

private:
    void updateState();

public:
    ShortReadsTableItem(QTreeWidget* widget, const QString& url);
    GUrl getUrl() const;
    ShortReadSet::LibraryType getType() const;
    ShortReadSet::MateOrder getOrder() const;
    void setLibraryType(const QString& libraryType);


    static void addItemToTable(ShortReadsTableItem* item, QTreeWidget* treeWidget);
};


class DnaAssemblyDialog : public QDialog, private Ui_AssemblyToRefDialog {
    Q_OBJECT

public:
    DnaAssemblyDialog(QWidget* p = NULL, const QStringList& shortReadsUrls = QStringList(), const QString& refSeqUrl = QString());
    const GUrl getRefSeqUrl();
    const QList<ShortReadSet> getShortReadSets();
    const QString getAlgorithmName();
    const QString getResultFileName();
    bool isPaired() const;
    bool isSamOutput() const;
    bool isPrebuiltIndex() const;
    QMap<QString,QVariant> getCustomSettings();

protected:
    bool eventFilter(QObject *, QEvent *);

private slots:
    void sl_onAddRefButtonClicked();
    void sl_onAddShortReadsButtonClicked();
    void sl_onRemoveShortReadsButtonClicked();
    void sl_onAlgorithmChanged(const QString &text);
    void sl_onSamBoxClicked();
    void sl_onLibraryTypeChanged();
    void sl_formatChanged(const QString &newFormat);

private:
    void updateState();
    void addGuiExtension();
    void accept();
    void initSaveController();
    void buildResultUrl(const QString &refUrl);

    const DnaAssemblyAlgRegistry*   assemblyRegistry;
    DnaAssemblyAlgorithmMainWidget* customGUI;
    SaveDocumentController *        saveController;

    static QString                  lastRefSeqUrl;
    static QStringList              lastShortReadsUrls;
    static QString                  methodName;
    static bool                     prebuiltIndex;
    static bool                     samOutput;
};

} // namespace

#endif //  _U2_DNA_ASSEMBLEY_DIALOG_H_
