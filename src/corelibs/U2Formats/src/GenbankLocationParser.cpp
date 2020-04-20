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

#include <QCoreApplication>

#include <U2Core/Log.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2Location.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2Type.h>

#include "GenbankLocationParser.h"

namespace U2 {

namespace Genbank {

namespace {

class CharacterStream {
public:
    CharacterStream(const QByteArray &input):
        input(input),
        position(0)
    {
    }

    char peek() {
        if(input.size() == position) {
            return '\0';
        }
        return input[position];
    }

    char next() {
        if(input.size() == position) {
            return '\0';
        }
        char result = input[position];
        position++;
        return result;
    }
    char prev() {
        if(0 == position) {
            return '\0';
        }
        char result = input[position];
        position--;
        return result;
    }

private:
    QByteArray input;
    int position;
};

class Token {
public:
    enum Type {
        INVALID,
        END_OF_INPUT,
        LEFT_PARENTHESIS,
        RIGHT_PARENTHESIS,
        CARET,
        LESS,
        GREATER,
        COLON,
        COMMA,
        PERIOD,
        DOUBLE_PERIOD,
        JOIN,
        ORDER,
        BOND,
        COMPLEMENT,
        NUMBER,
        NAME
    };

    Token(const QByteArray &string, Type type):
        string(string),
        type(type)
    {
    }

    const QByteArray &getString() const {
        return string;
    }

    Type getType() const {
        return type;
    }

private:
    QByteArray string;
    Type type;
};

bool isNameCharacter(char c) {
    const QBitArray& digitOrAlpha = TextUtils::ALPHA_NUMS;
    return (digitOrAlpha.testBit(c) || ('_' == c) || ('-' == c) || ('\'' == c) || ('*' == c));
}

class Lexer {
public:
    Lexer(const QByteArray &input):
        input(input),
        nextToken("", Token::INVALID),
        nextTokenValid(false)
    {
    }

    Token peek() {
        if(!nextTokenValid) {
            nextToken = readNext();
            nextTokenValid = true;
        }
        return nextToken;
    }

    Token next() {
        if(nextTokenValid) {
            nextTokenValid = false;
            return nextToken;
        }
        return readNext();
    }

private:
    Token readNext() {
        const QBitArray& WHITES = TextUtils::WHITES;
        char inputChar = input.peek();
        //while(isspace(inputChar)) {       //exclude the locale-specific function
        while(WHITES.testBit(inputChar)) {
            ioLog.trace(QString("GENBANK LOCATION PARSER: Space token (ascii code): %1").arg(static_cast<int>(input.peek())));
            input.next();
            inputChar = input.peek();
        }
        switch(input.peek()) {
        case '\0':
            return Token("<end>", Token::END_OF_INPUT);
        case '(':
            return Token(QByteArray(1, input.next()), Token::LEFT_PARENTHESIS);
        case ')':
            return Token(QByteArray(1, input.next()), Token::RIGHT_PARENTHESIS);
        case '^':
            return Token(QByteArray(1, input.next()), Token::CARET);
        case '<':
            return Token(QByteArray(1, input.next()), Token::LESS);
        case '>':
            return Token(QByteArray(1, input.next()), Token::GREATER);
        case ':':
            return Token(QByteArray(1, input.next()), Token::COLON);
        case ',':
            return Token(QByteArray(1, input.next()), Token::COMMA);
        case '.':
        {
            QByteArray tokenString(1, input.next());
            if('.' == input.peek()) {
                tokenString.append(input.next());
                return Token(tokenString, Token::DOUBLE_PERIOD);
            }
            return Token(tokenString, Token::PERIOD);
        }
        default:
        {
            const QBitArray& NUMS = TextUtils::NUMS;
            QByteArray tokenString;
            if(NUMS.testBit(input.peek()) || '-' == input.peek()) {
                if('-' == input.peek()) {
                    tokenString.append(input.next());
                }
                while(NUMS.testBit(input.peek())) {
                    tokenString.append(input.next());
                }
                if("-" == QString(tokenString)) {
                    tokenString = "";
                    input.prev();
                }
                else if(!isNameCharacter(input.peek())) {
                    return Token(tokenString, Token::NUMBER);
                }
            }
            if(isNameCharacter(input.peek())) {
                while(isNameCharacter(input.peek())) {
                    tokenString.append(input.next());
                }
                if("join" == tokenString) {
                    return Token(tokenString, Token::JOIN);
                }
                if("order" == tokenString) {
                    return Token(tokenString, Token::ORDER);
                }
                if("complement" == tokenString) {
                    return Token(tokenString, Token::COMPLEMENT);
                }
                if("bond" == tokenString) {
                    return Token(tokenString, Token::BOND);
                }
                return Token(tokenString, Token::NAME);
            }
            ioLog.trace(QString("GENBANK LOCATION PARSER: Invalid token (ascii code): %1, next token (ascii)").arg(static_cast<int>(input.peek())));
            char nextChar = input.next();
            ioLog.trace(QString("GENBANK LOCATION PARSER: Next token after invalid (ascii code)").arg(static_cast<int>(nextChar)));
            return Token(QByteArray(1, nextChar), Token::INVALID);
        }
        }
    }

private:
    CharacterStream input;
    Token nextToken;
    bool nextTokenValid;
};

U2Region toRegion(quint64 firstBase, quint64 secondBase) {
    quint64 minBase = qMin(firstBase, secondBase);
    quint64 maxBase = qMax(firstBase, secondBase);
    return U2Region(minBase - 1, maxBase - minBase + 1);
}

//ioLog added to trace an error which occurred on user's OS only
class Parser {
public:
    Parser(const QByteArray &input):
        lexer(input),
        join(false),
        order(false),
        bond(false)
    {
        seqLenForCircular = -1;
    }

