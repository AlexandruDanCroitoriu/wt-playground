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
#include <Wt/WMessageBox.h>

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
        config_files_combobox_->setStyleClass("max-w-[240px] border rounded-[8px] block w-full p-[8px] disabled:bg-[#F3F4F6] disabled:text-[#9CA3AF] disabled:cursor-not-allowed");
        config_files_combobox_->keyWentDown().connect(this, [=](Wt::WKeyEvent event)
        {
            Wt::WApplication::instance()->globalKeyWentDown().emit(event); // Emit the global key event
        });

     

        editor_->avalable_save().connect(this, [=](bool avalable)
        {
            if(avalable)
            {
                config_files_combobox_->setEnabled(false);
            }
            else
            {
                config_files_combobox_->setEnabled(true);
            }
        });

        editor_->save_file_signal().connect(this, [=](std::string file_path)
        {
            std::string file_name = config_files_combobox_->currentText().toUTF8();
            if(file_name == "")
            {
                file_name = default_config_file_name_;
            }
            std::ofstream file(config_folder_path_ + file_name);
            if (file.is_open())
            {
                file << editor_->getUnsavedText();
                file.close();
                editor_->textSaved();
                config_files_combobox_->setEnabled(true);
            }
        });


        auto add_file_btn = sidebar->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-add-file")));
        add_file_btn->setStyleClass("rounded-[6px] w-[40px] p-[6px] utility-button-colors");
        
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

        delete_file_btn_ = sidebar->addWidget(std::make_unique<Wt::WPushButton>(Wt::WString::tr("stylus-svg-trash"), Wt::TextFormat::XHTML));
        delete_file_btn_->setStyleClass("rounded-[6px] w-[40px] p-[6px] utility-button-colors disabled:cursor-not-allowed disabled:bg-[#EC133B]/50 hover:disabled:!bg-[#EC133B]/50");

        delete_file_btn_->clicked().connect(this, [=]()
        {
            if(config_files_combobox_->currentText().toUTF8().compare(default_config_file_name_) == 0)
            {
                delete_file_btn_->setEnabled(false);
                return;
            }
            auto message_box = addChild(std::make_unique<Wt::WMessageBox>("Are you sure you want to delete the file ?", 
                "<div class='flex-1 text-center font-bold text-2xl'>" + config_files_combobox_->currentText().toUTF8() + "</div>",
                Wt::Icon::Warning, Wt::StandardButton::None)
            );
            message_box->setOffsets(100, Wt::Side::Top);
            message_box->setModal(true);
        
            message_box->setStyleClass("");
            message_box->titleBar()->setStyleClass("flex items-center justify-center p-[8px] cursor-pointer border-b border-solid text-[18px] font-bold");
            message_box->contents()->addStyleClass("flex items-center");
            message_box->footer()->setStyleClass("flex items-center justify-between p-[8px]");
            auto delete_btn = message_box->addButton("Delete", Wt::StandardButton::Yes);
            auto cancel_btn = message_box->addButton("Cancel", Wt::StandardButton::No);
            delete_btn->setStyleClass("btn-red");
            cancel_btn->setStyleClass("btn-default");
            
            message_box->buttonClicked().connect([=] {
                if (message_box->buttonResult() == Wt::StandardButton::Yes)
                    {
                    std::filesystem::path file_path = config_folder_path_ + config_files_combobox_->currentText().toUTF8();
    
                    // delete file
                    if (std::filesystem::remove(file_path)) {
                        getConfigFiles();
                        config_files_combobox_->clear();
                        for(const auto &file : config_files_)
                        {
                            config_files_combobox_->addItem(file);
                        }
                        
                    } else {
                        Wt::WApplication::instance()->log("ERROR") << "\n\nError deleting file.\n\n";                    
                    }
                }
                removeChild(message_box);
            });
            
            message_box->show(); 
        });

        for(const auto &file : config_files_)
        {
            std::cout << "\n\n file: " << file << "\n\n";
            config_files_combobox_->addItem(file);
        }
        
        config_files_combobox_->activated().connect(this, [=]()
        {
            std::cout << "\n\n file: " << config_files_combobox_->currentText().toUTF8() << "\n\n";
            std::string file_name = config_files_combobox_->currentText().toUTF8();
            if(file_name == "")
            {
                file_name = default_config_file_name_;
            }
            std::cout << "\n\n file: " << config_files_combobox_->currentText().toUTF8() << "\n\n";
            
            editor_->setFile(config_folder_path_ + file_name);
            std::cout << "\n\n file: " << config_files_combobox_->currentText().toUTF8() << "\n\n";
            if(file_name == default_config_file_name_)
            {
                editor_->setEditorReadOnly(true);
                delete_file_btn_->setEnabled(false);
            }
            else
            {
                editor_->setEditorReadOnly(false);
                delete_file_btn_->setEnabled(true);
            }
            state_->tailwind_config_node_->SetAttribute("selected-file-name", file_name.c_str());
            state_->doc_.SaveFile(state_->file_path_.c_str());
        });
        config_files_combobox_->setCurrentIndex(config_files_combobox_->findText(state_->tailwind_config_node_->Attribute("selected-file-name")));
        std::cout << "\n\n current file: " << config_files_combobox_->currentIndex() << "\n\n";
        config_files_combobox_->activated().emit(config_files_combobox_->currentIndex());


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