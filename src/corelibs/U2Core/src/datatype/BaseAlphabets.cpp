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

#include "DNAAlphabetRegistryImpl.h"

#include <U2Core/TextUtils.h>

namespace U2 {


static void fillBitArray(QBitArray& map, const char* str, Qt::CaseSensitivity caseMode) {
    QByteArray line(str);
    foreach(char c, line) {
        if (caseMode == Qt::CaseSensitive) {
            map[(uchar)c]=true;
        } else {
            char lc = TextUtils::UPPER_CASE_MAP.at((uchar)c);
            char uc = TextUtils::LOWER_CASE_MAP.at((uchar)c);
            map[(uchar)lc] = true;
            map[(uchar)uc] = true;
        }
    }
}
#define CASE_OFFSET ('a'-'A')
static void fill(QBitArray& map, const char *s, Qt::CaseSensitivity caseMode) {
    for(const char* i =s; *i!=0; i++) {
        char c = *i;
        map[int((uchar)c)] = true;
        if (caseMode == Qt::CaseInsensitive && c >='A' && c<='Z') {
            map[int((uchar)c) + CASE_OFFSET] = true;
        }
    }
}

#define REG(a) \
    map[int((uchar)a)] = true; \
    if (caseMode == DNAAlphabet::CaseInsensitive) { \
        map[int((uchar)a) + CASE_OFFSET] = true; \
    }

void DNAAlphabetRegistryImpl::initBaseAlphabets() {
    //raw text
    {
        QBitArray map(256, true);
        const DNAAlphabet* a = new DNAAlphabet(BaseDNAAlphabetIds::RAW(), tr("Raw"), DNAAlphabet_RAW, map, Qt::CaseSensitive, '\0');
        registerAlphabet(a);
    }

    //default DNA nucl
    {
        QBitArray map(256, false);
        fill(map, "ACGTN-", Qt::CaseInsensitive);
        const DNAAlphabet* a = new DNAAlphabet(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT(), tr("Standard DNA"),
                                        DNAAlphabet_NUCL, map, Qt::CaseInsensitive, 'N');
        registerAlphabet(a);
    }

    //default RNA nucl
    {
        QBitArray map(256, false);
        fill(map, "ACGUN-", Qt::CaseInsensitive);
        const DNAAlphabet* a = new DNAAlphabet(BaseDNAAlphabetIds::NUCL_RNA_DEFAULT(), tr("Standard RNA"),
            DNAAlphabet_NUCL, map, Qt::CaseInsensitive, 'N');
        registerAlphabet(a);
    }


    //extended NUCL DNA
    {
        QBitArray map(256, false);
        fill(map, "ACGTMRWSYKVHDBNX-", Qt::CaseInsensitive); //X == N
        const DNAAlphabet* a = new DNAAlphabet(BaseDNAAlphabetIds::NUCL_DNA_EXTENDED(), tr("Extended DNA"),
                                        DNAAlphabet_NUCL, map, Qt::CaseInsensitive, 'N');
        registerAlphabet(a);
    }

    //extended NUCL RNA
    {
        QBitArray map(256, false);
        fill(map, "ACGUMRWSYKVHDBNX-", Qt::CaseInsensitive); //X == N
        const DNAAlphabet* a = new DNAAlphabet(BaseDNAAlphabetIds::NUCL_RNA_EXTENDED(), tr("Extended RNA"),
            DNAAlphabet_NUCL, map, Qt::CaseInsensitive, 'N');
        registerAlphabet(a);
    }

    //default amino
    {
        QBitArray map(256, false);
        //
        // http://www.jbc.org/cgi/reprint/243/13/3557.pdf IUPAC-IUB Commission on Biochemical Nomenclature
        fillBitArray(map, "ABCDEFGHIKLMNPQRSTVWXYZ*X-", Qt::CaseInsensitive);
        // http://en.wikipedia.org/wiki/Genetic_code#Variations_to_the_standard_genetic_code
        //O = pyrrolysine, U = selenocysteine
        fillBitArray(map, "OU", Qt::CaseInsensitive);

        const DNAAlphabet* a = new DNAAlphabet(BaseDNAAlphabetIds::AMINO_DEFAULT(), tr("Standard amino acid"),
            DNAAlphabet_AMINO, map, Qt::CaseInsensitive, 'X');
        registerAlphabet(a);
    }

    //extended amino
    {
        QBitArray map(256, false);
        //
        // http://www.jbc.org/cgi/reprint/243/13/3557.pdf IUPAC-IUB Commission on Biochemical Nomenclature
        fillBitArray(map, "ABCDEFGHIKLMNPQRSTVWXYZ*X-", Qt::CaseInsensitive);
        // http://en.wikipedia.org/wiki/Genetic_code#Variations_to_the_standard_genetic_code
        //O = pyrrolysine, U = selenocysteine
        fillBitArray(map, "OU", Qt::CaseInsensitive);
        //http://en.wikipedia.org/wiki/Amino_acid
        //J = Leucine or Isoleucine
        //Not recognized by some algorithms, some functions are turned off.
        fillBitArray(map, "J", Qt::CaseInsensitive);

        const DNAAlphabet* a = new DNAAlphabet(BaseDNAAlphabetIds::AMINO_EXTENDED(), tr("Extended amino acid"),
            DNAAlphabet_AMINO, map, Qt::CaseInsensitive, 'X');
        registerAlphabet(a);
    }

}



}//namespace
