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

#ifndef _U2_GT_RUNNABLES_EXPORT_IMAGE_DIALOG_FILLER_H_
#define _U2_GT_RUNNABLES_EXPORT_IMAGE_DIALOG_FILLER_H_

#include <QDir>

#include <utils/GTUtilsDialog.h>

#include <U2Core/U2Region.h>

namespace U2 {
using namespace HI;

class ExportImage : public Filler {
public:
    ExportImage(HI::GUITestOpStatus &os, const QString &filePath, const QString &comboValue = "", int spinValue = 0);
    ExportImage(HI::GUITestOpStatus &os, CustomScenario *scenario);

    void commonScenario();

protected:
    const QString filePath;
    const QString comboValue;
    int spinValue;
};

class CircularViewExportImage : public Filler {
public:
    CircularViewExportImage(HI::GUITestOpStatus &_os, QString _filePath, QString _comboValue = "", QString exportedSequenceName = "", int _spinValue=0) : Filler(_os, "ImageExportForm"),
        filePath(QDir::toNativeSeparators(_filePath)),
        comboValue(_comboValue),
        spinValue(_spinValue),
        exportedSequenceName(exportedSequenceName)
    {}
    CircularViewExportImage(HI::GUITestOpStatus &os, CustomScenario *scenario) : Filler(os, "ImageExportForm", scenario), spinValue(0) {}
    void commonScenario();
protected:
    QString filePath, comboValue;
    int spinValue;
    QString exportedSequenceName;
};

struct RegionMsa {
    RegionMsa(const U2Region &region = U2Region(),
              const QStringList &seqList = QStringList())
        : region(region),
          sequences(seqList) {}
    U2Region    region;
    QStringList sequences;
};

class ExportMsaImage : public ExportImage {
public:
    struct Settings {
        Settings(bool includeNames = false,
                 bool includeConsensus = false,
                 bool includeRuler = true)
            : includeNames(includeNames),
              includeConsensus(includeConsensus),
              includeRuler(includeRuler) {}
        bool includeNames;
        bool includeConsensus;
        bool includeRuler;
    };

    // default
    ExportMsaImage(HI::GUITestOpStatus &os, QString filePath,
                   QString comboValue = "",int spinValue = 0)
        : ExportImage(os, filePath, comboValue, spinValue),
          exportWholeAlignment(true),
          exportCurrentSelection(false)
    {}

    //  exportWholeAlignment = false,   exportCurrentSelection = false  : export of specified msa region, there should be no any selection on msa
    //  exportWholeAlignment = false,   exportCurrentSelection = true   : export of currently selected region, selection must be
    //  exportWholeAlignment = true,    exportCurrentSelection = false  : whole selection export
    //  exportWholeAlignment = true,    exportCurrentSelection = true   : error
    ExportMsaImage(HI::GUITestOpStatus &os, QString filePath,
                   Settings settings,
                   bool exportWholeAlignment = true,
                   bool exportCurrentSelection = false,
                   RegionMsa region = RegionMsa(),
                   QString comboValue = "", int spinValue = 0)
        : ExportImage(os, filePath, comboValue, spinValue),
          settings(settings),
          exportWholeAlignment(exportWholeAlignment),
          exportCurrentSelection(exportCurrentSelection),
          region(region) {}

    void commonScenario();

private:
    Settings    settings;
    bool        exportWholeAlignment;
    bool        exportCurrentSelection;
    RegionMsa   region;
};

class ExportSequenceImage : public ExportImage {
public:
    enum ExportType {
        CurrentView,
        ZoomedView,
        DetailsView
    };

    struct Settings {
        Settings(ExportType type = CurrentView, const U2Region r = U2Region())
            : type(type),
              region(r) {}
        ExportType  type;
        U2Region    region;
    };

    ExportSequenceImage(HI::GUITestOpStatus &os, QString filePath,
                        QString comboValue = "", int spinValue = 0)
        : ExportImage(os, filePath, comboValue, spinValue) {}

    ExportSequenceImage(HI::GUITestOpStatus &os, QString filePath,
                        Settings settings,
                        QString comboValue = "", int spinValue = 0)
        : ExportImage(os, filePath, comboValue, spinValue),
          settings(settings) {}

    void commonScenario();

private:
    Settings    settings;
};

class SelectSubalignmentFiller : public Filler {
public:
    SelectSubalignmentFiller(HI::GUITestOpStatus &_os, const RegionMsa &regionMsa)
        : Filler(_os, "SelectSubalignmentDialog"),
          msaRegion(regionMsa) {}
    void commonScenario();
private:
    RegionMsa   msaRegion;
};

class ImageExportFormFiller : public Filler {
public:
    class Parameters {
    public:
        Parameters():
            simpleOverviewChecked(true),
            graphOverviewChecked(true),
            fileName(""),
            format("png"){}

        bool simpleOverviewChecked;
        bool graphOverviewChecked;
        QString fileName;
        QString format;
    };

    ImageExportFormFiller(HI::GUITestOpStatus &os, const Parameters &parameters);
    void commonScenario();

private:
    Parameters parameters;
};

}

#endif //_U2_GT_RUNNABLES_EXPORT_IMAGE_DIALOG_FILLER_H_

