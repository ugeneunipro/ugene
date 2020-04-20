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

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "HRVisualSerializer.h"

namespace U2 {
using namespace WorkflowSerialize;

static const QString SCALE              = "scale";
static const QString POSITION_ATTR      = "pos";
static const QString STYLE_ATTR         = "style";
static const QString BG_COLOR           = "bg-color-";
static const QString FONT               = "font-";
static const QString BOUNDS             = "bounds";
static const QString PORT_ANGLE         = "angle";
static const QString TEXT_POS_ATTR      = "text-pos";

static const int DEFAULT_SCALE = 100;
static const int MAX_SCALE = 500;

/************************************************************************/
/* HRVisualParser */
/************************************************************************/
HRVisualParser::HRVisualParser(WorkflowSchemaReaderData &_data)
: data(_data)
{

}

HRVisualParser::~HRVisualParser() {

}

void HRVisualParser::parse(U2OpStatus &os) {
    try {
        while(data.tokenizer.look() != Constants::BLOCK_END) {
            QString tok = data.tokenizer.take();
            QString next = data.tokenizer.take();
            if (next == Constants::BLOCK_START) {
                QString actorName = str2aid(tok);
                parseVisualActorParams(tok);
                data.tokenizer.assertToken(Constants::BLOCK_END);
            } else if(next == Constants::DATAFLOW_SIGN) {
                QString to = data.tokenizer.take();
                parseLinkVisualBlock(tok, to);
            } else if (next == Constants::EQUALS_SIGN) {
                QString value = data.tokenizer.take();
                if (SCALE == tok) {
                    parseScale(value);
                }
            }
        }
    } catch(const ReadFailed &e) {
        os.setError(e.what);
    }
}

void HRVisualParser::parseScale(const QString &scaleStr) {
    CHECK(NULL != data.meta, );
    bool ok = false;
    int scale = scaleStr.toInt(&ok);
    CHECK(ok, );
    CHECK(scale > 0, );
    CHECK(scale < MAX_SCALE, );

    data.meta->scalePercent = scale;
}

void HRVisualParser::parseVisualActorParams(const QString &actorId) {
    if(!data.actorMap.contains(actorId)) {
        throw ReadFailed(HRVisualParser::tr("%1 element undefined in visual block").arg(actorId));
    }

    ParsedPairs pairs(data.tokenizer);
    if(!pairs.blockPairs.isEmpty()) {
        throw ReadFailed(HRVisualParser::tr("No other blocks allowed in visual block '%1'").arg(actorId));
    }

    ActorVisualData visual(actorId);

    if (pairs.equalPairs.contains(POSITION_ATTR)) {
        QString position = pairs.equalPairs.take(POSITION_ATTR);
        U2OpStatus2Log os;
        QPointF p = string2Point(position, os);
        if (!os.hasError()) {
            visual.setPos(p);
        }
    }

    if (pairs.equalPairs.contains(STYLE_ATTR)) {
        QString style = pairs.equalPairs.take(STYLE_ATTR);
        visual.setStyle(style);
        parseStyleData(visual, style, pairs);
    }

    foreach (const QString &key, pairs.equalPairs.keys()) {
        QStringList list = key.split(Constants::DOT);
        if(list.size() == 2 && list.at(1) == PORT_ANGLE) {
            QString portId = list.at(0);
            Port *port = data.actorMap[actorId]->getPort(portId);
            if (port == NULL) {
                throw ReadFailed(HRVisualParser::tr("Cannot find port '%1' at %2 element").arg(portId).arg(actorId));
            }
            bool ok = false;
            QString strVal = pairs.equalPairs.value(key);
            qreal orientation = strVal.toDouble(&ok);
            if(!ok) {
                throw ReadFailed(HRVisualParser::tr("Cannot parse real number from: '%1'").arg(strVal));
            }
            visual.setPortAngle(portId, orientation);
        } else {
            coreLog.details(HRVisualParser::tr("Undefined visual key: '%1' for actor %2").arg(key).arg(actorId));
        }
    }
    data.meta->setActorVisualData(visual);
}

void HRVisualParser::parseLinkVisualBlock(const QString &from, const QString &to) {
    bool hasBlock = data.tokenizer.look() == Constants::BLOCK_START;
    QString srcActorId = HRSchemaSerializer::parseAt(from, 0);
    Actor *srcActor = data.actorMap.value(srcActorId);
    if(srcActor == NULL) {
        throw ReadFailed(HRVisualParser::tr("Undefined element id: '%1'").arg(srcActorId));
    }
    QString srcPortId = HRSchemaSerializer::parseAt(from, 1);
    Port *srcPort = srcActor->getPort(srcPortId);
    if(srcPort == NULL) {
        throw ReadFailed(HRVisualParser::tr("Cannot find '%1' port at '%2'").arg(srcPortId).arg(srcActorId));
    }

    QString dstActorId = HRSchemaSerializer::parseAt(to, 0);
    Actor *dstActor = data.actorMap.value(dstActorId);
    if(dstActor == NULL) {
        throw ReadFailed(HRVisualParser::tr("Undefined element id: '%1'").arg(dstActorId));
    }
    QString dstPortId = HRSchemaSerializer::parseAt(to, 1);
    Port *dstPort = dstActor->getPort(dstPortId);
    if(dstPort == NULL) {
        throw ReadFailed(HRVisualParser::tr("Cannot find '%1' port at '%2'").arg(dstPortId).arg(dstActorId));
    }

    if(hasBlock) {
        data.tokenizer.assertToken(Constants::BLOCK_START);
        ParsedPairs pairs(data.tokenizer);
        data.tokenizer.assertToken(Constants::BLOCK_END);

        QString textPos = pairs.equalPairs.take(TEXT_POS_ATTR);
        if (!textPos.isEmpty()) {
            U2OpStatus2Log os;
            QPointF p = string2Point(textPos, os);
            if (!os.hasError()) {
                data.meta->setTextPos(srcActorId, srcPortId, dstActorId, dstPortId, p);
            }
        }
    }

    if (data.isGraphDefined()) {
        if (!data.schema->getActorBindingsGraph().contains(srcPort, dstPort)) {
            throw ReadFailed(HRSchemaSerializer::tr("Undefined data-flow link: '%1'. Define it in actor-bindings").arg(from));
        }
    } else {
        QPair<Port*, Port*> link(srcPort, dstPort);
        data.links << link;
    }
}

QPointF HRVisualParser::string2Point(const QString &str, U2OpStatus &os) {
    QStringList list = str.split(QRegExp("\\s"), QString::SkipEmptyParts);
    if (list.size() != 2) {
        os.setError(HRVisualParser::tr("Cannot parse coordinates from '%1'").arg(str));
        return QPointF(0.0, 0.0);
    }
    bool ok = false;
    qreal x = list.at(0).toDouble(&ok);
    if (!ok) {
        os.setError(HRVisualParser::tr("Cannot parse real value from '%1'").arg(list.at(0)));
        return QPointF(0.0, 0.0);
    }
    ok = false;
    qreal y = list.at(1).toDouble(&ok);
    if (!ok) {
        os.setError(HRVisualParser::tr("Cannot parse real value from '%1'").arg(list.at(1)));
        return QPointF(0.0, 0.0);
    }
    return QPointF(x, y);
}

QColor HRVisualParser::string2Color(const QString &str, U2OpStatus &os) {
    int r = 0, g = 0, b = 0, a = 0;
    QTextStream stream(str.toLatin1());
    stream >> r >> g >> b >> a;
    if (QTextStream::Ok != stream.status()) {
        os.setError(HRVisualParser::tr("Cannot parse 4 integer numbers from '%1'").arg(str));
    }
    return QColor(r, g, b, a);
}

QFont HRVisualParser::string2Font(const QString &str, U2OpStatus &os) {
    QFont f;
    if(!f.fromString(str)) {
        os.setError(HRVisualParser::tr("Cannot parse font from '%1'").arg(str));
    }
    return f;
}

QRectF HRVisualParser::string2Rect(const QString &str, U2OpStatus &os) {
    QStringList list = str.split(QRegExp("\\s"));
    if (list.size() != 4) {
        os.setError(HRVisualParser::tr("Cannot parse rectangle from '%1'").arg(str));
    }
    return QRectF(string2Point(list.at(0) + " " + list.at(1), os), string2Point(list.at(2) + " " + list.at(3), os));
}

void HRVisualParser::parseStyleData(ActorVisualData &visual, const QString &styleId, ParsedPairs &pairs) {
    QString bgColor = pairs.equalPairs.take(BG_COLOR + styleId);
    if (!bgColor.isEmpty()) {
        U2OpStatus2Log os;
        QColor c = string2Color(bgColor, os);
        if (!os.hasError()) {
            visual.setColor(c);
        }
    }

    QString font = pairs.equalPairs.take(FONT + styleId);
    if (!font.isEmpty()) {
        U2OpStatus2Log os;
        QFont f = string2Font(font, os);
        if (!os.hasError()) {
            visual.setFont(f);
        }
    }

    QString bounds = pairs.equalPairs.take(BOUNDS);
    if (!bounds.isEmpty()) {
        U2OpStatus2Log os;
        QRectF r = string2Rect(bounds, os);
        if (!os.hasError()) {
            visual.setRect(r);
        }
    }
}

/************************************************************************/
/* HRVisualSerializer */
/************************************************************************/
HRVisualSerializer::HRVisualSerializer(const Metadata &_meta, const HRSchemaSerializer::NamesMap &nmap)
: meta(_meta)
{
    meta.renameActors(nmap);
}

QString HRVisualSerializer::serialize(int depth) {
    QString vData;

    if (meta.scalePercent != DEFAULT_SCALE) {
        vData += HRSchemaSerializer::makeEqualsPair(SCALE, QString::number(meta.scalePercent), depth + 1);
    }

    foreach(const ActorVisualData &visual, meta.getActorsVisual()) {
        vData += actorVisualData(visual, depth + 1);
    }

    foreach(const QString &link, meta.getTextPosMap().keys()) {
        QPointF p = meta.getTextPosMap()[link];
        vData += linkVisualData(link, p, depth + 1);
    }

    return HRSchemaSerializer::makeBlock(Constants::VISUAL_START, Constants::NO_NAME, vData, depth);
}

QString HRVisualSerializer::actorVisualData(const ActorVisualData &visual, int depth) {
    QString aData;
    bool contains = false;

    QPointF p = visual.getPos(contains);
    if (contains) {
        aData += HRSchemaSerializer::makeEqualsPair(POSITION_ATTR, point2String(p), depth + 1);
    }

    QString s = visual.getStyle(contains);
    if (contains) {
        aData += HRSchemaSerializer::makeEqualsPair(STYLE_ATTR, s, depth + 1);

        QColor c = visual.getColor(contains);
        if (contains) {
            aData += HRSchemaSerializer::makeEqualsPair(BG_COLOR + s, color2String(c), depth + 1);
        }
        QFont f = visual.getFont(contains);
        if (contains) {
            aData += HRSchemaSerializer::makeEqualsPair(FONT + s, font2String(f), depth + 1);
        }
        QRectF r = visual.getRect(contains);
        if (contains) {
            aData += HRSchemaSerializer::makeEqualsPair(BOUNDS, rect2String(r), depth + 1);
        }
    }

    foreach (const QString &portId, visual.getAngleMap().keys()) {
        qreal a = visual.getAngleMap()[portId];
        aData += HRSchemaSerializer::makeEqualsPair(portId + Constants::DOT + PORT_ANGLE,
            QString::number(a), depth + 1);
    }
    return HRSchemaSerializer::makeBlock(visual.getActorId(),
        Constants::NO_NAME, aData, depth);
}

QString HRVisualSerializer::linkVisualData(const QString &link, const QPointF &p, int depth) {
    QString lData;
    lData += HRSchemaSerializer::makeEqualsPair(TEXT_POS_ATTR, point2String(p), depth + 1);
    return HRSchemaSerializer::makeBlock(link, Constants::NO_NAME, lData, depth);
}

QString HRVisualSerializer::point2String(const QPointF &point) {
    return QString("%1 %2").arg(point.x()).arg(point.y());
}

QString HRVisualSerializer::color2String(const QColor &color) {
    return QString("%1 %2 %3 %4").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha());
}

QString HRVisualSerializer::font2String(const QFont &font) {
    return font.toString();
}

QString HRVisualSerializer::rect2String(const QRectF &rect) {
    return QString("%1 %2").arg(point2String(rect.topLeft())).arg(point2String(rect.bottomRight()));
}

} // U2
