#pragma once
#include "100-Utils/FilesManager/FilesManager.h"
#include "101-Stylus/Brain.h"

namespace Stylus
{

    class XmlFilesManager : public FilesManager
    {
    public:
        XmlFilesManager(std::shared_ptr<Brain> brain);
        // virtual Wt::WContainerWidget* setTreeFileWidget(Wt::WContainerWidget* files_wrapper, std::string folder_name, std::string file_name) override;

    private:
        std::shared_ptr<Brain> brain_;
    };
}