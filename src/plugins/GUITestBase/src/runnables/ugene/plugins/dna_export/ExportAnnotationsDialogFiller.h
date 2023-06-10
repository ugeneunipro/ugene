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

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

class ExportAnnotationsFiller : public Filler {
public:
    enum FileFormat {
        bed,
        genbank,
        gff,
        gtf,
        csv,
        ugenedb
    };

    ExportAnnotationsFiller(const QString& exportToFile, const FileFormat& format);

    ExportAnnotationsFiller(
        const QString& _exportToFile,
        const FileFormat& _format,
        bool _addToProject,
        bool _saveSequencesUnderAnnotations = true,
        bool _saveSequenceNames = true,
        GTGlobals::UseMethod method = GTGlobals::UseMouse);
    ExportAnnotationsFiller(CustomScenario* scenario);

    void commonScenario();

private:
    void init(const QString& exportToFile);

    bool softMode;
    QString exportToFile;
    FileFormat format;
    QMap<FileFormat, QString> comboBoxItems;
    bool addToProject;
    bool saveSequencesUnderAnnotations;
    bool saveSequenceNames;

    GTGlobals::UseMethod useMethod;
};

}  // namespace U2
