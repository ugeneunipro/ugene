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

#ifndef _U2_BEDTOOLS_INTERSECT_TASK_H_
#define _U2_BEDTOOLS_INTERSECT_TASK_H_

#include <U2Core/ExternalToolRunTask.h>

#include <QFile>

namespace U2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BedtoolIntersectSettings
class BedtoolsIntersectSettings {
public:
    static const double DEFAULT_MIN_OVERLAP;

    enum Report {
        Report_OverlapedA = 0,      // -wa
        Report_NonOverlappedA = 1,  // -v
        Report_Intervals = 2
    };

    BedtoolsIntersectSettings(double minOverlap = DEFAULT_MIN_OVERLAP, bool unique = true, Report r = Report_OverlapedA)
        : minOverlap(minOverlap),
          unique(unique),
          report(r)
    {}

    double  minOverlap;     // -f (0..1]
    bool    unique;         // -u
    Report report;
};

class AnnotationTableObject;

class BedtoolsIntersectByEntityRefSettings : public BedtoolsIntersectSettings {
public:
    BedtoolsIntersectByEntityRefSettings()
        : BedtoolsIntersectSettings() {}

    BedtoolsIntersectByEntityRefSettings(const QList<U2EntityRef>& entityA, const QList<U2EntityRef>& entityB,
                                         double minOverlap = DEFAULT_MIN_OVERLAP, bool unique = true, Report r = Report_OverlapedA)
        : BedtoolsIntersectSettings(minOverlap, unique, r),
          entitiesA(entityA),
          entitiesB(entityB) {}

    QList<U2EntityRef> entitiesA; // -a
    QList<U2EntityRef> entitiesB; // -b
};

class BedtoolsIntersectFilesSettings : public BedtoolsIntersectSettings {
public:
    BedtoolsIntersectFilesSettings(const QString &inputA, const QStringList &inputB, const QString &output,
                              double minOverlap = DEFAULT_MIN_OVERLAP, bool unique = true, Report r = Report_OverlapedA)
        : BedtoolsIntersectSettings(minOverlap, unique, r),
          inputA(inputA),
          inputB(inputB),
          out(output) {}

    QString     inputA; // -a
    QStringList inputB; // -b
    QString     out;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BedToolsIntersectTask & BedtoolsIntersectLogParser
class BedtoolsIntersectTask : public ExternalToolSupportTask {
    Q_OBJECT
public:
    BedtoolsIntersectTask(const BedtoolsIntersectFilesSettings &settings);
    void prepare();

protected:
    const QStringList getParameters() const;

private:
    BedtoolsIntersectFilesSettings settings;
};

class BedtoolsIntersectLogParser : public ExternalToolLogParser {
public:
    BedtoolsIntersectLogParser(const QString &resultFile);

    void parseOutput(const QString &partOfLog);
    void parseErrOutput(const QString &/*partOfLog*/) {}
private:
    QFile result;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IntersectAnnotationsTask
class SaveMultipleDocuments;
class LoadDocumentTask;

class BedtoolsIntersectAnnotationsByEntityTask : public ExternalToolSupportTask {
    Q_OBJECT
public:
    BedtoolsIntersectAnnotationsByEntityTask(const BedtoolsIntersectByEntityRefSettings& settings);
    void prepare();
    QList<Task*> onSubTaskFinished(Task *subTask);

    QList<GObject*>  getResult() { return result; }

private:
    Document* createAnnotationsDocument(const QString& url, const QList<U2EntityRef>& enities);

    BedtoolsIntersectByEntityRefSettings    settings;
    QList<GObject*>                         result;

    QString tmpUrlA;
    QString tmpUrlB;
    QString tmpUrlResult;

    SaveMultipleDocuments*          saveAnnotationsTask;
    BedtoolsIntersectTask*          intersectTask;
    LoadDocumentTask*               loadResultTask;
};

} // namespace

#endif // _U2_BEDTOOLS_INTERSECT_TASK_H_
