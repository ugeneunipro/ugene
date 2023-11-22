#include "GTTestsRegressionScenarios_8001_9000.h"

#include <base_dialogs/GTFileDialog.h>

#include <primitives/GTLineEdit.h>
#include <primitives/GTMenu.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>

#include <utils/GTUtilsDialog.h>

#include "GTUtilsLog.h"
#include "GTUtilsTaskTreeView.h"

#include "runnables/ugene/plugins/external_tools/AlignToReferenceBlastDialogFiller.h"

namespace U2 {

namespace GUITest_regression_scenarios {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_8009) {
    /*
    * 1. Open Tools->Sanger data ahalysis-> Map reads to reference
    * 2. Set wrong format reference file from sample: sample/ACE/k26.ace
    * 3. Add reads: _common_data/sanger/sanger_03.ab1
    * 4. Click Map button
    * Expected: Error log message 'wrong reference format'
    * 5. Open Tools->Sanger data ahalysis-> Map reads to reference
    * 5. Set wrong format reference file from sample: path which not exists
    * 7. Add reads: _common_data/sanger/sanger_03.ab1
    * 8. Click Map button
    * Expected: Error log message 'reference file doesn't exist'
    */
    class SetRefAndAlign : public CustomScenario {
    public:
        void run() override {
            auto dialog = GTWidget::getActiveModalWidget();

            GTLineEdit::setText(GTWidget::findLineEdit("referenceLineEdit"), refUrl);

            GTUtilsDialog::waitForDialog(new GTFileDialogUtils_list({testDir + "_common_data/sanger/sanger_03.ab1"}));
            GTWidget::click(GTWidget::findPushButton("addReadButton"));
            GTUtilsTaskTreeView::waitTaskFinished();

            // Push "Align" button.
            GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
        }

        QString refUrl;
    };

    GTLogTracer lt;

    SetRefAndAlign setRefAndAlignScenario = SetRefAndAlign();
    setRefAndAlignScenario.refUrl = dataDir + "samples/ACE/K26.ace";
    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(&setRefAndAlignScenario));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(lt.hasMessage("wrong reference format"), "Expected message 'wrong reference format' not found!");    

    lt.clear();

    setRefAndAlignScenario.refUrl = dataDir + "not_existing_path";
    GTUtilsDialog::waitForDialog(new AlignToReferenceBlastDialogFiller(&setRefAndAlignScenario));
    GTMenu::clickMainMenuItem({"Tools", "Sanger data analysis", "Map reads to reference..."});
    GTUtilsTaskTreeView::waitTaskFinished();
    CHECK_SET_ERR(lt.hasMessage("reference file doesn't exist"), "Expected message 'reference file doesn't exist' not found!");    
}

}  // namespace GUITest_regression_scenarios

}  // namespace U2