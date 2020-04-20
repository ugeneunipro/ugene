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

#ifndef _U2_ACTORDOCUMENT_H_
#define _U2_ACTORDOCUMENT_H_

#include <U2Lang/Actor.h>

#include <QTextDocument>

namespace U2 {
namespace Workflow {

/**
 * base class for container of rich document of actor's description
 */
class U2LANG_EXPORT ActorDocument : public QTextDocument {
public:
    ActorDocument(Actor* a)
        : QTextDocument(a), target(a) {}
    virtual ~ActorDocument() {}

    // actor's description is changed when attribute value changes
    // e.g. "read sequence from '1.fa'"
    // see realization in PrompterBaseImpl
    virtual void update(const QVariantMap& ) = 0;

protected:
    // document's target
    Actor* target;

}; // ActorDocument


/**
 * base class for factory of Actor's documents
 */
class U2LANG_EXPORT Prompter {
public:
    virtual ActorDocument * createDescription(Actor *) = 0;
    virtual ~Prompter() {}
}; // Prompter

} // Workflow
} // U2

#endif // _U2_ACTORDOCUMENT_H_
