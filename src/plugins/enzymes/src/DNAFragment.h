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

#ifndef _U2_DNA_FRAGMENT_H_
#define _U2_DNA_FRAGMENT_H_

#include <QList>

#include <U2Core/AnnotationData.h>
#include <U2Core/U2Region.h>

namespace U2 {

class AnnotationTableObject;
class Annotation;
class DNAAlphabet;
class GObject;
class U2SequenceObject;
class U2OpStatus;

struct DNAFragmentTerm {
    DNAFragmentTerm(const QString& eId, const QByteArray& seq, bool directStrand)
        : enzymeId(eId.toLatin1()), overhang(seq), isDirect(directStrand)
    {

    }

    DNAFragmentTerm()
        : isDirect(true)
    {

    }

    QByteArray enzymeId;
    QByteArray overhang;
    QByteArray type;
    bool isDirect; // overhang strand
};

class DNAFragment {
private:
    // Contains fragment region and cuts info
    SharedAnnotationData annotatedFragment;
    // Parent sequence of the fragment
    U2SequenceObject *dnaObj;
    // Annotations associated with parent sequence
    // When ligate or export fragments they must be saved
    QList<AnnotationTableObject *> relatedAnnotations;
    // reverse complement
    bool reverseCompl;
    void updateTerms();
    void updateLeftTerm();
    void updateRightTerm();
    void toRevCompl(QByteArray& buf);
    void setTermType(const QByteArray& qName, const QByteArray& type);
    void setOverhang(const QByteArray& qName, const QByteArray& overhang);
    DNAFragmentTerm leftTerm, rightTerm;

public:
    DNAFragment() : annotatedFragment(NULL), dnaObj(NULL), reverseCompl(false) {}
    DNAFragment(const SharedAnnotationData &fragment, U2SequenceObject* sObj, const QList<AnnotationTableObject *> relatedAnns);
    DNAFragment(const DNAFragment& other);
    DNAFragment& operator=(const DNAFragment& other);
    bool isEmpty() const { return annotatedFragment.data() == NULL || dnaObj == NULL; }
    QString getName() const;
    QString getSequenceName() const;
    QString getSequenceDocName() const;
    QVector<U2Region> getFragmentRegions() const;
    QByteArray getSequence(U2OpStatus &os) const;
    int getLength() const;
    bool isInverted() const { return reverseCompl; }
    const DNAAlphabet* getAlphabet() const;
    QByteArray getSourceSequence(U2OpStatus &os) const;
    QByteArray getSourceSequenceRegion(const U2Region region, U2OpStatus& os) const;
    const DNAFragmentTerm& getLeftTerminus() const;
    const DNAFragmentTerm& getRightTerminus() const;
    void setInverted(bool inverted = true);
    void setRightTermType(const QByteArray& termType);
    void setLeftTermType(const QByteArray& termType);
    void setLeftOverhang(const QByteArray& overhang);
    void setRightOverhang(const QByteArray& overhang);
    void setLeftOverhangStrand(bool direct);
    void setRightOverhangStrand(bool direct);

    const QList<AnnotationTableObject *> & getRelatedAnnotations() const { return relatedAnnotations; }

    static QList<DNAFragment> findAvailableFragments();
    static QList<DNAFragment> findAvailableFragments(const QList<GObject*>& aObjects, const QList<GObject*>& sObjects);
};

} //namespace U2o

#endif // _U2_DNA_FRAGMENT_H_
