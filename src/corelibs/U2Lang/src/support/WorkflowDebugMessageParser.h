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

#ifndef _U2_WORKFLOW_DEBUG_MESSAGE_PARSER_H_
#define _U2_WORKFLOW_DEBUG_MESSAGE_PARSER_H_

#include <U2Lang/WorkflowInvestigationData.h>

namespace U2 {

namespace Workflow {

class WorkflowContext;
class Message;

}

class U2LANG_EXPORT WorkflowDebugMessageParser {
public:
                                        WorkflowDebugMessageParser( );
    virtual                             ~WorkflowDebugMessageParser( );

    void                                setContext( Workflow::WorkflowContext *context );
    void                                setSourceData( const QQueue<Workflow::Message> &sourceData );

    virtual WorkflowInvestigationData   getAllMessageValues( ) = 0;
    virtual void                        convertMessagesToDocuments( const QString &convertedType,
                                            const QString &schemeName, quint32 messageNumber ) = 0;

protected:
    QQueue<QVariantMap>                 sourceMessages;
    Workflow::WorkflowContext *         context;

    static QStringList                  possibleMessageTypes;
};

} // namespace U2

#endif // _U2_WORKFLOW_DEBUG_MESSAGE_PARSER_H_
