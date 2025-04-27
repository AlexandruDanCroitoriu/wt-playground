#include "101-Stylus/Stylus.h"
#include <Wt/WStackedWidget.h>
#include <Wt/WPushButton.h>
#include <Wt/WTemplate.h>
#include <Wt/WApplication.h>
#include <Wt/WStackedWidget.h>

namespace Stylus {


Stylus::Stylus()
{
    brain_ = std::make_shared<Brain>();
    setOffsets(0, Wt::Side::Top | Wt::Side::Bottom | Wt::Side::Left | Wt::Side::Right);
    // setOffsets(0, Wt::Side::Bottom | Wt::Side::Left | Wt::Side::Right);
    // setOffsets(200, Wt::Side::Top);
    titleBar()->children()[0]->removeFromParent();
    setStyleClass("!border-0 shadow-lg rounded-xl overflow-x-visible z-[1]");
    titleBar()->hide();
    titleBar()->setStyleClass("p-0 bg-gray-300 flex items-center overflow-x-visible h-[40px]");
    contents()->setStyleClass("h-[100vh] overflow-y-auto overflow-x-visible bg-gray-900 flex");
    // setModal(true);
    setModal(false);
    setResizable(false);
    setMovable(false);

    Wt::WApplication::instance()->useStyleSheet(Wt::WApplication::instance()->docRoot() + "/static/stylus/questionmark.css");

    auto navbar = contents()->addWidget(std::make_unique<Wt::WContainerWidget>());
    auto content_wrapper = contents()->addWidget(std::make_unique<Wt::WStackedWidget>());

    navbar->setStyleClass("flex flex-col space-y-2 h-full bg-gray-800 p-2 border-r border-solid border-gray-600");
    content_wrapper->setStyleClass("flex-1 h-full bg-gray-800 overflow-y-auto");

    auto templates_menu_item = navbar->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-xml-logo")));
    auto tailwind_menu_item = navbar->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-tailwind-logo")));
    auto css_menu_item = navbar->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-css-logo")));
    auto javascript_menu_item = navbar->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-javascript-logo")));
    std::string nav_btns_styles = "hover:bg-gray-700 rounded-md p-1 cursor-pointer flex items-center";

    templates_menu_item->setStyleClass(nav_btns_styles);
    tailwind_menu_item->setStyleClass(nav_btns_styles);
    css_menu_item->setStyleClass(nav_btns_styles);
    javascript_menu_item->setStyleClass(nav_btns_styles);
    
    templates_files_manager_ = content_wrapper->addWidget(std::make_unique<XmlFilesManager>(brain_));    
    tailwind_config_ = content_wrapper->addWidget(std::make_unique<WTConfig>(brain_));
    css_files_manager_ = content_wrapper->addWidget(std::make_unique<CssFilesManager>(brain_));
    js_files_manager_ = content_wrapper->addWidget(std::make_unique<JsFilesManager>(brain_));
    // brain_->generateCssFile();

    templates_menu_item->toggleStyleClass("?", true);
    tailwind_menu_item->toggleStyleClass("?", false);
    css_menu_item->toggleStyleClass("?", false);
    javascript_menu_item->toggleStyleClass("?", false);

    templates_menu_item->clicked().connect(this, [=]() {
        templates_menu_item->toggleStyleClass("?", true);
        tailwind_menu_item->toggleStyleClass("?", false);
        css_menu_item->toggleStyleClass("?", false);
        javascript_menu_item->toggleStyleClass("?", false);
        content_wrapper->setCurrentWidget(templates_files_manager_);
        // templates_files_manager_->editor_->resetLayout();
        // templates_files_manager_->editor_->reuploadText();
    });

    tailwind_menu_item->clicked().connect(this, [=]() {
        templates_menu_item->toggleStyleClass("?", false);
        tailwind_menu_item->toggleStyleClass("?", true);
        css_menu_item->toggleStyleClass("?", false);
        javascript_menu_item->toggleStyleClass("?", false);
        content_wrapper->setCurrentWidget(tailwind_config_);
    });

    css_menu_item->clicked().connect(this, [=]() {
        templates_menu_item->toggleStyleClass("?", false);
        tailwind_menu_item->toggleStyleClass("?", false);
        css_menu_item->toggleStyleClass("?", true);
        javascript_menu_item->toggleStyleClass("?", false);
        content_wrapper->setCurrentWidget(css_files_manager_);
        // css_files_manager_->editor_->resetLayout();
        // css_files_manager_->editor_->reuploadText();
    });

    javascript_menu_item->clicked().connect(this, [=]() {
        templates_menu_item->toggleStyleClass("?", false);
        tailwind_menu_item->toggleStyleClass("?", false);
        css_menu_item->toggleStyleClass("?", false);
        javascript_menu_item->toggleStyleClass("?", true);
        content_wrapper->setCurrentWidget(js_files_manager_);
        // js_files_manager_->editor_->resetLayout();
        // js_files_manager_->editor_->reuploadText();
    });

    content_wrapper->currentWidgetChanged().connect([=]() {
        if(content_wrapper->currentIndex() == 0)
        {
            templates_files_manager_->editor_->resetLayout();
        }else if(content_wrapper->currentIndex() == 2)
        {
            css_files_manager_->editor_->resetLayout();
        }else if(content_wrapper->currentIndex() == 3)
        {
            js_files_manager_->editor_->resetLayout();
        }
    });

    Wt::WApplication::instance()->globalKeyWentDown().connect([=](Wt::WKeyEvent e)
    { 
        if (e.modifiers().test(Wt::KeyboardModifier::Alt))
        {
            if (e.key() == Wt::Key::Q)
            {
                if(isHidden())
                {
                    show();
                }
                else
                {
                    hide();
                }
            }else if (e.key() == Wt::Key::Key_1)
            {
                templates_menu_item->clicked().emit(Wt::WMouseEvent());
            }else if (e.key() == Wt::Key::Key_2)
            {
                tailwind_menu_item->clicked().emit(Wt::WMouseEvent());
            }else if (e.key() == Wt::Key::Key_3)
            {
                css_menu_item->clicked().emit(Wt::WMouseEvent());
            }else if (e.key() == Wt::Key::Key_4)
            {
                javascript_menu_item->clicked().emit(Wt::WMouseEvent());
            }
        }
    });
}

}
