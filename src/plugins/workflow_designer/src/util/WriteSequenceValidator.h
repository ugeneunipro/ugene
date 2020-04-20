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

#ifndef _U2_WRITESEQUENCEVALIDATOR_H_
#define _U2_WRITESEQUENCEVALIDATOR_H_

#include <U2Lang/IntegralBusModel.h>

namespace U2 {
namespace Workflow {

class WriteSequenceValidator : public ScreenedParamValidator {
public:
    WriteSequenceValidator(const QString &attr, const QString &port, const QString &slot);
    virtual bool validate(const Configuration *cfg, NotificationsList &notificationList) const;

    static DocumentFormat * getFormatSafe(const Actor *actor);
    static bool isAnnotationsSupported(const DocumentFormat *format);

private:
    bool isAnnotationsBinded(const Actor *actor) const;
};

class WriteSequencePortValidator : public PortValidator {
public:
    bool validate(const IntegralBusPort *port, NotificationsList &notificationList) const;
};

} // Workflow
} // U2

#endif // _U2_WRITESEQUENCEVALIDATOR_H_
