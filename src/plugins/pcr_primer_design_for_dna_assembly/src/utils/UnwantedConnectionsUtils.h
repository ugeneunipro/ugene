/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#ifndef _U2_UNWANTED_CONNECTIONS_UTILS_H_
#define _U2_UNWANTED_CONNECTIONS_UTILS_H_

#include <QByteArray>

#include <U2Core/PrimerDimersFinder.h>

namespace U2 {

class UnwantedConnectionsUtils {
public:
    UnwantedConnectionsUtils() = delete;

    static bool isUnwantedSelfDimer(const QByteArray& forwardSequence,
                                    double unwantedDeltaG,
                                    double unwantedMeltingTemperatur,
                                    int unwantedDimerLength);

    static bool isUnwantedSelfDimer(const QByteArray &forwardSequence,
                                    double unwantedDeltaG,
                                    double unwantedMeltingTemperature,
                                    int unwantedDimerLength,
                                    QString &report);

    static bool isUnwantedHeteroDimer(const QByteArray &forwardSequence,
                                      const QByteArray &reverseSequence,
                                      double unwantedDeltaG,
                                      double unwantedMeltingTemperature,
                                      int unwantedDimerLength);

    static bool isUnwantedHeteroDimer(const QByteArray& forwardSequence,
                                      const QByteArray& reverseSequence,
                                      double unwantedDeltaG,
                                      double unwantedMeltingTemperature,
                                      int unwantedDimerLength,
                                      QString &report);

private:
    static bool areUnwantedParametersPresentedInDimersInfo(const DimerFinderResult& dimersInfo,
                                                           double unwantedDeltaG,
                                                           double unwantedMeltingTemperature,
                                                           int unwantedDimerLength,
                                                           QString &report);

private:
    static bool areUnwantedParametersPresentedInDimersInfo(const DimerFinderResult& dimersInfo,
                                                           double unwantedDeltaG,
                                                           double unwantedMeltingTemperature,
                                                           int unwantedDimerLength);



};

}

#endif