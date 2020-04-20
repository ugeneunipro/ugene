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

#ifndef _U2_TEST_RUNNER_SETTINGS_H_
#define _U2_TEST_RUNNER_SETTINGS_H_


namespace U2 {

class TestRunnerSettings {
public:
    QString getVar(const QString& name) const { return registry.value(name); }
    void setVar(const QString& name, const QString& val) { registry[name] = val; }
    void removeVar(const QString& name) { registry.remove(name); }
private:
    QMap<QString, QString> registry;
};

class APITestData {
public:
    template<class T>
    T getValue(const QString& key) const {
        const QVariant& val = d.value(key);
        return val.value<T>();
    }

    template<class T>
    void addValue(const QString& key, const T& val) {
        assert(!key.isEmpty());
        assert(!d.keys().contains(key));
        const QVariant& var = qVariantFromValue<T>(val);
        d[key] = var;
    }

    template<class T>
    QList<T> getList(const QString& key) const {
        const QVariant& val = d.value(key);
        if (val.type() != QVariant::List) {
            return QList<T>();
        }
        const QVariantList& varList = val.toList();
        QList<T> list;
        foreach(const QVariant& var, varList) {
            list << var.value<T>();
        }
        return list;
    }

    template<class T>
    void addList(const QString& key, const QList<T>& list) {
        assert(!key.isEmpty());
        assert(!d.keys().contains(key));
        QVariantList varList;
        foreach(const T& val, list) {
            varList << qVariantFromValue<T>(val);
        }
        d[key] = varList;
    }

private:
    QMap<QString, QVariant> d;
};

} //namespace

#endif
