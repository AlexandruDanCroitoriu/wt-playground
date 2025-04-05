#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WJavaScript.h>
#include <Wt/WStringStream.h>
#include <Wt/WSignal.h>

class MonacoCssEdditor : public Wt::WContainerWidget
{
    public:
        MonacoCssEdditor(std::string file_path);
        void setDarkTheme(bool dark);
        void resetLayout();
        void saveTextToFile();
        void setFile(std::string file_path);
        Wt::Signal<bool>& avalable_save() { return avalable_save_; }
        bool unsavedChanges();
        
        private:
        std::string getFileText(std::string file_path);
        void setCssEdditorText(std::string text);
        void cssEdditorTextChanged(const std::string text);
    
        Wt::JSignal<std::string> js_signal_text_changed_;
        Wt::Signal<bool> avalable_save_;
    
        std::string file_path_;
        std::string current_text_;
        std::string unsaved_text_;
        
};
