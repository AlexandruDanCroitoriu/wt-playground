#pragma once
#include <Wt/WApplication.h>
#include "002-Dbo/Session.h"

#include "005-TailwindConfigCenter/TailwindConfigCenter.h"

class App : public Wt::WApplication
{
public:
    App(const Wt::WEnvironment &env);

    // Session session_;

    private:
    TailwindConfigCenter* tailwind_config_center_;

};