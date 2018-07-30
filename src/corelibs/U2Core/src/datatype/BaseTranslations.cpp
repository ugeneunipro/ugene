/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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
#include "DNATranslationImpl.h"

#include <U2Core/TextUtils.h>

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>

#include <QDir>
#include <QTextStream>

#include <QXmlStreamReader>

namespace U2 {

const QString BaseDNATranslationIds::NUCL_DNA_DEFAULT_COMPLEMENT("NUCL_DNA_DEFAULT_COMPLEMENT");
const QString BaseDNATranslationIds::NUCL_RNA_DEFAULT_COMPLEMENT("NUCL_RNA_DEFAULT_COMPLEMENT");

const QString BaseDNATranslationIds::NUCL_DNA_EXTENDED_COMPLEMENT("NUCL_DNA_EXTENDED_COMPLEMENT");
const QString BaseDNATranslationIds::NUCL_RNA_EXTENDED_COMPLEMENT("NUCL_RNA_EXTENDED_COMPLEMENT");

#define DATA_DIR_KEY                QString("back_translation")
#define DATA_FILE_KEY               QString("back_translation/lastFile")
#define DEFAULT_ORGANISM_FILE       QString("tables.xml")

static void fill3To1(QList<Mapping3To1<char> >& map, QMap<DNATranslationRole,QList<Triplet> >& codons,
    const DNAAlphabet* srcAl, const DNAAlphabet* dstAl,
    const char* amino, const char* role, const char* n1, const char* n2, const char* n3)
{
    assert(srcAl->isNucleic()); Q_UNUSED(srcAl);
    assert(dstAl->isAmino()); Q_UNUSED(dstAl);

    int len = strlen(amino);
    assert(len == (int)strlen(role) && len == (int)strlen(n1) && len == (int)strlen(n2) && len == (int)strlen(n3));
    for(int i=0; i<len; i++) {
        char res = amino[i];
        char c1 = n1[i];
        char c2 = n2[i];
        char c3 = n3[i];
        assert(dstAl->contains(res));
        assert(srcAl->contains(c1));
        assert(srcAl->contains(c2));
        assert(srcAl->contains(c3));
        Triplet t(c1, c2, c3);
        Mapping3To1<char> m(t, res);
        map.append(m);
        if (role[i] == 'M') codons[DNATranslationRole_Start].append(t);
        else if (role[i] == 'L') codons[DNATranslationRole_Start_Alternative].append(t);
        else if (amino[i] == '*') codons[DNATranslationRole_Stop].append(t);
    }
}

static void fill1To3(BackTranslationRules& map,
    const DNAAlphabet* srcAl, const DNAAlphabet* dstAl,
    const char* amino, const int* prob,
    const char* n1, const char* n2, const char* n3)
{
    assert(srcAl->isAmino()); Q_UNUSED(srcAl);
    assert(dstAl->isNucleic()); Q_UNUSED(dstAl);

    TripletP t('N', 'N', 'N', 100);
    map.map.append(t);
    map.index['-'] = map.map.size();
    TripletP dash('-', '-', '-', 100);
    map.map.append(dash);
    int len = strlen(amino);
    assert(len == (int)strlen(n1) && len == (int)strlen(n2) && len == (int)strlen(n3));

    QByteArray alph = srcAl->getAlphabetChars();
    QList<TripletP> v;
    foreach (char c, alph) {
        v.clear();
        int sump = 0;
        for(int i=0; i<len; i++) {
            if (amino[i] != c) continue;
            char c1 = n1[i];
            char c2 = n2[i];
            char c3 = n3[i];
            int p = prob[i];
            sump += p;
#ifdef DEBUG
            char src = amino[i];
            assert(srcAl->contains(src));
            assert(dstAl->contains(c1));
            assert(dstAl->contains(c2));
            assert(dstAl->contains(c3));
            assert((0 <= p) && (p <= 100));
#endif
            TripletP t(c1, c2, c3, p);
            v.append(t);
        }
        if (v.empty()) {
            if (c != '-') {
                map.index[(int)c] = 0;
                map.index[(int)c] = 1;
            }
        } else {
            map.index[(int)c] = map.map.size();
            qSort(v);
            v.first().p += (100 - sump);
            foreach (TripletP t, v) {
                map.map.append(t);
            }
        }
    }
}

static bool parseCutFile(const QString& url, char* amino, int* prob, char* n1, char* n2, char* n3) {
    QFile organismFile(url);
    if (organismFile.open(QFile::ReadOnly)) {
        QTextStream data(&organismFile);
        QString line;
        QStringList parsedData;
        QByteArray buf(4, '\0');
        int pos = 0;
        bool ok = true;
        do {
            line = data.readLine();
            if (line.isEmpty() || line.startsWith("#")) continue;
            parsedData = line.split(QRegExp("\\s"), QString::SkipEmptyParts);
            if ( parsedData.size() != 5 ) return false;
            if ( parsedData[0].length() != 3 ) return false;
            buf = parsedData[0].toLatin1();
            n1[pos] = buf[0];
            n2[pos] = buf[1];
            n3[pos] = buf[2];
            if ( parsedData[1].length() != 1 ) return false;
            buf = parsedData[1].toLatin1();
            amino[pos] = buf[0];
            double pr = parsedData[2].toDouble(&ok);
            if (!ok) return false;
            prob[pos] = qRound(pr);
            pos++;
            amino[pos] = n1[pos] = n2[pos] = n3[pos] = '\0';
        } while (!line.isNull());
        return true;
    }
    return false;
}

static void regCodon(DNATranslationRegistry *treg, char symbol, const char *shortName, QString fullName, const char* link, DNACodonGroup gr) {
    DNACodon *c = new DNACodon(symbol, QString(shortName), fullName, gr);
    c->setLink(link);
    treg->registerDNACodon(c);
};

void DNAAlphabetRegistryImpl::reg4tables(const char* amino, const char* role, const char* n1, const char* n2, const char* n3,
    const QString& id, const QString& name)
{
    {
        const DNAAlphabet* srcAlphabet = findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
        const DNAAlphabet* dstAlphabet = findById(BaseDNAAlphabetIds::AMINO_DEFAULT());

        QList<Mapping3To1<char> > map;
        QMap<DNATranslationRole,QList<Triplet> > codons;
        fill3To1(map, codons, srcAlphabet, dstAlphabet, amino, role, n1, n2, n3);

        DNATranslation* t = new DNATranslation3to1Impl(id, name,
            srcAlphabet, dstAlphabet, map, 'X', codons);
        treg->registerDNATranslation(t);
    }

    //extended NUCL DNA to AMINO -> all extended symbols lead to "unknown"
    {
        const DNAAlphabet* srcAlphabet = findById(BaseDNAAlphabetIds::NUCL_DNA_EXTENDED());
        const DNAAlphabet* dstAlphabet = findById(BaseDNAAlphabetIds::AMINO_DEFAULT());

        QList<Mapping3To1<char> > map;
        QMap<DNATranslationRole,QList<Triplet> > codons;
        fill3To1(map, codons, srcAlphabet, dstAlphabet, amino, role, n1, n2, n3);

        DNATranslation* t = new DNATranslation3to1Impl(id, name,
            srcAlphabet, dstAlphabet, map, 'X', codons);
        treg->registerDNATranslation(t);
    }
    QByteArray an1(n1);
    const char* rn1 = an1.replace('T', 'U');
    QByteArray an2(n2);
    const char* rn2 = an2.replace('T', 'U');
    QByteArray an3(n3);
    const char* rn3 = an3.replace('T', 'U');
    {
        const DNAAlphabet* srcAlphabet = findById(BaseDNAAlphabetIds::NUCL_RNA_DEFAULT());
        const DNAAlphabet* dstAlphabet = findById(BaseDNAAlphabetIds::AMINO_DEFAULT());

        QList<Mapping3To1<char> > map;
        QMap<DNATranslationRole,QList<Triplet> > codons;
        fill3To1(map, codons, srcAlphabet, dstAlphabet, amino, role, rn1, rn2, rn3);

        DNATranslation* t = new DNATranslation3to1Impl(id, name,
            srcAlphabet, dstAlphabet, map, 'X', codons);
        treg->registerDNATranslation(t);
    }

    {
        const DNAAlphabet* srcAlphabet = findById(BaseDNAAlphabetIds::NUCL_RNA_EXTENDED());
        const DNAAlphabet* dstAlphabet = findById(BaseDNAAlphabetIds::AMINO_DEFAULT());

        QList<Mapping3To1<char> > map;
        QMap<DNATranslationRole,QList<Triplet> > codons;
        fill3To1(map, codons, srcAlphabet, dstAlphabet, amino, role, rn1, rn2, rn3);

        DNATranslation* t = new DNATranslation3to1Impl(id, name,
            srcAlphabet, dstAlphabet, map, 'X', codons);
        treg->registerDNATranslation(t);
    }
}

void DNAAlphabetRegistryImpl::regPtables(const char* amino, const int* prob, const char* n1, const char* n2, const char* n3,
    const QString& id, const QString& name)
{
    {
        const DNAAlphabet* srcAlphabet = findById(BaseDNAAlphabetIds::AMINO_DEFAULT());
        const DNAAlphabet* dstAlphabet = findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());

        BackTranslationRules map;
        fill1To3(map, srcAlphabet, dstAlphabet, amino, prob, n1, n2, n3);

        DNATranslation* t = new DNATranslation1to3Impl(id, name,
            srcAlphabet, dstAlphabet, map);
        treg->registerDNATranslation(t);
    }
}

