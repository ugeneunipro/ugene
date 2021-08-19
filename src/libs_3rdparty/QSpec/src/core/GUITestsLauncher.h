#ifndef GUITESTSLAUNCHER_H
#define GUITESTSLAUNCHER_H

#include <GTGlobals.h>
#include <core/GUITest.h>
#include <core/GUITestBase.h>

namespace HI {
class HI_EXPORT GUITestsLauncher : public QObject {
    Q_OBJECT
public:
    GUITestsLauncher() {
    }

public slots:
    virtual void sl_runTest();
    virtual void sl_onTestFinished();

protected:
    GUITestBase guiTestBase;

private:
    GUITestOpStatus os;
};

}  // namespace HI

#endif  // GUITESTSLAUNCHER_H
