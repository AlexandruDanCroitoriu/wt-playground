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

App::App(const Wt::WEnvironment &env)
    : WApplication(env)
//   session_(appRoot() + "../dbo.db"),
{
    // messageResourceBundle().use(docRoot() + "/templates");
    messageResourceBundle().use(docRoot() + "/stylus-resources/xml-templates/stylus/svg");

    // JSs
    // require(docRoot() + "/stylus-resources/js/experiments/console.js?v=" + Wt::WRandom::generateId());
    require(docRoot() + "/static/monaco-edditor.js");
    require("https://cdn.jsdelivr.net/npm/@tailwindcss/browser@4");
    // require("https://unpkg.com/monaco-editor@0.34.1/min/vs/loader.js");

    // CSS
    // useStyleSheet("static/css/questionmark.css");
    // useStyleSheet("../static/tailwind.css?v=" + Wt::WRandom::generateId());

    // Settings
    enableUpdates(true);

    declareJavaScriptFunction("dsblCtrlS", R"(
        function (id) {
          const inputElement = document.getElementById(id);
          if (!inputElement) {
            console.warn(`Element with id "${id}" not found.`);
            return;
          }

          inputElement.addEventListener('keydown', function(e) {
            console.log(e);
            if (e.ctrlKey && (e.key === 's' || e.key === 'S')) {
              e.preventDefault();
            }
          });
        }
    )");

    // Title
    setTitle("Starter App");

    // tailwind_config_center_ = root()->addChild(std::make_unique<TailwindConfigCenter>());
    // tailwind_config_center_->show();

    stylus_ = root()->addChild(std::make_unique<Stylus::Stylus>());

    // auto dark_mode_toggle = root()->addWidget(std::make_unique<DarkModeToggle>());
    // stylus_->xml_files_manager_->dark_mode_toggle_->dark_mode_changed_.connect(this, [=](bool dark){
    //     dark_mode_toggle->setDarkMode(dark);
    // });
    // dark_mode_toggle->dark_mode_changed_.connect(this, [=](bool dark){
    //     stylus_->xml_files_manager_->dark_mode_toggle_->setDarkMode(dark);
    // });

    root()->setStyleClass("flex flex-col items-start w-[100vw] h-[100vh] m-0 dark:bg-gray-900 transition duration-300 ease overflow-hidden");

    auto test_template = root()->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("app-root")));

    std::unique_ptr<WHBoxLayout> vbox = std::make_unique<WHBoxLayout>();
    auto w1 = std::make_unique<Wt::WContainerWidget>();
    w1->setStyleClass("p-12 m-1 bg-red-300 border");
    w1->addWidget(std::make_unique<Wt::WText>("widget 1"));
    auto w2 = std::make_unique<Wt::WContainerWidget>();
    w2->setStyleClass("p-12 m-1 bg-blue-300 border");
    w2->addWidget(std::make_unique<Wt::WText>("widget 2"));

    vbox->addWidget(std::move(w1), 1);
    vbox->addWidget(std::move(w2), 1);
    vbox->setResizable(0);

    auto test_containter = root()->addWidget(std::make_unique<Wt::WContainerWidget>());
    test_containter->setStyleClass("m-2 p-12 bg-green-300 border border-solid border-gray-800 rounded-md shadow-md w-full h-full");
    test_containter->setLayout(std::move(vbox));

    // stylus_->brain_->generateCssFile();

}
