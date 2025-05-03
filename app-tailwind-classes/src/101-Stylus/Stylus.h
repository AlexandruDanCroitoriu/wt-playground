
#pragma once
#include <Wt/WDialog.h>
// #include "003-Tailwind/WTConfig.h"
// #include "004-FilesManager/FilesManager.h"
// #include "006-TemplatesManager/TemplatesManager.h"
#include <Wt/WDoubleSpinBox.h>

#include "100-Utils/FilesManager/FilesManager.h"
#include "101-Stylus/001-XmlFilesManager/XmlFilesManager.h"
#include "101-Stylus/002-CssFilesManager/CssFilesManager.h"
#include "101-Stylus/003-JsFilesManager/JsFilesManager.h"
// #include "101-Stylus/004-TailwindConfigManager/WTConfig.h"
#include "101-Stylus/004-TailwindConfigManagerV2/TailwindConfigManager.h"
#include "101-Stylus/StylusState.h"

namespace Stylus {

    
    class Stylus : public Wt::WDialog
    {
    public:
        Stylus();
            
        
    
        // TemplatesManager* templates_files_manager_;
        XmlFilesManager* xml_files_manager_;
        CssFilesManager* css_files_manager_;
        JsFilesManager* js_files_manager_;
        TailwindConfigManager* tailwind_config_;

        
        private:
        std::shared_ptr<StylusState> state_;
        void generateCssFile();
        Wt::WString current_css_file_;
        Wt::WString prev_css_file_;

};
}