    LocationParser::ParsingResult parse(U2Location &result, QStringList &messages) {
        result->regions.clear();
        result->strand = U2Strand::Direct;
        return parseLocation(result, messages);
    }

    void setSeqLenForCircular(qint64 val) { seqLenForCircular = val; }
private:
    qint64 seqLenForCircular;


    bool parseNumber(qint64 &result) {
        if(lexer.peek().getType() != Token::NUMBER) {
            return false;
        }
        QByteArray string = lexer.next().getString();
        result = 0;

        int sign = 1;
        if('-' == string.at(0)) {
            sign = -1;
            string = string.right(1);
        }
        foreach(char c, string) {
            result *= 10;
            result += (quint64)c - '0';
        }
        result = result * sign;
        return true;
    }

    static LocationParser::ParsingResult mergeParsingResults(LocationParser::ParsingResult first, LocationParser::ParsingResult second) {
        if (LocationParser::Failure == first || LocationParser::Failure == second) {
            return LocationParser::Failure;
        }

        if (LocationParser::ParsedWithWarnings == first || LocationParser::ParsedWithWarnings == second) {
            return LocationParser::ParsedWithWarnings;
        }

        if (LocationParser::Success == first || LocationParser::Success == second) {
            return LocationParser::Success;
        }

        FAIL("An unexpected parsing result", LocationParser::Failure);
    }

