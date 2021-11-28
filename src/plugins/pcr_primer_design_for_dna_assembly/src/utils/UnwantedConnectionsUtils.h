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

    /**
     * Return true if @forwardSequence has selfdimers with:
     *  - Gibbs free energy <= than @unwantedDeltaG,
     *  - the melting temperature >= than @unwantedMeltingTemperatur,
     *  - the length of the dimer >= than @unwantedDimerLength.
     * Othervise return false.
     * @forwardSequence sequence we are looking for selfdimers in.
     * @unwantedDeltaG the Gibbs free energy threshold.
     * @unwantedMeltingTemperatur the melting temperature threshold.
     * @unwantedDimerLength the dimer length threshold.
     * @return true if unwanted connections exist, otherwise false.
     */
    static bool isUnwantedSelfDimer(const QByteArray& forwardSequence,
                                    double unwantedDeltaG,
                                    double unwantedMeltingTemperatur,
                                    int unwantedDimerLength);

    /**
     * Return true if @forwardSequence has selfdimers with:
     *  - Gibbs free energy <= than @unwantedDeltaG,
     *  - the melting temperature >= than @unwantedMeltingTemperatur,
     *  - the length of the dimer >= than @unwantedDimerLength.
     * Othervise return false.
     * Return the report in @report if unwanted connections exist.
     * @forwardSequence sequence we are looking for selfdimers in.
     * @unwantedDeltaG the Gibbs free energy threshold.
     * @unwantedMeltingTemperatur the melting temperature threshold.
     * @unwantedDimerLength the dimer length threshold.
     * @report report for the existed unwanted connections.
     * @return true if unwanted connections exist, otherwise false.
     */
    static bool isUnwantedSelfDimer(const QByteArray &forwardSequence,
                                    double unwantedDeltaG,
                                    double unwantedMeltingTemperature,
                                    int unwantedDimerLength,
                                    QString &report);

    /**
     * Return true if @forwardSequence and @reverseSequence have heterodimers with:
     *  - Gibbs free energy <= than @unwantedDeltaG,
     *  - the melting temperature >= than @unwantedMeltingTemperatur,
     *  - the length of the dimer >= than @unwantedDimerLength.
     * Othervise return false.
     * @forwardSequence sequence we are looking for selfdimers in.
     * @unwantedDeltaG the Gibbs free energy threshold.
     * @unwantedMeltingTemperatur the melting temperature threshold.
     * @unwantedDimerLength the dimer length threshold.
     * @return true if unwanted connections exist, otherwise false.
     */
    static bool isUnwantedHeteroDimer(const QByteArray &forwardSequence,
                                      const QByteArray &reverseSequence,
                                      double unwantedDeltaG,
                                      double unwantedMeltingTemperature,
                                      int unwantedDimerLength);

    /**
     * Return true if @forwardSequence and @reverseSequence have heterodimers with:
     *  - Gibbs free energy <= than @unwantedDeltaG,
     *  - the melting temperature >= than @unwantedMeltingTemperatur,
     *  - the length of the dimer >= than @unwantedDimerLength.
     * Othervise return false.
     * Return the report in @report if unwanted connections exist.
     * @forwardSequence sequence we are looking for selfdimers in.
     * @unwantedDeltaG the Gibbs free energy threshold.
     * @unwantedMeltingTemperatur the melting temperature threshold.
     * @unwantedDimerLength the dimer length threshold.
     * @report report for the existed unwanted connections.
     * @return true if unwanted connections exist, otherwise false.
     */
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



};

}

#endif // _U2_UNWANTED_CONNECTIONS_UTILS_H_
