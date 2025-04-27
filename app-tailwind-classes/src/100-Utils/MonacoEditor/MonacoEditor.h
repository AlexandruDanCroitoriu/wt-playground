#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WJavaScript.h>
#include <Wt/WStringStream.h>
#include <Wt/WSignal.h>

class MonacoEditor : public Wt::WContainerWidget
{
    public:
        MonacoEditor(std::string language);
        void setDarkTheme(bool dark);
        void resetLayout();
        void setFile(std::string file_path);
        Wt::Signal<bool>& avalable_save() { return avalable_save_; }
        bool unsavedChanges();
        Wt::Signal<std::string>& save_file_signal() { return save_file_signal_; }
        std::string getUnsavedText() { return unsaved_text_; }
        void textSaved();
        void reuploadText() { setCssEdditorText(current_text_); }
        private:
        std::string getFileText(std::string file_path);
        void setCssEdditorText(std::string text);
        void cssEdditorTextChanged(const std::string text);
    
        Wt::JSignal<std::string> js_signal_text_changed_;
        Wt::Signal<bool> avalable_save_;
        Wt::Signal<std::string> save_file_signal_;
    
        std::string current_text_;
        std::string unsaved_text_;
        std::string editor_js_var_name_;
        
    protected:
        // Custom implementation
        void layoutSizeChanged(int width, int height) override;      
};
