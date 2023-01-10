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

#ifndef _U2_AUTO_ANNOTATIONS_SUPPORT_H_
#define _U2_AUTO_ANNOTATIONS_SUPPORT_H_

#include <QSet>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/Task.h>

namespace U2 {

class ADVSequenceObjectContext;
class GObject;
class GHints;
class U2SequenceObject;
class DNAAlphabet;
class StateLock;

class AutoAnnotationsUpdater;
class AutoAnnotationsSupport;

/**
 * This object represents in-memory AnnotationTableObject.
 * Auto-annotations are used to represent temporary algorithm results,
 * such as for example restriction sites or ORFS.
 * Auto-annotations are controlled by AnnotatedDNAView.
 */
class U2CORE_EXPORT AutoAnnotationObject : public QObject {
    Q_OBJECT
public:
    AutoAnnotationObject(U2SequenceObject* obj, DNATranslation* aminoTT, QObject* parent);
    ~AutoAnnotationObject();

    AnnotationTableObject* getAnnotationObject() const {
        return annotationTableObject;
    }

    U2SequenceObject* getSequenceObject() const {
        return sequenceObject;
    }

    void setGroupEnabled(const QString& groupName, bool enabled);

    void updateAll();

    void updateTranslationDependent(DNATranslation* newAminoTT);

    void updateGroup(const QString& groupName);

    void emitStateChange(bool started);

    DNATranslation* getAminoTT() const;

    static const QString AUTO_ANNOTATION_HINT;

signals:
    void si_updateStarted();
    void si_updateFinished();

private slots:
    void sl_updateTaskFinished();

private:
    void handleUpdate(const QList<AutoAnnotationsUpdater*>& updaters);
    void addNewUpdateTask(AutoAnnotationsUpdater* updater, Task* updateTask);
    void addRunningUpdateTask(AutoAnnotationsUpdater* updater, Task* updateTask);
    bool cancelRunningUpdateTasks(AutoAnnotationsUpdater* updater);

    U2SequenceObject* sequenceObject;
    DNATranslation* aminoTT;
    AnnotationTableObject* annotationTableObject;
    AutoAnnotationsSupport* aaSupport;
    QSet<QString> enabledGroups;

    QMap<AutoAnnotationsUpdater*, QList<Task*>> runningUpdateTasks;
    QMap<AutoAnnotationsUpdater*, QList<Task*>> newUpdateTasks;
};

#define AUTO_ANNOTATION_SETTINGS "auto-annotations/"

/**
 * Maximum number of annotations UGENE can handle safely.
 * Auto-annotation tasks are recommended to avoid producing more annotation than given number to avoid UI freeze.
 * The value is the result of testing on standard laptop (2020): rendering of 150k annotations takes ~ 0.25 seconds.
 */
#define AUTO_ANNOTATION_MAX_ANNOTATIONS_ADV_CAN_HANDLE (150 * 1000)

struct U2CORE_EXPORT AutoAnnotationConstraints {
    AutoAnnotationConstraints();

    const DNAAlphabet* alphabet;
    GHints* hints;
};

class U2CORE_EXPORT AutoAnnotationsUpdater : public QObject {
    Q_OBJECT
public:
    AutoAnnotationsUpdater(const QString& name, const QString& groupName, bool isCantBeEnabledByDefault = false, bool translationDependant = false);
    virtual ~AutoAnnotationsUpdater();

    const QString& getGroupName() const {
        return groupName;
    }

    const QString& getName() const {
        return name;
    }

    bool isEnabledByDefault() const {
        return isOnByDefault;
    }

    void setEnabledByDefault(bool flag) {
        isOnByDefault = canBeEnabledByDefault && flag;
    }

    bool isDependsOnAminoTranslation() const {
        return dependsOnAminoTranslation;
    }

    virtual bool checkConstraints(const AutoAnnotationConstraints& constraints) = 0;

    /**
     * Creates task to compute auto annotation for the sequence associated with the given annotation object.
     * Returns nullptr if the task can't be created for some reason:
     *   for example:
     *     - there is a chance of overflow in number of resulted annotations for the given context
     *     - the configuration of the task is not correct.
     */
    virtual Task* createAutoAnnotationsUpdateTask(const AutoAnnotationObject* autoAnnotationObject) = 0;

private:
    QString groupName;

    QString name;

    /** If true the updater is enabled by default. */
    bool isOnByDefault;

    /** When false the updater can't never be enabled by default. */
    bool canBeEnabledByDefault;

    /** If true the updater is called on every amino translation table change. */
    bool dependsOnAminoTranslation;
};

class U2CORE_EXPORT AutoAnnotationsSupport : public QObject {
    Q_OBJECT
public:
    ~AutoAnnotationsSupport();

    void registerAutoAnnotationsUpdater(AutoAnnotationsUpdater* updater);

    QList<AutoAnnotationsUpdater*> getAutoAnnotationUpdaters() const {
        return updaterList;
    }

    AutoAnnotationsUpdater* findUpdaterByGroupName(const QString& groupName);
    AutoAnnotationsUpdater* findUpdaterByName(const QString& name);
    static bool isAutoAnnotationObject(const AnnotationTableObject* obj);
    static bool isAutoAnnotationObject(const GObject* obj);

private:
    QList<AutoAnnotationsUpdater*> updaterList;
};

class U2CORE_EXPORT AutoAnnotationsUpdateTask : public Task {
    Q_OBJECT
public:
    AutoAnnotationsUpdateTask(AutoAnnotationObject* autoAnnotationObject, QList<Task*> subtasks);
    virtual ~AutoAnnotationsUpdateTask();

    void prepare() override;
    void cleanup() override;
    ReportResult report() override;

    void setAutoAnnotationInvalid() {
        isAutoAnnotationObjectInvalid = true;
    }

    AutoAnnotationObject* getAutoAnnotationObject() const {
        return autoAnnotationObject;
    }

    U2SequenceObject* getSequenceObject() const {
        return sequenceObject;
    }

    static const QString NAME;

private slots:
    void sl_onSequenceDeleted();

protected:
    QPointer<AutoAnnotationObject> autoAnnotationObject;
    U2SequenceObject* sequenceObject;
    StateLock* lock;
    QList<Task*> subTasks;
    bool isAutoAnnotationObjectInvalid;
};

}  // namespace U2

#endif
