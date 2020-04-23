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

#ifndef _U2_ABSTRACT_ALIGNMENT_TASK_H_
#define _U2_ABSTRACT_ALIGNMENT_TASK_H_

#include <U2Core/global.h>
#include <U2Core/U2Type.h>
#include <U2Core/Task.h>
#include <U2Core/GUrl.h>
#include <U2Core/U2Alphabet.h>

#include <QByteArray>
#include <QString>
#include <QVariantMap>

#include <U2Core/DNASequence.h>

namespace U2 {

class U2ALGORITHM_EXPORT AbstractAlignmentTaskSettings {
public:
    AbstractAlignmentTaskSettings();
    AbstractAlignmentTaskSettings(const QVariantMap& someSettings);
    AbstractAlignmentTaskSettings(const AbstractAlignmentTaskSettings &s);
    virtual ~AbstractAlignmentTaskSettings();

    void appendCustomSettings(const QVariantMap& settings);
    QVariant getCustomValue(const QString& optionName, const QVariant& defaultVal) const;
    void setCustomValue(const QString& optionName, const QVariant& val);
    virtual bool convertCustomSettings();
    virtual bool isValid() const;

    QString algorithmName;
    QString realizationName;
    bool inNewWindow;
    U2EntityRef msaRef;                     //couldn't be in customSettings. Set manually.
    U2AlphabetId alphabet;                  //couldn't be in customSettings. Set manually.
    GUrl resultFileName;                    //could be in customSettings as a string only.

    static const QString RESULT_FILE_NAME;
    static const QString ALGORITHM_NAME;
    static const QString REALIZATION_NAME;
    static const QString IN_NEW_WINDOW;
    static const QString ALPHABET;

protected:
    QVariantMap customSettings;
};

class U2ALGORITHM_EXPORT AbstractAlignmentTask : public Task {
    Q_OBJECT

public:
    AbstractAlignmentTask(const QString& taskName, TaskFlags flags);
};

class U2ALGORITHM_EXPORT AbstractAlignmentTaskFactory {
public:
    virtual AbstractAlignmentTask* getTaskInstance(AbstractAlignmentTaskSettings* settings) const = 0;
    virtual ~AbstractAlignmentTaskFactory();

};

}   //namespace

#endif // _U2_ABSTRACT_ALIGNMENT_TASK_H_
