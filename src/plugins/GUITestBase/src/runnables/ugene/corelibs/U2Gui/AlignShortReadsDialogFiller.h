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

#ifndef _U2_GT_ALIGN_SHORT_READS_DIALOG_FILLER_H_
#define _U2_GT_ALIGN_SHORT_READS_DIALOG_FILLER_H_

#include "utils/GTUtilsDialog.h"
#include <base_dialogs/GTFileDialog.h>

namespace U2 {
using namespace HI;

// Copy file with reference sequence into sandbox: test can create an index for it in the same dir.
// Also, result file will be created in the same dir.
class AlignShortReadsFiller : public Filler {
public:
    class Parameters {
    public:
        enum AlignmentMethod {
            Bwa,
            BwaSw,
            BwaMem,
            Bowtie,
            Bowtie2,
            UgeneGenomeAligner
        };

        enum Library {
            SingleEnd,
            PairedEnd
        };

        Parameters(const QString& refDir = "",
                   const QString& refFileName = "",
                   const QString& readsDir = "",
                   const QString& readsFileName = "",
                   AlignmentMethod alignmentMethod = UgeneGenomeAligner);
        Parameters(const QString &reference,
                   const QStringList &readsFiles,
                   AlignmentMethod alignmentMethod = UgeneGenomeAligner);
        virtual ~Parameters() {}

        const QString getAlignmentMethod() const { return alignmentMethodMap[alignmentMethod]; }
        const QString getLibrary() const { return libraryMap[library]; }

        AlignmentMethod alignmentMethod;
        QString referenceFile;
        QString resultDir;
        QString resultFileName;
        QStringList readsFiles;
        Library library;
        bool prebuiltIndex;
        bool samOutput;
        bool useDefaultResultPath;

    private:
        static QMap<AlignmentMethod, QString> initAlignmentMethodMap();
        static QMap<Library, QString> initLibraryMap();

        static const QMap<AlignmentMethod, QString> alignmentMethodMap;
        static const QMap<Library, QString> libraryMap;
    };

    class Bowtie2Parameters : public Parameters {
    public:
        enum Mode {
            EntToEnd,
            Local
        };

        Bowtie2Parameters(const QString& refDir = "",
                          const QString& refFileName = "",
                          const QString& readsDir = "",
                          const QString& readsFileName = "") :
            Parameters(refDir, refFileName, readsDir, readsFileName, Bowtie2),
            mode(EntToEnd),
            numberOfMismatches(0),
            seedLengthCheckBox(false),
            seedLength(20),
            addColumnsToAllowGapsCheckBox(false),
            addColumnsToAllowGaps(15),
            disallowGapsCheckBox(false),
            disallowGaps(4),
            seedCheckBox(false),
            seed(0),
            threads(4),
            noUnpairedAlignments(false),
            noDiscordantAlignments(false),
            noForwardOrientation(false),
            noReverseComplementOrientation(false),
            noOverlappingMates(false),
            noMatesContainingOneAnother(false) {
            modeMap.insert(EntToEnd, "--end-to-end");
            modeMap.insert(Local, "--local");
        }

        const QString getMode() const { return modeMap[mode]; }

        // Additional parameters:
        Mode mode;
        int numberOfMismatches;
        bool seedLengthCheckBox;
        int seedLength;
        bool addColumnsToAllowGapsCheckBox;
        int addColumnsToAllowGaps;
        bool disallowGapsCheckBox;
        int disallowGaps;
        bool seedCheckBox;
        int seed;
        int threads;

        // Flags:
        bool noUnpairedAlignments;
        bool noDiscordantAlignments;
        bool noForwardOrientation;
        bool noReverseComplementOrientation;
        bool noOverlappingMates;
        bool noMatesContainingOneAnother;

    private:
        QMap<Mode, QString> modeMap;
    };

    class UgeneGenomeAlignerParams : public Parameters {
    public:
        UgeneGenomeAlignerParams(const QString &refDir,
                                 const QString &refFileName,
                                 const QString &readsDir,
                                 const QString &readsFileName,
                                 bool allowMismatches);
        UgeneGenomeAlignerParams(const QString &referenceFile,
                                 const QStringList &readsFiles);

        bool mismatchesAllowed;
        bool useBestMode;
    };

    class BwaSwParameters : public Parameters {
    public:
        BwaSwParameters(const QString &refDir = "",
                        const QString &refFileName = "",
                        const QString &readsDir = "",
                        const QString &readsFileName = "");

        int matchScore;
        int mismatchPenalty;
        int gapOpenPenalty;
        int gapExtensionPenalty;
        int bandWidth;
        double maskLevel;
        int threadsNumber;
        int readsChunkSize;
        int thresholdScore;
        int zBest;
        int seedsNumber;
        bool preferHardClippingInSam;
    };

    class BwaParameters : public Parameters {
    public:
        enum IndexAlgorithm {
            Autodetect,
            Bwtsw,
            Div,
            Is
        };

        BwaParameters(const QString &referenceFile,
                      const QStringList &readsFiles);
        BwaParameters(const QString &referenceFile,
                      const QString &readsFile);


        QString getIndexAlgorithmString() const;

        IndexAlgorithm indexAlgorithm;

    private:
        static QMap<IndexAlgorithm, QString> initIndexAlgorithmMap();
        static const QMap<IndexAlgorithm, QString> indexAlgorithmMap;
    };

    AlignShortReadsFiller(HI::GUITestOpStatus &os, Parameters* parameters) :
        Filler(os, "AssemblyToRefDialog"),
        parameters(parameters) {
        CHECK_SET_ERR(parameters, "Invalid filler parameters: NULL pointer");
    }
    AlignShortReadsFiller(HI::GUITestOpStatus &os, CustomScenario* c): Filler(os, "AssemblyToRefDialog", c), parameters(NULL) {}

    virtual void commonScenario();

private:
    void setCommonParameters(QWidget* dialog);
    void setAdditionalParameters(QWidget* dialog);
    void setBowtie2AdditionalParameters(Bowtie2Parameters* bowtie2Parameters, QWidget* dialog);
    void setUgaAdditionalParameters(UgeneGenomeAlignerParams *ugaParameters, QWidget* dialog);
    void setBwaAdditionalParameters(BwaParameters *bwaParameters, QWidget* dialog);
    void setBwaSwAdditionalParameters(BwaSwParameters *bwaSwParameters, QWidget* dialog);

    Parameters* parameters;
};

}

#endif
