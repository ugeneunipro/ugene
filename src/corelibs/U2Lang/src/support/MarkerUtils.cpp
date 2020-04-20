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

#include <QVariant>

#include "MarkerUtils.h"

namespace U2 {

const QString MarkerUtils::INTERVAL_OPERATION("..");
const QString MarkerUtils::LESS_OPERATION("<=");
const QString MarkerUtils::GREATER_OPERATION(">=");
const QString MarkerUtils::REST_OPERATION("rest");
const QString MarkerUtils::STARTS_OPERATION("starts");
const QString MarkerUtils::ENDS_OPERATION("ends");
const QString MarkerUtils::CONTAINS_OPERATION("contains");
const QString MarkerUtils::REGEXP_OPERATION("regexp");

void MarkerUtils::valueToString(MarkerDataType dataType, const QVariantList &value, QString &string) {
    assert(value.size() >= 1);
    if (REST_OPERATION == value.at(0).toString()) {
        string.clear();
        string.append(REST_OPERATION);
        return;
    }
    if (INTEGER == dataType) {
        assert(value.size() > 1);
        integerValueToString(value, string);
    } else if (FLOAT == dataType) {
        assert(value.size() > 1);
        floatValueToString(value, string);
    } else if (STRING == dataType) {
        assert(2 == value.size());
        textValueToString(value, string);
    }
}

bool MarkerUtils::stringToValue(MarkerDataType dataType, const QString &string, QVariantList &value) {
    if (REST_OPERATION == string) {
        value.append(string);
        return true;
    }

    if (INTEGER == dataType) {
        return stringToIntValue(string, value);
    } else if (FLOAT == dataType) {
        return stringToFloatValue(string, value);
    } else if (STRING == dataType) {
        return stringToTextValue(string, value);
    }

    return false;
}

bool MarkerUtils::stringToIntValue(const QString &string, QVariantList &value) {
    bool ok = false;
    if (string.startsWith(LESS_OPERATION)) {
        QString number = string.right(string.length() - LESS_OPERATION.length());

        value.append(QVariant(LESS_OPERATION));
        value.append(QVariant(number.toInt(&ok)));
        return ok;
    } else if (string.startsWith(GREATER_OPERATION)) {
        QString number = string.right(string.length() - GREATER_OPERATION.length());

        value.append(QVariant(GREATER_OPERATION));
        value.append(QVariant(number.toInt(&ok)));
        return ok;
    } else if (string.contains(INTERVAL_OPERATION)) {
        int pos = string.indexOf(INTERVAL_OPERATION);
        QString number1 = string.left(pos);
        QString number2 = string.right(string.length() - pos - INTERVAL_OPERATION.length());

        value.append(QVariant(INTERVAL_OPERATION));
        value.append(QVariant(number1.toInt(&ok)));

        bool ok2 = false;
        value.append(QVariant(number2.toInt(&ok2)));
        return (ok && ok2);
    } else {
        return false;
    }
}

bool MarkerUtils::stringToFloatValue(const QString &string, QVariantList &value) {
    bool ok = false;
    if (string.startsWith(LESS_OPERATION)) {
        QString number = string.right(string.length() - LESS_OPERATION.length());

        value.append(QVariant(LESS_OPERATION));
        value.append(QVariant(number.toFloat(&ok)));
        return ok;
    } else if (string.startsWith(GREATER_OPERATION)) {
        QString number = string.right(string.length() - GREATER_OPERATION.length());

        value.append(QVariant(GREATER_OPERATION));
        value.append(QVariant(number.toFloat(&ok)));
        return ok;
    } else if (string.contains(INTERVAL_OPERATION)) {
        int pos = string.indexOf(INTERVAL_OPERATION);
        QString number1 = string.left(pos);
        QString number2 = string.right(string.length() - pos - INTERVAL_OPERATION.length());

        value.append(QVariant(INTERVAL_OPERATION));
        value.append(QVariant(number1.toFloat(&ok)));

        bool ok2 = false;
        value.append(QVariant(number2.toFloat(&ok2)));
        return (ok && ok2);
    } else {
        return false;
    }
}

bool MarkerUtils::stringToTextValue(const QString &string, QVariantList &value) {
    QString expr;
    QString operation;
    if (string.startsWith(STARTS_OPERATION)) {
        expr = string.right(string.length() - STARTS_OPERATION.length()).trimmed();
        operation = STARTS_OPERATION;
    } else if (string.startsWith(ENDS_OPERATION)) {
        expr = string.right(string.length() - ENDS_OPERATION.length()).trimmed();
        operation = ENDS_OPERATION;
    } else if (string.startsWith(CONTAINS_OPERATION)) {
        expr = string.right(string.length() - CONTAINS_OPERATION.length()).trimmed();
        operation = CONTAINS_OPERATION;
    } else if (string.startsWith(REGEXP_OPERATION)) {
        expr = string.right(string.length() - REGEXP_OPERATION.length()).trimmed();
        operation = REGEXP_OPERATION;
    } else {
        return false;
    }

    value.append(QVariant(operation));
    value.append(QVariant(expr));

    return true;
}

void MarkerUtils::integerValueToString(const QVariantList &value, QString &string) {
    QString operation = value.at(0).toString();

    if (INTERVAL_OPERATION == operation) {
        assert(3 == value.size());
        bool ok1 = false;
        bool ok2 = false;
        QByteArray num1 = QByteArray::number(value.at(1).toInt(&ok1));
        QByteArray num2 = QByteArray::number(value.at(2).toInt(&ok2));
        string = num1 + INTERVAL_OPERATION + num2;
        assert(ok1);
        assert(ok2);
    } else if (LESS_OPERATION == operation) {
        assert(2 == value.size());
        bool ok = false;
        QByteArray num = QByteArray::number(value.at(1).toInt(&ok));
        string = LESS_OPERATION + num;
        assert(ok);
    } else if (GREATER_OPERATION == operation) {
        assert(2 == value.size());
        bool ok = false;
        QByteArray num = QByteArray::number(value.at(1).toInt(&ok));
        string = GREATER_OPERATION + num;
        assert(ok);
    } else {
        assert(0);
    }
}

void MarkerUtils::floatValueToString(const QVariantList &value, QString &string) {
    QString operation = value.at(0).toString();

    if (INTERVAL_OPERATION == operation) {
        assert(3 == value.size());
        bool ok1 = false;
        bool ok2 = false;
        QByteArray num1 = QByteArray::number(value.at(1).toFloat(&ok1));
        QByteArray num2 = QByteArray::number(value.at(2).toFloat(&ok2));
        string = num1 + INTERVAL_OPERATION + num2;
        assert(ok1);
        assert(ok2);
    } else if (LESS_OPERATION == operation) {
        assert(2 == value.size());
        bool ok = false;
        QByteArray num = QByteArray::number(value.at(1).toFloat(&ok));
        string = LESS_OPERATION + num;
        assert(ok);
    } else if (GREATER_OPERATION == operation) {
        assert(2 == value.size());
        bool ok = false;
        QByteArray num = QByteArray::number(value.at(1).toFloat(&ok));
        string = GREATER_OPERATION + num;
        assert(ok);
    } else {
        assert(0);
    }
}

void MarkerUtils::textValueToString(const QVariantList &value, QString &string) {
    string += value.at(0).toString() + " ";
    string += value.at(1).toString();
}


} // U2
