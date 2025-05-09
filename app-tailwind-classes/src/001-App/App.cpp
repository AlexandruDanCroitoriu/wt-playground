#include "App.h"
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
#include <Wt/WHBoxLayout.h>
#include <Wt/WPanel.h>
#include "100-Utils/MonacoEditor/MonacoEditor.h"

App::App(const Wt::WEnvironment &env)
    : WApplication(env)
//   session_(appRoot() + "../dbo.db"),
{
    // messageResourceBundle().use(docRoot() + "/templates");
    // messageResourceBundle().use(docRoot() + "/stylus-resources/xml-templates/stylus/svg");
    messageResourceBundle().use(docRoot() + "/templates");

    // JSs
    // require(docRoot() + "/stylus-resources/js/experiments/console.js?v=" + Wt::WRandom::generateId());
    require(docRoot() + "/static/monaco-edditor.js");
    require("https://cdn.jsdelivr.net/npm/@tailwindcss/browser@4");
    // require("https://unpkg.com/monaco-editor@0.34.1/min/vs/loader.js");
    
    // CSS
    // useStyleSheet("static/css/questionmark.css");
    // useStyleSheet(Wt::WApplication::instance()->docRoot() + "/static/stylus/questionmark.css");
    // useStyleSheet("../static/tailwind.css?v=" + Wt::WRandom::generateId());

    // Settings
    enableUpdates(true);

    // declareJavaScriptFunction("dsblCtrlS", R"(
    //     function (id) {
    //       const inputElement = document.getElementById(id);
    //       if (!inputElement) {
    //         console.warn(`Element with id "${id}" not found.`);
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

    // Title
    setTitle("Starter App");

    // auto test_editor = root()->addWidget(std::make_unique<MonacoEditor>("css"));
    // test_editor->setEditorText("static/tailwind.css");

    // tailwind_config_center_ = root()->addChild(std::make_unique<TailwindConfigCenter>());
    // tailwind_config_center_->show();

  
    auto dark_mode_toggle = root()->addWidget(std::make_unique<DarkModeToggle>());

    root()->setStyleClass("flex flex-col items-start w-[100vw] h-[100vh] m-0 dark:bg-gray-900 transition duration-300 ease overflow-hidden");

    stylus_ = root()->addChild(std::make_unique<Stylus::Stylus>());
    // stylus_->show();

    // stylus_->brain_->generateCssFile();

}
