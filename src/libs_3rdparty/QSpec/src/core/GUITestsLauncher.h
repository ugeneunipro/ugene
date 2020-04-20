#ifndef GUITESTSLAUNCHER_H
#define GUITESTSLAUNCHER_H

#include <core/GUITest.h>
#include <core/GUITestBase.h>
#include <GTGlobals.h>

namespace HI {
class HI_EXPORT GUITestsLauncher: public QObject
{
    Q_OBJECT
public:
    GUITestsLauncher() {}

public slots:
    virtual void sl_runTest();
    virtual void sl_onTestFinished();
protected:
    GUITestBase guiTestBase;
private:
    GUITestOpStatus os;

};

}

#endif // GUITESTSLAUNCHER_H
