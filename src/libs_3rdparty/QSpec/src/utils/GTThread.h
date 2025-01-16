/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#ifndef _HI_GT_THREAD_H_
#define _HI_GT_THREAD_H_

#include <GTGlobals.h>
#include <core/CustomScenario.h>
#include <type_traits>
#include <utility>

#include <QTimer>

namespace HI {

class HI_EXPORT GTThread {
public:
    static void waitForMainThread();
    static void runInMainThread(CustomScenario* scenario);

    template<typename T,
             typename = typename std::enable_if<!std::is_base_of<
                 CustomScenario,
                 typename std::remove_pointer<T>::type>::value>::type>
    static void runInMainThread(T&& func) {
        class Aux : public CustomScenario {
            T f;

        public:
            Aux(T f)
                : f(std::move(f)) {
            }
            void run() override {
                f();
            }
        };
        runInMainThread(new Aux(std::forward<T>(func)));
    }

    /** Returns true if the current thread is the main QT thread. */
    static bool isMainThread();
};

}  // namespace HI

#endif  // _HI_GT_THREAD_H_
