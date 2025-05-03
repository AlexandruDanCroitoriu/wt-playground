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
        void setEditorReadOnly(bool read_only);

        bool unsavedChanges();
        std::string getUnsavedText() { return unsaved_text_; }
        void textSaved();
        void reuploadText() { setEditorText(current_text_); }
        
        Wt::Signal<std::string>& save_file_signal() { return save_file_signal_; }
        Wt::Signal<bool>& avalable_save() { return avalable_save_; }
        Wt::Signal<Wt::WString> width_changed_;
        
    private:
        std::string getFileText(std::string file_path);
        void setEditorText(std::string text);
        void editorTextChanged(std::string text);
    
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
