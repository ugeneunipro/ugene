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

#ifndef __ASSEMBLY_CELL_RENDERER__
#define __ASSEMBLY_CELL_RENDERER__

#include <U2Core/U2Assembly.h>

#include <QMap>
#include <QHash>

#include <QColor>
#include <QPixmap>
#include <QFont>

namespace U2 {

/** Abstract cell renderer interface */
class AssemblyCellRenderer {
public:
    AssemblyCellRenderer() {}
    virtual ~AssemblyCellRenderer() {}

    /** Render pixmaps into cache */
    virtual void render(const QSize &size, int devicePixelRatio, bool text, const QFont &font) = 0;

    /** @returns cached cell pixmap */
    virtual QPixmap cellImage(char c) = 0;
    virtual QPixmap cellImage(const U2AssemblyRead &read, char c) = 0;
    virtual QPixmap cellImage(const U2AssemblyRead &read, char c, char ref) = 0;

protected:
    static void drawCell(QPixmap &img, const QSize &size, const QColor &topColor, const QColor&bottomColor, bool text, char c, const QFont &font, const QColor &textColor);
    static void drawCell(QPixmap &img, const QSize &size, const QColor &color, bool text, char c, const QFont &font, const QColor &textColor) {
        drawCell(img, size, color, color, text, c, font, textColor);
    }
};

class AssemblyCellRendererFactory {
public:
    AssemblyCellRendererFactory(const QString& _id, const QString& _name);
    virtual ~AssemblyCellRendererFactory() {}
    virtual AssemblyCellRenderer* create() = 0;

    const QString& getId() const {return id;}
    const QString& getName() const {return name;}

    static QString ALL_NUCLEOTIDES;
    static QString DIFF_NUCLEOTIDES;
    static QString STRAND_DIRECTION;
    static QString PAIRED;

private:
    QString id;
    QString name;
};

class AssemblyCellRendererFactoryRegistry : public QObject {
    Q_OBJECT
public:
    AssemblyCellRendererFactoryRegistry(QObject * parent = 0);
    const QList<AssemblyCellRendererFactory*>& getFactories() const { return factories; }
    AssemblyCellRendererFactory* getFactoryById(const QString& id) const;
    void addFactory(AssemblyCellRendererFactory* f);

    virtual ~AssemblyCellRendererFactoryRegistry();

private:
    void initBuiltInRenderers();

    QList<AssemblyCellRendererFactory*> factories;
};

} //ns

#endif
