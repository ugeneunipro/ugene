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

#include "OldUWL.h"
#include "HRSchemaSerializer.h"
#include "HRWizardSerializer.h"
#include "Utils.h"

#include "Constants.h"

#include "Tokenizer.h"

namespace U2 {
namespace WorkflowSerialize {

/************************************************************************/
/* Tokenizer */
/************************************************************************/
QString Tokenizer::take() {
    if(tokens.isEmpty()) {
        throw ReadFailed(QObject::tr("Unexpected end of file"));
    }
    return tokens.takeFirst();
}

QString Tokenizer::look() const {
    if(tokens.isEmpty()) {
        throw ReadFailed(QObject::tr("Unexpected end of file"));
    }
    return tokens.first();
}

void Tokenizer::appendToken(const QString & t, bool skipEmpty) {
    if(t.isEmpty() && skipEmpty) {
        return;
    }
    if(t == Constants::BLOCK_START) {
        depth++;
    }
    if(t == Constants::BLOCK_END) {
        depth--;
    }
    tokens.append(t);
}

void Tokenizer::addToken(const QString & t) {
    QString tok = t.trimmed().replace("'", "\"");
    if(tok.isEmpty() || tok == Constants::SEMICOLON) {
        return;
    }
    if(tok.contains(Constants::EQUALS_SIGN) && tok != Constants::EQUALS_SIGN) {
        int ind = tok.indexOf(Constants::EQUALS_SIGN);
        assert(ind != -1);
        appendToken(tok.mid(0, ind));
        appendToken(Constants::EQUALS_SIGN);
        appendToken(tok.mid(ind + 1));
        return;
    }
    if(tok.contains(Constants::DATAFLOW_SIGN) && tok != Constants::DATAFLOW_SIGN) {
        QStringList splitted = tok.split(Constants::DATAFLOW_SIGN);
        assert(splitted.size() == 2);
        appendToken(splitted.at(0));
        appendToken(Constants::DATAFLOW_SIGN);
        appendToken(splitted.at(1));
        return;
    }
    if(tok.endsWith(Constants::BLOCK_START) && tok != Constants::BLOCK_START) {
        appendToken(tok.mid(0, tok.size() - Constants::BLOCK_START.size()));
        appendToken(Constants::BLOCK_START);
        return;
    }
    if( tok.startsWith(Constants::BLOCK_START) && tok != Constants::BLOCK_START ) {
        appendToken(Constants::BLOCK_START);
        appendToken(tok.mid(1));
        return;
    }
    if(tok.startsWith(Constants::BLOCK_END) && tok != Constants::BLOCK_END) {
        appendToken(Constants::BLOCK_END);
        appendToken(tok.mid(1));
        return;
    }
    if(tok.endsWith(Constants::BLOCK_END) && tok != Constants::BLOCK_END) {
        appendToken(tok.mid(0, tok.size() - Constants::BLOCK_END.size()));
        appendToken(Constants::BLOCK_END);
        return;
    }
    appendToken(tok);
}

void Tokenizer::removeCommentTokens() {
    foreach(const QString & t, tokens) {
        if(t.startsWith(Constants::SERVICE_SYM)) {
            tokens.removeAll(t);
        }
    }
}

void Tokenizer::assertToken(const QString & etalon) {
    QString candidate = take();
    if( candidate != etalon ) {
        throw ReadFailed(QObject::tr("Expected '%1', got %2").arg(etalon).arg(candidate));
    }
}

static bool isBlockLine(const QString & str) {
    int bInd = str.indexOf(Constants::BLOCK_START);
    int eInd = str.indexOf(Constants::EQUALS_SIGN);
    if(bInd == -1) {
        return false;
    } else {
        if(eInd == -1) {
            return true;
        } else {
            return bInd < eInd;
        }
    }
}

static const int WIZARD_PAGE_DEPTH = 3;
static const int ESTIMATIONS_DEPTH = 2;
static const int ELEMENT_DEPTH = 1;
void Tokenizer::tokenizeSchema(const QString & d) {
    depth = 0;
    QString data = d;
    QTextStream stream(&data);
    bool isElemDef = false;
    bool elemDefHeader = false;
    bool pageDef = false;
    bool pageDefHeader = false;
    bool estDef = false;
    do {
        QString line = stream.readLine().trimmed();
        if(line.isEmpty()) {
            continue;
        }
        if( line.startsWith(Constants::SERVICE_SYM) ) {
            appendToken(line);
            continue;
        }
        if (ELEMENT_DEPTH == depth) {
            isElemDef = !line.startsWith(Constants::META_START) && !line.startsWith(Constants::DOT_ITERATION_START) && !line.contains(Constants::DATAFLOW_SIGN)
                && !line.startsWith(Constants::INPUT_START) && !line.startsWith(Constants::OUTPUT_START) && !line.startsWith(Constants::ATTRIBUTES_START);
            elemDefHeader = true;
        } else {
            elemDefHeader = false;
        }
        if (WIZARD_PAGE_DEPTH == depth) {
            pageDef = line.startsWith(HRWizardParser::PAGE);
            pageDefHeader = true;
        } else {
            pageDefHeader = false;
        }
        if (ESTIMATIONS_DEPTH == depth) {
            estDef = line.startsWith(Constants::ESTIMATIONS);
        }

        if(isBlockLine(line) && (
            (estDef && !isElemDef)
            || (pageDef && !pageDefHeader)
            || (isElemDef && !elemDefHeader)
            )) {
            tokenizeBlock(line, stream);
            continue;
        }
        tokenizeLine(line, stream);
    } while (!stream.atEnd());
}

void Tokenizer::tokenize(const QString &d, int unparseableBlockDepth) {
    depth = 0;
    QString data = d;
    QTextStream stream(&data);
    do {
        QString line = stream.readLine().trimmed();
        if (line.isEmpty()) {
            continue;
        }
        if (line.startsWith(Constants::SERVICE_SYM)) {
            appendToken(line);
            continue;
        }
        if (isBlockLine(line) && depth >= unparseableBlockDepth) {
            tokenizeBlock(line, stream);
        } else {
            tokenizeLine(line, stream);
        }
    } while (!stream.atEnd());
}

static void skipDelimiters(QTextStream & s) {
    while(!s.atEnd()) {
        qint64 curPos = s.pos();
        QChar ch; s >> ch;
        if(ch.isSpace() || ch == Constants::NEW_LINE.at(0) || ch == Constants::SEMICOLON.at(0)) {
            continue;
        }
        s.seek(curPos);
        break;
    }
}

void Tokenizer::tokenizeBlock(const QString & line, QTextStream & s) {
    if(!line.contains(Constants::BLOCK_START)) {
        throw ReadFailed(QObject::tr("Expected '%1', near '%2'").arg(Constants::BLOCK_START).arg(line));
    }
    QString tok = line.mid(0, line.indexOf(Constants::BLOCK_START)).trimmed();
    appendToken(tok);
    appendToken(Constants::BLOCK_START);
    QString blockTok;
    QString ln = line.mid(line.indexOf(Constants::BLOCK_START) + 1);
    if(ln.isEmpty()) {
        ln = s.readLine();
    }
    ln += Constants::NEW_LINE;
    QTextStream stream(&ln);
    int level = 0;
    while(!stream.atEnd()) {
        QChar ch; stream >> ch;
        if(ch == Constants::BLOCK_START.at(0)) {
            level++;
        }
        if(ch == Constants::BLOCK_END.at(0)) {
            if(level-- == 0) {
                appendToken(blockTok.trimmed(), false);
                appendToken(Constants::BLOCK_END);
                skipDelimiters(stream);
                if(!stream.atEnd()) {
                    tokenizeBlock(stream.readAll(), s);
                }
                return;
            }
        }
        blockTok.append(ch);
        if(stream.atEnd()) {
            ln = s.readLine() + Constants::NEW_LINE;
            stream.setString(&ln);
        }
    }
}

void Tokenizer::tokenizeLine(const QString & l, QTextStream & s) {
    QString line = l;
    QTextStream stream(&line);
    QString curToken;
    bool finishAtQuote = false;
    while(!stream.atEnd()) {
        QChar ch;
        stream >> ch;
        if( stream.atEnd() && finishAtQuote && ch != Constants::QUOTE.at(0) ) {
            do {
                curToken.append(ch);
                if (stream.atEnd() && finishAtQuote) {
                    curToken.append('\n');
                }
                line = s.readLine();
                if (line.isEmpty()) {
                    ch = '\n';
                } else {
                    stream.setString(&line);
                }
            } while (line.isEmpty() && !s.atEnd());
            stream >> ch;
        }
        if(ch.isSpace() || ch == Constants::SEMICOLON.at(0)) {
            if(!finishAtQuote) {
                addToken(curToken);
                curToken.clear();
                continue;
            } else {
                curToken.append(ch);
            }
        } else if(ch == Constants::QUOTE.at(0)) {
            if( finishAtQuote ) {
                appendToken(curToken, false);
                curToken.clear();
                finishAtQuote = false;
            } else {
                addToken(curToken);
                curToken.clear();
                finishAtQuote = true;
            }
            continue;
        } else if (ch == OldConstants::MARKER_START.at(0)) {
            if (finishAtQuote) {
                curToken.append(ch);
            } else {
                addToken(curToken);
                curToken.clear();
                addToken(ch);
            }
        } else {
            curToken.append(ch);
        }
    }
    addToken(curToken);
}

/************************************************************************/
/* ParsedPairs */
/************************************************************************/
ParsedPairs::ParsedPairs(Tokenizer & tokenizer, bool bigBlocks) {
    init(tokenizer, bigBlocks);
}

ParsedPairs::ParsedPairs(const QString & data, int unparseableBlockDepth) {
    Tokenizer tokenizer;
    tokenizer.tokenize(data, unparseableBlockDepth);
    init(tokenizer, false);
}

void ParsedPairs::init(Tokenizer & tokenizer, bool bigBlocks) {
    while(tokenizer.notEmpty() && tokenizer.look() != Constants::BLOCK_END) {
        QString tok = tokenizer.take();
        QString next = tokenizer.take();
        if( next == Constants::EQUALS_SIGN ) {
            QString value = tokenizer.take();
            equalPairs[tok] = value;
            equalPairsList << StrStrPair(tok, value);
        }
        else if(next == Constants::BLOCK_START) {
            QString value;
            if (bigBlocks) {
                value = skipBlock(tokenizer);
            } else {
                value = tokenizer.take();
                tokenizer.assertToken(Constants::BLOCK_END);
            }
            blockPairs.insertMulti(tok, value);
            blockPairsList << StrStrPair(tok, value);
        }
        else {
            throw ReadFailed(QObject::tr("Expected %3 or %1 after %2").arg(Constants::BLOCK_START).arg(tok).arg(Constants::EQUALS_SIGN));
        }
    }
}

QPair<QString, QString> ParsedPairs::parseOneEqual(Tokenizer & tokenizer) {
    QPair<QString, QString> res;
    res.first = tokenizer.take();
    if(tokenizer.take() != Constants::EQUALS_SIGN) {
        throw ReadFailed(QObject::tr("%2 expected after %1").arg(res.first).arg(Constants::EQUALS_SIGN));
    }
    res.second = tokenizer.take();
    return res;
}

QString ParsedPairs::skipBlock(Tokenizer &tokenizer) {
    QString skipped;
    while(tokenizer.look() != Constants::BLOCK_END) {
        QString tok = tokenizer.take();
        skipped += "\n" + HRSchemaSerializer::valueString(tok);
        if( tok == Constants::BLOCK_START ) {
            skipped += skipBlock(tokenizer);
            skipped += "\n" + Constants::BLOCK_END;
        }
    }
    tokenizer.take();
    return skipped;
}

} // WorkflowSerialize
} // U2
