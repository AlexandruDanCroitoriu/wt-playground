#include "101-Stylus/002-CssFilesManager/CssFilesManager.h"
#include <Wt/WApplication.h>
#include <Wt/WRandom.h>

namespace Stylus
{

    CssFilesManager::CssFilesManager(std::shared_ptr<Brain> brain)
        : FilesManager("../stylus-resources/tailwind4/css/", "css"),
          brain_(brain)
    {

        // file_saved().connect(this, [=](Wt::WString ile_path)
        // {
        // });
        get_folders_signal().connect(this, [=](std::vector<std::pair<std::string, std::vector<std::string>>> folders)
        {
            brain_->css_folders_ = folders;
            brain_->generateCssFile();
        });
        getFolders(); // to activate the signal and set the initial folders in brain_
    }
}