#include "101-Stylus/004-TailwindConfigManagerV2/TailwindConfigManager.h"
#include <Wt/WApplication.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WLineEdit.h>
#include <Wt/WRandom.h>
#include <boost/regex.hpp>
#include <filesystem>
#include <iostream>
#include <fstream>

namespace Stylus
{


    TailwindConfigManager::TailwindConfigManager(std::shared_ptr<Brain> brain)
    : brain_(brain),
    config_folder_path_("../stylus-resources/tailwind-config/"),
    default_config_file_name_("0.css")
    {
        auto layout = std::make_unique<Wt::WVBoxLayout>();
       
        auto sidebar = layout->insertWidget(0, std::make_unique<Wt::WContainerWidget>(), 0);
        editor_ = layout->insertWidget(1, std::make_unique<MonacoEditor>("css"), 1);
       
        // layout->setResizable(0);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        layout_ = layout.get();
        setLayout(std::move(layout));   

        config_files_ = getConfigFiles();

        sidebar->setStyleClass("flex items-center");
        config_files_combobox_ = sidebar->addWidget(std::make_unique<Wt::WComboBox>());
        config_files_combobox_->setStyleClass("max-w-[240px] border rounded-lg block w-full p-2");
        config_files_combobox_->keyWentDown().connect(this, [=](Wt::WKeyEvent event)
        {
            Wt::WApplication::instance()->globalKeyWentDown().emit(event); // Emit the global key event
        });

        for(const auto &file : config_files_)
        {
            std::cout << "\n\n file: " << file << "\n\n";
            config_files_combobox_->addItem(file);
        }
        editor_->setFile(config_folder_path_ + default_config_file_name_);
        editor_->setEditorReadOnly(true);
    }

    std::vector<std::string> TailwindConfigManager::getConfigFiles()
    {
        std::vector<std::string> config_files;
        for (const auto &entry : std::filesystem::directory_iterator(config_folder_path_))
        {
            if (entry.is_regular_file())
            {
                config_files.push_back(entry.path().filename().string());
            }
        }
        config_files_ = config_files;
        return config_files;
    }

}