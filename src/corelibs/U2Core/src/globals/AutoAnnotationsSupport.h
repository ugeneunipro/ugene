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
    AutoAnnotationObject(U2SequenceObject *obj, DNATranslation *aminoTT, QObject *parent);
    ~AutoAnnotationObject();
    AnnotationTableObject *     getAnnotationObject() const { return aobj; }
    U2SequenceObject *          getSeqObject() const { return dnaObj; }
    void                        setGroupEnabled(const QString &groupName, bool enabled);
    void                        updateAll();
    void                        updateTranslationDependent(DNATranslation *newAminoTT);
    void                        updateGroup(const QString &groupName);
    void                        emitStateChange(bool started);
    DNATranslation*             getAminoTT() const;

    static const QString        AUTO_ANNOTATION_HINT;

signals:
    void                        si_updateStarted();
    void                        si_updateFinshed();

    private slots:
    void                        sl_updateTaskFinished();

private:
    void                        handleUpdate(const QList<AutoAnnotationsUpdater *> &updaters);
    void                        addNewUpdateTask(AutoAnnotationsUpdater *updater, Task *updateTask);
    void                        addRunningUpdateTask(AutoAnnotationsUpdater *updater, Task *updateTask);
    bool                        cancelRunningUpdateTasks(AutoAnnotationsUpdater *updater);

    U2SequenceObject *          dnaObj;
    DNATranslation              *aminoTT;
    AnnotationTableObject *     aobj;
    AutoAnnotationsSupport *    aaSupport;
    QSet<QString>               enabledGroups;

    QMap<AutoAnnotationsUpdater *, QList<Task *> > runningUpdateTasks;
    QMap<AutoAnnotationsUpdater *, QList<Task *> > newUpdateTasks;
};

#define AUTO_ANNOTATION_SETTINGS "auto-annotations/"

struct U2CORE_EXPORT AutoAnnotationConstraints {
    AutoAnnotationConstraints();

    const DNAAlphabet *     alphabet;
    GHints *                hints;
};

class U2CORE_EXPORT AutoAnnotationsUpdater : public QObject {
    Q_OBJECT
public:
    AutoAnnotationsUpdater(const QString &nm, const QString &gName, bool offByDefault = false, bool translationDependant = false);
    virtual             ~AutoAnnotationsUpdater();
    const QString &     getGroupName() { return groupName; }
    const QString &     getName() { return name; }
    bool                isCheckedByDefault() { return checkedByDefault; }
    bool                isTranslationDependent();
    void                setCheckedByDefault(bool checked) { checkedByDefault = alwaysOffByDefault ? false : checked; }
    virtual bool        checkConstraints(const AutoAnnotationConstraints &constraints) = 0;
    virtual Task *      createAutoAnnotationsUpdateTask(const AutoAnnotationObject *aa) = 0;

private:
    QString             groupName;
    QString             name;
    bool                checkedByDefault;
    bool                alwaysOffByDefault;
    bool                translationDependent;
};

class U2CORE_EXPORT AutoAnnotationsSupport : public QObject {
    Q_OBJECT
public:
    ~AutoAnnotationsSupport();
    void                                registerAutoAnnotationsUpdater(AutoAnnotationsUpdater *updater);
    void                                unregisterAutoAnnotationsUpdater(AutoAnnotationsUpdater *updater);
    QList<AutoAnnotationsUpdater *>     getAutoAnnotationUpdaters();
    AutoAnnotationsUpdater*             findUpdaterByGroupName(const QString& groupName);
    AutoAnnotationsUpdater*             findUpdaterByName(const QString &name);
    static bool                         isAutoAnnotation(const AnnotationTableObject *obj);
    static bool                         isAutoAnnotation(const GObject *obj);

private:
    QList<AutoAnnotationsUpdater *>     aaUpdaters;
};

class U2CORE_EXPORT AutoAnnotationsUpdateTask : public Task {
    Q_OBJECT
public:
    AutoAnnotationsUpdateTask(AutoAnnotationObject *aaObj, QList<Task *> subtasks);
    virtual                 ~AutoAnnotationsUpdateTask();
    virtual void            prepare();
    virtual void            cleanup();
    ReportResult            report();
    void                    setAutoAnnotationInvalid() { aaObjectInvalid = true; }
    AutoAnnotationObject *  getAutoAnnotationObject() { return aa; }
    U2SequenceObject*       getSequenceObject() const;

    static const QString    NAME;

private slots:
    void sl_onSequenceDeleted();

protected:
    QPointer<AutoAnnotationObject> aa;
    U2SequenceObject *      aaSeqObj;
    StateLock *             lock;
    QList<Task *>           subTasks;
    bool                    aaObjectInvalid;
};

} // namespace U2

#endif
