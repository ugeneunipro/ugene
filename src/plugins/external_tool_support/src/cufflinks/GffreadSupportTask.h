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

#ifndef _U2_GFFRAD_SUPPORT_TASK_H_
#define _U2_GFFRAD_SUPPORT_TASK_H_

#include <U2Core/Task.h>
#include <U2Core/U2IdTypes.h>
#include <U2Core/ExternalToolRunTask.h>

namespace U2 {

class GffreadSettings {
public:
    QString transcriptsUrl; // GTF
    QString genomeUrl; // FASTA (mutli-FASTA)
    QString outputUrl; // FASTA

    QStringList getArguments() const;
};

class GffreadSupportTask : public ExternalToolSupportTask {
    Q_OBJECT
public:
    GffreadSupportTask(const GffreadSettings &settings);

    void prepare();
    QString result() const;

private:
    void checkFormat(const QString &url, const DocumentFormatId &target);

private:
    GffreadSettings settings;
};

} // U2

#endif // _U2_GFFRAD_SUPPORT_TASK_H_