    LocationParser::ParsingResult parseLocationDescriptor(U2Location &location, QStringList& messages) {
        LocationParser::ParsingResult parsingResult = LocationParser::Success;
        bool remoteEntry = false;
        Token token = lexer.peek();
        if (token.getType() == Token::NAME) { // remote entries
            remoteEntry = true;
            QByteArray accession = lexer.next().getString();
            if (!match(Token::PERIOD)) {
                messages << QString("GENBANK LOCATION PARSER: Must be PERIOD instead of %1").arg(lexer.peek().getString().data());
                ioLog.trace(messages.last());
                return LocationParser::Failure;
            }
            qint64 version = 0;
            if (!parseNumber(version)) {
                messages << QString("GENBANK LOCATION PARSER: can't parse Number. Token: %1").arg(lexer.peek().getString().data());
                ioLog.trace(messages.last());
                return LocationParser::Failure;
            }
            if (!match(Token::COLON)) {
                messages << QString("GENBANK LOCATION PARSER: Must be COLON instead of %1").arg(lexer.peek().getString().data());
                ioLog.trace(messages.last());
                return LocationParser::Failure;
            }
            messages <<  LocationParser::REMOTE_ENTRY_WARNING + ": " + QString(accession) + "." + QString::number(version);
            parsingResult =  LocationParser::ParsedWithWarnings;
        }

        if (token.getType() == Token::COMPLEMENT) {
            lexer.next();
            return mergeParsingResults(parsingResult, parseComplement(location, messages));
        }

        qint64 firstBase = 0;
        bool firstBaseIsFromRange = false;
        if (match(Token::LEFT_PARENTHESIS)) { // cases like (1.2)..
            firstBaseIsFromRange = true;
            if (!parseNumber(firstBase)) { // use the first number as a region boundary
                messages << QString("GENBANK LOCATION PARSER: can't parse Number. Token: %1").arg(lexer.peek().getString().data());
                ioLog.trace(messages.last());
                return LocationParser::Failure;
            }
            else if (firstBase < 0) {
                messages << QString("GENBANK LOCATION PARSER: region boundary can not be less then zero. Token: %1%2").arg(firstBase).arg(lexer.peek().getString().data());
                ioLog.trace(messages.last());
                return LocationParser::Failure;
            }
            if (!match(Token::PERIOD)) {
                messages << QString("GENBANK LOCATION PARSER: Must be PERIOD instead of %1").arg(lexer.peek().getString().data());
                ioLog.trace(messages.last());
                return LocationParser::Failure;
            }
            if (!match(Token::NUMBER)) { // ignore the second number
                messages << QString("GENBANK LOCATION PARSER: Must be NUMBER instead of %1").arg(lexer.peek().getString().data());
                ioLog.trace(messages.last());
                return LocationParser::Failure;
            }
            if (!match(Token::RIGHT_PARENTHESIS)) {
                messages << QString("GENBANK LOCATION PARSER: Must be RIGHT_PARENTHESIS instead of %1").arg(lexer.peek().getString().data());
                ioLog.trace(messages.last());
                return LocationParser::Failure;
            }
            ioLog.info(LocationParser::tr("'a single base from a range' in combination with 'sequence span' is not supported"));
        } else {
            if (match(Token::LESS)) {
                ioLog.info(LocationParser::tr("Ignoring '<' at start position"));
            }
            if (!parseNumber(firstBase)) {
                messages << QString("GENBANK LOCATION PARSER: can't parse Number. Token: %1").arg(lexer.peek().getString().data());
                ioLog.trace(messages.last());
                return LocationParser::Failure;
            }
            else if (firstBase < 0) {
                messages << QString("Region boundary can not be less then zero: %1%2").arg(firstBase).arg(lexer.peek().getString().data());
                ioLog.trace("GENBANK LOCATION PARSER:" + messages.last());
                return LocationParser::Failure;
            }
        }
        if (match(Token::PERIOD)) {
            if (firstBaseIsFromRange) { // ranges are only allowed in spans
                messages << QString("GENBANK LOCATION PARSER: ranges are only allowed in spans. Token: %1").arg(lexer.peek().getString().data());
                ioLog.trace(messages.last());
                return LocationParser::Failure;
            }
            qint64 secondNumber = 0;
            if (!parseNumber(secondNumber)) {
                messages << QString("GENBANK LOCATION PARSER: can't parse Number. Token: %1").arg(lexer.peek().getString().data());
                ioLog.trace(messages.last());
                return LocationParser::Failure;
            }
            else if (secondNumber < 0) {
                messages << QString("Region boundary can not be less then zero: %1%2").arg(secondNumber).arg(lexer.peek().getString().data());
                ioLog.trace("GENBANK LOCATION PARSER:" + messages.last());
                return LocationParser::Failure;
            }
            if (!location->isEmpty()) {
                messages << QString("GENBANK LOCATION PARSER: location is not empty. Token: %1").arg(lexer.peek().getString().data());
                ioLog.trace(messages.last());
                return LocationParser::Failure;
            }
            if (!remoteEntry) { // ignore remote entries
                location->regions.append(toRegion(firstBase, secondNumber));
                location->regionType = U2LocationRegionType_SingleBase;
                messages << QString("GENBANK LOCATION PARSER: remote entries are not supported").arg(lexer.peek().getString().data());
                parsingResult = mergeParsingResults(parsingResult, LocationParser::ParsedWithWarnings);
            }
        } else if (match(Token::DOUBLE_PERIOD)) {
            qint64 secondNumber = 0;
            if (match(Token::LEFT_PARENTHESIS)) { // cases like ..(1.2)
                if (!match(Token::NUMBER)) { // ignore the first number
                    messages << QString("GENBANK LOCATION PARSER: Must be NUMBER instead of %1").arg(lexer.peek().getString().data());
                    ioLog.trace(messages.last());
                    return LocationParser::Failure;
                }
                if (!match(Token::PERIOD)) {
                    messages << QString("GENBANK LOCATION PARSER: Must be PERIOD instead of %1").arg(lexer.peek().getString().data());
                    ioLog.trace(messages.last());
                    return LocationParser::Failure;
                }
                if(!parseNumber(secondNumber)) { // use the second number as a region boudary
                    messages << QString("GENBANK LOCATION PARSER: can't parse Number. Token: %1").arg(lexer.peek().getString().data());
                    ioLog.trace(messages.last());
                    return LocationParser::Failure;
                }
                else if (secondNumber < 0) {
                    messages << QString("Region boundary can not be less then zero: %1%2").arg(secondNumber).arg(lexer.peek().getString().data());
                    ioLog.trace("GENBANK LOCATION PARSER:" + messages.last());
                    return LocationParser::Failure;
                }
                if (!match(Token::RIGHT_PARENTHESIS)) {
                    messages << QString("GENBANK LOCATION PARSER: Must be RIGHT_PARENTHESIS instead of %1").arg(lexer.peek().getString().data());
                    ioLog.trace(messages.last());
                    return LocationParser::Failure;
                }
                ioLog.info(LocationParser::tr("'a single base from a range' in combination with 'sequence span' is not supported"));
            } else {
                if (match(Token::GREATER)) {
                    ioLog.info(LocationParser::tr("Ignoring '>' at end position"));
                }
                if (!parseNumber(secondNumber)) {
                    messages << QString("GENBANK LOCATION PARSER: can't parse Number. Token: %1").arg(lexer.peek().getString().data());
                    ioLog.trace(messages.last());
                    return LocationParser::Failure;
                }
                else if (secondNumber < 0) {
                    messages << QString("Region boundary can not be less then zero: %1%2").arg(secondNumber).arg(lexer.peek().getString().data());
                    ioLog.trace("GENBANK LOCATION PARSER:" + messages.last());
                    return LocationParser::Failure;
                }
            }

            if (!remoteEntry) { // ignore remote entries
                if (seqLenForCircular != -1 && firstBase > secondNumber) {
                    location->regions.append(toRegion(1, secondNumber));
                    location->regions.append(toRegion(firstBase, seqLenForCircular));
                    location->regionType = U2LocationRegionType_Default;
                    location->op = U2LocationOperator_Join;
                } else {
                    location->regions.append(toRegion(firstBase, secondNumber));
                    location->regionType = U2LocationRegionType_Default;
                }
            }
        } else if (match(Token::CARET)) {
            if (firstBaseIsFromRange) { // ranges are only allowed in spans
                return LocationParser::Failure;
            }
            qint64 secondBase = 0;
            if (!parseNumber(secondBase)) {
                return LocationParser::Failure;
            }
            if (!location->isEmpty()) {
                return LocationParser::Failure;
            }
            if (!remoteEntry) { // ignore remote entries
                if (seqLenForCircular != -1 && firstBase > secondBase) {
                    location->regions.append(toRegion(1, secondBase));
                    location->regions.append(toRegion(firstBase, seqLenForCircular));
                    location->regionType = U2LocationRegionType_Default;
                    location->op = U2LocationOperator_Join;
                } else {
                    location->regions.append(toRegion(firstBase, secondBase));
                    location->regionType = U2LocationRegionType_Site;
                }
            }
        } else {
            if (firstBaseIsFromRange) { // ranges are only allowed in spans
                return LocationParser::Failure;
            }
            if (!remoteEntry) { // ignore remote entries
                location->regions.append(toRegion(firstBase, firstBase));
                location->regionType = U2LocationRegionType_Default;
            }
        }
        return parsingResult;
    }

