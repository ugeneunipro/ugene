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

#ifndef _U2_WORKFLOW_MARKER_H_
#define _U2_WORKFLOW_MARKER_H_

#include <U2Core/global.h>
#include <U2Lang/Datatype.h>
#include <U2Lang/Descriptor.h>

#undef TEXT

namespace U2 {

enum MarkerDataType {
    INTEGER,
    FLOAT,
    STRING,
    BOOLEAN
};

class U2LANG_EXPORT MarkerTypes : public QObject {
    Q_OBJECT
public:
    static const QString SEQ_LENGTH_MARKER_ID;
    static const QString SEQ_NAME_MARKER_ID;
    static const QString ANNOTATION_COUNT_MARKER_ID;
    static const QString ANNOTATION_LENGTH_MARKER_ID;
    static const QString QUAL_INT_VALUE_MARKER_ID;
    static const QString QUAL_TEXT_VALUE_MARKER_ID;
    static const QString QUAL_FLOAT_VALUE_MARKER_ID;
    static const QString TEXT_MARKER_ID;

    static MarkerDataType getDataTypeById(const QString &typeId);

    static const Descriptor SEQ_LENGTH();
    static const Descriptor SEQ_NAME();
    static const Descriptor ANNOTATION_COUNT();
    static const Descriptor ANNOTATION_LENGTH();
    static const Descriptor QUAL_INT_VALUE();
    static const Descriptor QUAL_TEXT_VALUE();
    static const Descriptor QUAL_FLOAT_VALUE();
    static const Descriptor TEXT();
};

class U2LANG_EXPORT MarkerSlots : public QObject {
    Q_OBJECT
public:
    static const Descriptor getSlotByMarkerType(const QString &markerId, const QString &slotName);
};

class U2LANG_EXPORT MarkerPorts : public QObject {
    Q_OBJECT
public:
    static const QString IN_MARKER_SEQ_PORT();
    static const QString OUT_MARKER_SEQ_PORT();
};

/************************************************************************/
/* Marker */
/************************************************************************/
enum MarkerGroup {
    SEQUENCE,
    ANNOTATION,
    QUALIFIER,
    TEXT
};

enum ParameterState {
    NONE,
    NOT_REQUIRED,
    REQUIRED
};

class U2LANG_EXPORT Marker : public QObject{
    Q_OBJECT
public:
    Marker(const QString &markerType, const QString &markerName);
    Marker(const Marker &m);
    virtual ~Marker() {}
    virtual void addValue(QString name, QString value);
    virtual QString getMarkingResult(const QVariant &object);
    virtual MarkerGroup getGroup() = 0;
    virtual Marker *clone() = 0;
    virtual ParameterState hasAdditionalParameter();
    virtual void setAdditionalParameter(const QVariant &param);
    virtual QVariant getAdditionalParameter();
    virtual QString getAdditionalParameterName();

    const QString &getName() const;
    const QString &getType() const;
    const QMap<QString, QString> &getValues() const;
    QMap<QString, QString> &getValues();
    void setName(const QString &newName);

    const QString toString() const;

protected:
    QString type;
    QString name;
    MarkerDataType dataType;
    QMap<QString, QString> values;

private:
    bool getMarkerIntResult(const QVariant &object, QVariantList &expr);
    bool getMarkerFloatResult(const QVariant &object, QVariantList &expr);
    bool getMarkerStringResult(const QVariant &object, QVariantList &expr);
};

class U2LANG_EXPORT MarkerFactory {
public:
    static Marker *createInstanse(const QString &type, const QVariant &additionalParam);
};

/************************************************************************/
/* SequencerMarker */
/************************************************************************/
class U2LANG_EXPORT SequenceMarker : public Marker {
    Q_OBJECT
public:
    SequenceMarker(const QString &markerType, const QString &markerName) : Marker(markerType, markerName) {}
    virtual QString getMarkingResult(const QVariant &object);
    virtual MarkerGroup getGroup();
    virtual Marker *clone();
};

/************************************************************************/
/* QualifierMarker */
/************************************************************************/
class U2LANG_EXPORT QualifierMarker : public Marker {
    Q_OBJECT
public:
    QualifierMarker(const QString &markerType, const QString &markerName, const QString &qualName)
        : Marker(markerType, markerName), qualName(qualName) {}
    virtual QString getMarkingResult(const QVariant &object);
    virtual MarkerGroup getGroup();
    virtual Marker *clone();
    virtual ParameterState hasAdditionalParameter();
    virtual void setAdditionalParameter(const QVariant &param);
    virtual QVariant getAdditionalParameter();
    virtual QString getAdditionalParameterName();

    const QString &getQualifierName() const;

private:
    QString qualName;
};

/************************************************************************/
/* AnnotationMarker */
/************************************************************************/
class U2LANG_EXPORT AnnotationMarker : public Marker {
    Q_OBJECT
public:
    AnnotationMarker(const QString &markerType, const QString &markerName, const QString &annName)
        : Marker(markerType, markerName), annName(annName) {}
    virtual QString getMarkingResult(const QVariant &object);
    virtual MarkerGroup getGroup();
    virtual Marker *clone();
    virtual ParameterState hasAdditionalParameter();
    virtual void setAdditionalParameter(const QVariant &param);
    virtual QVariant getAdditionalParameter();
    virtual QString getAdditionalParameterName();

    const QString &getAnnotationName() const;

private:
    QString annName;
};

/************************************************************************/
/* TextMarker */
/************************************************************************/
class U2LANG_EXPORT TextMarker : public Marker {
    Q_OBJECT
public:
    TextMarker(const QString &markerType, const QString &markerName) : Marker(markerType, markerName) {}
    virtual QString getMarkingResult(const QVariant &object);
    virtual MarkerGroup getGroup();
    virtual Marker *clone();
};

} // U2

#endif // _U2_WORKFLOW_MARKER_H_
