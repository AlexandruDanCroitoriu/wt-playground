#include "101-Stylus/Stylus.h"
#include <Wt/WStackedWidget.h>
#include <Wt/WPushButton.h>
#include <Wt/WTemplate.h>
#include <Wt/WApplication.h>
#include <Wt/WStackedWidget.h>
#include <fstream>
#include <Wt/WRandom.h>

namespace Stylus {


Stylus::Stylus()
    : state_(std::make_shared<StylusState>())
{
    
    setOffsets(0, Wt::Side::Top | Wt::Side::Bottom | Wt::Side::Left | Wt::Side::Right);
    titleBar()->children()[0]->removeFromParent();
    setStyleClass("!border-0 overflow-auto !bg-[#FFF]");
    titleBar()->hide();
    titleBar()->setStyleClass("p-0 flex items-center overflow-x-visible h-[40px]");
    contents()->setStyleClass("h-[100vh] overflow-y-auto overflow-x-visible flex");
    // setModal(true);
    setModal(false);
    setResizable(false);
    setMovable(false);
    
    auto navbar = contents()->addWidget(std::make_unique<Wt::WContainerWidget>());
    auto content_wrapper = contents()->addWidget(std::make_unique<Wt::WStackedWidget>());

    navbar->setStyleClass("flex flex-col h-full border-r border-solid dark:border-[#FFF]/50 stylus-background");
    content_wrapper->setStyleClass("w-screen h-screen stylus-background");

    auto templates_menu_item = navbar->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-xml-logo")));
    auto css_menu_item = navbar->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-css-logo")));
    auto javascript_menu_item = navbar->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-javascript-logo")));
    auto tailwind_menu_item = navbar->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-tailwind-logo")));
    std::string nav_btns_styles = "w-[40px] p-[4px] m-[4px] cursor-pointer rounded-md flex items-center filesManager-menu";

    templates_menu_item->setStyleClass(nav_btns_styles);
    tailwind_menu_item->setStyleClass(nav_btns_styles);
    css_menu_item->setStyleClass(nav_btns_styles);
    javascript_menu_item->setStyleClass(nav_btns_styles);
    
    xml_files_manager_ = content_wrapper->addWidget(std::make_unique<XmlFilesManager>(state_));    
    css_files_manager_ = content_wrapper->addWidget(std::make_unique<CssFilesManager>(state_));
    js_files_manager_ = content_wrapper->addWidget(std::make_unique<JsFilesManager>(state_));
    tailwind_config_ = content_wrapper->addWidget(std::make_unique<TailwindConfigManager>(state_));


    templates_menu_item->clicked().connect(this, [=]() {
        templates_menu_item->toggleStyleClass("filesManager-menu-selected", true);
        tailwind_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        css_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        javascript_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        content_wrapper->setCurrentWidget(xml_files_manager_);
        state_->stylus_node_->SetAttribute("selected-menu", "templates");
        state_->doc_.SaveFile(state_->file_path_.c_str());
    });

    tailwind_menu_item->clicked().connect(this, [=]() {
        templates_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        tailwind_menu_item->toggleStyleClass("filesManager-menu-selected", true);
        css_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        javascript_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        content_wrapper->setCurrentWidget(tailwind_config_);
        state_->stylus_node_->SetAttribute("selected-menu", "tailwind");
        state_->doc_.SaveFile(state_->file_path_.c_str());
    });

    css_menu_item->clicked().connect(this, [=]() {
        templates_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        tailwind_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        css_menu_item->toggleStyleClass("filesManager-menu-selected", true);
        javascript_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        content_wrapper->setCurrentWidget(css_files_manager_);
        state_->stylus_node_->SetAttribute("selected-menu", "css");
        state_->doc_.SaveFile(state_->file_path_.c_str());
    });

    javascript_menu_item->clicked().connect(this, [=]() {
        templates_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        tailwind_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        css_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        javascript_menu_item->toggleStyleClass("filesManager-menu-selected", true);
        content_wrapper->setCurrentWidget(js_files_manager_);
        state_->stylus_node_->SetAttribute("selected-menu", "javascript");
        state_->doc_.SaveFile(state_->file_path_.c_str());
    });

    auto selected_menu = state_->stylus_node_->Attribute("selected-menu");
    if (std::strcmp(selected_menu, "templates") == 0)
        templates_menu_item->clicked().emit(Wt::WMouseEvent());
    else if (std::strcmp(selected_menu, "tailwind") == 0)
        tailwind_menu_item->clicked().emit(Wt::WMouseEvent());
    else if (std::strcmp(selected_menu, "css") == 0)
        css_menu_item->clicked().emit(Wt::WMouseEvent());
    else if (std::strcmp(selected_menu, "javascript") == 0)
        javascript_menu_item->clicked().emit(Wt::WMouseEvent());

    if(state_->stylus_node_->BoolAttribute("open"))
        show();
    else
        hide();

    if(state_->xml_node_->BoolAttribute("navigation-bar-hidden"))
    {
        navbar->hide();
        xml_files_manager_->grid_layout_->itemAt(0)->widget()->hide();
        css_files_manager_->grid_layout_->itemAt(0)->widget()->hide();
        js_files_manager_->grid_layout_->itemAt(0)->widget()->hide();
    }


    Wt::WApplication::instance()->globalKeyWentDown().connect([=](Wt::WKeyEvent e)
    { 
        // if (e.modifiers().test(Wt::KeyboardModifier::Alt) && e.modifiers().test(Wt::KeyboardModifier::Shift))
        if (e.modifiers().test(Wt::KeyboardModifier::Alt))
        {
            if (e.key() == Wt::Key::Q)
            {
                if(isHidden()){
                    show();
                    content_wrapper->setCurrentWidget(content_wrapper->currentWidget());
                    state_->stylus_node_->SetAttribute("open", "true");
                }else{
                    hide();
                    state_->stylus_node_->SetAttribute("open", "false");
                }
                state_->doc_.SaveFile(state_->file_path_.c_str());
            }else if (e.key() == Wt::Key::Key_1){
                templates_menu_item->clicked().emit(Wt::WMouseEvent());
            }else if (e.key() == Wt::Key::Key_2){
                css_menu_item->clicked().emit(Wt::WMouseEvent());
            }else if (e.key() == Wt::Key::Key_3){
                javascript_menu_item->clicked().emit(Wt::WMouseEvent());
            }else if (e.key() == Wt::Key::Key_4){
                tailwind_menu_item->clicked().emit(Wt::WMouseEvent());
            }else if (e.key() == Wt::Key::A){
                if(navbar->isHidden())
                {
                    xml_files_manager_->grid_layout_->itemAt(0)->widget()->show();
                    css_files_manager_->grid_layout_->itemAt(0)->widget()->show();
                    js_files_manager_->grid_layout_->itemAt(0)->widget()->show();
                    navbar->show();
                    state_->xml_node_->SetAttribute("navigation-bar-hidden", "false");
                }else
                {
                    xml_files_manager_->grid_layout_->itemAt(0)->widget()->hide();
                    css_files_manager_->grid_layout_->itemAt(0)->widget()->hide();
                    js_files_manager_->grid_layout_->itemAt(0)->widget()->hide();
                    navbar->hide();
                    state_->xml_node_->SetAttribute("navigation-bar-hidden", "true");
                }
                state_->doc_.SaveFile(state_->file_path_.c_str());
            }
        }
    });

    css_files_manager_->file_saved().connect(this, [=]()
    {
        tailwind_config_->generateCssFile();
    });
    xml_files_manager_->file_saved().connect(this, [=]()
    {
        tailwind_config_->generateCssFile();
    });

}

}
