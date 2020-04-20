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

#ifndef _U2_GENOMIC_LIBRARY_DIALOG_H_
#define _U2_GENOMIC_LIBRARY_DIALOG_H_

#include <QDialog>

#include <U2Designer/DatasetsController.h>

class Ui_GenomicLibraryDialog;

namespace U2 {
namespace LocalWorkflow {

class SingleDatasetController : public DatasetsController {
public:
    SingleDatasetController(const Dataset &dataset, QObject *parent);
    ~SingleDatasetController();

    void renameDataset(int dsNum, const QString &newName, U2OpStatus &os);
    void deleteDataset(int dsNum);
    void addDataset(const QString &name, U2OpStatus &os);
    void onUrlAdded(URLListController *ctrl, URLContainer *url);

    QWidget *getWigdet();
    const Dataset &getDataset() const;

protected:
    QStringList names() const;
    void checkName(const QString &name, U2OpStatus &os, const QString &exception = "");

private:
    QSet<GObjectType> compatibleObjTypes;
    Dataset dataset;
    URLListController *widgetController;
};

class GenomicLibraryDialog : public QDialog {
    Q_OBJECT
public:
    GenomicLibraryDialog(const Dataset &dataset, QWidget *parent);
    ~GenomicLibraryDialog();

    Dataset getDataset() const;

private:
    Ui_GenomicLibraryDialog *ui;
    SingleDatasetController *singleDatasetController;
};

}   // namespace LocalWorkflow
}   // namespace U2

#endif // _U2_GENOMIC_LIBRARY_DIALOG_H_