#define CASE_OFFSET ('a'-'A')
#define MAP(a, b) \
map[int(a)] = b; \
if (!srcAlphabet->isCaseSensitive()) { \
map[int(a) + CASE_OFFSET]=(b)+CASE_OFFSET; \
}


void DNAAlphabetRegistryImpl::initBaseTranslations() {

    //default NUCL DNA complement
    {
        const DNAAlphabet* srcAlphabet = findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
        const DNAAlphabet* dstAlphabet = srcAlphabet;

        QByteArray map = TextUtils::createMap(srcAlphabet->getMap(), 'N');

        MAP('A','T');
        MAP('C','G');
        MAP('G','C');
        MAP('T','A');

        DNATranslation* t = new DNATranslation1to1Impl(BaseDNATranslationIds::NUCL_DNA_DEFAULT_COMPLEMENT,
            tr("Complement for standard DNA"),
            srcAlphabet, dstAlphabet, map);
        treg->registerDNATranslation(t);
    }

    //default NUCL RNA complement
    {
        const DNAAlphabet* srcAlphabet = findById(BaseDNAAlphabetIds::NUCL_RNA_DEFAULT());
        const DNAAlphabet* dstAlphabet = srcAlphabet;

        QByteArray map = TextUtils::createMap(srcAlphabet->getMap(), 'N');

        MAP('A','U');
        MAP('C','G');
        MAP('G','C');
        MAP('U','A');

        DNATranslation* t = new DNATranslation1to1Impl(BaseDNATranslationIds::NUCL_RNA_DEFAULT_COMPLEMENT,
            tr("Complement for standard RNA"),
            srcAlphabet, dstAlphabet, map);
        treg->registerDNATranslation(t);
    }

    //extended NUCL DNA complement
    {
        //source: http://www.geneinfinity.org/sp_nucsymbols.html
        const DNAAlphabet* srcAlphabet = findById(BaseDNAAlphabetIds::NUCL_DNA_EXTENDED());
        const DNAAlphabet* dstAlphabet = srcAlphabet;

        QByteArray map = TextUtils::createMap(srcAlphabet->getMap(), 'N');
        MAP('A','T');
        MAP('C','G');
        MAP('G','C');
        MAP('T','A');
        MAP('M','K');
        MAP('R','Y');
        MAP('W','W');
        MAP('S','S');
        MAP('Y','R');
        MAP('K','M');
        MAP('V','B');
        MAP('H','D');
        MAP('D','H');
        MAP('B','V');

        DNATranslation* t = new DNATranslation1to1Impl(BaseDNATranslationIds::NUCL_DNA_EXTENDED_COMPLEMENT,
            tr("Complement for extended DNA"),
            srcAlphabet, dstAlphabet, map);
        treg->registerDNATranslation(t);
    }


    //extended NUCL RNA complement
    {
        //source: http://www.geneinfinity.org/sp_nucsymbols.html
        const DNAAlphabet* srcAlphabet = findById(BaseDNAAlphabetIds::NUCL_RNA_EXTENDED());
        const DNAAlphabet* dstAlphabet = srcAlphabet;

        QByteArray map = TextUtils::createMap(srcAlphabet->getMap(), 'N');
        MAP('A','U');
        MAP('C','G');
        MAP('G','C');
        MAP('U','A');
        MAP('M','K');
        MAP('R','Y');
        MAP('W','W');
        MAP('S','S');
        MAP('Y','R');
        MAP('K','M');
        MAP('V','B');
        MAP('H','D');
        MAP('D','H');
        MAP('B','V');

        DNATranslation* t = new DNATranslation1to1Impl(BaseDNATranslationIds::NUCL_RNA_EXTENDED_COMPLEMENT,
            tr("Complement for extended RNA"),
            srcAlphabet, dstAlphabet, map);
        treg->registerDNATranslation(t);
    }

    // source: http://www.ncbi.nlm.nih.gov/Taxonomy/Utils/wprintgc.cgi
    // 1. The Standard Code (transl_table=1)
    reg4tables(
        "FFLLSSSSYY**CC*WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
        "---L---------------L---------------M----------------------------",
        "TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG",
        "TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG",
        "TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG",
        DNATranslationID(1),
        tr("1. The Standard Genetic Code"));

    //2. The Vertebrate Mitochondrial Code (transl_table=2)
    reg4tables(
        "FFLLSSSSYY**CCWWLLLLPPPPHHQQRRRRIIMMTTTTNNKKSS**VVVVAAAADDEEGGGG",
        "--------------------------------LLLM---------------L------------",
        "TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG",
        "TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG",
        "TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG",
        DNATranslationID(2),
        tr("2. The Vertebrate Mitochondrial Code"));

    //3. The Yeast Mitochondrial Code (transl_table=3)
    reg4tables(
        "FFLLSSSSYY**CCWWTTTTPPPPHHQQRRRRIIMMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
        "----------------------------------LM----------------------------",
        "TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG",
        "TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG",
        "TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG",
        DNATranslationID(3),
        tr("3. The Yeast Mitochondrial Code"));

    //4. The Mold, Protozoan, and Coelenterate Mitochondrial Code and the Mycoplasma/Spiroplasma Code (transl_table=4)
    reg4tables(
        "FFLLSSSSYY**CCWWLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
        "--LL---------------L------------LLLM---------------L------------",
        "TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG",
        "TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG",
        "TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG",
        DNATranslationID(4),
        tr("4. The Mold, Protozoan, and Coelenterate Mitochondria and the Mycoplasma Code"));

    //5. The Invertebrate Mitochondrial Code (transl_table=5)
    reg4tables(
        "FFLLSSSSYY**CCWWLLLLPPPPHHQQRRRRIIMMTTTTNNKKSSSSVVVVAAAADDEEGGGG",
        "---L----------------------------LLLM---------------L------------",
        "TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG",
        "TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG",
        "TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG",
        DNATranslationID(5),
        tr("5. The Invertebrate Mitochondrial Code"));

    //6. The Ciliate, Dasycladacean and Hexamita Nuclear Code (transl_table=6)
    reg4tables(
        "FFLLSSSSYYQQCC*WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
        "-----------------------------------M----------------------------",
        "TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG",
        "TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG",
        "TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG",
        DNATranslationID(6),
        tr("6. The Ciliate, Dasycladacean and Hexamita Nuclear Code"));

    //9. The Echinoderm and Flatworm Mitochondrial Code (transl_table=9)
    reg4tables(
        "FFLLSSSSYY**CCWWLLLLPPPPHHQQRRRRIIIMTTTTNNNKSSSSVVVVAAAADDEEGGGG",
        "-----------------------------------M---------------L------------",
        "TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG",
        "TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG",
        "TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG",
        DNATranslationID(9),
        tr("9. The Echinoderm and Flatworm Mitochondrial Code"));

    //10. The Euplotid Nuclear Code (transl_table=10)
    reg4tables(
        "FFLLSSSSYY**CCCWLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
        "-----------------------------------M----------------------------",
        "TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG",
        "TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG",
        "TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG",
        DNATranslationID(10),
        tr("10. The Euplotid Nuclear Code"));

    //11. The Bacterial and Plant Plastid Code (transl_table=11)
    reg4tables(
        "FFLLSSSSYY**CC*WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
        "---L---------------L------------LLLM---------------L------------",
        "TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG",
        "TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG",
        "TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG",
        DNATranslationID(11),
        tr("11. The Bacterial and Plant Plastid Code"));

    //12. The Alternative Yeast Nuclear Code (transl_table=12)
    reg4tables(
        "FFLLSSSSYY**CC*WLLLSPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
        "-------------------L---------------M----------------------------",
        "TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG",
        "TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG",
        "TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG",
        DNATranslationID(12),
        tr("12. The Alternative Yeast Nuclear Code"));

    //13. The Ascidian Mitochondrial Code (transl_table=13)
    reg4tables(
        "FFLLSSSSYY**CCWWLLLLPPPPHHQQRRRRIIMMTTTTNNKKSSGGVVVVAAAADDEEGGGG",
        "---L------------------------------LM---------------L------------",
        "TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG",
        "TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG",
        "TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG",
        DNATranslationID(13),
        tr("13. The Ascidian Mitochondrial Code"));

    //14. The Alternative Flatworm Mitochondrial Code (transl_table=14)
    reg4tables(
        "FFLLSSSSYYY*CCWWLLLLPPPPHHQQRRRRIIIMTTTTNNNKSSSSVVVVAAAADDEEGGGG",
        "-----------------------------------M----------------------------",
        "TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG",
        "TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG",
        "TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG",
        DNATranslationID(14),
        tr("14. The Alternative Flatworm Mitochondrial Code"));

    //15. Blepharisma Nuclear Code (transl_table=15)
    reg4tables(
        "FFLLSSSSYY*QCC*WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
        "-----------------------------------M----------------------------",
        "TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG",
        "TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG",
        "TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG",
        DNATranslationID(15),
        tr("15. Blepharisma Nuclear Code"));


    //16. Chlorophycean Mitochondrial Code (transl_table=16)
    reg4tables(
        "FFLLSSSSYY*LCC*WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
        "-----------------------------------M----------------------------",
        "TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG",
        "TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG",
        "TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG",
        DNATranslationID(16),
        tr("16. Chlorophycean Mitochondrial Code"));

    //21. Trematode Mitochondrial Code (transl_table=21)
    reg4tables(
        "FFLLSSSSYY**CCWWLLLLPPPPHHQQRRRRIIMMTTTTNNNKSSSSVVVVAAAADDEEGGGG",
        "-----------------------------------M---------------L------------",
        "TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG",
        "TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG",
        "TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG",
        DNATranslationID(21),
        tr("21. Trematode Mitochondrial Code"));

    //22. Scenedesmus obliquus mitochondrial Code (transl_table=22)
    reg4tables(
        "FFLLSS*SYY*LCC*WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
        "-----------------------------------M----------------------------",
        "TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG",
        "TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG",
        "TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG",
        DNATranslationID(22),
        tr("22. Scenedesmus obliquus Mitochondrial Code"));


    //23. Thraustochytrium Mitochondrial Code (transl_table=23)
    reg4tables(
        "FF*LSSSSYY**CC*WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
        "--------------------------------L--M---------------L------------",
        "TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG",
        "TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG",
        "TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG",
        DNATranslationID(23),
        tr("23. Thraustochytrium Mitochondrial Code"));

    //24. Pterobranchia Mitochondrial Code (transl_table=24)
    reg4tables(
        "FFLLSSSSYY**CCWWLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSSKVVVVAAAADDEEGGGG",
        "---M------**-------M---------------M---------------M------------",
        "TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG",
        "TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG",
        "TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG",
        DNATranslationID(24),
        tr("24. Pterobranchia Mitochondrial Code"));

    //25. Candidate Division SR1 and Gracilibacteria Code (transl_table=25)
    reg4tables(
        "FFLLSSSSYY**CCGWLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
        "---M------**-----------------------M---------------M------------",
        "TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG",
        "TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG",
        "TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG",
        DNATranslationID(25),
        tr("25. Candidate Division SR1 and Gracilibacteria Code"));

    //26. Pachysolen tannophilus Nuclear Code (transl_table=26)
    reg4tables(
        "FFLLSSSSYY**CC*WLLLAPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
        "----------**--*----M---------------M----------------------------",
        "TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG",
        "TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG",
        "TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG",
        DNATranslationID(26),
        tr("26. Pachysolen tannophilus Nuclear Code"));

    //27. Karyorelict Nuclear (transl_table=27)
    reg4tables(
        "FFLLSSSSYYQQCCWWLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
        "--------------*--------------------M----------------------------",
        "TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG",
        "TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG",
        "TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG",
        DNATranslationID(27),
        tr("27. Karyorelict Nuclear"));

    //28. Condylostoma Nuclear (transl_table=28)
    reg4tables(
        "FFLLSSSSYYQQCCWWLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
        "----------**--*--------------------M----------------------------",
        "TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG",
        "TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG",
        "TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG",
        DNATranslationID(28),
        tr("28. Condylostoma Nuclear"));

    //29. Mesodinium Nuclear (transl_table=29)
    reg4tables(
        "FFLLSSSSYYYYCC*WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
        "--------------*--------------------M----------------------------",
        "TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG",
        "TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG",
        "TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG",
        DNATranslationID(29),
        tr("29. Mesodinium Nuclear"));

    //30. Peritrich Nuclear (transl_table=30)
    reg4tables(
        "FFLLSSSSYYEECC*WLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
        "--------------*--------------------M----------------------------",
        "TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG",
        "TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG",
        "TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG",
        DNATranslationID(30),
        tr("30. Peritrich Nuclear"));

    //31. Blastocrithidia Nuclear (transl_table=31)
    reg4tables(
        "FFLLSSSSYYEECCWWLLLLPPPPHHQQRRRRIIIMTTTTNNKKSSRRVVVVAAAADDEEGGGG",
        "----------**-----------------------M----------------------------",
        "TTTTTTTTTTTTTTTTCCCCCCCCCCCCCCCCAAAAAAAAAAAAAAAAGGGGGGGGGGGGGGGG",
        "TTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGGTTTTCCCCAAAAGGGG",
        "TCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAGTCAG",
        DNATranslationID(30),
        tr("31. Blastocrithidia Nuclear"));

    char amino[65], n1[65], n2[65], n3[65];
    int prob[64];

    QString dir = QDir::searchPaths( PATH_PREFIX_DATA ).first() + "/back_translation/";
    QString lastOrganismFile = AppContext::getSettings()->getValue(DATA_FILE_KEY).toString();
    if (lastOrganismFile.isEmpty() || !QFile::exists(lastOrganismFile)) {
        lastOrganismFile = dir + DEFAULT_ORGANISM_FILE;
        AppContext::getSettings()->setValue(DATA_FILE_KEY, lastOrganismFile);
    }
    QXmlStreamReader xml;
    QFile file(lastOrganismFile);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return;
    }
    xml.setDevice(&file);
    QString currentType;
    QString currentName;
    QString fileName;
    QString readableName;

    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType tt = xml.readNext();
        if (tt == QXmlStreamReader::StartDocument) {
            continue;
        }
        if (tt == QXmlStreamReader::StartElement) {
            if (xml.name() == "CodonFrequencyTableList") {
                continue;
            }
            if (xml.name() == "Folder" && xml.attributes().value("name") == "Codon Frequency Tables") {
                continue;
            }
            if (xml.name() == "Folder") {
                currentType = xml.attributes().value("name").toString();
                continue;
            }
            if (xml.name() == "CodonFrequencyTable") {
                currentName = xml.attributes().value("name").toString();
                QStringList buf = currentName.split(".");
                buf.removeLast();
                readableName = buf.join(".").replace("_", " ");
                fileName = currentType + "/" + currentName;
                if (parseCutFile(dir + fileName, amino, prob, n1, n2, n3))
                    regPtables(amino, prob, n1, n2, n3, "BackTranslation/" + fileName, readableName);
            }
        }
    }

    // init codon info
    regCodon(treg, 'A', "Ala", tr("Alanine"),       "http://en.wikipedia.org/wiki/Alanine",         DNACodonGroup_NONPOLAR);
    regCodon(treg, 'C', "Cys", tr("Cysteine"),      "http://en.wikipedia.org/wiki/Cysteine" ,       DNACodonGroup_POLAR);
    regCodon(treg, 'D', "Asp", tr("Aspartic acid"), "http://en.wikipedia.org/wiki/Aspartic_acid",   DNACodonGroup_ACIDIC);
    regCodon(treg, 'E', "Glu", tr("Glutamic acid"), "http://en.wikipedia.org/wiki/Glutamic_acid",   DNACodonGroup_ACIDIC);
    regCodon(treg, 'F', "Phe", tr("Phenylalanine"), "http://en.wikipedia.org/wiki/Phenylalanine",   DNACodonGroup_NONPOLAR);
    regCodon(treg, 'G', "Gly", tr("Glycine"),       "http://en.wikipedia.org/wiki/Glycine",         DNACodonGroup_NONPOLAR);
    regCodon(treg, 'H', "His", tr("Histidine"),     "http://en.wikipedia.org/wiki/Histidine",       DNACodonGroup_BASIC);
    regCodon(treg, 'I', "Ile", tr("Isoleucine"),    "http://en.wikipedia.org/wiki/Isoleucine",      DNACodonGroup_NONPOLAR);
    regCodon(treg, 'K', "Lys", tr("Lysine"),        "http://en.wikipedia.org/wiki/Lysine",          DNACodonGroup_BASIC);
    regCodon(treg, 'L', "Leu", tr("Leucine"),       "http://en.wikipedia.org/wiki/Leucine",         DNACodonGroup_NONPOLAR);
    regCodon(treg, 'M', "Met", tr("Methionine"),    "http://en.wikipedia.org/wiki/Methionine",      DNACodonGroup_NONPOLAR);
    regCodon(treg, 'N', "Asn", tr("Asparagine"),    "http://en.wikipedia.org/wiki/Asparagine",      DNACodonGroup_POLAR);
    regCodon(treg, 'P', "Pro", tr("Proline"),       "http://en.wikipedia.org/wiki/Proline",         DNACodonGroup_NONPOLAR);
    regCodon(treg, 'Q', "Gln", tr("Glutamine"),     "http://en.wikipedia.org/wiki/Glutamine",       DNACodonGroup_POLAR);
    regCodon(treg, 'R', "Arg", tr("Arginine"),      "http://en.wikipedia.org/wiki/Arginine",        DNACodonGroup_BASIC);
    regCodon(treg, 'S', "Ser", tr("Serine"),        "http://en.wikipedia.org/wiki/Serine",          DNACodonGroup_POLAR);
    regCodon(treg, 'T', "Thr", tr("Threonine"),     "http://en.wikipedia.org/wiki/Threonine",       DNACodonGroup_POLAR);
    regCodon(treg, 'V', "Val", tr("Valine"),        "http://en.wikipedia.org/wiki/Valine",          DNACodonGroup_NONPOLAR);
    regCodon(treg, 'W', "Trp", tr("Tryptophan"),    "http://en.wikipedia.org/wiki/Tryptophan",      DNACodonGroup_NONPOLAR);
    regCodon(treg, 'Y', "Tyr", tr("Tyrosine"),      "http://en.wikipedia.org/wiki/Tyrosine",        DNACodonGroup_POLAR);
    regCodon(treg, '*', "*",   tr("Stop codon"),    "http://en.wikipedia.org/wiki/Stop_codon",      DNACodonGroup_STOP);

}

} //namespace
