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
    setStyleClass("!border-0 overflow-auto !bg-[#FFF]");
    titleBar()->hide();
    titleBar()->setStyleClass("p-0 flex items-center overflow-x-visible h-[40px]");
    contents()->setStyleClass("h-[100vh] overflow-y-auto overflow-x-visible flex");
    // setModal(true);
    setModal(false);
    setResizable(false);
    setMovable(false);
    
    Wt::WApplication::instance()->useStyleSheet(Wt::WApplication::instance()->docRoot() + "/static/stylus/questionmark.css");

    auto navbar = contents()->addWidget(std::make_unique<Wt::WContainerWidget>());
    auto content_wrapper = contents()->addWidget(std::make_unique<Wt::WStackedWidget>());

    navbar->setStyleClass("flex flex-col h-full border-r border-solid");
    content_wrapper->setStyleClass("flex-1 h-full overflow-y-auto");

    auto templates_menu_item = navbar->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-xml-logo")));
    auto css_menu_item = navbar->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-css-logo")));
    auto javascript_menu_item = navbar->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-javascript-logo")));
    auto tailwind_menu_item = navbar->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-tailwind-logo")));
    std::string nav_btns_styles = "w-[50px] p-[10px] cursor-pointer flex items-center filesManager-menu";

    templates_menu_item->setStyleClass(nav_btns_styles);
    tailwind_menu_item->setStyleClass(nav_btns_styles);
    css_menu_item->setStyleClass(nav_btns_styles);
    javascript_menu_item->setStyleClass(nav_btns_styles);
    
    xml_files_manager_ = content_wrapper->addWidget(std::make_unique<XmlFilesManager>(brain_));    
    css_files_manager_ = content_wrapper->addWidget(std::make_unique<CssFilesManager>(brain_));
    js_files_manager_ = content_wrapper->addWidget(std::make_unique<JsFilesManager>(brain_));
    tailwind_config_ = content_wrapper->addWidget(std::make_unique<TailwindConfigManager>(brain_));

    brain_->generateCssFile();

    templates_menu_item->clicked().connect(this, [=]() {
        templates_menu_item->toggleStyleClass("filesManager-menu-selected", true);
        tailwind_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        css_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        javascript_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        content_wrapper->setCurrentWidget(xml_files_manager_);
        xml_files_manager_->editor_->resetLayout();
        // xml_files_manager_->editor_->reuploadText();
        brain_->state_.stylus_open_node_->SetAttribute("selected-menu", "templates");
        brain_->state_.doc_.SaveFile(brain_->state_.file_path_.c_str());
    });

    tailwind_menu_item->clicked().connect(this, [=]() {
        templates_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        tailwind_menu_item->toggleStyleClass("filesManager-menu-selected", true);
        css_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        javascript_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        content_wrapper->setCurrentWidget(tailwind_config_);
        tailwind_config_->editor_->resetLayout();
        brain_->state_.stylus_open_node_->SetAttribute("selected-menu", "tailwind");
        brain_->state_.doc_.SaveFile(brain_->state_.file_path_.c_str());
    });

    css_menu_item->clicked().connect(this, [=]() {
        templates_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        tailwind_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        css_menu_item->toggleStyleClass("filesManager-menu-selected", true);
        javascript_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        content_wrapper->setCurrentWidget(css_files_manager_);
        css_files_manager_->editor_->resetLayout();
        // css_files_manager_->editor_->reuploadText();
        brain_->state_.stylus_open_node_->SetAttribute("selected-menu", "css");
        brain_->state_.doc_.SaveFile(brain_->state_.file_path_.c_str());
    });

    javascript_menu_item->clicked().connect(this, [=]() {
        templates_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        tailwind_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        css_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        javascript_menu_item->toggleStyleClass("filesManager-menu-selected", true);
        content_wrapper->setCurrentWidget(js_files_manager_);
        js_files_manager_->editor_->resetLayout();
        // js_files_manager_->editor_->reuploadText();
        brain_->state_.stylus_open_node_->SetAttribute("selected-menu", "javascript");
        brain_->state_.doc_.SaveFile(brain_->state_.file_path_.c_str());
    });

    content_wrapper->currentWidgetChanged().connect([=]() {
        if(content_wrapper->currentIndex() == 0){
            xml_files_manager_->editor_->resetLayout();
        }else if(content_wrapper->currentIndex() == 2){
            css_files_manager_->editor_->resetLayout();
        }else if(content_wrapper->currentIndex() == 3){
            js_files_manager_->editor_->resetLayout();
        }
    });


    auto selected_menu = brain_->state_.stylus_open_node_->Attribute("selected-menu");
    if (std::strcmp(selected_menu, "templates") == 0)
        templates_menu_item->clicked().emit(Wt::WMouseEvent());
    else if (std::strcmp(selected_menu, "tailwind") == 0)
        tailwind_menu_item->clicked().emit(Wt::WMouseEvent());
    else if (std::strcmp(selected_menu, "css") == 0)
        css_menu_item->clicked().emit(Wt::WMouseEvent());
    else if (std::strcmp(selected_menu, "javascript") == 0)
        javascript_menu_item->clicked().emit(Wt::WMouseEvent());

    if(std::strcmp(brain_->state_.stylus_open_node_->Attribute("open"), "true") == 0)
        show();
    else
        hide();


    Wt::WApplication::instance()->globalKeyWentDown().connect([=](Wt::WKeyEvent e)
    { 
        if (e.modifiers().test(Wt::KeyboardModifier::Alt))
        {
            if (e.key() == Wt::Key::Q)
            {
                if(isHidden()){
                    show();
                    brain_->state_.stylus_open_node_->SetAttribute("open", "true");
                }else{
                    hide();
                    brain_->state_.stylus_open_node_->SetAttribute("open", "false");
                }
                brain_->state_.doc_.SaveFile(brain_->state_.file_path_.c_str());
            }else if (e.key() == Wt::Key::Key_1){
                templates_menu_item->clicked().emit(Wt::WMouseEvent());
            }else if (e.key() == Wt::Key::Key_2){
                css_menu_item->clicked().emit(Wt::WMouseEvent());
            }else if (e.key() == Wt::Key::Key_3){
                javascript_menu_item->clicked().emit(Wt::WMouseEvent());
            }else if (e.key() == Wt::Key::Key_4){
                tailwind_menu_item->clicked().emit(Wt::WMouseEvent());
            }else if (e.key() == Wt::Key::A){
                if(xml_files_manager_->layout_->itemAt(0)->widget()->isHidden())
                {
                    xml_files_manager_->layout_->itemAt(0)->widget()->show();
                }else
                {
                    xml_files_manager_->layout_->itemAt(0)->widget()->hide();
                }
            }
        }
    });



}

}
