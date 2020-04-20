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

#ifndef _U2_CUFFLINKS_SUPPORT_H
#define _U2_CUFFLINKS_SUPPORT_H

#include <U2Core/ExternalToolRegistry.h>

namespace U2 {

class CufflinksSupport : public ExternalTool
{
    Q_OBJECT

public:
    CufflinksSupport(const QString& id, const QString& name, const QString& path = "");

    static const QString  ET_CUFFCOMPARE;
    static const QString  ET_CUFFCOMPARE_ID;
    static const QString  ET_CUFFDIFF;
    static const QString  ET_CUFFDIFF_ID;
    static const QString  ET_CUFFLINKS;
    static const QString  ET_CUFFLINKS_ID;
    static const QString  ET_CUFFMERGE;
    static const QString  ET_CUFFMERGE_ID;
    static const QString  ET_GFFREAD;
    static const QString  ET_GFFREAD_ID;

    static const QString  CUFFLINKS_TMP_DIR;
    static const QString  CUFFDIFF_TMP_DIR;
    static const QString  CUFFMERGE_TMP_DIR;
};



} // namespace

#endif
