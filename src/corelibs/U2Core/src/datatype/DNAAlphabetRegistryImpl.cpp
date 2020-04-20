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

#include <U2Core/DNAAlphabet.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2AlphabetUtils.h>

namespace U2 {

/* TRANSLATOR U2::DNAAlphabetRegistryImpl */

DNAAlphabetRegistryImpl::DNAAlphabetRegistryImpl(DNATranslationRegistry* _tr) {
    treg = _tr;
    initBaseAlphabets();
    initBaseTranslations();
}

DNAAlphabetRegistryImpl::~DNAAlphabetRegistryImpl() {
    foreach(const DNAAlphabet* a, alphabets) {
        delete a;
    }
    alphabets.clear();
}

static bool alphabetComplexityComparator(const DNAAlphabet* a1, const DNAAlphabet* a2) {
    int a1Size = a1->getMap().count(true); //TODO: cache this val
    int a2Size = a2->getMap().count(true);
    return a1Size < a2Size;
}


bool DNAAlphabetRegistryImpl::registerAlphabet(const DNAAlphabet* a) {
    if (findById(a->getId())!=NULL) {
        return false;
    }
    alphabets.push_back(a);
    //WARN: original order for equal alphabets must not be changed (DNA must be before RNA)
    qStableSort(alphabets.begin(), alphabets.end(), alphabetComplexityComparator);
    return true;
}

void DNAAlphabetRegistryImpl::unregisterAlphabet(const DNAAlphabet* a) {
    int n = alphabets.removeAll(a);
    assert(n==1); Q_UNUSED(n);
}

const DNAAlphabet* DNAAlphabetRegistryImpl::findById(const QString& id) const {
    foreach(const DNAAlphabet* al, alphabets) {
        if (al->getId() == id) {
            return al;
        }
    }
    return NULL;
}



}//namespace
