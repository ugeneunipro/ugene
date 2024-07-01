/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#pragma once

#include <U2Core/Log.h>
#include <U2Core/U2OpStatus.h>

namespace U2 {

/**
    Default helper stub for U2OpStatus
    Note: implementation is not thread safe!
*/
class U2CORE_EXPORT U2OpStatusImpl : public U2OpStatus {
public:
//#define FORCE_OP_STATUS_CHECK
#ifdef FORCE_OP_STATUS_CHECK
    U2OpStatusImpl()
        : cancelFlag(false), progress(-1), muted(false), checked(false) {
    }

    ~U2OpStatusImpl() {
        if (!muted) {
            assert(checked);
        }
    }
    void markChecked() const {
        checked = true;
    }
#else
    U2OpStatusImpl()
        : cancelFlag(false), progress(-1) {
    }
    void markChecked() const {
    }
#endif

    void setError(const QString& err) override {
        error = err;
    }
    QString getError() const override {
        markChecked();
        return error;
    }

    bool hasError() const override {
        markChecked();
        return !error.isEmpty();
    }

    bool isCanceled() const override {
        return cancelFlag != 0;
    }
    void setCanceled(bool v) override {
        cancelFlag = v;
    }

    int getProgress() const override {
        return progress;
    }
    void setProgress(int v) override {
        progress = v;
    }

    QString getDescription() const override {
        return statusDesc;
    }
    void setDescription(const QString& desc) override {
        statusDesc = desc;
    }

    bool hasWarnings() const override {
        return !warnings.isEmpty();
    }
    void addWarning(const QString& w) override {
        warnings << w;
    }
    QStringList getWarnings() const override {
        return warnings;
    }
    void addWarnings(const QStringList& wList) override {
        warnings << wList;
    }

protected:
    /** Keeps error message if operation failed */
    QString error;
    /** Keeps current operation state description */
    QString statusDesc;
    /** Keeps warning message */
    QStringList warnings;
    /** Indicates if operation is canceled or not. If yes - processing must be stopped */
    int cancelFlag;
    /** Current operation progress. -1 - unknown */
    int progress;

#ifdef FORCE_OP_STATUS_CHECK
    /** Operation check state. If not muted - user must ask operation if there was an error! */
    bool muted;
    /** If true, operation result was checked by user */
    mutable bool checked;
#endif
};

/** Logs operation status error using specified log category */
#define LOG_OP(os) \
    if (os.hasError()) { \
        coreLog.error(QString("Operation failed: %1 at %2:%3").arg(os.getError()).arg(__FILE__).arg(__LINE__)); \
    }

/**
    Used to dump error ops to coreLog.
    LogLevel is specified as param. Default is ERROR
*/
class U2CORE_EXPORT U2OpStatus2Log : public U2OpStatusImpl {
public:
    U2OpStatus2Log(LogLevel l = LogLevel_ERROR)
        : level(l) {
    }
    void setError(const QString& err) override {
        U2OpStatusImpl::setError(err);
        coreLog.message(level, err);
    }

private:
    LogLevel level;
};

class U2OpStatusMapping {
public:
    U2OpStatusMapping(int _start, int _len)
        : start(_start), len(_len) {
    }
    int start;
    int len;
};

class U2OpStatusChildImpl : public U2OpStatusImpl {
public:
    U2OpStatusChildImpl(U2OpStatus* _parent = 0, const U2OpStatusMapping& _mapping = U2OpStatusMapping(0, 100))
        : parent(_parent), mapping(_mapping) {
    }
    void setError(const QString& err) override {
        parent->setError(err);
        error = err;
    }

    bool hasError() const override {
        return parent->hasError() || !error.isEmpty();
    }

    bool isCanceled() const override {
        return parent->isCanceled() || cancelFlag != 0;
    }
    void setCanceled(bool v) override {
        parent->setCanceled(v);
        cancelFlag = v;
    }

    void setProgress(int v) override {
        parent->setProgress(mapping.start + v * mapping.len / 100);
        progress = v;
    }

    void setDescription(const QString& desc) override {
        parent->setDescription(desc);
        statusDesc = desc;
    }

private:
    U2OpStatus* parent;
    U2OpStatusMapping mapping;
};

}  // namespace U2
