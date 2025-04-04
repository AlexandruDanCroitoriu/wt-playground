#pragma once
#include <Wt/WApplication.h>
#include "002-Dbo/Session.h"
#include "003-Tailwind/WTConfig.h"
#include <Wt/WDoubleSpinBox.h>

class App : public Wt::WApplication
{
public:
    App(const Wt::WEnvironment &env);

    // Session session_;

private:
    WTConfig* tailwind_config_;
    void updateTailwindCssFile();
    Wt::WDoubleSpinBox *test_input;
};