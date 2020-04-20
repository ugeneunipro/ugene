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

#ifndef _U2_CLONING_UTIL_TASKS_H_
#define _U2_CLONING_UTIL_TASKS_H_

#include <U2Algorithm/EnzymeModel.h>
#include "DNAFragment.h"

#include <limits>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/Task.h>
#include <U2Core/U2Region.h>
#include <U2Core/GUrl.h>

#include <QObject>
#include <QList>
#include <QMutex>

namespace U2 {

class Document;
class U2SequenceObject;

struct DigestSequenceTaskConfig {
    DigestSequenceTaskConfig() : searchForRestrictionSites(false), forceCircular(false) {}
    QMap<QString,U2Region> conservedRegions;
    QList<SEnzymeData> enzymeData;
    bool searchForRestrictionSites;
    bool forceCircular;
    QString annDescription;
};

struct GenomicPosition {
    GenomicPosition() : coord(-1), directStrand(true) {}
    GenomicPosition(int crd, bool strnd) : coord(crd), directStrand(strnd) {}
    friend bool operator<(const GenomicPosition& left, const GenomicPosition& right);

    int coord;
    bool directStrand;
};

class DigestSequenceTask : public Task {
    Q_OBJECT
public:
    DigestSequenceTask(U2SequenceObject* dnaObj, AnnotationTableObject *destTable,
        AnnotationTableObject *sourceTable, const DigestSequenceTaskConfig& cfg);
    virtual void prepare();
    virtual void run();
    virtual QString generateReport() const;
    virtual ReportResult report();

private:

    void findCutSites();
    void saveResults();
    SharedAnnotationData createFragment(int pos1, const DNAFragmentTerm& leftTerm, int pos2, const DNAFragmentTerm& rightTerm);
    void checkForConservedAnnotations();
    qint64 correctPos(const qint64 pos) const;
    QByteArray getOverhang(const U2Region& region) const;
    bool isCircular;
    U2Region seqRange;
    AnnotationTableObject *sourceObj, *destObj;
    U2SequenceObject *dnaObj;
    DigestSequenceTaskConfig cfg;
    QMap<GenomicPosition, SEnzymeData> cutSiteMap;
    QList<SharedAnnotationData> results;

};

struct LigateFragmentsTaskConfig {
    LigateFragmentsTaskConfig() : annotateFragments(false), checkOverhangs(true), makeCircular(false),
    addDocToProject(true), openView(false), saveDoc(false) {}
    GUrl docUrl;
    QString seqName;
    bool annotateFragments;
    bool checkOverhangs;
    bool makeCircular;
    bool addDocToProject;
    bool openView;
    bool saveDoc;
};

class DNAAlphabet;

class LigateFragmentsTask : public Task {
    Q_OBJECT
public:
    LigateFragmentsTask(const QList<DNAFragment>& fragments, const LigateFragmentsTaskConfig& cfg);
    Document* getResultDocument() {return resultDoc; }
    void prepare();

private:
    static QList<SharedAnnotationData> cloneAnnotationsInRegion(const U2Region& region, AnnotationTableObject* source, int globalOffset);
    static QList<SharedAnnotationData> cloneAnnotationsInFragmentRegion(const DNAFragment& fragment, AnnotationTableObject* source, int globalOffset);
    static SharedAnnotationData createSourceAnnotation(int regLen);
    static SharedAnnotationData createFragmentAnnotation(const DNAFragment &, int startPos);
    void createDocument(const QByteArray& seq, const QList<SharedAnnotationData> &annotations);
    void processOverhangs(const DNAFragment& leftFragment, const DNAFragment& rightFragment, QByteArray& overhangAddition);
    bool overhangsAreConsistent(const DNAFragmentTerm& leftTerm, const DNAFragmentTerm& rightTerm);

    QList<DNAFragment> fragmentList;
    QList<SharedAnnotationData> annotations;
    LigateFragmentsTaskConfig cfg;
    Document* resultDoc;
    const DNAAlphabet* resultAlphabet;
    QMap<AnnotationData,int> offsetMap;
};

} //namespace

#endif // _U2_CLONING_UTIL_TASKS_H_
