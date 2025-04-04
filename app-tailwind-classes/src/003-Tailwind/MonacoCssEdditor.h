#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WJavaScript.h>
#include <Wt/WStringStream.h>

class MonacoCssEdditor : public Wt::WContainerWidget
{
    public:
        MonacoCssEdditor();
        void setDarkTheme(bool dark);
        void setCssEdditorText(std::string text);
    private:
        Wt::JSignal<std::string> js_signal_text_changed_;
        void cssEdditorTextChanged(const std::string text);
};
