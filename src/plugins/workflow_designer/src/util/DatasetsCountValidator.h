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

#ifndef _U2_DATASETSCOUNTVALIDATOR_H_
#define _U2_DATASETSCOUNTVALIDATOR_H_

#include <U2Lang/ActorModel.h>

namespace U2 {
namespace Workflow {

/**
 * The options of the validator:
 * min - the minimum count of datasets [0]
 * max - the maximum count of datasets [INT_MAX]
 * attribute - the datasets attribute id [url-in]
 */
class DatasetsCountValidator : public ActorValidator {
public:
    virtual bool validate(const Actor *actor, NotificationsList &notificationList, const QMap<QString, QString> &options) const;
    static const QString ID;
};

} // Workflow
} // U2

#endif // _U2_DATASETSCOUNTVALIDATOR_H_
