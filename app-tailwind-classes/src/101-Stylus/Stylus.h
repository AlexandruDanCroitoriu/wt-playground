
#pragma once
#include <Wt/WDialog.h>
// #include "003-Tailwind/WTConfig.h"
// #include "004-FilesManager/FilesManager.h"
// #include "006-TemplatesManager/TemplatesManager.h"
#include <Wt/WDoubleSpinBox.h>

#include "100-Utils/FilesManager/FilesManager.h"
#include "101-Stylus/003-TailwindConfigManager/WTConfig.h"
#include "101-Stylus/001-JsFilesManager/JsFilesManager.h"
#include "101-Stylus/002-CssFilesManager/CssFilesManager.h"
#include "101-Stylus/003-XmlFilesManager/XmlFilesManager.h"
#include "101-Stylus/Brain.h"

namespace Stylus {

    
    class Stylus : public Wt::WDialog
    {
    public:
        Stylus();
        
        
    
        // TemplatesManager* templates_files_manager_;
        XmlFilesManager* templates_files_manager_;
        WTConfig* tailwind_config_;
        CssFilesManager* css_files_manager_;
        JsFilesManager* js_files_manager_;

        
        std::shared_ptr<Brain> brain_;
    private:
    
};
}