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

#ifndef _U2_FIND_ENZYMES_TASK_H_
#define _U2_FIND_ENZYMES_TASK_H_

#include <U2Core/Task.h>
#include <U2Core/U2Region.h>

#include "FindAlgorithm.h"

#include <QMutex>

namespace U2 {

class DNATranslation;

class U2ALGORITHM_EXPORT FindAlgorithmTaskSettings : public FindAlgorithmSettings {
public:
    FindAlgorithmTaskSettings()
        : searchIsCircular(false), countTask(true) {}
    FindAlgorithmTaskSettings(const FindAlgorithmSettings& f)
            : FindAlgorithmSettings(f), searchIsCircular(false), countTask(true) {}

    QByteArray  sequence;
    bool        searchIsCircular;
    QString     name;
    bool        countTask;
};

class U2ALGORITHM_EXPORT FindAlgorithmTask : public Task, public FindAlgorithmResultsListener {
    Q_OBJECT
public:
    FindAlgorithmTask(const FindAlgorithmTaskSettings& s);

    virtual void run();
    virtual void onResult(const FindAlgorithmResult& r);

    QList<FindAlgorithmResult> popResults();

    const FindAlgorithmTaskSettings& getSettings() const {return config;}

private:
    FindAlgorithmTaskSettings config;

    bool    complementRun;

    QList<FindAlgorithmResult> newResults;
    QMutex lock;
};


class Document;

class U2ALGORITHM_EXPORT LoadPatternsFileTask: public Task{
    Q_OBJECT
public:
    LoadPatternsFileTask(const QString &_filePath, const QString &annotationName = QString());
    QList<QPair<QString, QString> > getNamesPatterns(){return namesPatterns;}
    void run();

private:
    Document *getDocumentFromFilePath();

    QString filePath;
    QList<QPair<QString, QString> > namesPatterns;
    bool isRawSequence;
    QString annotationName;
};

} //namespace

#endif
