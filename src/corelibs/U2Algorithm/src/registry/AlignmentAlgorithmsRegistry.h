/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#ifndef _U2_ALIGNMENT_ALGORITHMS_REGISTRY_H_
#define _U2_ALIGNMENT_ALGORITHMS_REGISTRY_H_

#include <QList>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QStringList>

#include "U2Core/global.h"

namespace U2 {

class AlignmentAlgorithm;
class AbstractAlignmentTaskFactory;
class AlignmentAlgorithmGUIExtensionFactory;
class AlgorithmRealization;
class DNAAlphabet;

enum AlignmentAlgorithmType {
    /** Aligns a given alignment. */
    Align,

    /** Adds new sequences to an alignment. */
    AlignNewSequencesToAlignment,

    /** Adds a new alignment to an alignment. */
    AlignNewAlignmentToAlignment,

    /** Aligns some sequences from the alignment to the same alignment. */
    AlignSelectionToAlignment,

    /** Pairwise alignment algorithm. */
    PairwiseAlignment,
};

class U2ALGORITHM_EXPORT AlignmentAlgorithmsRegistry : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(AlignmentAlgorithmsRegistry)

public:
    AlignmentAlgorithmsRegistry(QObject *pOwn = 0);
    ~AlignmentAlgorithmsRegistry();

    bool registerAlgorithm(AlignmentAlgorithm *algorithm);
    AlignmentAlgorithm *getAlgorithm(const QString &id) const;

    QStringList getAvailableAlgorithmIds(AlignmentAlgorithmType type) const;

private:
    mutable QMutex mutex;
    QMap<QString, AlignmentAlgorithm *> algorithms;
};

class U2ALGORITHM_EXPORT AlignmentAlgorithm {
    Q_DISABLE_COPY(AlignmentAlgorithm)

public:
    AlignmentAlgorithm(AlignmentAlgorithmType alignmentType,
                       const QString &id,
                       const QString &actionName,
                       AbstractAlignmentTaskFactory *taskFactory,
                       AlignmentAlgorithmGUIExtensionFactory *guiFactory = nullptr,
                       const QString &realizationId = "default");

    virtual ~AlignmentAlgorithm();
    virtual AbstractAlignmentTaskFactory *getFactory(const QString &_realizationId = QString("default")) const;
    virtual AlignmentAlgorithmGUIExtensionFactory *getGUIExtFactory(const QString &realizationId = QString("default")) const;

    const QString &getId() const;

    /** Returns visual algorithm action name. */
    const QString &getActionName() const;

    QStringList getRealizationsList() const;
    bool addAlgorithmRealization(AbstractAlignmentTaskFactory *tf, AlignmentAlgorithmGUIExtensionFactory *guif, const QString &_realizationId);
    AlgorithmRealization *getAlgorithmRealization(const QString &_realizationId) const;
    AlignmentAlgorithmType getAlignmentType() const;

    virtual bool isAlgorithmAvailable() const;

    virtual bool checkAlphabet(const DNAAlphabet *) const {
        return true;
    }

protected:
    mutable QMutex mutex;

    /** Unique algorithm identifier. */
    QString id;

    /** Visual localizable algorithm action name, as shown in menus. */
    QString actionName;

    QMap<QString, AlgorithmRealization *> realizations;

    AlignmentAlgorithmType alignmentType;
};

class U2ALGORITHM_EXPORT AlgorithmRealization {
public:
    AlgorithmRealization(const QString &_realizationId, AbstractAlignmentTaskFactory *tf, AlignmentAlgorithmGUIExtensionFactory *guif);
    ~AlgorithmRealization();

    const QString &getRealizationId() const;
    AbstractAlignmentTaskFactory *getTaskFactory() const;
    AlignmentAlgorithmGUIExtensionFactory *getGUIExtFactory() const;

private:
    QString realizationId;
    AbstractAlignmentTaskFactory *alignmentAlgorithmTaskFactory;
    AlignmentAlgorithmGUIExtensionFactory *alignmentAlgorithmGUIExtensionsFactory;
};

}  // namespace U2

#endif  // _U2_ALIGNMENT_ALGORITHMS_REGISTRY_H_
