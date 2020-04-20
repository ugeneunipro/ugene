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

#include "KalignUtils.h"
#include <U2Core/Task.h>

extern "C" {

#include "kalign2/kalign2_context.h"
#include <stdarg.h>
#include <stdio.h>

char* k_printf(const char *format, ...) {
	if (format[0] <= 31 || strlen(format) == 1) {
		return 0;
	}
	static char str[1024];
	va_list ArgList;
	va_start(ArgList, format);
	int n = vsprintf(str, format, ArgList);
	assert(n>=0 && n < 1024);
	Q_UNUSED(n);
    va_end(ArgList);
	U2::setTaskDesc(get_kalign_context(), str);
	return str;
}

void set_task_progress(int percent) {
	U2::setTaskProgress(get_kalign_context(),percent);
}

int check_task_canceled(kalign_context *ctx) {
	return U2::isCanceled(ctx);
}
};

namespace U2 {

void setTaskProgress(struct kalign_context* ctx, int percent) {
	((TaskStateInfo*)ctx->ptask_state)->progress = percent;
}

void setTaskDesc(struct kalign_context* ctx, const char *str ) {
	TaskStateInfo *tsi = (TaskStateInfo*)ctx->ptask_state;
    QString description = QString::fromLatin1(str);
    description.replace('\n', " ");
    tsi->setDescription(description);
}

bool isCanceled(struct kalign_context* ctx) {
	return ((TaskStateInfo*)ctx->ptask_state)->cancelFlag;
}

} //namespace U2

