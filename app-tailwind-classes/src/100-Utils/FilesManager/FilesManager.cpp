#include "100-Utils/FilesManager/FilesManager.h"
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
#include <Wt/WRandom.h>
#include <Wt/WApplication.h>

FilesManager::FilesManager(std::string default_folder_path, std::string language)
    : default_folder_path_(default_folder_path),
    file_extension_(language)
{
    
    // setHeight(Wt::WLength(100, Wt::LengthUnit::ViewportHeight));
    if (file_extension_.compare("javascript") == 0)
    {
        file_extension_ = "js";
    }

    auto contents = addWidget(std::make_unique<Wt::WContainerWidget>());
    auto layout = std::make_unique<Wt::WHBoxLayout>();

    // tree wrapper
    sidebar_wrapper_ = layout->insertWidget(0, std::make_unique<Wt::WContainerWidget>(), 1);
    sidebar_wrapper_->setStyleClass("flex flex-col h-screen");
    editor_ = layout->insertWidget(1, std::make_unique<MonacoEditor>(language), 1);

    layout->setResizable(0, true, Wt::WLength(240, Wt::LengthUnit::Pixel));
    // layout->setResizable(0);
    // std::cout << "\n\n\n\n ----------------- layout->isResizable(0) = " << layout->isResizable(0) << "\n\n\n\n";
    // layout->isResizable(0);
    // layout->setResizable(1, true);
    // layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout_ = layout.get();
    contents->setLayout(std::move(layout));
    // layout->setResizable(0);


    // contents->setPadding(0, Wt::Side::Left | Wt::Side::Right | Wt::Side::Top | Wt::Side::Bottom);
    // tree header
    auto tree_header = sidebar_wrapper_->addWidget(std::make_unique<Wt::WContainerWidget>());
    tree_header->setStyleClass("group min-w-[240px] flex items-center border-b border-solid border-gray-700");
    tree_header_title_ = tree_header->addWidget(std::make_unique<Wt::WText>("selected " + file_extension_ + " file"));
    tree_header_title_->setStyleClass("text-md m-1 text-green-400");

    // add folder btn
    auto add_folder_btn = tree_header->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-add-folder")));
    add_folder_btn->setStyleClass("ml-auto group-hover:block hidden hover:bg-gray-900 rounded-md p-1");
    add_folder_btn->clicked().preventPropagation();

    folders_tree_wrapper_ = sidebar_wrapper_->addWidget(std::make_unique<Wt::WContainerWidget>());
    folders_tree_wrapper_->setStyleClass("w-full flex-1 overflow-y-auto flex flex-col");

    navigation_footer_ = sidebar_wrapper_->addWidget(std::make_unique<Wt::WContainerWidget>());
    navigation_footer_->setStyleClass("flex items-center justify-between p-2 border-t border-solid border-gray-700");

    folders_ = std::make_shared<std::vector<std::pair<std::string, std::vector<std::string>>>>();
    auto folders = getFolders();
    for (const auto &folder : folders)
    {
        folders_->push_back(folder);
    }


    editor_->avalable_save().connect(this, [=](bool avalable)
                                        {
    if(selected_file_wrapper_ == nullptr) return;
    if(avalable)
    {
        selected_file_wrapper_->toggleStyleClass("[&>*:first-child]:block", true);
        
    }else {
        selected_file_wrapper_->toggleStyleClass("[&>*:first-child]:block", false);
    } });
    setTreeFolderWidgets();

    editor_->save_file_signal().connect(this, [=](std::string text)
                                        {
    // std::cout << "\n\n Save file signal received.\n\n";
    if(selected_file_path_.compare("") == 0) {
        std::cout << "\n\n No file selected to save.\n\n";
        return;
    }
    std::ofstream file(selected_file_path_);
    if (!file.is_open()) {
        std::cout << "\n\n Failed to open file: " << selected_file_path_ << "\n\n";
        return;
    }
    file << text;
    file.close();
    // std::cout << "\n\n File saved: " << selected_file_path_ << "\n\n";
    editor_->textSaved(); // this is fo monaco to set the current text and emit avalable_save_ signal to fase
    
    file_saved_.emit(selected_file_path_);
    });

    add_folder_btn->clicked().connect(this, [=]()
                                        {
    auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Create new folder"));

    dialog->setModal(true);
    dialog->rejectWhenEscapePressed();
    dialog->setOffsets(100, Wt::Side::Top);

    dialog->setStyleClass("bg-gray-800 text-gray-200");
    dialog->titleBar()->setStyleClass("flex items-center justify-center p-2 cursor-pointer bg-gray-800 text-gray-300 hover:bg-gray-700 border-b border-solid border-gray-700 text-xl font-bold");
    dialog->contents()->setStyleClass("flex flex-col bg-gray-800 text-gray-200");

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
    new_folder_name_input->setStyleClass("w-full min-w-[200px] placeholder:text-slate-400 text-sm border rounded-md px-3 py-2 transition duration-300 ease focus:outline-none shadow-sm focus:shadow bg-gray-800 placeholder:text-gray-500 text-gray-200 border-gray-600 focus:border-gray-400 hover:border-gray-500");
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
            folders_->clear();
            auto folders = getFolders();
            for (const auto &folder : folders)
            {
                folders_->push_back(folder);
            }
            setTreeFolderWidgets();
        }
        removeChild(dialog);
    });
    dialog->show(); });
}

