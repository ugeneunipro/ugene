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

#ifndef _U2_EXPORT_PROJECT_VIEW_ITEMS_H_
#define _U2_EXPORT_PROJECT_VIEW_ITEMS_H_

#include <QObject>

class QAction;
class QMenu;

namespace U2 {

class GObject;
class GUrl;

class ExportProjectViewItemsContoller : public QObject {
    Q_OBJECT
public:
    ExportProjectViewItemsContoller(QObject* p);

private slots:
    void sl_addToProjectViewMenu(QMenu&);

    void sl_saveSequencesToSequenceFormat();
    void sl_saveCorrespondingSequence();
    void sl_saveSequencesAsAlignment();
    void sl_saveAlignmentAsSequences();
    void sl_exportMcaToMsa();
    void sl_exportNucleicAlignmentToAmino();
    void sl_importAnnotationsFromCSV();
    void sl_exportChromatogramToSCF();
    void sl_exportAnnotations();
    void sl_exportSequenceQuality();
    void sl_exportObject();

private:
    void addExportImportMenu(QMenu& m);
    void exportSequences(const QList<GObject *> &seqs);

    QAction* exportSequencesToSequenceFormatAction;
    QAction* exportCorrespondingSeqsAction;
    QAction* exportSequencesAsAlignmentAction;
    QAction* exportAlignmentAsSequencesAction;
    QAction* exportMcaToMsaAction;
    QAction* exportNucleicAlignmentToAminoAction;
    QAction* importAnnotationsFromCSVAction;
    QAction* exportDNAChromatogramAction;
    QAction* exportAnnotations2CSV;
    QAction* exportSequenceQuality;
    QAction* exportObjectAction;
};

} // namespace U2

#endif
