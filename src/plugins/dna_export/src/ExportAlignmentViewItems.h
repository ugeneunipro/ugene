/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include <U2Core/U2Region.h>

#include <U2Gui/ObjectViewModel.h>

namespace U2 {

class ADVSequenceObjectContext;
class AnnotatedDNAView;
class AnnotationSelection;
class Annotation;
class LRegionsSelection;
class MsaEditor;

class ExportAlignmentViewItemsController : public GObjectViewWindowContext {
    Q_OBJECT
public:
    ExportAlignmentViewItemsController(QObject* p);

protected:
    void initViewContext(GObjectViewController* view) override;

    void buildStaticOrContextMenu(GObjectViewController* view, QMenu* menu) override;
};

class MSAExportContext : public QObject {
    Q_OBJECT
public:
    MSAExportContext(MsaEditor* e);
    void buildMenu(QMenu* m);

protected slots:
    void sl_exportNucleicMsaToAmino();
    void sl_exportSelectedMsaRowsToSeparateFiles();

    void updateActions();

private:
    MsaEditor* editor = nullptr;
    QAction* exportNucleicMsaToAminoAction = nullptr;
    QAction* exportMsaToSequenceFileFormatAction = nullptr;
    QAction* exportSelectedMsaRowsToSeparateFilesAction = nullptr;
};

}  // namespace U2
