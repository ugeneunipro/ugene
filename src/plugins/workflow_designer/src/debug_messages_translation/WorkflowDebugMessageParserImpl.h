/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

#include <U2Lang/WorkflowDebugMessageParser.h>

namespace U2 {

class BaseMessageTranslator;
class GObject;

class WorkflowDebugMessageParserImpl : public WorkflowDebugMessageParser {
public:
    WorkflowInvestigationData getAllMessageValues();
    void convertMessagesToDocuments(const QString& convertedType,
                                    const QString& schemeName,
                                    quint32 messageNumber);

private:
    void initParsedInfo();
    QString convertToString(const QString& contentIdentifier,
                            const QVariant& content) const;
    QString getMessageTypeFromIdentifier(const QString& messageIdentifier) const;
    BaseMessageTranslator* createMessageTranslator(const QString& messageType,
                                                   const QVariant& messageData) const;
    GObject* fetchObjectFromMessage(const QString& messageType,
                                    const QVariant& messageData) const;

    QStringList messageTypes;
    WorkflowInvestigationData parsedInfo;
};

}  // namespace U2
