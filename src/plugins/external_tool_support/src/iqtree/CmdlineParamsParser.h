/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#ifndef _U2_CMDLINE_PARAMS_PARSER_H_
#define _U2_CMDLINE_PARAMS_PARSER_H_

#include <QObject>

namespace U2 {

class U2OpStatus;

/**
 * Utility methods to parse CMD-line parameters for external tools.
 * Experimental. Used for IQ-TREE only today.
 */
class CmdlineParamsParser : public QObject {
    Q_OBJECT
    CmdlineParamsParser() = delete;  // Utility class.

public:
    /**
     * Parses 'rawParamsString' and returns list of parameter tokens.
     * Drop double-quotes around parameters if needed.
     * Returns error if parsing can't be completed: for example a non-closed double-quote is found.
     * To pass double-quote as a value use escape character: \".
     */
    static QStringList parse(U2OpStatus& os, const QString& rawParamsString);

    /** Returns the next token after 'name' or empty string if 'name' is not found or of 'name' is the last. */
    static QString getParameterValue(const QString& name, const QStringList& parameters);

    /** Removes 'name' and the value (next token) from the 'parameters'. */
    static void removeParameterNameAndValue(const QString& name, QStringList& parameters);
};

}  // namespace U2

#endif  // _U2_CMDLINE_PARAMS_PARSER_H_
