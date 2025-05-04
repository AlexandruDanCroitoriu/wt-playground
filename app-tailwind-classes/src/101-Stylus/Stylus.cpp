#include "101-Stylus/Stylus.h"
#include <Wt/WStackedWidget.h>
#include <Wt/WPushButton.h>
#include <Wt/WTemplate.h>
#include <Wt/WApplication.h>
#include <Wt/WStackedWidget.h>
#include <fstream>
#include <Wt/WApplication.h>
#include <Wt/WServer.h>
#include <Wt/WIOService.h>
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
    
    Wt::WApplication::instance()->useStyleSheet(Wt::WApplication::instance()->docRoot() + "/static/stylus/questionmark.css");
    // Wt::WApplication::instance()->declareJavaScriptFunction("dsblCtrlS", R"(
    //     function (inputId) {
    //       const inputElement = document.getElementById(inputId);
    //       if (!inputElement) {
    //         console.warn(`Element with id "${inputId}" not found.`);
    //         return;
    //       }

    //       inputElement.addEventListener('keydown', function(e) {
    //         console.log(e);
    //         if (e.ctrlKey && (e.key === 's' || e.key === 'S')) {
    //           e.preventDefault();
    //         }
    //       });
    //     }
    // )");


    auto navbar = contents()->addWidget(std::make_unique<Wt::WContainerWidget>());
    auto content_wrapper = contents()->addWidget(std::make_unique<Wt::WStackedWidget>());

    navbar->setStyleClass("flex flex-col h-full border-r border-solid");
    content_wrapper->setStyleClass("flex-1 h-full overflow-y-auto");

    auto templates_menu_item = navbar->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-xml-logo")));
    auto css_menu_item = navbar->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-css-logo")));
    auto javascript_menu_item = navbar->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-javascript-logo")));
    auto tailwind_menu_item = navbar->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-tailwind-logo")));
    std::string nav_btns_styles = "w-[30px] p-[10px] m-[4px] cursor-pointer rounded-md flex items-center filesManager-menu";

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
        xml_files_manager_->editor_->resetLayout();
        // xml_files_manager_->editor_->reuploadText();
        state_->stylus_node_->SetAttribute("selected-menu", "templates");
        state_->doc_.SaveFile(state_->file_path_.c_str());
    });

    tailwind_menu_item->clicked().connect(this, [=]() {
        templates_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        tailwind_menu_item->toggleStyleClass("filesManager-menu-selected", true);
        css_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        javascript_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        content_wrapper->setCurrentWidget(tailwind_config_);
        tailwind_config_->editor_->resetLayout();
        state_->stylus_node_->SetAttribute("selected-menu", "tailwind");
        state_->doc_.SaveFile(state_->file_path_.c_str());
    });

    css_menu_item->clicked().connect(this, [=]() {
        templates_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        tailwind_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        css_menu_item->toggleStyleClass("filesManager-menu-selected", true);
        javascript_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        content_wrapper->setCurrentWidget(css_files_manager_);
        css_files_manager_->editor_->resetLayout();
        // css_files_manager_->editor_->reuploadText();
        state_->stylus_node_->SetAttribute("selected-menu", "css");
        state_->doc_.SaveFile(state_->file_path_.c_str());
    });

    javascript_menu_item->clicked().connect(this, [=]() {
        templates_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        tailwind_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        css_menu_item->toggleStyleClass("filesManager-menu-selected", false);
        javascript_menu_item->toggleStyleClass("filesManager-menu-selected", true);
        content_wrapper->setCurrentWidget(js_files_manager_);
        js_files_manager_->editor_->resetLayout();
        // js_files_manager_->editor_->reuploadText();
        state_->stylus_node_->SetAttribute("selected-menu", "javascript");
        state_->doc_.SaveFile(state_->file_path_.c_str());
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
        xml_files_manager_->layout_->itemAt(0)->widget()->hide();
    }else{
        navbar->show();
        xml_files_manager_->layout_->itemAt(0)->widget()->show();
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
            }else if (e.key() == Wt::Key::A && content_wrapper->currentWidget() == xml_files_manager_){
                if(xml_files_manager_->layout_->itemAt(0)->widget()->isHidden())
                {
                    xml_files_manager_->layout_->itemAt(0)->widget()->show();
                    navbar->show();
                    state_->xml_node_->SetAttribute("navigation-bar-hidden", "false");
                }else
                {
                    xml_files_manager_->layout_->itemAt(0)->widget()->hide();
                    navbar->hide();
                    state_->xml_node_->SetAttribute("navigation-bar-hidden", "true");
                }
                state_->doc_.SaveFile(state_->file_path_.c_str());
            }
        }
    });



    css_files_manager_->file_saved().connect(this, [=]()
    {
        generateCssFile();
    });
    xml_files_manager_->file_saved().connect(this, [=]()
    {
        generateCssFile();
    });
    generateCssFile();

}




    void Stylus::generateCssFile()
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
        for(auto folder : css_files_manager_->folders_)
        {
            for (auto file_name : folder.second)
            {
                file << "@import \"./css/" << folder.first << "/" << file_name << "\";\n";
            }
        }
        file << "\n";
        file << "/* Source additional templates and styles */\n";
        file << "@source \"../xml-templates/\";\n";
        file << "@source \"../../src/\";\n\n";

        file << "/* Define custom variants */\n";
        file << "@custom-variant dark (&:where(.dark, .dark *));\n\n";

        file << "/* Define custom theme */\n";
        file << tailwind_config_->getConfig() << "\n\n";

        // std::cout << "\n\nFile written successfully: " << "../stylus-resources/tailwind4/input.css\n\n";
        file.close();

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
