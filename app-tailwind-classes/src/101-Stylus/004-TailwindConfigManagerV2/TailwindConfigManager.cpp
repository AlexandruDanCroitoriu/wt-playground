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
#include <Wt/WTemplate.h>
#include <Wt/WDialog.h>
#include <Wt/WLabel.h>
#include <regex>

namespace Stylus
{


    TailwindConfigManager::TailwindConfigManager(std::shared_ptr<StylusState> state)
    : state_(state),
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

        sidebar->setStyleClass("flex items-center space-x-[10px]");
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


        auto add_file_btn = sidebar->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-add-file")));
        add_file_btn->setStyleClass("rounded-md w-[40px] p-[6px] utility-button-colors");
        
        add_file_btn->clicked().connect(this, [=]()
                                        {
        auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Create new config file"));
        dialog->setOffsets(100, Wt::Side::Top);
        dialog->setModal(true);
        dialog->rejectWhenEscapePressed();
        dialog->setStyleClass("z-[2]");
        dialog->titleBar()->setStyleClass("flex items-center justify-center p-[8px] cursor-pointer border-b border-solid text-xl font-bold");
        dialog->contents()->setStyleClass("flex flex-col");

        auto content = dialog->contents()->addWidget(std::make_unique<Wt::WContainerWidget>());
        auto footer = dialog->contents()->addWidget(std::make_unique<Wt::WContainerWidget>());
        
        content->setStyleClass("p-[8px]");
        footer->setStyleClass("flex items-center justify-between p-[8px]");
            
        auto input_wrapper = content->addWidget(std::make_unique<Wt::WContainerWidget>());
        input_wrapper->setStyleClass("flex flex-col items-center justify-center");
        auto error_label = content->addWidget(std::make_unique<Wt::WText>(""));
        error_label->setStyleClass("w-full text-md font-semibold");
        
        auto label = input_wrapper->addWidget(std::make_unique<Wt::WLabel>("Name"));
        auto new_file_name_input = input_wrapper->addWidget(std::make_unique<Wt::WLineEdit>());
        new_file_name_input->setStyleClass("w-full min-w-[200px] text-sm border rounded-md px-3 py-2 transition duration-300 ease focus:outline-none shadow-sm focus:shadow");
        label->setBuddy(new_file_name_input);
        
        auto confirm_btn = footer->addWidget(std::make_unique<Wt::WPushButton>("Confirm"));
        confirm_btn->setStyleClass("btn-default");
        auto cancel_btn = footer->addWidget(std::make_unique<Wt::WPushButton>("Cancel"));
        cancel_btn->setStyleClass("btn-red");

        cancel_btn->clicked().connect(this, [=](){ dialog->reject(); });
        new_file_name_input->enterPressed().connect(this, [=](){ confirm_btn->clicked().emit(Wt::WMouseEvent()); });

        confirm_btn->clicked().connect(this, [=](){ 
            // check if the folder already exists
            std::string new_file_name = new_file_name_input->text().toUTF8();
            std::string pattern = R"(^[a-z-]+$)";
            if(std::regex_match(new_file_name, std::regex(pattern)) == false) {
                error_label->setText("Match reges:" + pattern);
                return;
            }
            std::filesystem::path new_path(config_folder_path_ + "/" + new_file_name + ".css");
            if (std::filesystem::exists(new_path)) {
                error_label->setText("file with the same name already exists.");
            }else {
                dialog->accept();                
            }
        });
        dialog->finished().connect(this, [=](){
            if (dialog->result() == Wt::DialogCode::Accepted) {
                std::string new_file_name = new_file_name_input->text().toUTF8();
                std::filesystem::path new_path(config_folder_path_ + "/" + new_file_name + ".css");
                std::ofstream new_file(new_path);

                getConfigFiles();
                config_files_combobox_->clear();
                for(const auto &file : config_files_)
                {
                    config_files_combobox_->addItem(file);
                }
            }
            removeChild(dialog);
        });

        dialog->show();
        });

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


    std::string TailwindConfigManager::getConfig()
    {
        std::string file_name = config_files_combobox_->currentText().toUTF8();
        if(file_name == "")
        {
            file_name = default_config_file_name_;
        }
        std::ifstream file(config_folder_path_ + file_name);
        std::string config((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return config;
    }

}