    LocationParser::ParsingResult parseLocation(U2Location &location, QStringList &messages) {
        LocationParser::ParsingResult parsingResult = LocationParser::Success;
        if (match(Token::JOIN)) {
            if (!match(Token::LEFT_PARENTHESIS)) {
                ioLog.trace(QString("GENBANK LOCATION PARSER: Wrong token after JOIN %1").arg(lexer.peek().getString().data()));
                messages << LocationParser::tr("Wrong token after JOIN %1").arg(lexer.peek().getString().data());
                return LocationParser::Failure;
            }
            if (order) {
                ioLog.trace(QString("GENBANK LOCATION PARSER: Wrong token after JOIN  - order %1").arg(lexer.peek().getString().data()));
                messages << LocationParser::tr("Wrong token after JOIN  - order %1").arg(lexer.peek().getString().data());
                return LocationParser::Failure;
            }
            join = true;
            location->op = U2LocationOperator_Join;
            do {
                parsingResult = mergeParsingResults(parsingResult, parseLocation(location, messages));
                if (LocationParser::Failure == parsingResult) {
                    ioLog.trace(QString("GENBANK LOCATION PARSER: Can't parse location on JOIN"));
                    messages << LocationParser::tr("Can't parse location on JOIN");
                    return LocationParser::Failure;
                }
            } while (match(Token::COMMA));
            if (!match(Token::RIGHT_PARENTHESIS)) {
                ioLog.trace(QString("GENBANK LOCATION PARSER: Must be RIGHT_PARENTHESIS instead of %1").arg(lexer.peek().getString().data()));
                messages << LocationParser::tr("Must be RIGHT_PARENTHESIS instead of %1").arg(lexer.peek().getString().data());
                return LocationParser::Failure;
            }
        } else if (match(Token::ORDER)) {
            if (!match(Token::LEFT_PARENTHESIS)) {
                ioLog.trace(QString("GENBANK LOCATION PARSER: Wrong token after ORDER %1").arg(lexer.peek().getString().data()));
                messages << LocationParser::tr("Wrong token after ORDER %1").arg(lexer.peek().getString().data());
                return LocationParser::Failure;
            }
            if (join) {
                ioLog.trace(QString("GENBANK LOCATION PARSER: Wrong token after ORDER - join %1").arg(lexer.peek().getString().data()));
                messages << LocationParser::tr("Wrong token after ORDER - join %1").arg(lexer.peek().getString().data());
                return LocationParser::Failure;
            }
            order = true;
            location->op = U2LocationOperator_Order;
            do {
                parsingResult = mergeParsingResults(parsingResult, parseLocation(location, messages));
                if (LocationParser::Failure == parsingResult) {
                    ioLog.trace(QString("GENBANK LOCATION PARSER: Can't parse location on ORDER"));
                    messages << LocationParser::tr("Can't parse location on ORDER");
                    return LocationParser::Failure;
                }
            } while (match(Token::COMMA));
            if (!match(Token::RIGHT_PARENTHESIS)) {
                ioLog.trace(QString("GENBANK LOCATION PARSER: Must be RIGHT_PARENTHESIS instead of %1").arg(lexer.peek().getString().data()));
                messages << LocationParser::tr("Must be RIGHT_PARENTHESIS instead of %1").arg(lexer.peek().getString().data());
                return LocationParser::Failure;
            }
        } else if (match(Token::BOND)) {
            if (!match(Token::LEFT_PARENTHESIS)) {
                ioLog.trace(QString("GENBANK LOCATION PARSER: Wrong token after BOND %1").arg(lexer.peek().getString().data()));
                messages << LocationParser::tr("Wrong token after BOND %1").arg(lexer.peek().getString().data());
                return LocationParser::Failure;
            }
            bond = true;
            location->op = U2LocationOperator_Bond;
            do {
                parsingResult = mergeParsingResults(parsingResult, parseLocation(location, messages));
                if (LocationParser::Failure == parsingResult) {
                    ioLog.trace(QString("GENBANK LOCATION PARSER: Can't parse location on BOND"));
                    messages << LocationParser::tr("Can't parse location on BONDs");
                    return LocationParser::Failure;
                }
            } while (match(Token::COMMA));
            if (!match(Token::RIGHT_PARENTHESIS)) {
                ioLog.trace(QString("GENBANK LOCATION PARSER: Must be RIGHT_PARENTHESIS instead of %1").arg(lexer.peek().getString().data()));
                messages << LocationParser::tr("Must be RIGHT_PARENTHESIS instead of %1").arg(lexer.peek().getString().data());
                return LocationParser::Failure;
            }

        } else if (match(Token::COMPLEMENT)) {
            return mergeParsingResults(parsingResult, parseComplement(location, messages));
        } else {
            do {
                parsingResult = mergeParsingResults(parsingResult, parseLocationDescriptor(location, messages));
                if (LocationParser::Failure == parsingResult) {
                    ioLog.trace(QString("GENBANK LOCATION PARSER: Can't parse location descriptor"));
                    return LocationParser::Failure;
                }
            } while (match(Token::COMMA));
        }
        return parsingResult;
    }

