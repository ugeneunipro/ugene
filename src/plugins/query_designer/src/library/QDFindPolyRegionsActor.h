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

#ifndef _U2_QD_FIND_POLY_ACTOR_H_
#define _U2_QD_FIND_POLY_ACTOR_H_

#include <U2Lang/QDScheme.h>
#include <U2Lang/QueryDesignerRegistry.h>

#include <U2Core/DNASequence.h>


namespace U2 {

class DNATranslation;

class FindPolyRegionsSettings {
public:
    char ch;
    int percent;
    qint64 minLen;
    qint64 offset;
    DNATranslation* complTT;
    QDStrandOption strand;
    FindPolyRegionsSettings() : ch('A'), percent(0), minLen(0), offset(0), complTT(NULL) {}
};

class FindPolyRegionsTask : public Task {
    Q_OBJECT
public:
    FindPolyRegionsTask(const FindPolyRegionsSettings& settings, const DNASequence& sequence)
        : Task(tr("Find base content task"), TaskFlag_None), settings_(settings), sequence_(sequence) {}
    void run();
    QList<SharedAnnotationData> getResultAsAnnotations() const;
private:
    void find(const char* seq,
        qint64 seqLen,
        char ch,
        int percent,
        qint64 len,
        QVector<U2Region>& result
        );
    static QList<SharedAnnotationData> createAnnotations(const QVector<U2Region>& regions, qint64 offset, U2Strand::Direction strand);
private:
    FindPolyRegionsSettings settings_;
    DNASequence sequence_;
    QVector<U2Region> directResults;
    QVector<U2Region> compResults;
};

class QDFindPolyActor : public QDActor {
    Q_OBJECT
public:
    QDFindPolyActor(QDActorPrototype const* proto);
    int getMinResultLen() const;
    int getMaxResultLen() const;
    QString getText() const;
    Task* getAlgorithmTask(const QVector<U2Region>& location);
    QColor defaultColor() const { return QColor(0xff, 0xc6, 0); }
private slots:
    void sl_onTaskFinished(Task*);
};

class QDFindPolyActorPrototype : public QDActorPrototype {
public:
    QDFindPolyActorPrototype();
    virtual QDActor* createInstance() const { return new QDFindPolyActor(this); }
};

} //namespace

#endif
