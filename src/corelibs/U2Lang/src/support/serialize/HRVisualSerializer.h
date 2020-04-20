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

#ifndef _U2_HRVISUALSERIALIZER_H_
#define _U2_HRVISUALSERIALIZER_H_

#include <U2Core/U2OpStatus.h>

#include <U2Lang/HRSchemaSerializer.h>

namespace U2 {

class HRVisualParser : public QObject {
    Q_OBJECT
public:
    HRVisualParser(WorkflowSchemaReaderData &data);
    virtual ~HRVisualParser();

    void parse(U2OpStatus &os);

private:
    WorkflowSchemaReaderData &data;

private:
    void parseVisualActorParams(const QString &actorId);
    void parseLinkVisualBlock(const QString &from, const QString &to);
    void parseStyleData(ActorVisualData &visual, const QString &styleId,
        ParsedPairs &pairs);
    void parseScale(const QString &scaleStr);

    static QPointF string2Point(const QString &str, U2OpStatus &os);
    static QColor string2Color(const QString &str, U2OpStatus &os);
    static QFont string2Font(const QString &str, U2OpStatus &os);
    static QRectF string2Rect(const QString &str, U2OpStatus &os);
};

class HRVisualSerializer {
public:
    HRVisualSerializer(const Metadata &meta, const HRSchemaSerializer::NamesMap &nmap);
    QString serialize(int depth);

private:
    Metadata meta;

private:
    QString actorVisualData(const ActorVisualData &visual, int depth);
    QString linkVisualData(const QString &link, const QPointF &p, int depth);

    static QString point2String(const QPointF &point);
    static QString color2String(const QColor &color);
    static QString font2String(const QFont &font);
    static QString rect2String(const QRectF &rect);
};

} // U2

#endif // _U2_HRVISUALSERIALIZER_H_
