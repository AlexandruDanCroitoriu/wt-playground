
#pragma once
#include <Wt/WDialog.h>
#include "005-TailwindConfigCenter/TailwindConfigCenter.h"
#include "003-Tailwind/WTConfig.h"
#include "004-FilesManager/FilesManager.h"
#include "006-TemplatesManager/TemplatesManager.h"
#include <Wt/WDoubleSpinBox.h>

class TailwindConfigCenter : public Wt::WDialog
{
public:
    TailwindConfigCenter();

    
    
    // TemplatesManager* templates_files_manager_;
    WTConfig* tailwind_config_;
    FilesManager* css_files_manager_;
    FilesManager* js_files_manager_;

    // virtual void setTabIndex(int index) override;

private:


};