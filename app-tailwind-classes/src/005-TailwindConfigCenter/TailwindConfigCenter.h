
#pragma once
#include <Wt/WDialog.h>
#include "005-TailwindConfigCenter/TailwindConfigCenter.h"
#include "003-Tailwind/WTConfig.h"
#include "004-CssFilesManager/CssFilesManager.h"
#include <Wt/WDoubleSpinBox.h>

class TailwindConfigCenter : public Wt::WDialog
{
public:
    TailwindConfigCenter();

    WTConfig* tailwind_config_;
    Wt::WDoubleSpinBox *test_input;
    CssFilesManager* css_files_manager_;
private:
};