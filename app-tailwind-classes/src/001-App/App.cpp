#include "App.h"
#include "003-Tailwind/WTVariable.h"
#include <Wt/WRandom.h>
#include <Wt/WServer.h>
#include <Wt/WIOService.h>
#include <Wt/WString.h>
#include <Wt/WText.h>
#include <Wt/WLineEdit.h>
#include <Wt/WCheckBox.h>
#include "010-TestWidgets/DarkModeToggle.h"
#include "010-TestWidgets/Test.h"
#include <Wt/WTemplate.h>

App::App(const Wt::WEnvironment &env)
    : WApplication(env)
    //   session_(appRoot() + "../dbo.db"),
{
    messageResourceBundle().use(appRoot() + "../templates");

    // JSs
    require(docRoot() + "/static/js/utils.js");
    require(docRoot() + "/static/js/monaco-edditor.js");
    require(docRoot() + "/static/js/monaco-css-edditor.js");
    require("https://cdn.jsdelivr.net/npm/@tailwindcss/browser@4");
    
    // CSS
    useStyleSheet("static/css/questionmark.css");
    // require("https://unpkg.com/monaco-editor@0.34.1/min/vs/loader.js");
    
    // Settings
    enableUpdates(true);
    
    // Title
    setTitle("Starter App");


    tailwind_config_center_ = root()->addChild(std::make_unique<TailwindConfigCenter>());
    tailwind_config_center_->show();
    
    auto dark_mode_toggle = root()->addWidget(std::make_unique<DarkModeToggle>());
    root()->setStyleClass("flex flex-col items-start w-[100vw] h-[100vh] m-0 dark:bg-gray-900 transition duration-300 ease");
    
   
    
    Wt::WApplication::instance()->setHtmlClass("dark");
    dark_mode_toggle->dark_mode_changed_.connect(this, [=] (bool dark) {
        if(dark){
            Wt::WApplication::instance()->setHtmlClass("dark");
            tailwind_config_center_->css_files_manager_->css_editor_->setDarkTheme(true);
        }
        else{
            Wt::WApplication::instance()->setHtmlClass("");
            tailwind_config_center_->css_files_manager_->css_editor_->setDarkTheme(false);
        }
    });


    globalKeyWentDown().connect([=](Wt::WKeyEvent e)
    { 
        if (e.modifiers().test(Wt::KeyboardModifier::Alt))
        {
            if (e.key() == Wt::Key::Q)
            {
                if(tailwind_config_center_->isHidden())
                {
                    tailwind_config_center_->show();
                }
                else
                {
                    tailwind_config_center_->hide();
                }
            }
        }
    });

}
