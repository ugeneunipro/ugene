#ifndef _U2_UHMM_SEARCH_H_
#define _U2_UHMM_SEARCH_H_

#include <float.h>

#include <QObject>

#include <U2Core/U2Region.h>

struct plan7_s;

namespace U2 {

class TaskStateInfo;

enum HMMSearchAlgo {
    HMMSearchAlgo_Conservative,
    HMMSearchAlgo_SSEOptimized,
};

class UHMMSearchSettings {
public:
    UHMMSearchSettings()
        : globE(10), eValueNSeqs(1), domE(0.9999999), domT(-FLT_MAX), searchChunkSize(1000000), extraLen(-1),
          alg(HMMSearchAlgo_Conservative) {
    }

    float globE;
    int eValueNSeqs;
    float domE;
    float domT;
    int searchChunkSize;
    int extraLen;  //-1 -> auto
    HMMSearchAlgo alg;
};

class UHMMSearchResult {
public:
    UHMMSearchResult(const U2Region& _r, float sc, float eval)
        : r(_r), score(sc), evalue(eval) {
    }
    U2Region r;
    float score;
    float evalue;
};

class UHMMSearch : public QObject {
    Q_OBJECT

public:
    static QList<UHMMSearchResult> search(plan7_s* hmm, const char* seq, int seqLen, const UHMMSearchSettings& s, TaskStateInfo& si);
};

}  // namespace U2

#endif
