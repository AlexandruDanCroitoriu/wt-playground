#include "004-FilesManager/FilesManager.h"
#include <filesystem>
#include <Wt/WPushButton.h>
#include <Wt/WMenu.h>
#include <Wt/WMenuItem.h>
#include <Wt/WPanel.h>
#include <Wt/WTemplate.h>
#include <fstream>
#include <Wt/WMessageBox.h>
#include <Wt/WLineEdit.h>
#include <Wt/WLabel.h>
#include <Wt/WDialog.h>
#include <regex>
#include <Wt/WApplication.h>

FilesManager::FilesManager(std::string default_folder_path, std::string language)
    : default_folder_path_(default_folder_path)
{
    setStyleClass("w-full h-full flex");

    // tree wrapper
    auto folder_tree_wrapper = addWidget(std::make_unique<Wt::WContainerWidget>());
    folder_tree_wrapper->setStyleClass("min-w-[240px] flex flex-col");

    // tree header
    auto tree_header = folder_tree_wrapper->addWidget(std::make_unique<Wt::WContainerWidget>());
    tree_header->setStyleClass("group flex items-center border-b border-solid border-gray-200 dark:border-gray-700");
    tree_header_title_ = tree_header->addWidget(std::make_unique<Wt::WText>("selected "+language+" file"));
    tree_header_title_->setStyleClass("text-md m-1 text-green-500 dark:text-green-400");

    // add folder btn
    auto add_folder_btn = tree_header->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-add-folder")));
    add_folder_btn->setStyleClass("ml-auto group-hover:block hidden hover:bg-gray-200 dark:hover:bg-gray-900 rounded-md p-1");
    add_folder_btn->clicked().preventPropagation();

    folders_tree_wrapper_ = folder_tree_wrapper->addWidget(std::make_unique<Wt::WContainerWidget>());
    folders_tree_wrapper_->setStyleClass("w-full flex-1 overflow-y-auto flex flex-col");

    folders_ = getFolders();
    editor_ = addWidget(std::make_unique<MonacoEdditor>(language));
    editor_->avalable_save().connect(this, [=] (bool avalable) {
        if(avalable)
        {
            selected_file_wrapper_->toggleStyleClass("[&>*:first-child]:block", true);
            
        }else {
            selected_file_wrapper_->toggleStyleClass("[&>*:first-child]:block", false);
        }
    });
    setTreeFolderWidgets();
    
    editor_->save_file_signal().connect(this, [=] (std::string text) {
        std::ofstream file(selecter_file_path_);
        if (!file.is_open()) {
            std::cout << "\n\n Failed to open file: " << selecter_file_path_ << "\n\n";
            return;
        }
        file << text;
        file.close();
        editor_->textSaved();
    });

    add_folder_btn->clicked().connect(this, [=](){
        auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Create new folder"));

        dialog->setModal(true);
        dialog->rejectWhenEscapePressed();
        dialog->setOffsets(100, Wt::Side::Top);

        dialog->setStyleClass("bg-white dark:bg-gray-800 dark:text-gray-200 z-[2]");
        dialog->titleBar()->setStyleClass("flex items-center justify-center bg-white p-2 cursor-pointer dark:bg-gray-800 hover:bg-gray-200 dark:text-gray-300 dark:hover:bg-gray-700 border-b border-solid border-gray-200 dark:border-gray-700 text-xl font-bold");
        dialog->contents()->setStyleClass("flex flex-col bg-white dark:bg-gray-800 dark:text-gray-200");

        auto content = dialog->contents()->addWidget(std::make_unique<Wt::WContainerWidget>());
        auto footer = dialog->contents()->addWidget(std::make_unique<Wt::WContainerWidget>());
        
        content->setStyleClass("p-2");
        footer->setStyleClass("flex items-center justify-between p-2");
        
        auto input_wrapper = content->addWidget(std::make_unique<Wt::WContainerWidget>());
        input_wrapper->setStyleClass("flex flex-col items-center justify-center");
        auto error_label = content->addWidget(std::make_unique<Wt::WText>(""));
        error_label->setStyleClass("w-full text-red-500 text-md font-semibold");
        
        auto label = input_wrapper->addWidget(std::make_unique<Wt::WLabel>("Name"));
        auto new_folder_name_input = input_wrapper->addWidget(std::make_unique<Wt::WLineEdit>());
        new_folder_name_input->setStyleClass("w-full min-w-[200px] bg-transparent placeholder:text-slate-400 text-slate-700 text-sm border border-slate-200 rounded-md px-3 py-2 transition duration-300 ease focus:outline-none focus:border-slate-400 hover:border-slate-300 shadow-sm focus:shadow  dark:bg-gray-800 dark:placeholder:text-gray-500 dark:text-gray-200 dark:border-gray-600 dark:focus:border-gray-400 dark:hover:border-gray-500");
        label->setBuddy(new_folder_name_input);
        
        auto confirm_btn = footer->addWidget(std::make_unique<Wt::WPushButton>("Confirm"));
        confirm_btn->setStyleClass("btn-default");
        auto cancel_btn = footer->addWidget(std::make_unique<Wt::WPushButton>("Cancel"));
        cancel_btn->setStyleClass("btn-red");

        cancel_btn->clicked().connect(this, [=](){ dialog->reject(); });
        new_folder_name_input->enterPressed().connect(this, [=](){ confirm_btn->clicked().emit(Wt::WMouseEvent()); });
        confirm_btn->clicked().connect(this, [=](){ 
            // check if the folder already exists
            std::string new_folder_name = new_folder_name_input->text().toUTF8();
            std::string pattern = R"(^[a-z0-9-_]+$)";
            if(std::regex_match(new_folder_name, std::regex(pattern)) == false) {
                error_label->setText("Match reges:" + pattern);
                return;
            }
            std::filesystem::path new_path(default_folder_path_ + new_folder_name);
            if (std::filesystem::exists(new_path)) {
                error_label->setText("Folder with the same name already exists.");
            }else {
                dialog->accept();                
            }
        });
        dialog->finished().connect(this, [=](){
            if (dialog->result() == Wt::DialogCode::Accepted) {
                std::string new_folder_name = new_folder_name_input->text().toUTF8();
                std::filesystem::path new_path(default_folder_path_ + new_folder_name);
                std::filesystem::create_directory(new_path);
                folders_tree_wrapper_->clear();
                folders_ = getFolders();
                setTreeFolderWidgets();
            }
            removeChild(dialog);
        });
        dialog->show();
    });
}


