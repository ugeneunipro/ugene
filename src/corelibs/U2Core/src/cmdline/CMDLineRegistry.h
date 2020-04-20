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

#ifndef _U2_CMD_LINE_REGISTRY_
#define _U2_CMD_LINE_REGISTRY_

#include <QMap>
#include <QStringList>

#include <U2Core/StrPackUtils.h>

namespace U2 {

class CMDLineHelpProvider;

/*  Reads command arguments into UGENE's command line parameters

    Convention:
    --paramName=paramValue -> (paramName, paramValue)
    -paramName paramValue -> (paramName, paramValue)
    paramValue1 paramValue2  -> ("", paramValue1), ("", paramValue2)
    */
class U2CORE_EXPORT CMDLineRegistry : public QObject {
    Q_OBJECT
public:
    static const QString PLUGINS_ARG;
    static const QString VERIFY_ARG;

    CMDLineRegistry(const QStringList& arguments);
    virtual ~CMDLineRegistry();

    const QList<StrStrPair> & getParameters() const;
    // as they were in cmdline. Empty keys also here
    QStringList getOrderedParameterNames() const;

    // finding starts at startsWithIdx
    bool hasParameter(const QString & paramName, int startsWithIdx = 0) const;
    // returns value of first appearance from startWithIdx of paramName key
    QString getParameterValue(const QString & paramName, int startWithIdx = 0) const;

    void registerCMDLineHelpProvider(CMDLineHelpProvider* provider);
    void unregisterCMDLineHelpProvider(CMDLineHelpProvider* provider);

    const QList<CMDLineHelpProvider* >& listCMDLineHelpProviders() const { return helpProviders; }

private:
    QList<StrStrPair>                   params; // pairs (paramName, paramValue) ordered as in the cmdline
    QList<CMDLineHelpProvider* >        helpProviders; // sorted by section name

}; // CMDLineRegistry

} //namespace U2

#endif // _U2_CMD_LINE_REGISTRY_
