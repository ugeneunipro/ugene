#pragma once

#include <U2Core/Msa.h>
#include <U2Core/Task.h>

namespace U2 {
	extern double QScore(const Msa& maTest, const Msa& maRef, TaskStateInfo& ti);
}