void FilesManager::setTreeFolderWidgets()
{
    folders_tree_wrapper_->clear();
    selected_file_wrapper_ = nullptr;
    // add folders and files in the UI
    
    for(const auto& folder : folders_)
    {
        auto panel = folders_tree_wrapper_->addWidget(std::make_unique<Wt::WPanel>());
        panel->setCollapsible(true);
        // panel->setCollapsed(true);
        panel->setAnimation(Wt::WAnimation(Wt::AnimationEffect::SlideInFromTop, Wt::TimingFunction::EaseInOut, 250));
        panel->setStyleClass("!border-none ");
        panel->titleBarWidget()->setStyleClass("group relative flex items-center px-2 cursor-pointer tracking-widest dark:bg-gray-800 hover:bg-gray-200 dark:text-gray-300 dark:hover:bg-gray-700 border-b border-solid border-gray-200 dark:border-gray-700");       

        auto folder_title = panel->titleBarWidget()->addWidget(std::make_unique<Wt::WText>(folder.first));
        folder_title->setStyleClass("ml-2 hover:text-green-500 dark:hover:text-green-400");
      
        auto edit_file_name_btn = panel->titleBarWidget()->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-edit")));
        edit_file_name_btn->setStyleClass("group-hover:block hidden absolute right-6 rounded-md p-1 hover:bg-gray-200 dark:hover:bg-gray-900 group-hover:block hidden");
        edit_file_name_btn->clicked().preventPropagation();

        // add File button
        auto add_file_btn = panel->titleBarWidget()->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-add-file")));
        add_file_btn->setStyleClass("group-hover:block hidden absolute right-0 hover:bg-gray-200 dark:hover:bg-gray-900 rounded-md p-1 overflow-hidden");
        add_file_btn->clicked().preventPropagation();

        edit_file_name_btn->clicked().connect(this, [=] () {
            auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Change folder name from " + folder.first));
            dialog->setTabIndex(10000);
            dialog->setOffsets(100, Wt::Side::Top);
            dialog->setModal(true);
            dialog->rejectWhenEscapePressed();
            dialog->setStyleClass("bg-white dark:bg-gray-800 dark:text-gray-200 z-[2]");
            dialog->titleBar()->setStyleClass("flex items-center justify-center bg-white p-2 cursor-pointer dark:bg-gray-800 hover:bg-gray-200 dark:text-gray-300 dark:hover:bg-gray-700 border-b border-solid border-gray-200 dark:border-gray-700 text-xl font-bold");
            dialog->contents()->setStyleClass("flex flex-col bg-white dark:bg-gray-800 dark:text-gray-200");
    
            auto content = dialog->contents()->addWidget(std::make_unique<Wt::WContainerWidget>());
            auto footer = dialog->contents()->addWidget(std::make_unique<Wt::WContainerWidget>());
            
            content->setStyleClass("p-2");
            footer->setStyleClass("flex items-center justify-between p-2");
            
            auto input_wrapper = content->addWidget(std::make_unique<Wt::WContainerWidget>());
            input_wrapper->setStyleClass("flex flex-col items-center justify-center");
            auto error_label = content->addWidget(std::make_unique<Wt::WText>(""));
            error_label->setStyleClass("w-full text-red-500 text-md font-semibold");
            
            auto label = input_wrapper->addWidget(std::make_unique<Wt::WLabel>("Name"));
            auto new_file_name_input = input_wrapper->addWidget(std::make_unique<Wt::WLineEdit>());
            new_file_name_input->setStyleClass("w-full min-w-[200px] bg-transparent placeholder:text-slate-400 text-slate-700 text-sm border border-slate-200 rounded-md px-3 py-2 transition duration-300 ease focus:outline-none focus:border-slate-400 hover:border-slate-300 shadow-sm focus:shadow  dark:bg-gray-800 dark:placeholder:text-gray-500 dark:text-gray-200 dark:border-gray-600 dark:focus:border-gray-400 dark:hover:border-gray-500");
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
                std::string pattern = R"(^[a-z0-9-_]+$)";
                if(std::regex_match(new_file_name, std::regex(pattern)) == false) {
                    error_label->setText("Match reges:" + pattern);
                    return;
                }
                std::filesystem::path new_path(default_folder_path_ + new_file_name);
                if (std::filesystem::exists(new_path)) {
                    error_label->setText("Folder with the same name already exists.");
                }else {
                    dialog->accept();                
                }
            });
            dialog->finished().connect(this, [=](){
                if (dialog->result() == Wt::DialogCode::Accepted) {
                    std::string new_file_name = new_file_name_input->text().toUTF8();
                    std::filesystem::path old_path(default_folder_path_ + folder.first);
                    std::filesystem::path new_path(default_folder_path_ + new_file_name);
                    std::filesystem::rename(old_path, new_path);
                    folders_tree_wrapper_->clear();
                    folders_ = getFolders();
                    setTreeFolderWidgets();
                }
                removeChild(dialog);
            });

            dialog->show();
        });


        add_file_btn->clicked().connect(this, [=](){
            auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Create new file in folder " + folder.first));
            dialog->setOffsets(100, Wt::Side::Top);
            dialog->setModal(true);
            dialog->rejectWhenEscapePressed();
            dialog->setStyleClass("bg-white dark:bg-gray-800 dark:text-gray-200 z-[2]");
            dialog->titleBar()->setStyleClass("flex items-center justify-center bg-white p-2 cursor-pointer dark:bg-gray-800 hover:bg-gray-200 dark:text-gray-300 dark:hover:bg-gray-700 border-b border-solid border-gray-200 dark:border-gray-700 text-xl font-bold");
            dialog->contents()->setStyleClass("flex flex-col bg-white dark:bg-gray-800 dark:text-gray-200");
    
            auto content = dialog->contents()->addWidget(std::make_unique<Wt::WContainerWidget>());
            auto footer = dialog->contents()->addWidget(std::make_unique<Wt::WContainerWidget>());
            
            content->setStyleClass("p-2");
            footer->setStyleClass("flex items-center justify-between p-2");
            
            auto input_wrapper = content->addWidget(std::make_unique<Wt::WContainerWidget>());
            input_wrapper->setStyleClass("flex flex-col items-center justify-center");
            auto error_label = content->addWidget(std::make_unique<Wt::WText>(""));
            error_label->setStyleClass("w-full text-red-500 text-md font-semibold");
            
            auto label = input_wrapper->addWidget(std::make_unique<Wt::WLabel>("Name"));
            auto new_file_name_input = input_wrapper->addWidget(std::make_unique<Wt::WLineEdit>());
            new_file_name_input->setStyleClass("w-full min-w-[200px] bg-transparent placeholder:text-slate-400 text-slate-700 text-sm border border-slate-200 rounded-md px-3 py-2 transition duration-300 ease focus:outline-none focus:border-slate-400 hover:border-slate-300 shadow-sm focus:shadow  dark:bg-gray-800 dark:placeholder:text-gray-500 dark:text-gray-200 dark:border-gray-600 dark:focus:border-gray-400 dark:hover:border-gray-500");
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
                std::filesystem::path new_path(default_folder_path_ + folder.first + "/" + new_file_name + ".css");
                if (std::filesystem::exists(new_path)) {
                    error_label->setText("file with the same name already exists.");
                }else {
                    dialog->accept();                
                }
            });
            dialog->finished().connect(this, [=](){
                if (dialog->result() == Wt::DialogCode::Accepted) {
                    std::string new_file_name = new_file_name_input->text().toUTF8();
                    std::filesystem::path new_path(default_folder_path_ + folder.first + "/" + new_file_name + ".css");
                    std::ofstream new_file(new_path);
                    folders_tree_wrapper_->clear();
                    folders_ = getFolders();
                    setTreeFolderWidgets();
                }
                removeChild(dialog);
            });

            dialog->show();
        });
        
        // delete folder button
        if(folder.second.size() == 0)
        {
            auto delete_folder_btn = panel->titleBarWidget()->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-trash")));
            delete_folder_btn->setStyleClass("absolute right-1 rounded-md p-1 hover:bg-gray-200 dark:hover:bg-gray-900");
            add_file_btn->addStyleClass("!right-5");
            add_file_btn->addStyleClass("!right-11");
            delete_folder_btn->clicked().preventPropagation();
            delete_folder_btn->clicked().connect(this, [=](){
                // delete folder 
                std::string title = "Are you sure you want to delete the file ?";
                std::string content = "<div class='flex-1 text-center font-bold text-2xl'>" + folder.first + "</div>";
                auto messageBox = createMessageBox(title, content);
                auto delete_btn = messageBox->addButton("Delete", Wt::StandardButton::Yes);
                auto cancel_btn = messageBox->addButton("Cancel", Wt::StandardButton::No);
                delete_btn->setStyleClass("btn-red");
                cancel_btn->setStyleClass("btn-default");
                
                messageBox->buttonClicked().connect([=] {
                    if (messageBox->buttonResult() == Wt::StandardButton::Yes)
                        {
                        std::filesystem::path file_path = default_folder_path_ + folder.first;

                        // delete file
                        if (std::filesystem::remove(file_path)) {
                            folders_tree_wrapper_->clear();
                            folders_ = getFolders();
                            setTreeFolderWidgets();
                        } else {
                            Wt::WApplication::instance()->log("ERROR") << "\n\nError deleting file.\n\n";                    
                        }
                    }
                    removeChild(messageBox);
                });
                
                messageBox->show();
                auto folder_path = default_folder_path_ + folder.first;
                std::filesystem::remove_all(folder_path);
                folders_ = getFolders();
                setTreeFolderWidgets();
            });

        }

        auto central_widget = panel->setCentralWidget(std::make_unique<Wt::WContainerWidget>());
        central_widget->setStyleClass("w-full bg-white dark:bg-gray-800");

        for(const auto& file : folder.second)
        {
            auto file_wrapper = central_widget->addWidget(std::make_unique<Wt::WContainerWidget>());
            file_wrapper->setStyleClass("group w-full relative flex items-center cursor-pointer hover:bg-gray-200 dark:hover:bg-gray-700 rounded-md text-gray-700 dark:text-gray-200");

            // file buttons
            auto save_file_btn = file_wrapper->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-green-checked")));
            save_file_btn->setStyleClass("absolute left-1 hover:bg-gray-200 dark:hover:bg-gray-900 hidden");
            save_file_btn->clicked().preventPropagation();
            save_file_btn->clicked().connect(this, [=](){
                std::ofstream sys_file(selecter_file_path_);
                if (!sys_file.is_open()) {
                    std::cout << "\n\n Failed to open file: " << selecter_file_path_ << "\n\n";
                    return;
                }
                sys_file << editor_->getUnsavedText();
                sys_file.close();
                editor_->textSaved();
            });


            file_wrapper->addWidget(std::make_unique<Wt::WText>(file))->setStyleClass("ml-7 font-thin text-sm");

            auto delete_file_btn = file_wrapper->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-trash")));
            delete_file_btn->setStyleClass("absolute right-0 rounded-md p-1 hover:bg-gray-200 dark:hover:bg-gray-900 group-hover:block hidden");
            delete_file_btn->clicked().preventPropagation();
            delete_file_btn->clicked().connect(this, [=](){
                // delete file 
                std::string title = "Are you sure you want to delete the file ?";
                std::string content = "<div class='flex-1 text-center font-bold text-2xl'>" + file + "</div>";
                auto messageBox = createMessageBox(title, content);
                auto delete_btn = messageBox->addButton("Delete", Wt::StandardButton::Yes);
                auto cancel_btn = messageBox->addButton("Cancel", Wt::StandardButton::No);
                delete_btn->setStyleClass("btn-red");
                cancel_btn->setStyleClass("btn-default");
                
                messageBox->buttonClicked().connect([=] {
                    if (messageBox->buttonResult() == Wt::StandardButton::Yes)
                        {
                        std::filesystem::path file_path = default_folder_path_ + folder.first + "/" + file;

                        // delete file
                        if (std::filesystem::remove(file_path)) {
                            folders_tree_wrapper_->clear();
                            folders_ = getFolders();
                            setTreeFolderWidgets();
                        } else {
                            Wt::WApplication::instance()->log("ERROR") << "\n\nError deleting file.\n\n";                    
                        }
                    }
                    removeChild(messageBox);
                });
                
                messageBox->show();
            });                

            auto edit_file_name_btn = file_wrapper->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-edit")));
            edit_file_name_btn->setStyleClass("absolute right-5 rounded-md p-1 hover:bg-gray-200 dark:hover:bg-gray-900 group-hover:block hidden");
            edit_file_name_btn->clicked().preventPropagation();
            edit_file_name_btn->clicked().connect(this, [=](){
                auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("change file name from " + folder.first));
                dialog->setOffsets(100, Wt::Side::Top);
                dialog->setModal(true);
                dialog->rejectWhenEscapePressed();
                dialog->setStyleClass("bg-white dark:bg-gray-800 dark:text-gray-200 z-[2]");
                dialog->titleBar()->setStyleClass("flex items-center justify-center bg-white p-2 cursor-pointer dark:bg-gray-800 hover:bg-gray-200 dark:text-gray-300 dark:hover:bg-gray-700 border-b border-solid border-gray-200 dark:border-gray-700 text-xl font-bold");
                dialog->contents()->setStyleClass("flex flex-col bg-white dark:bg-gray-800 dark:text-gray-200");
        
                auto content = dialog->contents()->addWidget(std::make_unique<Wt::WContainerWidget>());
                auto footer = dialog->contents()->addWidget(std::make_unique<Wt::WContainerWidget>());
                
                content->setStyleClass("p-2");
                footer->setStyleClass("flex items-center justify-between p-2");
                
                auto input_wrapper = content->addWidget(std::make_unique<Wt::WContainerWidget>());
                input_wrapper->setStyleClass("flex flex-col items-center justify-center");
                auto error_label = content->addWidget(std::make_unique<Wt::WText>(""));
                error_label->setStyleClass("w-full text-red-500 text-md font-semibold");
                
                auto label = input_wrapper->addWidget(std::make_unique<Wt::WLabel>("Name"));
                auto new_file_name_input = input_wrapper->addWidget(std::make_unique<Wt::WLineEdit>());
                new_file_name_input->setStyleClass("w-full min-w-[200px] bg-transparent placeholder:text-slate-400 text-slate-700 text-sm border border-slate-200 rounded-md px-3 py-2 transition duration-300 ease focus:outline-none focus:border-slate-400 hover:border-slate-300 shadow-sm focus:shadow  dark:bg-gray-800 dark:placeholder:text-gray-500 dark:text-gray-200 dark:border-gray-600 dark:focus:border-gray-400 dark:hover:border-gray-500");
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
                    std::filesystem::path new_path(default_folder_path_ + folder.first + "/" + new_file_name + ".css");
                    if (std::filesystem::exists(new_path)) {
                        error_label->setText("file with the same name already exists.");
                    }else {
                        dialog->accept();                
                    }
                });
                dialog->finished().connect(this, [=](){
                    if (dialog->result() == Wt::DialogCode::Accepted) {
                        std::string new_file_name = new_file_name_input->text().toUTF8();
                        std::filesystem::path old_path(default_folder_path_ + folder.first + "/" + file);
                        std::filesystem::path new_path(default_folder_path_ + folder.first + "/" + new_file_name + ".css");
                        std::filesystem::rename(old_path, new_path);
                        folders_tree_wrapper_->clear();
                        folders_ = getFolders();
                        setTreeFolderWidgets();
                    }
                    removeChild(dialog);
                });
    
                dialog->show();
            });

            file_wrapper->clicked().connect(this, [=] () {
                if(selecter_file_path_.compare(default_folder_path_ + folder.first + "/" + file) == 0)
                {
                    return;
                }
                if(editor_->unsavedChanges()){
                    // std::cout << "\nUnsaved changes, please save the file first\n";
                    auto title = "Save the changes from the current file ?";
                    auto messageBox = createMessageBox(title, "");

                    messageBox->setStyleClass("bg-white dark:bg-gray-800 dark:text-gray-200");
                    messageBox->titleBar()->setStyleClass("flex items-center justify-center bg-white p-2 cursor-pointer dark:bg-gray-800 hover:bg-gray-200 dark:text-gray-300 dark:hover:bg-gray-700 border-b border-solid border-gray-200 dark:border-gray-700 text-xl font-bold");
                    messageBox->contents()->addStyleClass("flex items-center bg-white dark:bg-gray-800 dark:text-gray-200");
                    messageBox->footer()->setStyleClass("flex items-center justify-between bg-white p-2 dark:bg-gray-800 hover:bg-gray-200 dark:text-gray-300 dark:hover:bg-gray-700");

                    auto save_btn = messageBox->addButton("Save", Wt::StandardButton::Yes);
                    auto cancel_btn = messageBox->addButton("Cancel", Wt::StandardButton::No);
                    auto discard_btn = messageBox->addButton("Discard", Wt::StandardButton::Ignore);
                    save_btn->setStyleClass("btn-green");
                    cancel_btn->setStyleClass("btn-default");
                    discard_btn->setStyleClass("btn-red");
                    
                    
                    messageBox->buttonClicked().connect([=] {
                        if (messageBox->buttonResult() == Wt::StandardButton::Yes)
                        {
                            std::cout << "\n\n Save the file: " << selecter_file_path_ << "\n\n";
                            std::ofstream sys_file(selecter_file_path_);
                            if (!sys_file.is_open()) {
                                std::cout << "\n\n Failed to open file: " << selecter_file_path_ << "\n\n";
                                return;
                            }
                            // std::cout << "\n\n curent text: " << editor_->getUnsavedText() << "\n\n";
                            sys_file << editor_->getUnsavedText();
                            sys_file.close();
                            editor_->textSaved();
                            selected_file_wrapper_->removeStyleClass("?");
                            selected_file_wrapper_ = file_wrapper;
                            selected_file_wrapper_->addStyleClass("?");
                            tree_header_title_->setText(file);
                            selecter_file_path_ = default_folder_path_ + folder.first + "/" + file;
                            editor_->setFile(default_folder_path_ + folder.first + "/" + file);
                        }else if(messageBox->buttonResult() == Wt::StandardButton::Ignore)
                        {
                            selected_file_wrapper_->toggleStyleClass("[&>*:first-child]:block", false);
                            selected_file_wrapper_->removeStyleClass("?");
                            selected_file_wrapper_ = file_wrapper;
                            selected_file_wrapper_->addStyleClass("?");
                            selecter_file_path_ = default_folder_path_ + folder.first + "/" + file;
                            editor_->setFile(default_folder_path_ + folder.first + "/" + file);
                            tree_header_title_->setText(file);
                        }
                        removeChild(messageBox);
                    });
                    
                    messageBox->show();
                }else {
                    selecter_file_path_ = default_folder_path_ + folder.first + "/" + file;
                    editor_->setFile(default_folder_path_ + folder.first + "/" + file);
                    if(selected_file_wrapper_ != nullptr) 
                    {
                        selected_file_wrapper_->removeStyleClass("?");
                    }
                    selected_file_wrapper_ = file_wrapper;
                    selected_file_wrapper_->addStyleClass("?");
                    tree_header_title_->setText(file);
                    selecter_file_path_ = default_folder_path_ + folder.first + "/" + file;
                    editor_->setFile(default_folder_path_ + folder.first + "/" + file);
                }
            });

            // Select the first found file from the first found folder 
            if(selected_file_wrapper_ == nullptr)
            {
                // editor_->setCssEdditorText(getCssFromFile(default_folder_path_ + folder.first + "/" + file));
                std::cout << "\n\n -------------- path: " << default_folder_path_ + folder.first + "/" + file << "\n\n";
                selected_file_wrapper_ = file_wrapper;
                selected_file_wrapper_->addStyleClass("?");
                tree_header_title_->setText(file);
                selecter_file_path_ = default_folder_path_ + folder.first + "/" + file;
                editor_->setFile(default_folder_path_ + folder.first + "/" + file);
                std::cout << "\n\n path: " << default_folder_path_ + folder.first + "/" + file << "\n\n";
            }
        }
    }
}


