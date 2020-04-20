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

#ifndef _U2_STRINGTIE_GENE_ABUNDANCE_REPORT_WORKER_H_
#define _U2_STRINGTIE_GENE_ABUNDANCE_REPORT_WORKER_H_

#include <U2Lang/LocalDomain.h>

namespace U2 {
namespace LocalWorkflow {

class StringtieGeneAbundanceReportWorker : public BaseWorker {
    Q_OBJECT
public:
    StringtieGeneAbundanceReportWorker(Actor* actor);

    void init();
    Task *tick();
    void cleanup();

private slots:
    void sl_taskSucceeded(Task *task);

private:
    IntegralBus * input;
    QStringList stringtieReports;

    static const QString OUTPUT_DIR;
};

}   // namespace LocalWorkflow
}   // namespace U2

#endif // _U2_STRINGTIE_GENE_ABUNDANCE_REPORT_WORKER_H_