void FilesManager::setTreeFolderWidgets()
{
    folders_tree_wrapper_->clear();
    selected_file_wrapper_ = nullptr;
    // add folders and files in the UI

    for (const auto &folder : *folders_)
    {
        auto panel = folders_tree_wrapper_->addWidget(std::make_unique<Wt::WPanel>());
        panel->setCollapsible(true);
        // panel->setThemeStyleEnabled(false); 
        // panel->setCollapsed(true);
        panel->setAnimation(Wt::WAnimation(Wt::AnimationEffect::SlideInFromTop, Wt::TimingFunction::EaseInOut, 250));
        panel->setStyleClass("!border-none ");
        panel->titleBarWidget()->setStyleClass("group relative flex items-center px-2 cursor-pointer tracking-widest bg-gray-800 text-gray-300 hover:bg-gray-700 border-b border-solid border-gray-700");

        auto folder_title = panel->titleBarWidget()->addWidget(std::make_unique<Wt::WText>(folder.first));
        folder_title->setStyleClass("ml-2");

        auto edit_folder_name_btn = panel->titleBarWidget()->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-edit")));
        edit_folder_name_btn->setStyleClass("group-hover:block hidden absolute right-6 rounded-md p-1 hover:bg-gray-900 group-hover:block hidden");
        edit_folder_name_btn->clicked().preventPropagation();

        // add File button
        auto add_file_btn = panel->titleBarWidget()->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-add-file")));
        add_file_btn->setStyleClass("group-hover:block hidden absolute right-0 hover:bg-gray-900 rounded-md p-1 overflow-hidden");
        add_file_btn->clicked().preventPropagation();

        edit_folder_name_btn->clicked().connect(this, [=]()
                                                {
            auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Change folder name from " + folder.first));
            dialog->setTabIndex(10000);
            dialog->setOffsets(100, Wt::Side::Top);
            dialog->setModal(true);
            dialog->rejectWhenEscapePressed();
            dialog->setStyleClass("bg-gray-800 text-gray-200");
            dialog->titleBar()->setStyleClass("flex items-center justify-center p-2 cursor-pointer bg-gray-800 text-gray-300 hover:bg-gray-700 border-b border-solid border-gray-700 text-xl font-bold");
            dialog->contents()->setStyleClass("flex flex-col bg-gray-800 text-gray-200");
    
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
            new_file_name_input->setStyleClass("w-full min-w-[200px] placeholder:text-slate-400 text-sm border rounded-md px-3 py-2 transition duration-300 ease focus:outline-none shadow-sm focus:shadow bg-gray-800 placeholder:text-gray-500 text-gray-200 border-gray-600 focus:border-gray-400 hover:border-gray-500");
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
                    folders_->clear();
                    auto folders = getFolders();
                    for (const auto &folder : folders)
                    {
                        folders_->push_back(folder);
                    }
                    setTreeFolderWidgets();
                }
                removeChild(dialog);
            });

            dialog->show(); 
        });

        add_file_btn->clicked().connect(this, [=]()
                                        {
        auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("Create new file in folder " + folder.first));
        dialog->setOffsets(100, Wt::Side::Top);
        dialog->setModal(true);
        dialog->rejectWhenEscapePressed();
        dialog->setStyleClass("bg-gray-800 text-gray-200 z-[2]");
        dialog->titleBar()->setStyleClass("flex items-center justify-center p-2 cursor-pointer bg-gray-800 text-gray-300 hover:bg-gray-700 border-b border-solid border-gray-700 text-xl font-bold");
        dialog->contents()->setStyleClass("flex flex-col bg-gray-800");

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
        new_file_name_input->setStyleClass("w-full min-w-[200px] placeholder:text-slate-400  text-sm border rounded-md px-3 py-2 transition duration-300 ease focus:outline-none focus:border-slate-400 hover:border-slate-300 shadow-sm focus:shadow bg-gray-800 placeholder:text-gray-500 border-gray-600 focus:border-gray-400 hover:border-gray-500");
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
                std::filesystem::path new_path(default_folder_path_ + folder.first + "/" + new_file_name + "." + file_extension_);
                std::ofstream new_file(new_path);
                folders_tree_wrapper_->clear();
                folders_->clear();
                auto folders = getFolders();
                for (const auto &folder : folders)
                {
                    folders_->push_back(folder);
                }
                setTreeFolderWidgets();
            }
            removeChild(dialog);
        });

        dialog->show();
        });

        // delete folder button
        if (folder.second.size() == 0)
        {
            auto delete_folder_btn = panel->titleBarWidget()->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-trash")));
            delete_folder_btn->setStyleClass("absolute right-1 rounded-md p-1 hover:bg-gray-900");
            add_file_btn->addStyleClass("!right-5");
            add_file_btn->addStyleClass("!right-11");
            delete_folder_btn->clicked().preventPropagation();
            delete_folder_btn->clicked().connect(this, [=]()
                                                    {
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
                        auto folders = getFolders();
                        folders_->clear();
                        for (const auto &folder : folders)
                        {
                            folders_->push_back(folder);
                        }
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
            folders_->clear();
            auto folders = getFolders();
            for (const auto &folder : folders)
            {
                folders_->push_back(folder);
            }

            setTreeFolderWidgets(); });
        }

        // panel->centralWidget()->setStyleClass("asjkdhfaksjdfhajsdfk");
        auto central_widget = panel->setCentralWidget(std::make_unique<Wt::WContainerWidget>());
        central_widget->setStyleClass("w-full bg-gray-800");

        for (const auto &file : folder.second)
        {
            auto file_wrapper = setTreeFileWidget(central_widget, folder.first, file);

            // Select the first found file from the first found folder
            if (selected_file_wrapper_ == nullptr)
            {
                // editor_->setCssEdditorText(getCssFromFile(default_folder_path_ + folder.first + "/" + file));
                std::cout << "\n\n -------------- path: " << default_folder_path_ + folder.first + "/" + file << "\n\n";
                selected_file_wrapper_ = file_wrapper;
                selected_file_wrapper_->addStyleClass("?");
                tree_header_title_->setText(file);
                selected_file_path_ = default_folder_path_ + folder.first + "/" + file;
                editor_->setFile(default_folder_path_ + folder.first + "/" + file);
                // file_selected_.emit(default_folder_path_ + folder.first + "/" + file);
                // std::cout << "\n\n path: " << default_folder_path_ + folder.first + "/" + file << "\n\n";
            }
        }
    }
}