std::vector<std::pair<std::string, std::vector<std::string>>> FilesManager::getFolders()
{
    std::vector<std::pair<std::string, std::vector<std::string>>> css_folders;
    std::vector<std::string> folders;
    // if(!std::filesystem::exists(default_folder_path_))
    // {
    //     std::filesystem::create_directory(default_folder_path_);
    // }
    // if(!std::filesystem::exists(default_folder_path_ + "/default"))
    // {
    //     std::filesystem::create_directory(default_folder_path_ + "/default");
    // }

    for (const auto& entry : std::filesystem::directory_iterator(default_folder_path_)) {
        if (entry.is_directory()) {
            folders.push_back(entry.path().filename().string());
        }
    }
    for(const auto& folder : folders)
    {
        std::vector<std::string> files;
        for (const auto& entry : std::filesystem::directory_iterator(default_folder_path_ + folder)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().filename().string());
            }
        }
        css_folders.push_back(std::make_pair(folder, files));
    }
    // sort
    std::sort(css_folders.begin(), css_folders.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });
    for(auto& folder : css_folders)
    {
        std::sort(folder.second.begin(), folder.second.end());
    }
    return css_folders;
}

Wt::WMessageBox* FilesManager::createMessageBox(std::string title, std::string temp)
{
    auto message_box = addChild(std::make_unique<Wt::WMessageBox>(title, temp, Wt::Icon::Warning , Wt::StandardButton::None));
    message_box->setOffsets(100, Wt::Side::Top);
    message_box->setModal(true);
    
    message_box->setStyleClass("bg-white dark:bg-gray-800 dark:text-gray-200");
    message_box->titleBar()->setStyleClass("flex items-center justify-center bg-white p-2 cursor-pointer dark:bg-gray-800 hover:bg-gray-200 dark:text-gray-300 dark:hover:bg-gray-700 border-b border-solid border-gray-200 dark:border-gray-700 text-xl font-bold");
    message_box->contents()->addStyleClass("flex items-center bg-white dark:bg-gray-800 dark:text-gray-200");
    message_box->footer()->setStyleClass("flex items-center justify-between bg-white p-2 dark:bg-gray-800 hover:bg-gray-200 dark:text-gray-300 dark:hover:bg-gray-700");
    return message_box;
}