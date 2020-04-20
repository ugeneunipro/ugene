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

#include "OutputCollector.h"

namespace U2 {

OutputCollector::OutputCollector(bool _skipMessageWithCommand)
    : ExternalToolListener(),
      skipMessageWithCommand(_skipMessageWithCommand),
      firstLineSkipped(false)
{

}

void OutputCollector::addNewLogMessage(const QString& message, int /*messageType*/) {
    //skip first line of the output with tool calling string
    if (skipMessageWithCommand && collectedLog.isEmpty() && !firstLineSkipped) {
        firstLineSkipped = true;
        return;
    }
    collectedLog.append(message);
}

const QString& OutputCollector::getLog() const {
    return collectedLog;
}

}