Wt::WContainerWidget* FilesManager::setTreeFileWidget(Wt::WContainerWidget* files_wrapper, std::string folder_name, std::string file_name)
{
    auto file_wrapper = files_wrapper->addWidget(std::make_unique<Wt::WContainerWidget>());
    file_wrapper->setStyleClass("group w-full relative flex items-center cursor-pointer hover:bg-gray-700 rounded-md text-gray-200");

    // file buttons
    auto save_file_btn = file_wrapper->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-green-checked")));
    save_file_btn->setStyleClass("absolute left-1 hover:bg-gray-900 hidden");
    save_file_btn->clicked().preventPropagation();
    save_file_btn->clicked().connect(this, [=]()
    {
        std::ofstream sys_file(selected_file_path_);
        if (!sys_file.is_open()) {
            std::cout << "\n\n Failed to open file: " << selected_file_path_ << "\n\n";
            return;
        }
        sys_file << editor_->getUnsavedText();
        sys_file.close();
        editor_->textSaved(); 
    });

    file_wrapper->addWidget(std::make_unique<Wt::WText>(file_name))->setStyleClass("ml-7 font-thin text-md");

    auto delete_file_btn = file_wrapper->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-trash")));
    delete_file_btn->setStyleClass("absolute right-0 rounded-md p-1 hover:bg-gray-900 group-hover:block hidden");
    delete_file_btn->clicked().preventPropagation();
    delete_file_btn->clicked().connect(this, [=]()
                                        {
    // delete file 
    std::string title = "Are you sure you want to delete the file ?";
    std::string content = "<div class='flex-1 text-center font-bold text-2xl'>" + file_name + "</div>";
    auto messageBox = createMessageBox(title, content);
    auto delete_btn = messageBox->addButton("Delete", Wt::StandardButton::Yes);
    auto cancel_btn = messageBox->addButton("Cancel", Wt::StandardButton::No);
    delete_btn->setStyleClass("btn-red");
    cancel_btn->setStyleClass("btn-default");
    
    messageBox->buttonClicked().connect([=] {
        if (messageBox->buttonResult() == Wt::StandardButton::Yes)
            {
            std::filesystem::path file_path = default_folder_path_ + folder_name + "/" + file_name;

            // delete file
            if (std::filesystem::remove(file_path)) {
                folders_tree_wrapper_->clear();
                auto folders = getFolders();
                folders_->clear();
                for (const auto &folder : folders)
                {
                    folders_->push_back(folder);
                }
                setTreeFolderWidgets();
            } else {
                Wt::WApplication::instance()->log("ERROR") << "\n\nError deleting file.\n\n";                    
            }
        }
        removeChild(messageBox);
    });
    
    messageBox->show(); });

    auto edit_file_name_btn = file_wrapper->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-edit")));
    edit_file_name_btn->setStyleClass("absolute right-5 rounded-md p-1 hover:bg-gray-900 group-hover:block hidden");
    edit_file_name_btn->clicked().preventPropagation();
    edit_file_name_btn->clicked().connect(this, [=]()
                                            {
    auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("change file name from " + folder_name));
    dialog->setOffsets(100, Wt::Side::Top);
    dialog->setModal(true);
    dialog->rejectWhenEscapePressed();
    dialog->setStyleClass("bg-gray-800 text-gray-200 z-[2]");
    dialog->titleBar()->setStyleClass("flex items-center justify-center p-2 cursor-pointer bg-gray-800 text-gray-300 hover:bg-gray-700 border-b border-solid border-gray-700 text-xl font-bold");
    dialog->contents()->setStyleClass("flex flex-col bg-gray-800 text-gray-200");

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
    new_file_name_input->setStyleClass("w-full min-w-[200px] text-sm border rounded-md px-3 py-2 transition duration-300 ease focus:outline-none shadow-sm focus:shadow bg-gray-800 placeholder:text-gray-500 text-gray-200 border-gray-600 focus:border-gray-400 hover:border-gray-500");
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
        std::filesystem::path new_path(default_folder_path_ + folder_name + "/" + new_file_name + ".css");
        if (std::filesystem::exists(new_path)) {
            error_label->setText("file with the same name already exists.");
        }else {
            dialog->accept();                
        }
    });
    dialog->finished().connect(this, [=](){
        if (dialog->result() == Wt::DialogCode::Accepted) {
            std::string new_file_name = new_file_name_input->text().toUTF8();
            std::filesystem::path old_path(default_folder_path_ + folder_name + "/" + file_name);
            std::filesystem::path new_path(default_folder_path_ + folder_name + "/" + new_file_name + "." + file_extension_);
            std::filesystem::rename(old_path, new_path);
            folders_tree_wrapper_->clear();
            auto folders = getFolders();
            folders_->clear();
            for (const auto &folder : folders)
            {
                folders_->push_back(folder);
            }
            setTreeFolderWidgets();
        }
        removeChild(dialog);
    });

    dialog->show(); });

    file_wrapper->clicked().connect(this, [=]()
    {
        // Same file was chosen 
        if(selected_file_path_.compare(default_folder_path_ + folder_name + "/" + file_name) == 0)
        {  
            // if(editor_->unsavedChanges()) return;
            editor_->reuploadText();
            return;
        }
        // Other file was chosen but the current file has unsaved changes
        else  if(editor_->unsavedChanges())
        {
            // std::cout << "\nUnsaved changes, please save the file first\n";
            auto title = "Save the changes from the current file ?";
            auto messageBox = createMessageBox(title, "");

            auto save_btn = messageBox->addButton("Save", Wt::StandardButton::Yes);
            auto cancel_btn = messageBox->addButton("Cancel", Wt::StandardButton::No);
            auto discard_btn = messageBox->addButton("Discard", Wt::StandardButton::Ignore);
            save_btn->setStyleClass("btn-green");
            cancel_btn->setStyleClass("btn-default");
            discard_btn->setStyleClass("btn-red");
            
            
            messageBox->buttonClicked().connect([=] {
                if (messageBox->buttonResult() == Wt::StandardButton::Yes)
                {
                    std::cout << "\n\n Save the file: " << selected_file_path_ << "\n\n";
                    std::ofstream sys_file(selected_file_path_);
                    if (!sys_file.is_open()) {
                        std::cout << "\n\n Failed to open file: " << selected_file_path_ << "\n\n";
                        return;
                    }
                    // std::cout << "\n\n curent text: " << editor_->getUnsavedText() << "\n\n";
                    sys_file << editor_->getUnsavedText();
                    sys_file.close();
                    editor_->textSaved();
                    selected_file_wrapper_->removeStyleClass("?");
                    selected_file_wrapper_ = file_wrapper;
                    selected_file_wrapper_->addStyleClass("?");
                    tree_header_title_->setText(file_name);
                    selected_file_path_ = default_folder_path_ + folder_name + "/" + file_name;
                    editor_->setFile(default_folder_path_ + folder_name + "/" + file_name);
                }else if(messageBox->buttonResult() == Wt::StandardButton::Ignore)
                {
                    selected_file_wrapper_->toggleStyleClass("[&>*:first-child]:block", false);
                    selected_file_wrapper_->removeStyleClass("?");
                    selected_file_wrapper_ = file_wrapper;
                    selected_file_wrapper_->addStyleClass("?");
                    selected_file_path_ = default_folder_path_ + folder_name + "/" + file_name;
                    editor_->setFile(default_folder_path_ + folder_name + "/" + file_name);
                    tree_header_title_->setText(file_name);
                }
                removeChild(messageBox);
            });
            
            messageBox->show();
        }
        // Other file was chosen and the current file has no unsaved changes
        else 
        {
            selected_file_path_ = default_folder_path_ + folder_name + "/" + file_name;
            editor_->setFile(default_folder_path_ + folder_name + "/" + file_name);
            if(selected_file_wrapper_ != nullptr) 
            {
                selected_file_wrapper_->removeStyleClass("?");
            }
            selected_file_wrapper_ = file_wrapper;
            selected_file_wrapper_->addStyleClass("?");
            tree_header_title_->setText(file_name);
            selected_file_path_ = default_folder_path_ + folder_name + "/" + file_name;
            editor_->setFile(default_folder_path_ + folder_name + "/" + file_name);
        } 
        file_selected_.emit(default_folder_path_ + folder_name + "/" + file_name);
        
    });
    return file_wrapper;
}


