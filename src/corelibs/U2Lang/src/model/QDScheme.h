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

#ifndef _U2_QD_SCHEME_H_
#define _U2_QD_SCHEME_H_

#include <U2Core/AnnotationData.h>
#include <U2Core/DNASequence.h>
#include <U2Lang/Configuration.h>

#include <QSharedData>
#include <QPair>


namespace U2 {

enum QDStrandOption { QDStrand_DirectOnly, QDStrand_ComplementOnly, QDStrand_Both };

class U2LANG_EXPORT QDParameters : public QObject, public Configuration {
    Q_OBJECT
public:
    virtual void setParameter(const QString& name, const QVariant& val);
    virtual void remap(const QMap<ActorId, ActorId>&) {}
signals:
    void si_modified();
};

class U2LANG_EXPORT QDActorParameters : public QDParameters {
    Q_OBJECT
public:
    QDActorParameters() : annKey("misc_feature") {}
    void setLabel(const QString& l) { label = l; emit si_modified(); }
    const QString& getLabel() const { return label; }
    void setAnnotationKey(const QString& key) { annKey = key; emit si_modified(); }
    const QString& getAnnotationKey() const { return annKey; }
private:
    QString label;
    QString annKey;
};

class QDSchemeUnit;
class U2LANG_EXPORT QDResultUnitData : public QSharedData {
public:
    QDResultUnitData() : owner(NULL){}

    QDSchemeUnit*           owner;
    U2Strand                strand;
    U2Region                region;
    QVector<U2Qualifier>    quals;
};

typedef QSharedDataPointer<QDResultUnitData> QDResultUnit;

class U2LANG_EXPORT QDResultGroup {
public:
    QDResultGroup(QDStrandOption _strand = QDStrand_DirectOnly) : strand(_strand), startPos(0), endPos(0) {}
    U2Region location() const;
    void add(const QDResultUnit& res);
    void add(const QList<QDResultUnit>& res);
    const QList<QDResultUnit>& getResultsList() const;
public:
    static void buildGroupFromSingleResult(const QDResultUnit& ru, QList<QDResultGroup*>& results);
public:
    QDStrandOption strand;
private:
    int startPos;
    int endPos;
    QList<QDResultUnit> results;
};

inline U2Region QDResultGroup::location() const { return U2Region(startPos, endPos-startPos); }
inline const QList<QDResultUnit>& QDResultGroup::getResultsList() const { return results; }

typedef QPair<QString, QString> StringAttribute;

class QDScheme;
class QDConstraint;
class QDActor;

class U2LANG_EXPORT QDActorPrototype {
public:
    QDActorPrototype() : editor(NULL) {}
    virtual ~QDActorPrototype();
    const QList<Attribute*>& getParameters() const { return attributes; }
    ConfigurationEditor* getEditor() const { return editor; }

    const Descriptor& getDescriptor() const { return descriptor; }
    QString getId() const { return descriptor.getId(); }
    QString getDisplayName() const { return descriptor.getDisplayName(); }
    virtual QIcon getIcon() const { return QIcon(); }

    virtual QDActor* createInstance() const=0;
protected:
    QList<Attribute*> attributes;
    ConfigurationEditor* editor;
    Descriptor descriptor;
};

class U2LANG_EXPORT QDActor : public QObject {
    friend class QDScheme;
    Q_OBJECT
public:
    QDActor(QDActorPrototype const* proto);
    virtual ~QDActor();

    QDScheme* getScheme() const { return scheme; }
    QDActorPrototype const* getProto() const { return proto; }
    QDActorParameters* getParameters() const { return cfg; }
    QString getUnitId(QDSchemeUnit* su) const { return units.key(su); }
    QDSchemeUnit* getSchemeUnit(const QString& id) const { return units.value(id); }
    QList<QDSchemeUnit*> getSchemeUnits() const { return units.values(); }
    const QList<QDConstraint*>& getParamConstraints() const { return paramConstraints; }

    QDStrandOption getStrand() const;
    void setStrand(QDStrandOption stOp);

    virtual void updateEditor() {}

    bool hasAnyDirection() const { return getStrand()==QDStrand_Both; }
    bool hasForwardDirection() const { return getStrand()==QDStrand_DirectOnly; }
    bool hasBackwardDirection() const { return getStrand()==QDStrand_ComplementOnly; }

    bool isSimmetric() const { return simmetric; }

    QList<QDConstraint*> getConstraints() const;

    QString getActorType() const { return proto->getId(); }
    const QString& annotateAs() const { return cfg->getAnnotationKey(); }

    void filterResults(const QVector<U2Region>& location);

    QList<QDResultGroup*> popResults();
    void reset();

