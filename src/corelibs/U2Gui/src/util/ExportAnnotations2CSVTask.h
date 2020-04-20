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

#ifndef _U2_EXPORT_ANNOTATIONS_2_CSV_TASK_H_
#define _U2_EXPORT_ANNOTATIONS_2_CSV_TASK_H_

#include <U2Core/Task.h>

namespace U2 {

class Annotation;
class DNATranslation;

class U2GUI_EXPORT ExportAnnotations2CSVTask : public Task {
    Q_OBJECT
public:
    ExportAnnotations2CSVTask(const QList<Annotation *> &annotations, const QByteArray &sequence, const QString &seqName,
        const DNATranslation *complementTranslation, bool exportSequence, bool exportSequenceName, const QString &url,
        bool append = false, const QString &sep = ",");

    void                    run();
private:
    QList<Annotation *>     annotations;
    QByteArray              sequence;
    QString                 seqName;
    const DNATranslation *  complementTranslation;
    bool                    exportSequence;
    bool                    exportSequenceName;
    QString                 url;
    bool                    append;
    QString                 separator;

public:
    static const QString    SEQUENCE_NAME;
};

} // namespace U2

#endif // _U2_EXPORT_ANNOTATIONS_2_CSV_TASK_H_
