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

#ifndef _U2_SEC_STRUCT_PREDICT_TASK_H_
#define _U2_SEC_STRUCT_PREDICT_TASK_H_

#include <U2Core/Task.h>
#include <U2Core/AnnotationData.h>

namespace U2 {

class U2ALGORITHM_EXPORT SecStructPredictTask : public Task {
    Q_OBJECT
public:
    SecStructPredictTask(const QByteArray& seq);
    void prepare();
    const QList<SharedAnnotationData> & getResults() const { return results; }
    const QByteArray getSSFormatResults() const { return output; }

protected:
    QByteArray sequence, output;
    QList<SharedAnnotationData> results;
};


class U2ALGORITHM_EXPORT SecStructPredictTaskFactory {
public:
    virtual SecStructPredictTask* createTaskInstance(const QByteArray& inputSeq) = 0;
    virtual ~SecStructPredictTaskFactory() {}
};

#define SEC_STRUCT_PREDICT_TASK_FACTORY(c) \
public: \
    static const QString taskName; \
    class Factory : public SecStructPredictTaskFactory { \
    public: \
        Factory() { } \
        SecStructPredictTask* createTaskInstance(const QByteArray& inputSeq) { return new c(inputSeq); } \
    };


} //namespace

#endif // _U2_SEC_STRUCT_PREDICT_TASK_H_
