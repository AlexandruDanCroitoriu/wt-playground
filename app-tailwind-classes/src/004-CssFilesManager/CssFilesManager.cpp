#include "004-CssFilesManager/CssFilesManager.h"
#include <filesystem>
#include <Wt/WPushButton.h>
#include <Wt/WMenu.h>
#include <Wt/WMenuItem.h>
#include <Wt/WPanel.h>
#include <Wt/WTemplate.h>
#include <fstream>

CssFilesManager::CssFilesManager()
{
    setStyleClass("w-full min-h-[600px] flex");

    // Add the default css file
    auto folder_tree_wrapper = addWidget(std::make_unique<Wt::WContainerWidget>());
    folder_tree_wrapper->setStyleClass("min-w-[280px]");
    auto selected_css_file_wrapper = folder_tree_wrapper->addWidget(std::make_unique<Wt::WContainerWidget>());
    selected_css_file_wrapper->setStyleClass("flex items-center");
    auto file_name = selected_css_file_wrapper->addWidget(std::make_unique<Wt::WText>(default_css_file_));
    selected_css_file_wrapper->setStyleClass("group flex items-center cursor-pointer hover:bg-gray-200 dark:hover:bg-gray-700 rounded-md text-gray-700 dark:text-gray-200");
    file_name->setStyleClass("text-md ml-2");
    auto btns_wrapper = selected_css_file_wrapper->addWidget(std::make_unique<Wt::WContainerWidget>());
    btns_wrapper->setStyleClass("ml-auto flex items-center");
    
    // default css file buttons
    auto save_file_btn = btns_wrapper->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-green-checked")));
    save_file_btn->setStyleClass("rounded-md p-1 hover:bg-gray-200 dark:hover:bg-gray-900");
    save_file_btn->clicked().preventPropagation();

    selected_css_file_wrapper->clicked().connect(this, [=](){
        css_editor_->setCssEdditorText(getCssFromFile(default_css_path_ + "/" + default_css_file_));
        selected_css_file_wrapper_->removeStyleClass("?");
        selected_css_file_wrapper_ = selected_css_file_wrapper;
        selected_css_file_wrapper_->addStyleClass("?");
    });
    selected_css_file_wrapper->addStyleClass("?");
    selected_css_file_wrapper_ = selected_css_file_wrapper;

    css_editor_ = addWidget(std::make_unique<MonacoCssEdditor>(getCssFromFile(default_css_path_ + default_css_file_)));
    css_editor_->setStyleClass("flex-1 w-full");


    // add folders and files in the UI
    auto css_folders = getCssFolders();
    for(const auto& folder : css_folders)
    {
        auto panel = folder_tree_wrapper->addWidget(std::make_unique<Wt::WPanel>());
        panel->setCollapsible(true);
        // panel->setCollapsed(true);
        panel->setAnimation(Wt::WAnimation(Wt::AnimationEffect::SlideInFromTop, Wt::TimingFunction::EaseInOut, 250));
        panel->setStyleClass("!border-none ");
        panel->titleBarWidget()->setStyleClass("group flex items-center px-2 cursor-pointer tracking-widest  dark:bg-gray-800 hover:bg-gray-200 dark:text-gray-300 dark:hover:bg-gray-700 border-b border-solid border-gray-200 dark:border-gray-700");
        panel->titleBarWidget()->addWidget(std::make_unique<Wt::WText>(folder.first))->setStyleClass("text-md ml-2 font-semibold");

        // folder buttons
        auto add_file_btn = panel->titleBarWidget()->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-add-folder")));
        add_file_btn->setStyleClass("ml-auto group-hover:block hidden hover:bg-gray-200 dark:hover:bg-gray-900 rounded-md p-1");
        add_file_btn->clicked().preventPropagation();

        auto central_widget = panel->setCentralWidget(std::make_unique<Wt::WContainerWidget>());
        central_widget->setStyleClass("dark:bg-gray-800 ");

        for(const auto& file : folder.second)
        {
            auto file_wrapper = central_widget->addWidget(std::make_unique<Wt::WContainerWidget>());
            auto file_name = file_wrapper->addWidget(std::make_unique<Wt::WText>(file));
            file_wrapper->setStyleClass("group flex items-center cursor-pointer hover:bg-gray-200 dark:hover:bg-gray-700 rounded-md text-gray-700 dark:text-gray-200");
            file_name->setStyleClass("text-md ml-5");
            auto btns_wrapper = file_wrapper->addWidget(std::make_unique<Wt::WContainerWidget>());
            btns_wrapper->setStyleClass("ml-auto flex items-center");

            // file buttons
            auto add_file_btn = btns_wrapper->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-add-file")));
            add_file_btn->setStyleClass("group-hover:block hidden hover:bg-gray-200 dark:hover:bg-gray-900 rounded-md p-1");
            add_file_btn->clicked().preventPropagation();
            auto save_file_btn = btns_wrapper->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-green-checked")));
            save_file_btn->setStyleClass("rounded-md p-1 hover:bg-gray-200 dark:hover:bg-gray-900");
            save_file_btn->clicked().preventPropagation();

            file_wrapper->clicked().connect(this, [=]   (){
                css_editor_->setCssEdditorText(getCssFromFile(default_css_path_ + folder.first + "/" + file));
                selected_css_file_wrapper_->removeStyleClass("?");
                selected_css_file_wrapper_ = file_wrapper;
                selected_css_file_wrapper_->addStyleClass("?");
            });
        }
    }


    // auto btn = folder_tree_wrapper->addWidget(std::make_unique<Wt::WPushButton>("Test"));
    // btn->setStyleClass("btn-default inline-block");
    // btn->clicked().connect(this, [=](){
    //     // css_editor_->setCssTextFromFile(default_css_path_ + css_folders[0].first + "/" + css_folders[0].second[0]);
    //     css_editor_->setCssTextFromFile(default_css_path_ + css_folders[0].first + "/" + css_folders[0].second[1]);
    // });


 
}

std::string CssFilesManager::getCssFromFile(std::string file_path)
{
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cout << "\n\n Failed to open file: " << file_path << "\n\n";
        return "";
    }

    std::string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    return file_content;
}

std::vector<std::pair<std::string, std::vector<std::string>>> CssFilesManager::getCssFolders()
{
    std::vector<std::pair<std::string, std::vector<std::string>>> css_folders;
    std::vector<std::string> folders;
    for (const auto& entry : std::filesystem::directory_iterator(default_css_path_)) {
        if (entry.is_directory()) {
            folders.push_back(entry.path().filename().string());
        }
    }
    for(const auto& folder : folders)
    {
        std::vector<std::string> files;
        for (const auto& entry : std::filesystem::directory_iterator(default_css_path_ + folder)) {
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
