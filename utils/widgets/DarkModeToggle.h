#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WCheckBox.h>


class DarkModeToggle : public Wt::WContainerWidget
{
    public:
        DarkModeToggle();
        void setDarkMode();
    private:
        Wt::WCheckBox *checkBox_;

};