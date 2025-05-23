#pragma once
#include "101-Stylus/FilesManager.h"
#include "101-Stylus/StylusState.h"
#include "010-TestWidgets/DarkModeToggle.h"
#include <Wt/WSignal.h>

namespace Stylus
{

    class XmlFilesManager : public FilesManager
    {
    public:
        XmlFilesManager(std::shared_ptr<StylusState> state);
        // virtual Wt::WContainerWidget* setTreeFileWidget(Wt::WContainerWidget* files_wrapper, std::string folder_name, std::string file_name) override;
        DarkModeToggle* dark_mode_toggle_;
    private:
        std::shared_ptr<StylusState> state_;
    };
}