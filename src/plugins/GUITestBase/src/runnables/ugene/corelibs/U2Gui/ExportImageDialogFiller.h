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

#include <utils/GTUtilsDialog.h>

#include <QDir>

#include <U2Core/U2Region.h>

namespace U2 {
using namespace HI;

class ExportImage : public Filler {
public:
    ExportImage(const QString& filePath, const QString& comboValue = "", int spinValue = 0);
    ExportImage(CustomScenario* scenario);

    void commonScenario();

protected:
    const QString filePath;
    const QString comboValue;
    int spinValue;
};

class CircularViewExportImage : public Filler {
public:
    CircularViewExportImage(QString _filePath, QString _comboValue = "", QString exportedSequenceName = "", int _spinValue = 0)
        : Filler("ImageExportForm"),
          filePath(QDir::toNativeSeparators(_filePath)),
          comboValue(_comboValue),
          spinValue(_spinValue),
          exportedSequenceName(exportedSequenceName) {
    }
    CircularViewExportImage(CustomScenario* scenario)
        : Filler("ImageExportForm", scenario), spinValue(0) {
    }
    void commonScenario();

protected:
    QString filePath, comboValue;
    int spinValue;
    QString exportedSequenceName;
};

struct RegionMsa {
    RegionMsa(const U2Region& region = U2Region(),
              const QStringList& seqList = QStringList())
        : region(region),
          sequences(seqList) {
    }
    U2Region region;
    QStringList sequences;
};

class ExportMsaImage : public ExportImage {
public:
    struct Settings {
        Settings(bool includeNames = false,
                 bool includeConsensus = false,
                 bool includeRuler = true,
                 bool multilineMode = false,
                 int basesPerLine = 0)
            : includeNames(includeNames),
              includeConsensus(includeConsensus),
              includeRuler(includeRuler),
              multilineMode(multilineMode),
              basesPerLine(basesPerLine) {
        }
        bool includeNames;
        bool includeConsensus;
        bool includeRuler;
        bool multilineMode;
        int basesPerLine;
    };

    // default
    ExportMsaImage(QString filePath, QString comboValue = "", int spinValue = 0)
        : ExportImage(filePath, comboValue, spinValue) {
    }

    //  exportWholeAlignment = false,   exportCurrentSelection = false  : export of specified msa region, there should be no any selection on msa
    //  exportWholeAlignment = false,   exportCurrentSelection = true   : export of currently selected region, selection must be
    //  exportWholeAlignment = true,    exportCurrentSelection = false  : whole selection export
    //  exportWholeAlignment = true,    exportCurrentSelection = true   : error
    ExportMsaImage(QString filePath, Settings settings, bool exportWholeAlignment = true, bool exportCurrentSelection = false, RegionMsa region = RegionMsa(), QString comboValue = "", int spinValue = 0)
        : ExportImage(filePath, comboValue, spinValue),
          settings(settings),
          exportWholeAlignment(exportWholeAlignment),
          exportCurrentSelection(exportCurrentSelection),
          region(region) {
    }

    void commonScenario() override;

private:
    Settings settings;
    bool exportWholeAlignment = true;
    bool exportCurrentSelection = false;
    RegionMsa region;
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
              region(r) {
        }
        ExportType type;
        U2Region region;
    };

    ExportSequenceImage(QString filePath, QString comboValue = "", int spinValue = 0)
        : ExportImage(filePath, comboValue, spinValue) {
    }

    ExportSequenceImage(QString filePath, Settings settings, QString comboValue = "", int spinValue = 0)
        : ExportImage(filePath, comboValue, spinValue),
          settings(settings) {
    }

    void commonScenario();

private:
    Settings settings;
};

class SelectSubalignmentFiller : public Filler {
public:
    SelectSubalignmentFiller(const RegionMsa& regionMsa)
        : Filler("SelectSubalignmentDialog"),
          msaRegion(regionMsa) {
    }
    void commonScenario();

private:
    RegionMsa msaRegion;
};

class ImageExportFormFiller : public Filler {
public:
    class Parameters {
    public:
        Parameters()
            : simpleOverviewChecked(true),
              graphOverviewChecked(true),
              fileName(""),
              format("png") {
        }

        bool simpleOverviewChecked;
        bool graphOverviewChecked;
        QString fileName;
        QString format;
    };

    ImageExportFormFiller(const Parameters& parameters);
    void commonScenario();

private:
    Parameters parameters;
};

}  // namespace U2