    virtual int getMinResultLen() const=0;
    virtual int getMaxResultLen() const=0;
    virtual QString getText() const=0;
    virtual Task* getAlgorithmTask(const QVector<U2Region>& location)=0;
    virtual bool hasStrand() const { return true; }
    //implement in subclass to provide configuration representation in serialized scheme
    virtual QList< QPair<QString, QString> > saveConfiguration() const;
    virtual void loadConfiguration(const QList< QPair<QString,QString> >& strMap);
    //implement in subclass to customize the color of the visual actor representation
    virtual QColor defaultColor() const { return QColor(0x98,0xff, 0xc5); }
signals:
    void si_strandChanged(QDStrandOption);
public:
    static const int DEFAULT_MAX_RESULT_LENGTH;
protected:
    QDStrandOption getStrandToRun() const;
protected:
    QDScheme* scheme;
    QDActorPrototype const* proto;
    QDActorParameters* cfg;
    QMap<QString, QDSchemeUnit*> units;
    QList<QDConstraint*> paramConstraints;
    QList<QDResultGroup*> results;
    QMap<QString, QVariant> defaultCfg;
    QDStrandOption strand;
    bool simmetric;
};

class QDDistanceConstraint;
class U2LANG_EXPORT QDSchemeUnit {
    friend class QDScheme;
public:
    QDSchemeUnit(QDActor* parent) : actor(parent) {}
    const QList<QDConstraint*>& getConstraints() const { return schemeConstraints; }
    QList<QDDistanceConstraint*> getDistanceConstraints() const;
    QDActor* getActor() const { return actor; }
    QString getPersonalName() const;
    QString getId() const {
        QDSchemeUnit* thisPtr = const_cast<QDSchemeUnit*>(this);
        return actor->getUnitId(thisPtr); }
private:
    QDActor* actor;
    QList<QDConstraint*> schemeConstraints;
};

class U2LANG_EXPORT QDPath {
public:
    QDPath() : pathSrc(NULL), pathDst(NULL), overallConstraint(NULL) {}
    ~QDPath();
    QDPath* clone() const;
    bool addConstraint(QDDistanceConstraint* dc);
    QDSchemeUnit const* getSrc() const { return pathSrc; }
    QDSchemeUnit const* getDst() const { return pathDst; }
    const QList<QDDistanceConstraint*>& getConstraints() const { return constraints; }
    //returns unsorted list of involved scheme units
    const QList<QDSchemeUnit*>& getSchemeUnits() const { return schemeUnits; }
    QDDistanceConstraint* toConstraint();
private:
    QDSchemeUnit* pathSrc;
    QDSchemeUnit* pathDst;
    QDDistanceConstraint* overallConstraint;
    QList<QDDistanceConstraint*> constraints;
    QList<QDSchemeUnit*> schemeUnits;
};

class U2SequenceObject;
class U2LANG_EXPORT QDScheme : public QObject {
    Q_OBJECT
public:
    QDScheme() :  strand(QDStrand_Both) {}
    ~QDScheme();
    void clear();
    void addActor(QDActor* a);
    void addConstraint(QDConstraint* constraint);
    bool removeActor(QDActor* a);
    void removeConstraint(QDConstraint* constraint);
    const DNASequence& getSequence() const { return dna; }
    U2EntityRef getEntityRef() const {return entityRef;}
    void setSequence(const DNASequence& sequence) { dna = sequence; }
    void setEntityRef(const U2EntityRef& _entityRef ){entityRef = _entityRef;}
    const QList<QDActor*>& getActors() const { return actors; }
    QList<QDConstraint*> getConstraints() const;
    QList<QDConstraint*> getConstraints(QDSchemeUnit const* su1, QDSchemeUnit const* su2) const;
    QList<QDPath*> findPaths(QDSchemeUnit* src, QDSchemeUnit* dst);
    void setOrder(QDActor* a, int serialNum);
    bool isEmpty() const { return actors.isEmpty(); }
    bool isValid() const;
    QDActor* getActorByLabel(const QString& label) const;

    QDStrandOption getStrand() const { return strand; }
    void setStrand(QDStrandOption opt) { strand = opt; }

    //actor group methods
    void adaptActorsOrder();
    QString getActorGroup(QDActor* a) const;
    QList<QString> getActorGroups() const { return actorGroups.keys(); }
    QList<QDActor*> getActors(const QString& group) const { return actorGroups.value(group); }
    void addActorToGroup(QDActor* a, const QString& group);
    bool removeActorFromGroup(QDActor* a);
    void createActorGroup(const QString& name);
    bool removeActorGroup(const QString& name);
    bool validateGroupName(const QString& name) const;
    int getRequiredNumber(const QString& group) const { return actorGroupReqNum.value(group); }
    void setRequiredNum(const QString& group, int num);
signals:
    void si_schemeChanged();
private:
    void findRoute(QDSchemeUnit* curSu);
private:
    QList<QDActor*> actors;
    QMap< QString, QList<QDActor*> > actorGroups;
    QMap< QString, int > actorGroupReqNum;
    U2EntityRef entityRef;
    DNASequence dna;
    QDStrandOption strand;
};

class U2LANG_EXPORT QDAttributeNameConverter {
public:
    static QString convertAttrName(const QString& str) {
        return str.toLower().replace(' ', '_');
    }
};

class U2LANG_EXPORT QDAttributeValueMapper {
public:
    enum ValueType { BOOLEAN_TYPE, UNKNOWN_TYPE };

    static QVariant stringToAttributeValue(const QString& str);
    static ValueType getType(const QString& val);
public:
    static const QMap<QString, bool> BOOLEAN_MAP;
private:
    static QMap<QString, bool> initBooleanMap();
};

}//namespace

Q_DECLARE_METATYPE(U2::QDActorPrototype *)

#endif
