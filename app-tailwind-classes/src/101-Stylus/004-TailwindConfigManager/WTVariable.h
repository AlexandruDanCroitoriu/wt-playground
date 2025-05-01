#pragma once
#include "101-Stylus/004-TailwindConfigManager/TVariable.h"
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>

namespace Stylus
{

    class WTVariable : public Wt::WContainerWidget
    {
    public:
        WTVariable(std::shared_ptr<TVariable> tVariable);
        void set_read_only();

    private:
        Wt::WPushButton *save_btn_;
        Wt::WPushButton *delete_btn_;
        Wt::WContainerWidget *inputs_wrapper_;

        std::shared_ptr<TVariable> tVariable_;
    };

}
