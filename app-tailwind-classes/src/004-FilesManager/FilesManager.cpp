#include "004-FilesManager/FilesManager.h"
#include <filesystem>
#include <Wt/WPushButton.h>
#include <Wt/WMenu.h>
#include <Wt/WMenuItem.h>
#include <Wt/WPanel.h>
#include <Wt/WTemplate.h>
#include <fstream>
#include <Wt/WMessageBox.h>

FilesManager::FilesManager(std::string default_folder_path, std::string language)
    : default_folder_path_(default_folder_path)
{
    setStyleClass("w-full h-full flex");

    // Add the default css file
    auto folder_tree_wrapper = addWidget(std::make_unique<Wt::WContainerWidget>());
    folder_tree_wrapper->setStyleClass("min-w-[280px]");
    auto tree_header = folder_tree_wrapper->addWidget(std::make_unique<Wt::WContainerWidget>());
    tree_header->setStyleClass("group flex items-center border-b border-solid border-gray-200 dark:border-gray-700");
    tree_header_title_ = tree_header->addWidget(std::make_unique<Wt::WText>("selected css file"));
    tree_header_title_->setStyleClass("text-xl font-semibold m-1 text-green-500 dark:text-green-400");

    // folder buttons
    auto add_file_btn = tree_header->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-add-folder")));
    add_file_btn->setStyleClass("ml-auto group-hover:block hidden hover:bg-gray-200 dark:hover:bg-gray-900 rounded-md p-1");
    add_file_btn->clicked().preventPropagation();

    folders_tree_wrapper_ = folder_tree_wrapper->addWidget(std::make_unique<Wt::WContainerWidget>());
    folders_tree_wrapper_->setStyleClass("w-full flex-1 overflow-y-auto flex flex-col");

    setTreeFolderWidgets();

    editor_ = addWidget(std::make_unique<MonacoEdditor>(default_folder_path_ +folders_[0].first + "/" + folders_[0].second[0], language));
    editor_->avalable_save().connect(this, [=] (bool avalable) {
        if(avalable)
        {
            selected_file_wrapper_->toggleStyleClass("[&>*:last-child]:block", true);
            
        }else {
            selected_file_wrapper_->toggleStyleClass("[&>*:last-child]:block", false);
        }
    });
}


void FilesManager::setTreeFolderWidgets()
{
    folders_tree_wrapper_->clear();
    selected_file_wrapper_ = nullptr;
    // add folders and files in the UI
    folders_ = getCssFolders();
    for(const auto& folder : folders_)
    {
        auto panel = folders_tree_wrapper_->addWidget(std::make_unique<Wt::WPanel>());
        panel->setCollapsible(true);
        // panel->setCollapsed(true);
        panel->setAnimation(Wt::WAnimation(Wt::AnimationEffect::SlideInFromTop, Wt::TimingFunction::EaseInOut, 250));
        panel->setStyleClass("!border-none ");
        panel->titleBarWidget()->setStyleClass("group relative flex items-center px-2 cursor-pointer tracking-widest  dark:bg-gray-800 hover:bg-gray-200 dark:text-gray-300 dark:hover:bg-gray-700 border-b border-solid border-gray-200 dark:border-gray-700");       
        panel->titleBarWidget()->addWidget(std::make_unique<Wt::WText>(folder.first))->setStyleClass("text-xl ml-2 font-semibold");

        // add Files button
        auto add_file_btn = panel->titleBarWidget()->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-add-file")));
        add_file_btn->setStyleClass("group-hover:block hidden absolute right-1 hover:bg-gray-200 dark:hover:bg-gray-900 rounded-md p-1 ");
        add_file_btn->clicked().preventPropagation();

        auto central_widget = panel->setCentralWidget(std::make_unique<Wt::WContainerWidget>());
        central_widget->setStyleClass("w-full bg-white dark:bg-gray-800");

        for(const auto& file : folder.second)
        {
            auto file_wrapper = central_widget->addWidget(std::make_unique<Wt::WContainerWidget>());
            file_wrapper->setStyleClass("group relative flex items-center cursor-pointer hover:bg-gray-200 dark:hover:bg-gray-700 rounded-md text-gray-700 dark:text-gray-200");
            auto delete_file_btn = file_wrapper->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-trash")));
            delete_file_btn->setStyleClass("absolute left-0 rounded-md p-1 hover:bg-gray-200 dark:hover:bg-gray-900 group-hover:block hidden");
            delete_file_btn->clicked().preventPropagation();

            auto file_name = file_wrapper->addWidget(std::make_unique<Wt::WText>(file));
            file_name->setStyleClass("text-md ml-9");

            // file buttons
            auto save_file_btn = file_wrapper->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-green-checked")));
            save_file_btn->setStyleClass("ml-auto rounded-md p-1 hover:bg-gray-200 dark:hover:bg-gray-900 hidden");
            save_file_btn->clicked().preventPropagation();
            save_file_btn->clicked().connect(this, [=](){
                editor_->saveTextToFile();
            });

            file_wrapper->clicked().connect(this, [=] () {
                if(editor_->unsavedChanges()){
                    std::cout << "\nUnsaved changes, please save the file first\n";
                    auto messageBox =
                    addChild(std::make_unique<Wt::WMessageBox>("Save the changes from the current file ?", "", Wt::Icon::None, Wt::StandardButton::None));
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
                    
                    messageBox->setModal(true);
                    
                    messageBox->buttonClicked().connect([=] {
                        if (messageBox->buttonResult() == Wt::StandardButton::Yes)
                        {
                            editor_->saveTextToFile();
                            selected_file_wrapper_->removeStyleClass("?");
                            selected_file_wrapper_ = file_wrapper;
                            selected_file_wrapper_->addStyleClass("?");
                            tree_header_title_->setText(file);
                            editor_->setFile(default_folder_path_ + folder.first + "/" + file);
                        }else if(messageBox->buttonResult() == Wt::StandardButton::Ignore)
                        {
                            selected_file_wrapper_->toggleStyleClass("[&>*:last-child]:block", false);
                            selected_file_wrapper_->removeStyleClass("?");
                            selected_file_wrapper_ = file_wrapper;
                            selected_file_wrapper_->addStyleClass("?");
                            editor_->setFile(default_folder_path_ + folder.first + "/" + file);
                            tree_header_title_->setText(file);
                        }
                        removeChild(messageBox);
                    });
                    
                    messageBox->show();
                }else {
                    editor_->setFile(default_folder_path_ + folder.first + "/" + file);
                    selected_file_wrapper_->removeStyleClass("?");
                    selected_file_wrapper_ = file_wrapper;
                    selected_file_wrapper_->addStyleClass("?");
                    tree_header_title_->setText(file);
                }
            });

            if(!selected_file_wrapper_)
            {
                // editor_->setCssEdditorText(getCssFromFile(default_folder_path_ + folder.first + "/" + file));
                selected_file_wrapper_ = file_wrapper;
                selected_file_wrapper_->addStyleClass("?");
                tree_header_title_->setText(file);
                std::cout << "\n\n path: " << default_folder_path_ + folder.first + "/" + file << "\n\n";
            }
        }
    }
}


std::vector<std::pair<std::string, std::vector<std::string>>> FilesManager::getCssFolders()
{
    std::vector<std::pair<std::string, std::vector<std::string>>> css_folders;
    std::vector<std::string> folders;
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
