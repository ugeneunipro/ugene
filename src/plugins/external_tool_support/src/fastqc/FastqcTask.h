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

#ifndef _U2_FASTQC_TASK_H_
#define _U2_FASTQC_TASK_H_

#include <QTemporaryDir>

#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/Task.h>

namespace U2 {

class FastQCSetting {
public:
    FastQCSetting() {
    }

    /** Input file url for FastQC. */
    QString inputFileUrl;

    /** Output dir for the FastQC. Must be created before run. */
    QString outputDirUrl;

    /**
     * Specifies a non-default file which contains the list of adapter sequences.
     * Same as '-a' parameter.
     */
    QString adaptersFileUrl;

    /**
     * A non-default file which contains the list of contaminants.
     * Same as '-c' parameter.
     */
    QString contaminantsFileUrl;

    /** Name of the output file in the outputDir. Optional. If not provided the inputFile base name is used. */
    QString outputFileNameOverride;
};

class FastQCTask : public ExternalToolSupportTask {
    Q_OBJECT
public:
    FastQCTask(const FastQCSetting& settings);

    void prepare() override;
    void run() override;

    const QString& getResult() const {
        return resultFileUrl;
    }

    QString getTmpResultFileUrl() const;

protected:
    QStringList getParameters(U2OpStatus& os) const;

protected:
    FastQCSetting settings;
    QString resultFileUrl;

private:
    QTemporaryDir temporaryDir;
};

class FastQCParser : public ExternalToolLogParser {
    Q_OBJECT
public:
    FastQCParser(const QString& inputFile);

    int getProgress() override;

protected:
    void processErrLine(const QString& line) override;
    void setLastError(const QString& value) override;

private:
    enum ErrorType {
        Common,
        Multiline
    };

    bool isCommonError(const QString& err) const;
    bool isMultiLineError(const QString& err);

    static const QMap<ErrorType, QString> initWellKnownErrors();

    QString lastErrLine;
    QString inputFile;
    int progress;

    static const QMap<ErrorType, QString> WELL_KNOWN_ERRORS;
};

}  // namespace U2

#endif  // _U2_FASTQC_TASK_H_
