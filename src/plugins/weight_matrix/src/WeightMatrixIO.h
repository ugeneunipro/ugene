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

#ifndef _U2_WEIGHT_MATRIX_IO_H_
#define _U2_WEIGHT_MATRIX_IO_H_

#include <QObject>
#include <QString>

#include <U2Core/Task.h>

#include <U2Core/PFMatrix.h>
#include <U2Core/PWMatrix.h>

namespace U2 {

class IOAdapterFactory;

class WeightMatrixIO : public QObject {
    Q_OBJECT
public:

    //IDs for LastOpenDirHelper
    static const QString WEIGHT_MATRIX_ID;
    static const QString FREQUENCY_MATRIX_ID;

    //extensions for PFMatrix and PWMatrix files
    static const QString WEIGHT_MATRIX_EXT;
    static const QString FREQUENCY_MATRIX_EXT;

    static QString getAllMatrixFileFilter(bool includeAll = true);
    static QString getPFMFileFilter(bool includeAll = true);
    static QString getPWMFileFilter(bool includeAll = true);
    static PFMatrix readPFMatrix(IOAdapterFactory* iof, const QString& url, TaskStateInfo& si);
    static PWMatrix readPWMatrix(IOAdapterFactory* iof, const QString& url, TaskStateInfo& si);
    static void writePFMatrix(IOAdapterFactory* iof, const QString& url, TaskStateInfo& si, const PFMatrix& model);
    static void writePWMatrix(IOAdapterFactory* iof, const QString& url, TaskStateInfo& si, const PWMatrix& model);
};

class PFMatrixReadTask: public Task {
    Q_OBJECT
public:
    PFMatrixReadTask(const QString& url)
        : Task(tr("Read Frequency Matrix"), TaskFlag_None), url(url) {}
    void run();
    PFMatrix getResult() const {return model;}
    QString getURL() const {return url;}
private:
    QString url;
    PFMatrix model;
};

class PWMatrixReadTask: public Task {
    Q_OBJECT
public:
    PWMatrixReadTask(const QString& url)
        : Task(tr("Read Weight Matrix"), TaskFlag_None), url(url) {}
    void run();
    PWMatrix getResult() const {return model;}
    QString getURL() const {return url;}
private:
    QString url;
    PWMatrix model;
};

class PFMatrixWriteTask : public Task {
    Q_OBJECT
public:
    PFMatrixWriteTask(const QString& url, const PFMatrix& model, uint f = 0)
        : Task(tr("Save position frequency matrix"), TaskFlag_None), url(url), model(model), fileMode(f) {}
    virtual void run();
private:
    QString url;
    PFMatrix model;
    uint fileMode;
};

class PWMatrixWriteTask : public Task {
    Q_OBJECT
public:
    PWMatrixWriteTask(const QString& url, const PWMatrix& model, uint f = 0)
        : Task(tr("Save weight matrix"), TaskFlag_None), url(url), model(model), fileMode(f) {}
    virtual void run();
private:
    QString url;
    PWMatrix model;
    uint fileMode;
};

}//namespace

#endif
