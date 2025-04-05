#include "005-TailwindConfigCenter/TailwindConfigCenter.h"
#include <Wt/WStackedWidget.h>
#include <Wt/WPushButton.h>
#include <Wt/WTemplate.h>

TailwindConfigCenter::TailwindConfigCenter()
{
    setOffsets(0, Wt::Side::Top | Wt::Side::Bottom | Wt::Side::Left | Wt::Side::Right);
    // setOffsets(0, Wt::Side::Bottom | Wt::Side::Left | Wt::Side::Right);
    // setOffsets(30, Wt::Side::Top);
    titleBar()->children()[0]->removeFromParent();
    setStyleClass("!border-0 shadow-lg rounded-xl overflow-x-visible");
    titleBar()->hide();
    titleBar()->setStyleClass("p-0 bg-gray-300 flex items-center overflow-x-visible h-[40px]");
    contents()->setStyleClass("h-[100vh] overflow-y-auto overflow-x-visible bg-white dark:bg-gray-900 flex");
    setModal(false);
    setResizable(false);
    setMovable(false);

    auto navbar = contents()->addWidget(std::make_unique<Wt::WContainerWidget>());
    auto content_wrapper = contents()->addWidget(std::make_unique<Wt::WContainerWidget>());

    navbar->setStyleClass("flex flex-col space-y-2 h-full bg-white dark:bg-gray-800 p-2 border-r border-solid border-gray-200 dark:border-gray-600");
    content_wrapper->setStyleClass("flex-1 h-full bg-white dark:bg-gray-800 overflow-y-auto");

    auto tailwind_menu_item = navbar->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-tailwind-logo")));
    auto css_menu_item = navbar->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-css-logo")));
    auto javascript_menu_item = navbar->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-javascript-logo")));

    std::string nav_btns_styles = "hover:bg-gray-200 dark:hover:bg-gray-700 rounded-md p-2 cursor-pointer flex items-center";

    tailwind_menu_item->setStyleClass(nav_btns_styles);
    css_menu_item->setStyleClass(nav_btns_styles);
    javascript_menu_item->setStyleClass(nav_btns_styles);
    
    css_files_manager_ = content_wrapper->addWidget(std::make_unique<FilesManager>("../../tailwind4/css/", "css"));
    tailwind_config_ = content_wrapper->addWidget(std::make_unique<WTConfig>());
    js_files_manager_ = content_wrapper->addWidget(std::make_unique<FilesManager>("../../static/js/", "javascript"));
    
    tailwind_menu_item->toggleStyleClass("?", false);
    tailwind_config_->toggleStyleClass("hidden", true);

    css_menu_item->toggleStyleClass("?", true);
    css_files_manager_->toggleStyleClass("hidden", false);

    javascript_menu_item->toggleStyleClass("?", false);
    js_files_manager_->toggleStyleClass("hidden", true);

    tailwind_menu_item->clicked().connect(this, [=]() {
        if(tailwind_config_->hasStyleClass("?")) return;

        tailwind_menu_item->toggleStyleClass("?", true);
        tailwind_config_->toggleStyleClass("hidden", false);
        css_menu_item->toggleStyleClass("?", false);
        css_files_manager_->toggleStyleClass("hidden", true);
        javascript_menu_item->toggleStyleClass("?", false);
        js_files_manager_->toggleStyleClass("hidden", true);
    });

    css_menu_item->clicked().connect(this, [=]() {
        if(css_files_manager_->hasStyleClass("?")) return;
        tailwind_menu_item->toggleStyleClass("?", false);
        tailwind_config_->toggleStyleClass("hidden", true);
        css_menu_item->toggleStyleClass("?", true);
        css_files_manager_->toggleStyleClass("hidden", false);
        javascript_menu_item->toggleStyleClass("?", false);
        js_files_manager_->toggleStyleClass("hidden", true);
        css_files_manager_->editor_->resetLayout();
    });

    javascript_menu_item->clicked().connect(this, [=]() {
        if(js_files_manager_->hasStyleClass("?")) return;
        tailwind_menu_item->toggleStyleClass("?", false);
        tailwind_config_->toggleStyleClass("hidden", true);
        css_menu_item->toggleStyleClass("?", false);
        css_files_manager_->toggleStyleClass("hidden", true);
        javascript_menu_item->toggleStyleClass("?", true);
        js_files_manager_->toggleStyleClass("hidden", false);
        js_files_manager_->editor_->resetLayout();
    });
    
        

}