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

#ifndef _U2_DNA_TRANSLATION_H_
#define _U2_DNA_TRANSLATION_H_

#include <U2Core/global.h>
#include <QStringList>

namespace U2 {

class DNAAlphabet;

enum DNATranslationType {
    DNATranslationType_UNKNOWN,
    DNATranslationType_NUCL_2_NUCL,
    DNATranslationType_NUCL_2_COMPLNUCL,
    DNATranslationType_NUCL_2_AMINO,
    DNATranslationType_AMINO_2_AMINO,
    DNATranslationType_AMINO_2_NUCL,
    DNATranslationType_RAW_2_NUCL,
    DNATranslationType_RAW_2_AMINO
};

class U2CORE_EXPORT BaseDNATranslationIds: public QObject {
public:
    static const QString NUCL_DNA_DEFAULT_COMPLEMENT;
    static const QString NUCL_RNA_DEFAULT_COMPLEMENT;

    static const QString NUCL_DNA_EXTENDED_COMPLEMENT;
    static const QString NUCL_RNA_EXTENDED_COMPLEMENT;
};


#define DNATranslationID(n) "NCBI-GenBank #"#n
#define DNABackTranslationID(type, name) "BackTranslation/"#type"/"#name
#define AMINO_TT_GOBJECT_HINT "AminoTT"

class U2CORE_EXPORT DNATranslation {
public:
    DNATranslation(const QString& _id, const QString& _name, const DNAAlphabet* src, const DNAAlphabet* dst);
    virtual ~DNATranslation(){}

    virtual qint64 translate(const char* src, qint64 src_len, char* dst, qint64 dst_capacity) const = 0;
    virtual int translate(char* src_and_dst, int len) const = 0;

    const QString& getTranslationName() const {return name;}
    const QString& getTranslationId() const {return id;}

    DNATranslationType getDNATranslationType() const {return type;}

    const DNAAlphabet* getSrcAlphabet() const {return srcAlphabet;}

    const DNAAlphabet* getDstAlphabet() const {return dstAlphabet;}

    virtual bool isOne2One() const {return false;}

    virtual bool isThree2One() const {return false;}

    virtual bool isOne2Three() const {return false;}

    virtual QByteArray getOne2OneMapper() const {
        assert(0); return QByteArray(256, 0);
    }

    virtual char translate3to1(char c1, char c2, char c3) const {
        Q_UNUSED(c1); Q_UNUSED(c2); Q_UNUSED(c3);
        assert(0); return 0;
    }

    virtual char* translate1to3(char c) const {
        Q_UNUSED(c);
        assert(0); return NULL;
    }

protected:
    DNATranslationType type;
    QString name;
    QString id;
    const DNAAlphabet* srcAlphabet;
    const DNAAlphabet* dstAlphabet;
};

enum DNACodonGroup {
    DNACodonGroup_POLAR,
    DNACodonGroup_NONPOLAR,
    DNACodonGroup_BASIC,
    DNACodonGroup_ACIDIC,
    DNACodonGroup_STOP
};

class U2CORE_EXPORT DNACodon {
public:
    DNACodon(char s, QString code, QString name, DNACodonGroup gr)
        : symbol(s), threeLetterCode(code), fullName(name), group(gr) {}
    virtual ~DNACodon(){}
    void setLink(QString _link) {link = _link; }

    char            getSymbol() const { return symbol; }
    QString         getTreeLetterCode() const { return threeLetterCode; }
    QString         getFullName() const { return fullName; }
    DNACodonGroup   getCodonGroup() const { return group; }
    QString         getLink() const {return link; }
private:
    char            symbol;
    QString         threeLetterCode;
    QString         fullName;
    QString         link;
    DNACodonGroup   group;
};


class U2CORE_EXPORT DNATranslationRegistry : public QObject {
public:
    DNATranslationRegistry(QObject* p=0) :QObject(p){}
    ~DNATranslationRegistry();

    DNATranslation* getStandardGeneticCodeTranslation(const DNAAlphabet* srcAlphabet);

    QStringList getDNATranlations() const;

    QStringList getDNATranslationIds() const;

    QStringList getDNATranslationIds(const QString& name) const;

    void registerDNATranslation(DNATranslation* t);

    void registerDNACodon(DNACodon* codon);

    QList<DNATranslation*> lookupTranslation(const DNAAlphabet* srcAlphabet, DNATranslationType type);

    DNATranslation* lookupTranslation(const DNAAlphabet* srcAlphabet, DNATranslationType type, const QString& id);

    DNATranslation* lookupTranslation(const QString& id);

    DNATranslation* lookupTranslation(const DNAAlphabet* srcAlphabet, const QString& id);

    DNATranslation* lookupComplementTranslation(const DNAAlphabet* srcAlphabet);

    DNACodon* lookupCodon(char symbol);

private:
    QList<DNATranslation*> translations;
    QList<DNACodon*> codons;
};

} //namespace

#endif

