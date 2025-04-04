#include "App.h"
#include <Wt/WContainerWidget.h>

App::App(const Wt::WEnvironment &env)
    : WApplication(env),
      session_(appRoot() + "../dbo.db")

{
    setTitle("Starter App");

    // messageResourceBundle().use("../xml-templates/app/test");
    // messageResourceBundle().use("../xml-templates/default/app");
    // messageResourceBundle().use("../xml-templates/default/strings");
    // messageResourceBundle().use("../xml-templates/app/inputs");
    // messageResourceBundle().use("../xml-templates/app/calendar");
    messageResourceBundle().use(appRoot() + "../../xml-templates/default/ui");

    // messageResourceBundle().use("../xml-templates/overide-wt/auth_test");
    // messageResourceBundle().use("../xml-templates/default/auth");
    // messageResourceBundle().use("../xml-templates/default/auth_strings");

    // CSS
    require("https://cdn.tailwindcss.com");

    useStyleSheet("static/css/tailwind-out.css");
    // JSs
    require(docRoot() + "/static/js/utils.js");

    root()->setStyleClass("flex !max-w-[100vw] m-0 p-0 flex");


    
}
