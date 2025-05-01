#pragma once
#include "003-Tailwind/TVariable.h"
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>


class WTVariableText : public Wt::WContainerWidget 
{
    public:
        WTVariableText(std::shared_ptr<TVariableText> tVariable_text);
        void set_read_only();
        private:
        Wt::WPushButton* save_btn_;
        Wt::WPushButton* delete_btn_;
        Wt::WContainerWidget* inputs_wrapper_;
        
        std::shared_ptr<TVariableText> tVariable_text_;

};