std::vector<std::pair<std::string, std::vector<std::string>>> FilesManager::getFolders()
{
    std::vector<std::pair<std::string, std::vector<std::string>>> return_folders;
    std::vector<std::string> folders;
    // if(!std::filesystem::exists(default_folder_path_))
    // {
    //     std::filesystem::create_directory(default_folder_path_);
    // }
    // if(!std::filesystem::exists(default_folder_path_ + "/default"))
    // {
    //     std::filesystem::create_directory(default_folder_path_ + "/default");
    // }

    for (const auto &entry : std::filesystem::directory_iterator(default_folder_path_))
    {
        if (entry.is_directory())
        {
            folders.push_back(entry.path().filename().string());
        }
    }
    for (const auto &folder : folders)
    {
        std::vector<std::string> files;
        for (const auto &entry : std::filesystem::directory_iterator(default_folder_path_ + folder))
        {
            if (entry.is_regular_file())
            {
                files.push_back(entry.path().filename().string());
            }
        }
        return_folders.push_back(std::make_pair(folder, files));
    }
    // sort
    std::sort(return_folders.begin(), return_folders.end(), [](const auto &a, const auto &b)
                { return a.first < b.first; });
    for (auto &folder : return_folders)
    {
        std::sort(folder.second.begin(), folder.second.end());
    }
    get_folders_signal_.emit();
    return return_folders;
}

Wt::WMessageBox *FilesManager::createMessageBox(std::string title, std::string temp)
{
    auto message_box = addChild(std::make_unique<Wt::WMessageBox>(title, temp, Wt::Icon::Warning, Wt::StandardButton::None));
    message_box->setOffsets(100, Wt::Side::Top);
    message_box->setModal(true);

    message_box->setStyleClass("bg-gray-800 text-gray-200");
    message_box->titleBar()->setStyleClass("flex items-center justify-center p-2 cursor-pointer bg-gray-800 text-gray-300 hover:bg-gray-700 border-b border-solid border-gray-700 text-xl font-bold");
    message_box->contents()->addStyleClass("flex items-center bg-gray-800 text-gray-200");
    message_box->footer()->setStyleClass("flex items-center justify-between  p-2 bg-gray-800 text-gray-300 hover:bg-gray-700");
    return message_box;
}