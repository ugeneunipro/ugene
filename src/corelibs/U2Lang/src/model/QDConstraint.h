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

#ifndef _U2_QD_CONSTRAINT_H_
#define _U2_QD_CONSTRAINT_H_

#include <U2Lang/QDScheme.h>


namespace U2 {

typedef QString QDConstraintType;

class U2LANG_EXPORT QDConstraintTypes {
public:
    static const QDConstraintType DISTANCE;
};

class U2LANG_EXPORT QDConstraint {
public:
    QDConstraint(const QList<QDSchemeUnit*>& _units, QDConstraintType _type=QDConstraintTypes::DISTANCE)
        : cfg(NULL), type(_type), units(_units) {}
    virtual ~QDConstraint() {}
    QDParameters* getParameters() const { return cfg; }
    const QList<QDSchemeUnit*>& getSchemeUnits() const { return units; }
    virtual QDConstraintType constraintType() const { return type; }
    virtual QString getText(QDSchemeUnit* u1, QDSchemeUnit* u2) const=0;
    virtual bool drawArrow(QDSchemeUnit* u1, QDSchemeUnit* u2) const=0;
    void setUIEditor(ConfigurationEditor* ed) { cfg->setEditor(ed); }
protected:
    QDParameters* cfg;
    QDConstraintType type;
    QList<QDSchemeUnit*> units;
};

enum QDDistanceType {E2S, E2E, S2S, S2E};

class U2LANG_EXPORT QDDistanceConstraint : public QDConstraint {
public:
    QDDistanceConstraint(const QList<QDSchemeUnit*>& _units, QDDistanceType type, int min, int max);
    virtual ~QDDistanceConstraint();
    QDSchemeUnit* getSource() const { return units.at(0); }
    QDSchemeUnit* getDestination() const { return units.at(1); }
    int getMin() const;
    int getMax() const;
    void setMin(int min);
    void setMax(int max);
    QDDistanceType distanceType() const { return distType; }
    QString getText(QDSchemeUnit*, QDSchemeUnit*) const;
    bool drawArrow(QDSchemeUnit*, QDSchemeUnit*) const { return true; }
    void invert();
private:
    QDDistanceType distType;
};

class U2LANG_EXPORT QDConstraintController {
public:
    static const QString DISTANCE_CONSTRAINT_EL;
    static const QString TYPE_ATTR;
    static const QString MIN_LEN_ATTR;
    static const QString MAX_LEN_ATTR;
    static const QString SRC_ATTR;
    static const QString DST_ATTR;
public:
    static bool match(QDConstraint* c, const QDResultUnit& r1, const QDResultUnit& r2, bool complement = false);
    static U2Region matchLocation(QDDistanceConstraint* dc, const QDResultUnit& r, bool complement = false);
    static QDDistanceType getInvertedType(QDDistanceType type);
private:
    static bool match(const U2Region& srcReg, const U2Region& dstReg, const QDDistanceType& type, int min, int max);
};

}//namespace

#endif
