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
#include "003-Tailwind/CssFilesManager.h"

App::App(const Wt::WEnvironment &env)
    : WApplication(env)
    //   session_(appRoot() + "../dbo.db"),
{
  
    // JSs
    require(docRoot() + "/static/js/utils.js");
    require(docRoot() + "/static/js/monaco-edditor.js");
    require(docRoot() + "/static/js/monaco-css-edditor.js");
    
    // CSS
    require("https://cdn.jsdelivr.net/npm/@tailwindcss/browser@4");
    // require("https://unpkg.com/monaco-editor@0.34.1/min/vs/loader.js");
    
    
    // Title
    setTitle("Starter App");
    auto dark_mode_toggle = root()->addWidget(std::make_unique<DarkModeToggle>());
    root()->setStyleClass("flex flex-col items-start !max-w-[100vw] m-0 p-12 dark:bg-gray-900 transition duration-300 ease");
    
    enableUpdates(true);
    auto btn = root()->addWidget(std::make_unique<Wt::WPushButton>("Test"));
    btn->setStyleClass("btn-default inline-block");
    auto css_files_manager = root()->addWidget(std::make_unique<CssFilesManager>());
    btn->clicked().connect(this, [=](){
        css_files_manager->css_editor_->setCssEdditorText("body { background-color: red; }");
    });


    tailwind_config_ = root()->addWidget(std::make_unique<WTConfig>());

    Wt::WApplication::instance()->setHtmlClass("dark");
    dark_mode_toggle->dark_mode_changed_.connect(this, [=] (bool dark) {
        if(dark){
            Wt::WApplication::instance()->setHtmlClass("dark");
            css_files_manager->css_editor_->setDarkTheme(true);
        }
        else{
            Wt::WApplication::instance()->setHtmlClass("");
            css_files_manager->css_editor_->setDarkTheme(false);
        }
    });
}
