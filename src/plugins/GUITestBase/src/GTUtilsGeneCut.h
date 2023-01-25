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

#ifndef _GTUTILS_GENECUT_H_
#define _GTUTILS_GENECUT_H_

#include <GTGlobals.h>

#include <U2Core/U2OpStatus.h>
#include <U2Core/U2Type.h>

class QTableView;

namespace U2 {
using namespace HI;

class GTUtilsGeneCut {
public:
    enum class Steps {
        OptimizeCodonContext,
        ExcludeRestrictionSites,
        LongFragmentsAssembly,
        OligonucleotidesAssembly
    };

    enum class FileType {
        Input,
        Result
    };

    enum class Status {
        Completed,
        CompletedWithError,
        Interrupted
    };

    static void login(HI::GUITestOpStatus& os, const QString& email = "genecut@unipro.ru", const QString& password = "genecut_test_password", bool showPassword = false, bool rememberMe = false);
    static void resetPassword(HI::GUITestOpStatus& os, const QString& email = "genecut@unipro.ru");
    static void createNewUser(HI::GUITestOpStatus& os,
                              const QString& email = "genecut@unipro.ru",
                              const QString& password = "genecut_test_password",
                              const QString& passwordConfirm = "genecut_test_password",
                              const QString& firstName = "Genecut",
                              const QString& secondName = "by Unipro" );
    static void selectResultByIndex(HI::GUITestOpStatus& os, int index, bool fetchResults = true);
    static void checkResultInfo(HI::GUITestOpStatus& os, const QString& inputFileName, const QList<Steps>& steps, Status status = Status::Completed);
    static void compareFiles(HI::GUITestOpStatus& os, FileType fileType, const QString& seqenceFilePath, bool sequencesOnly = false);

private:
    static const QMap<Steps, QString> STEP_ENUM_2_STRING;
    static const QMap<Status, QString> STATUS_ENUM_2_STRING;
    static constexpr int GENECUT_MAIN_FORM_INDEX = 2;
    static constexpr int GENECUT_REGISTER_FORM_INDEX = 4;
};

}  // namespace U2

#endif  // _GTUTILS_GENECUT_H_
