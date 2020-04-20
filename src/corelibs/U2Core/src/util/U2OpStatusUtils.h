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

#ifndef _U2_OPSTATUS_UTILS_H_
#define _U2_OPSTATUS_UTILS_H_

#include <U2Core/U2OpStatus.h>
#include <U2Core/Log.h>

namespace U2 {

/**
    Default helper stub for U2OpStatus
    Note: implementation is not thread safe!
*/
class U2CORE_EXPORT U2OpStatusImpl : public U2OpStatus {
public:

//#define FORCE_OP_STATUS_CHECK
#ifdef FORCE_OP_STATUS_CHECK
    U2OpStatusImpl() : cancelFlag(false), progress(-1) , muted(false), checked(false) {}

    ~U2OpStatusImpl() {
        if (!muted) {
            assert(checked);
        }
    }
    void markChecked() const {checked = true;}
#else
    U2OpStatusImpl() : cancelFlag(false), progress(-1) {}
    void markChecked() const {}
#endif

    virtual void setError(const QString & err) {error = err;}
    virtual QString getError() const  {markChecked(); return error;}

    virtual bool hasError() const {markChecked(); return !error.isEmpty();}

    virtual bool isCanceled() const {return cancelFlag != 0;}
    virtual void setCanceled(bool v)  {cancelFlag = v;}

    virtual int getProgress() const {return progress;}
    virtual void setProgress(int v)  {progress = v;}

    virtual QString getDescription() const {return statusDesc;}
    virtual void setDescription(const QString& desc)  {statusDesc = desc;}

    virtual bool hasWarnings() const { return !warnings.isEmpty(); }
    virtual void addWarning(const QString &w) { warnings << w; }
    virtual QStringList getWarnings() const { return warnings; }
    virtual void addWarnings(const QStringList &wList) { warnings << wList; }

protected:
    /** Keeps error message if operation failed */
    QString error;
    /** Keeps current operation state description */
    QString statusDesc;
    /** Keeps warning message */
    QStringList warnings;
    /** Indicates if operation is canceled or not. If yes - processing must be stopped */
    int     cancelFlag;
    /** Current operation progress. -1 - unknown */
    int     progress;

#ifdef FORCE_OP_STATUS_CHECK
    /** Operation check state. If not muted - user must ask operation if there was an error! */
    bool    muted;
    /** If true, operation result was checked by user */
    mutable bool    checked;
#endif
};

/** Logs operation status error using specified log category */
#define LOG_OP(os)\
    if (os.hasError()) {\
        coreLog.error(QString("Operation failed: %1 at %2:%3").arg(os.getError()).arg(__FILE__).arg(__LINE__));\
    }


/**
    Used to dump error ops to coreLog.
    LogLevel is specified as param. Default is ERROR
*/
class U2CORE_EXPORT U2OpStatus2Log : public U2OpStatusImpl {
public:
    U2OpStatus2Log(LogLevel l = LogLevel_ERROR) : level (l){}
    virtual void setError(const QString &err) {
        U2OpStatusImpl::setError(err);
        coreLog.message(level, err);
    }

private:
    LogLevel level;
};

class U2OpStatusMapping{
public:
    U2OpStatusMapping(int _start, int _len):start(_start),len(_len){}
    int start;
    int len;

};

class U2OpStatusChildImpl : public U2OpStatusImpl{
public:
    U2OpStatusChildImpl(U2OpStatus* _parent = 0, const U2OpStatusMapping& _mapping = U2OpStatusMapping(0, 100)):parent(_parent), mapping(_mapping){}
    virtual void setError(const QString & err) {
        parent->setError(err);
        error = err;
    }

    virtual bool hasError() const {
        return parent->hasError() || !error.isEmpty();
    }

    virtual bool isCanceled() const {
        return parent->isCanceled() || cancelFlag != 0;
    }
    virtual void setCanceled(bool v)  {
        parent->setCanceled(v);
        cancelFlag = v;
    }

    virtual void setProgress(int v)  {
        parent->setProgress(mapping.start + v * mapping.len / 100);
        progress = v;
    }

    virtual void setDescription(const QString& desc)  {
        parent->setDescription(desc);
        statusDesc = desc;
    }
private:
    U2OpStatus* parent;
    U2OpStatusMapping mapping;
};

}// namespace

#endif
