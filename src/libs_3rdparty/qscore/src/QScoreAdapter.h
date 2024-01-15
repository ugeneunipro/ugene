#ifndef _U2_QSCORE_ADAPTER_H_
#define _U2_QSCORE_ADAPTER_H_

#include <U2Core/Msa.h>
#include <U2Core/Task.h>

namespace U2 {
	extern double QScore(const Msa& maTest, const Msa& maRef, TaskStateInfo& ti);
}

#endif //_U2_QSCORE_ADAPTER_H_
