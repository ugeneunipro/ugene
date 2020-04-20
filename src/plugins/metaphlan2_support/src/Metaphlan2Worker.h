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

#ifndef _U2_METAPHLAN2_WORKER_H_
#define _U2_METAPHLAN2_WORKER_H_

#include <U2Lang/LocalDomain.h>

#include "Metaphlan2Task.h"

namespace U2 {
namespace LocalWorkflow {

class Metaphlan2Worker : public BaseWorker {
    Q_OBJECT
public:
    Metaphlan2Worker(Actor* actor);

    void init();
    Task* tick();
    void cleanup();

private slots:
    void sl_taskFinished(Task* task);

private:
    enum Output{
        Bowtie2,
        MetaPhlAn2
    };
    bool isReadyToRun() const;
    bool dataFinished() const;
    Metaphlan2TaskSettings getSettings(U2OpStatus &os);
    QString getDefaultOutputDir() const;
    QString createOutputToolDirectory(const QString& tmpDir,
                                      const Message& message,
                                      const bool isPairedEnd,
                                      const Output out) const;
    void createDirectory(QString& dir) const;
    void addOutputToDashboard(const QString& outputUrl, const QString& outputName) const;

    IntegralBus *input;

    static const QString METAPHLAN2_ROOT_DIR;
    static const QString BOWTIE2OUT_DIR;
    static const QString BOWTIE2OUT_SUFFIX;
    static const QString PROFILE_DIR;
    static const QString PROFILE_SUFFIX;
};

}   // namespace LocalWorkflow
}   // namespace U2

#endif // _U2_METAPHLAN2_WORKER_H_
