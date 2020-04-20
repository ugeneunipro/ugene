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

#ifndef _TLS_TASK_H_
#define _TLS_TASK_H_

#include <U2Core/Task.h>
#include <QThreadStorage>

namespace U2 {
/************************************************************************/
/* Task with task local storage                                         */
/************************************************************************/

class U2CORE_EXPORT TLSContext {
friend class TLSUtils;
public:
    TLSContext(QString _id) : id(_id) {}
private:
    // class unique id
    const QString id;
};

class TLSContextRef {
public:
  TLSContextRef(TLSContext* _ctx) : ctx(_ctx){}
  TLSContext* ctx;
 };

class U2CORE_EXPORT TLSUtils {
public:
  // Gets task local context, assigned to current thread
  static TLSContext* current(QString contextId);

  // Creates TLSContextRef for current thread
  static void bindToTLSContext(TLSContext *ctx);

  // Deletes TLSContextRef for current thread
  static void detachTLSContext();

private:
  static QThreadStorage<TLSContextRef*> tls;
};

class U2CORE_EXPORT TLSTask : public Task {
Q_OBJECT
public:
  TLSTask(const QString& _name, TaskFlags _flags = TaskFlags_FOSCOE, bool deleteContext = true);
  ~TLSTask(); // tls context removed here
  void run();
  void prepare();
protected:

  // Unsafe run. Use this method in derivate classes
  virtual void _run() {}

  // Creates instance of TLSContext. By default invokes in prepare()
  virtual TLSContext* createContextInstance()=0;

  TLSContext* taskContext;
  bool deleteContext;
};

} //namespace


#endif // _TLS_TASK_H_
