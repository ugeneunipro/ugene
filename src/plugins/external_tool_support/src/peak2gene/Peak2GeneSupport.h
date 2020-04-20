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

#ifndef _U2_PEAK2GENE_SUPPORT_
#define _U2_PEAK2GENE_SUPPORT_

#include <U2Core/ExternalToolRegistry.h>

namespace U2 {

class Peak2GeneSupport : public ExternalTool {
    Q_OBJECT
public:
    Peak2GeneSupport(const QString& id, const QString &name);

    static const QString ET_PEAK2GENE;
    static const QString ET_PEAK2GENE_ID;
    static const QString REFGENE_DIR_NAME;
    static const QString REF_GENES_DATA_NAME;
    static const QString TRANSLATIONS_DIR_NAME;
    static const QString ENTREZ_TRANSLATION_DATA_NAME;
private:
    void initialize();
};

} // U2

#endif // _U2_PEAK2GENE_SUPPORT_
