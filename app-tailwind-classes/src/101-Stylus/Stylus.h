
#pragma once
#include <Wt/WDialog.h>
// #include "003-Tailwind/WTConfig.h"
// #include "004-FilesManager/FilesManager.h"
// #include "006-TemplatesManager/TemplatesManager.h"
#include <Wt/WDoubleSpinBox.h>

#include "101-Stylus/001-FilesManager/FilesManager.h"
#include "101-Stylus/003-TailwindConfigManager/WTConfig.h"

namespace Stylus {

    
    class Stylus : public Wt::WDialog
    {
        public:
        Stylus();
        
        
    
    // TemplatesManager* templates_files_manager_;
    FilesManager* templates_files_manager_;
    WTConfig* tailwind_config_;
    FilesManager* css_files_manager_;
    FilesManager* js_files_manager_;

    // virtual void setTabIndex(int index) override;
    
    private:
    
    
};
}