    LocationParser::ParsingResult parseComplement(U2Location &location, QStringList &messages) {
        LocationParser::ParsingResult parsingResult = LocationParser::Success;
        if (!match(Token::LEFT_PARENTHESIS)) {
            ioLog.trace(QString("GENBANK LOCATION PARSER: Must be LEFT_PARENTHESIS instead of %1").arg(lexer.peek().getString().data()));
            messages << LocationParser::tr("Must be LEFT_PARENTHESIS instead of %1").arg(lexer.peek().getString().data());
            return LocationParser::Failure;
        }

        if (location->regions.isEmpty()) {
            location->strand = U2Strand::Complementary;
        } else {
            ioLog.trace(QString("GENBANK LOCATION PARSER: Locations on different strands are not supported"));
            messages << LocationParser::JOIN_COMPLEMENT_WARNING;
            parsingResult = mergeParsingResults(parsingResult, LocationParser::ParsedWithWarnings);
        }

        // the following doesn't match the specification
        do {
            parsingResult = mergeParsingResults(parsingResult, parseLocation(location, messages));
            if (LocationParser::Failure == parsingResult) {
                ioLog.trace(QString("GENBANK LOCATION PARSER: Can't parse location on COMPLEMENT"));
                messages << LocationParser::tr("Can't parse location on COMPLEMENT");
                return LocationParser::Failure;
            }
        } while (match(Token::COMMA));

        if (!match(Token::RIGHT_PARENTHESIS)) {
            ioLog.trace(QString("GENBANK LOCATION PARSER: Must be RIGHT_PARENTHESIS instead of %1").arg(lexer.peek().getString().data()));
            messages << LocationParser::tr("Must be RIGHT_PARENTHESIS instead of %1").arg(lexer.peek().getString().data());
            return LocationParser::Failure;
        }

        return parsingResult;
    }

    bool match(Token::Type type) {
        if(lexer.peek().getType() == type) {
            lexer.next();
            return true;
        }
        return false;
    }

private:
    Lexer lexer;
    bool join;
    bool order;
    bool bond;
};

}

const QString LocationParser::REMOTE_ENTRY_WARNING = QCoreApplication::translate("LocationParser", "Ignoring remote entry");
const QString LocationParser::JOIN_COMPLEMENT_WARNING = QCoreApplication::translate("LocationParser", "Ignoring different strands in JOIN");

LocationParser::ParsingResult LocationParser::parseLocation(const char *str, int len, U2Location &location, qint64 seqlenForCircular) {
    QStringList messages;
    return parseLocation(str, len,location, messages, seqlenForCircular);
}

LocationParser::ParsingResult LocationParser::parseLocation(const char* str, int len, U2Location& location, QStringList &messages, qint64 seqlenForCircular) {
    Parser parser(QByteArray(str, len));
    parser.setSeqLenForCircular(seqlenForCircular);

    LocationParser::ParsingResult parsingResult = parser.parse(location, messages);
    if (LocationParser::Failure == parsingResult) {
        location->regions.clear();
    }
    return parsingResult;
}

}

}//namespace
