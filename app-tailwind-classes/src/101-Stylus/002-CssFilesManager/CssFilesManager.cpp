#include "101-Stylus/002-CssFilesManager/CssFilesManager.h"
#include <Wt/WApplication.h>
#include <Wt/WRandom.h>

namespace Stylus
{

    CssFilesManager::CssFilesManager(std::shared_ptr<Brain> brain)
        : FilesManager("../stylus-resources/tailwind4/css/", "css"),
          brain_(brain)
    {

        // brain_->css_folders_ = folders_;
        // file_saved().connect(this, [=](Wt::WString ile_path)
        // {
        //     brain_->generateCssFile();
        // });
        
        // getFolders(); // to activate the signal and set the initial folders in brain_
    }
}