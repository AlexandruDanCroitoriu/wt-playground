#include "101-Stylus/004-TailwindConfigManager/TailwindConfigManager.h"
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

#include <Wt/WApplication.h>
#include <Wt/WServer.h>
#include <Wt/WIOService.h>

namespace Stylus
{

    TailwindConfigManager::TailwindConfigManager(std::shared_ptr<StylusState> state)
    : state_(state),
    config_folder_path_("../stylus-resources/tailwind-config/"),
    default_config_file_name_("0.css")
    {
        auto sidebar = addWidget(std::make_unique<WContainerWidget>());
        auto editors_wrapper = addWidget(std::make_unique<WContainerWidget>());
        auto layout = std::make_unique<Wt::WHBoxLayout>();
        
        // auto sidebar = layout->insertWidget(0, std::make_unique<Wt::WContainerWidget>(), 1);
        config_editor_ = layout->insertWidget(0, std::make_unique<MonacoEditor>("css"), 1);
        output_editor_ = layout->insertWidget(1, std::make_unique<MonacoEditor>("css"), 1);
        
        // layout->setResizable(0);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        layout->setResizable(0, true, Wt::WLength(state_->tailwind_config_node_->IntAttribute("editor-width"), Wt::LengthUnit::Pixel));
        
        layout_ = layout.get();
        editors_wrapper->setLayout(std::move(layout));   
        config_files_ = getConfigFiles();
        
        output_editor_->setEditorReadOnly(true);
        setStyleClass("flex flex-col h-screen stylus-background h-screen");
        editors_wrapper->setStyleClass("flex-1 min-h-[200px]");
        sidebar->setStyleClass("flex items-center space-x-[10px] stylus-background z-[1]");
        config_editor_->addStyleClass("min-h-[200px]");
        config_files_combobox_ = sidebar->addWidget(std::make_unique<Wt::WComboBox>());
        config_files_combobox_->setStyleClass("max-w-[240px] m-1 bg-gray-50 border border-gray-300 text-gray-900 text-sm rounded-lg focus:ring-blue-500 focus:border-blue-500 block w-full p-2.5 dark:bg-gray-700 dark:border-gray-600 dark:placeholder-gray-400 dark:text-white dark:focus:ring-blue-500 dark:focus:border-blue-500");
        config_files_combobox_->keyWentDown().connect(this, [=](Wt::WKeyEvent event)
        {
            Wt::WApplication::instance()->globalKeyWentDown().emit(event); // Emit the global key event
        });

        std::string btn_styles = " rounded-[6px] w-[40px] h-[40px] p-[6px] utility-button-colors ";
        auto add_file_btn = sidebar->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-add-file")));
        add_file_btn->setStyleClass(btn_styles);
        
        delete_file_btn_ = sidebar->addWidget(std::make_unique<Wt::WPushButton>(Wt::WString::tr("stylus-svg-trash"), Wt::TextFormat::XHTML));
        // delete_file_btn_ = sidebar->addWidget(std::make_unique<Wt::WPushButton>(""));
        delete_file_btn_->setAttributeValue("tabindex", "-1");
        delete_file_btn_->setStyleClass(btn_styles + "disabled:cursor-not-allowed disabled:bg-[#EC133B]/50 hover:disabled:!bg-[#EC133B]/50");

        auto save_file_btn = sidebar->addWidget(std::make_unique<Wt::WPushButton>(Wt::WString::tr("stylus-svg-green-checked"), Wt::TextFormat::XHTML));
        save_file_btn->setAttributeValue("tabindex", "-1");
        save_file_btn->setStyleClass(btn_styles);


        config_editor_->width_changed_.connect(this, [=](Wt::WString width)
        {
            state_->tailwind_config_node_->SetAttribute("editor-width", std::stoi(width.toUTF8()));
            state_->doc_.SaveFile(state_->file_path_.c_str());
        });

        config_editor_->avalable_save().connect(this, [=](bool avalable)
        {
            if(avalable)
            {
                config_files_combobox_->setEnabled(false);
                save_file_btn->setEnabled(true);
                save_file_btn->setHidden(false);
            }
            else
            {
                config_files_combobox_->setEnabled(true);
                save_file_btn->setEnabled(false);
                save_file_btn->setHidden(true);
            }
        });

        config_editor_->save_file_signal().connect(this, [=](std::string file_path)
        {
            std::string file_name = config_files_combobox_->currentText().toUTF8();
            if(file_name == "")
            {
                file_name = default_config_file_name_;
            }
            std::ofstream file(config_folder_path_ + file_name);
            if (file.is_open())
            {
                file << config_editor_->getUnsavedText();
                file.close();
                config_editor_->textSaved();
                config_files_combobox_->setEnabled(true);
            }
        });

        save_file_btn->clicked().connect(this, [=]()
        {
            config_editor_->save_file_signal().emit(config_editor_->getUnsavedText());
        });
        
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

       
        
        config_files_combobox_->activated().connect(this, [=]()
        {
            std::string file_name = config_files_combobox_->currentText().toUTF8();
            if(file_name == "")
            {
                file_name = default_config_file_name_;
            }
            
            config_editor_->setFile(config_folder_path_ + file_name);
            if(file_name == default_config_file_name_)
            {
                config_editor_->setEditorReadOnly(true);
                delete_file_btn_->setEnabled(false);
            }
            else
            {
                config_editor_->setEditorReadOnly(false);
                delete_file_btn_->setEnabled(true);
            }
            state_->tailwind_config_node_->SetAttribute("selected-file-name", file_name.c_str());
            state_->doc_.SaveFile(state_->file_path_.c_str());
            save_file_btn->setEnabled(false);
            save_file_btn->setHidden(true);
        });

        for(const auto &file : config_files_)
        {
            config_files_combobox_->addItem(file);
        }

        config_files_combobox_->setCurrentIndex(config_files_combobox_->findText(state_->tailwind_config_node_->Attribute("selected-file-name")));
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



    void TailwindConfigManager::generateCssFile()
    {
        // std::cout << "\n\n start writing file \n\n";

        std::ofstream file("../stylus-resources/tailwind4/input.css");
        if (!file.is_open())
        {
            std::cerr << "Error opening file for writing: " << "../stylus-resources/tailwind4/input.css" << std::endl;
            return;
        }   
        // std::cout << "\n\n file opened \n\n";
        file << "/* Import TailwindCSS base styles */\n";
        file << "@import \"tailwindcss\";\n\n";
        file << "/* Import custom CSS files for additional styles */\n";
        // std::cout << "\n\n writing file imports \n\n";

        for (const auto &css_folder : std::filesystem::directory_iterator(css_files_root_path_))
        {
            if (css_folder.is_directory())
            {
                std::vector<std::string> files;
                for (const auto &css_file : std::filesystem::directory_iterator(css_files_root_path_ + css_folder.path().filename().string()))
                {
                    if (css_file.is_regular_file())
                    {
                        file << "@import \"./css/" << css_folder.path().filename().string() << "/" << css_file.path().filename().string() << "\";\n";
                    }
                }
            }
        }
        file << "\n";
        file << "/* Source additional templates and styles */\n";
        file << "@source \"../xml-templates/\";\n";
        file << "@source \"../../src/\";\n\n";

        file << "/* Define custom variants */\n";
        file << "@custom-variant dark (&:where(.dark, .dark *));\n\n";

        file << "/* Define custom theme */\n";
        file << getConfig() << "\n\n";

        // std::cout << "\n\nFile written successfully: " << "../stylus-resources/tailwind4/input.css\n\n";
        file.close();

        output_editor_->setFile("../static/tailwind.css");
        // std::cout << "\n\nGenerating CSS file...\n\n";
        auto session_id = Wt::WApplication::instance()->sessionId();
        Wt::WServer::instance()->ioService().post([this, session_id](){
            std::system("cd ../stylus-resources/tailwind4 && npm run build");
            Wt::WServer::instance()->post(session_id, [this]() {
                current_css_file_ = Wt::WApplication::instance()->docRoot() + "/../static/tailwind.css?v=" + Wt::WRandom::generateId();
                Wt::WApplication::instance()->removeStyleSheet(prev_css_file_.toUTF8());
                Wt::WApplication::instance()->useStyleSheet(current_css_file_.toUTF8());
                prev_css_file_ = current_css_file_;
                Wt::WApplication::instance()->triggerUpdate();
            }); 
        });
    }

}