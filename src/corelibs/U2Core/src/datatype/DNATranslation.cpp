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

#include "DNATranslation.h"
#include "DNAAlphabet.h"

#include <U2Core/U2SafePoints.h>

namespace U2 {

DNATranslation::DNATranslation(const QString& _id, const QString& _name, const DNAAlphabet* src, const DNAAlphabet* dst) {
    name = _name;
    id = _id;
    srcAlphabet = src;
    dstAlphabet = dst;

    assert(srcAlphabet!=NULL && dstAlphabet!=NULL);

    DNAAlphabetType srcType = srcAlphabet->getType();
    DNAAlphabetType dstType = dstAlphabet->getType();
    type = DNATranslationType_UNKNOWN;

    if (srcType == DNAAlphabet_NUCL) {
        switch(dstType) {
            case DNAAlphabet_NUCL :
                type = dstAlphabet == srcAlphabet ? DNATranslationType_NUCL_2_COMPLNUCL : DNATranslationType_NUCL_2_NUCL; break;
            case DNAAlphabet_AMINO:
                type = DNATranslationType_NUCL_2_AMINO; break;
                        default: assert(0); break;
        }
    } else if (srcType == DNAAlphabet_AMINO) {
        switch(dstType) {
            case DNAAlphabet_AMINO :
                type = DNATranslationType_AMINO_2_AMINO; break;
            case DNAAlphabet_NUCL :
                type = DNATranslationType_AMINO_2_NUCL; break;
            default: assert(0); break;
        }
    } else if (srcType == DNAAlphabet_RAW) {
        switch(dstType) {
            case DNAAlphabet_NUCL :
                type = DNATranslationType_RAW_2_NUCL; break;
            case DNAAlphabet_AMINO:
                type = DNATranslationType_RAW_2_AMINO; break;
                        default: assert(0); break;
        }
    }
    assert(type!=DNATranslationType_UNKNOWN);
}


//////////////////////////////////////////////////////////////////////////
/// Registry

QStringList DNATranslationRegistry::getDNATranlations() const {
    QStringList l;
    foreach(DNATranslation* t, translations) {
        l<<t->getTranslationName();
    }
    return l;
}


QStringList DNATranslationRegistry::getDNATranslationIds() const
{
    QStringList l;
    foreach(DNATranslation* t, translations) {
        l<<t->getTranslationId();
    }
    return l;

}

QStringList DNATranslationRegistry::getDNATranslationIds(const QString& name) const
{
    QStringList l;
    foreach(DNATranslation* t, translations) {
        if (t->getTranslationName() == name) {
            l<<t->getTranslationId();
        }
    }
    return l;

}


void DNATranslationRegistry::registerDNATranslation(DNATranslation* t) {
    translations.push_back(t);
}

void DNATranslationRegistry::registerDNACodon(DNACodon* codon) {
    codons.push_back(codon);
}

DNATranslationRegistry::~DNATranslationRegistry() {
    foreach(DNATranslation* t, translations) {
        delete t;
    }
    translations.clear();

    foreach(DNACodon* c, codons) {
        delete c;
    }
    codons.clear();
}

QList<DNATranslation*> DNATranslationRegistry::lookupTranslation(const DNAAlphabet* srcAlphabet, DNATranslationType type) {
    QList<DNATranslation*> res;
    foreach(DNATranslation* t, translations) {
        if (t->getSrcAlphabet() == srcAlphabet && t->getDNATranslationType() == type) {
            res.append(t);
        }
    }
    return res;
}

DNATranslation* DNATranslationRegistry::getStandardGeneticCodeTranslation(const DNAAlphabet* srcAlphabet){
    if (srcAlphabet->isNucleic()) {
        return lookupTranslation(srcAlphabet, DNATranslationID(1));
    }
    FAIL("Standard genetic code is used only with source nucleic alphabet", NULL);
}

DNATranslation* DNATranslationRegistry::lookupTranslation(const DNAAlphabet* srcAlphabet,
                                                          DNATranslationType type,
                                                          const QString& id)
{
    foreach(DNATranslation* t, translations) {
        if (t->getTranslationId() == id && t->getSrcAlphabet() == srcAlphabet && t->getDNATranslationType() == type) {
            return t;
        }
    }
    return NULL;
}

DNATranslation* DNATranslationRegistry::lookupTranslation(const DNAAlphabet* srcAlphabet, const QString& id){
    foreach(DNATranslation* t, translations) {
        if (t->getTranslationId() == id && srcAlphabet == t->getSrcAlphabet()) {
            return t;
        }
    }
    return NULL;
}

DNATranslation* DNATranslationRegistry::lookupTranslation(const QString& id) {
    foreach(DNATranslation* t, translations) {
        if (t->getTranslationId() == id) {
            return t;
        }
    }
    return NULL;
}

DNATranslation* DNATranslationRegistry::lookupComplementTranslation(const DNAAlphabet* srcAlphabet) {
    assert(srcAlphabet->isNucleic());
    if (srcAlphabet->getId() == BaseDNAAlphabetIds ::NUCL_DNA_DEFAULT()) {
        return lookupTranslation(BaseDNATranslationIds::NUCL_DNA_DEFAULT_COMPLEMENT);
    } else if (srcAlphabet->getId() == BaseDNAAlphabetIds ::NUCL_DNA_EXTENDED()) {
        return lookupTranslation(BaseDNATranslationIds::NUCL_DNA_EXTENDED_COMPLEMENT);
    } else if (srcAlphabet->getId() == BaseDNAAlphabetIds ::NUCL_RNA_DEFAULT()) {
        return lookupTranslation(BaseDNATranslationIds::NUCL_RNA_DEFAULT_COMPLEMENT);
    } else if (srcAlphabet->getId() == BaseDNAAlphabetIds ::NUCL_RNA_EXTENDED()) {
        return lookupTranslation(BaseDNATranslationIds::NUCL_RNA_EXTENDED_COMPLEMENT);
    } else {
        FAIL("Complement translation not supported for alphabet", NULL);
    }
}

DNACodon* DNATranslationRegistry::lookupCodon(char symbol) {
    foreach (DNACodon* c, codons) {
        if (c->getSymbol() == symbol) {
            return c;
        }
    }
    return NULL;
}

}//namespace
