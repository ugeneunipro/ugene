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

#ifndef _U2_GT_RUNNABLES_IMPORT_ANNOTATIONS_TO_CSV_DIALOG_FILLER_H_
#define _U2_GT_RUNNABLES_IMPORT_ANNOTATIONS_TO_CSV_DIALOG_FILLER_H_

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

class ImportAnnotationsToCsvFiller : public Filler {
public:
    struct RoleParameter {
        virtual ~RoleParameter() {}
        virtual void ke(){}
    };

    enum fileFormat {BED, EMBL, FPKM, GFF, GTF, Genbank, Swiss_Prot};
    struct RoleColumnParameter {
        RoleColumnParameter(int _column, RoleParameter* _p) : column(_column), parameter(_p){}
        int column;
        RoleParameter* parameter;
    };
    typedef QList<RoleColumnParameter> RoleParameters;

    struct StartParameter : public RoleParameter {
        StartParameter(bool _addOffset, int _numberOfBp=0) : addOffset(_addOffset), numberOfBp(_numberOfBp){}
        bool addOffset;
        int numberOfBp;
    };

    struct EndParameter : public RoleParameter {
        EndParameter(bool _endPos) : endPos(_endPos){}
        bool endPos;
    };

    struct LengthParameter : public RoleParameter {
    };

    struct StrandMarkParameter : public RoleParameter {
        StrandMarkParameter(bool _markValue, QString _markValueName) : markValue(_markValue), markValueName(_markValueName){}
        bool markValue;
        QString markValueName;
    };

    struct IgnoreParameter : public RoleParameter {
    };

    struct NameParameter : public RoleParameter {
    };

    struct QualifierParameter : public RoleParameter {
        QualifierParameter(const QString& _name) : name(_name){}
        QString name;
    };

    ImportAnnotationsToCsvFiller(HI::GUITestOpStatus &_os,
                                 const QString &_fileToRead,
                                 const QString &_resultFile,
                                 ImportAnnotationsToCsvFiller::fileFormat _format,
                                 bool _addResultFileToProject,
                                 bool _columnSeparator,
                                 const QString &_separator,
                                 int _numberOfLines,
                                 const QString &_skipAllLinesStartsWith,
                                 bool _interpretMultipleAsSingle,
                                 bool _removeQuotesButton,
                                 const QString &_defaultAnnotationName,
                                 const RoleParameters& roleParameters = RoleParameters(),
                                 GTGlobals::UseMethod method = GTGlobals::UseMouse);
    ImportAnnotationsToCsvFiller(HI::GUITestOpStatus &_os, CustomScenario* c);

    virtual void commonScenario();

private:
    QString fileToRead;
    QString resultFile;
    fileFormat format;
    bool addResultFileToProject;
    bool columnSeparator;
    int numberOfLines;
    QString separator;
    QString skipAllLinesStartsWith;
    bool interpretMultipleAsSingle;
    bool removeQuotesButton;
    QString defaultAnnotationName;
    QMap<fileFormat, QString> comboBoxItems;
    RoleParameters roleParameters;
    GTGlobals::UseMethod useMethod;
};

class RoleFiller : public Filler {
public:
    RoleFiller(HI::GUITestOpStatus &os, ImportAnnotationsToCsvFiller::RoleParameter* _parameter)
        : Filler(os, "CSVColumnConfigurationDialog"), parameter(_parameter) {}
    RoleFiller(HI::GUITestOpStatus &os, CustomScenario* c):
        Filler(os, "CSVColumnConfigurationDialog", c), parameter(NULL) {}

    virtual void commonScenario();
private:
    ImportAnnotationsToCsvFiller::RoleParameter *parameter;
};

}   // namesapce U2

#endif // _U2_GT_RUNNABLES_IMPORT_ANNOTATIONS_TO_CSV_DIALOG_FILLER_H_
