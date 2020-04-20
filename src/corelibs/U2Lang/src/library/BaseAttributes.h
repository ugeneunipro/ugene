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

#ifndef __U2_WORKFLOW_BASE_ATTRIBUTES_H_
#define __U2_WORKFLOW_BASE_ATTRIBUTES_H_

#include <U2Lang/Descriptor.h>

namespace U2 {
namespace Workflow {

class U2LANG_EXPORT BaseAttributes : public QObject {
    Q_OBJECT
public:
    static const Descriptor URL_IN_ATTRIBUTE();
    static const Descriptor URL_OUT_ATTRIBUTE();
    static const Descriptor URL_SUFFIX();
    static const Descriptor URL_LOCATION_ATTRIBUTE();
    static const Descriptor DATABASE_ATTRIBUTE();
    static const Descriptor DB_PATH();
    static const Descriptor DOCUMENT_FORMAT_ATTRIBUTE();
    static const Descriptor ACCUMULATE_OBJS_ATTRIBUTE();
    static const Descriptor READ_BY_LINES_ATTRIBUTE();
    static const Descriptor FILE_MODE_ATTRIBUTE();
    static const Descriptor SPLIT_SEQ_ATTRIBUTE();

    static const Descriptor STRAND_ATTRIBUTE();
    static const QString STRAND_BOTH();
    static const QString STRAND_DIRECT();
    static const QString STRAND_COMPLEMENTARY();
    static const QVariantMap STRAND_ATTRIBUTE_VALUES_MAP();

    static const Descriptor DATA_STORAGE_ATTRIBUTE();
    static const QString LOCAL_FS_DATA_STORAGE();
    static const QString SHARED_DB_DATA_STORAGE();
    static const QVariantMap DATA_STORAGE_ATTRIBUTE_VALUES_MAP();

}; // BaseAttributes

} // Workflow
} // U2

#endif // __U2_WORKFLOW_BASE_ATTRIBUTES_H_
