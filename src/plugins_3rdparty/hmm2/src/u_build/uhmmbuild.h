#ifndef _U2_UHMM_BUILD_H_
#define _U2_UHMM_BUILD_H_

struct plan7_s;
struct msa_struct;

#include <QObject>
#include <QString>

namespace U2 {

class TaskStateInfo;

// algorithm configuration strategy

enum HMMBuildStrategy {
    P7_BASE_CONFIG,  // hmmbuild -g
    P7_LS_CONFIG,  // hmmbuild <no options>
    P7_FS_CONFIG,  // hmmbuild -f
    P7_SW_CONFIG  // hmmbuild -s
};

class UHMMBuildSettings {
public:
    UHMMBuildSettings()
        : strategy(P7_LS_CONFIG) {};

    HMMBuildStrategy strategy;
    QString name;  // name of the hmm
};

class UHMMBuild : public QObject {
    Q_OBJECT

public:
    static plan7_s* build(msa_struct* msa, int atype, const UHMMBuildSettings& s, TaskStateInfo& si);
};

}  // namespace U2

#endif
