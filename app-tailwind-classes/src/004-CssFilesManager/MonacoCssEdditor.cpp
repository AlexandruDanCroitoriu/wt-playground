#include "004-CssFilesManager/MonacoCssEdditor.h"
#include <Wt/WText.h>
#include <Wt/WApplication.h>
#include <Wt/WPushButton.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <regex>

MonacoCssEdditor::MonacoCssEdditor(std::string file_path)
    : js_signal_text_changed_(this, "cssEdditorTextChanged")
{
    setStyleClass("flex-1");
    js_signal_text_changed_.connect(this, &MonacoCssEdditor::cssEdditorTextChanged);

    
    current_text_ = getFileText(file_path);
    unsaved_text_ = current_text_;
    file_path_ = file_path;
    std::string member = "initializeCssEditor('" + id() + "', '"+current_text_+"');";
    setJavaScriptMember("cssEdditorTextChanged", member);

    keyWentDown().connect([=](Wt::WKeyEvent e)
    { 
        if (e.modifiers().test(Wt::KeyboardModifier::Control))
        {
            if (e.key() == Wt::Key::S)
            {
                saveTextToFile();
            }
        }
    });
}

void MonacoCssEdditor::saveTextToFile()
{
    // std::cout << "\n\n MonacoCssEdditor::saveTextToFile()\n\n";
    std::ofstream file(file_path_);
    if (!file.is_open()) {
        std::cout << "\n\n Failed to open file: " << file_path_ << "\n\n";
        return;
    }
    file << unsaved_text_;
    file.close();
    current_text_ = unsaved_text_;
    avalable_save_.emit(false);
}

void MonacoCssEdditor::cssEdditorTextChanged(const std::string text)
{
    if(text.compare(unsaved_text_) == 0) return;
    if(text.compare(current_text_) == 0)
    {
        avalable_save_.emit(false);
        unsaved_text_ = text;
        std::cout << "\n\n Text Did Not Change as it was the SAME \n\n";
        return;
    }
    std::cout << "\n\n MonacoCssEdditor::cssEdditorTextChanged()\n\n";
    avalable_save_.emit(true);
    unsaved_text_ = text;
}

bool MonacoCssEdditor::unsavedChanges() 
{
    // std::cout << "\n\n MonacoCssEdditor::unsavedChanges()";
    if(current_text_.compare(unsaved_text_) == 0)
    {
        std::cout << " No unsaved changes \n\n";
        return false;
    }
    std::cout << "unsaved changes \n\n";
    return true;
}


void MonacoCssEdditor::setFile(std::string file_path)
{
    file_path_ = file_path;
    setCssEdditorText(getFileText(file_path));
}


void MonacoCssEdditor::setCssEdditorText(std::string text)
{
    doJavaScript("updateCssEditorValue('" + text + "');");
    current_text_ = text;
    unsaved_text_ = text;
}

void MonacoCssEdditor::resetLayout()
{
    doJavaScript("setTimeout(function() { edditor_css.layout() }, 1);");
}

void MonacoCssEdditor::setDarkTheme(bool dark)
{
    if(dark)
        doJavaScript("if(monaco.editor) monaco.editor.setTheme('vs-dark');");
    else
        doJavaScript("if(monaco.editor) monaco.editor.setTheme('vs');");
}

std::string MonacoCssEdditor::getFileText(std::string file_path)
{
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cout << "\n\n Failed to open file: " << file_path << "\n\n";
        return "";
    }

    std::string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    Wt::WString file_content_wt = Wt::WString::fromUTF8(file_content);
    file_content = std::regex_replace(file_content, std::regex("\n"), "\\n");
    file_content = std::regex_replace(file_content, std::regex("\r"), "\\r");
    file_content = std::regex_replace(file_content, std::regex("\t"), "\\t");
    return file_content;
}
