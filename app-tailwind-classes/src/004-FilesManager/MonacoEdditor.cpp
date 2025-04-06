#include "004-FilesManager/MonacoEdditor.h"
#include <Wt/WText.h>
#include <Wt/WApplication.h>
#include <Wt/WPushButton.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <regex>

MonacoEdditor::MonacoEdditor(std::string language)
    : js_signal_text_changed_(this, "cssEdditorTextChanged")
{
    setStyleClass("flex-1");
    js_signal_text_changed_.connect(this, &MonacoEdditor::cssEdditorTextChanged);
    doJavaScript(R"(require.config({ paths: { 'vs': 'https://unpkg.com/monaco-editor@0.34.1/min/vs' } });)");
    editor_js_var_name_ = language + "_editor";

    // file_path_ = file_path;
    // current_text_ = getFileText(file_path);
    // unsaved_text_ = current_text_;

    std::string initializer = 
    R"(
        require(['vs/editor/editor.main'], function () {
            window.)" + editor_js_var_name_ + R"(_current_text = `)" + current_text_ + R"(`;
            window.)" + editor_js_var_name_ + R"( = monaco.editor.create(document.getElementById(')" + id() + R"('), {
            language: ')" + language + R"(',
            theme: 'vs-dark',
            wordWrap: 'on',
            lineNumbers: 'on',
            tabSize: 4,
            insertSpaces: false,
            detectIndentation: false,
            trimAutoWhitespace: false,
            lineEnding: '\n'
        });

        window.)" + editor_js_var_name_ + R"(.onDidChangeModelContent(function (event) {
            if (window.)" + editor_js_var_name_ + R"(_current_text !== window.)" + editor_js_var_name_ + R"(.getValue()) {
                window.)" + editor_js_var_name_ + R"(_current_text = window.)" + editor_js_var_name_ + R"(.getValue();
                Wt.emit(')" + id() + R"(', 'cssEdditorTextChanged', window.)" + editor_js_var_name_ + R"(.getValue());
            }
        });

        // Override the Ctrl+S command
        window.)" + editor_js_var_name_ + R"(.getDomNode().addEventListener('keydown', function(e) {
            if ((e.ctrlKey || e.metaKey) && e.key === 's') {
                e.preventDefault();
            }
        });
    });
)";

    setJavaScriptMember("cssEdditorTextChanged", initializer);
    // setFile(file_path);
    // setJavaScriptMember("cssEdditorTextChanged", "initializeCssEditor('" + id() + "', '"+current_text_+"');");

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

void MonacoEdditor::saveTextToFile()
{
    // std::cout << "\n\n MonacoEdditor::saveTextToFile()\n\n";
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

void MonacoEdditor::cssEdditorTextChanged(const std::string text)
{
    if(text.compare(unsaved_text_) == 0) return;
    if(text.compare(current_text_) == 0)
    {
        avalable_save_.emit(false);
        unsaved_text_ = text;
        std::cout << "\n\n Text Did Not Change as it was the SAME \n\n";
        return;
    }
    std::cout << "\n\n MonacoEdditor::cssEdditorTextChanged()\n\n";
    avalable_save_.emit(true);
    unsaved_text_ = text;
}

bool MonacoEdditor::unsavedChanges() 
{
    // std::cout << "\n\n MonacoEdditor::unsavedChanges()";
    if(current_text_.compare(unsaved_text_) == 0)
    {
        std::cout << " No unsaved changes \n\n";
        return false;
    }
    std::cout << "unsaved changes \n\n";
    return true;
}


void MonacoEdditor::setFile(std::string file_path)
{
    file_path_ = file_path;
    setCssEdditorText(getFileText(file_path));
}


void MonacoEdditor::setCssEdditorText(std::string text)
{
    doJavaScript(R"(
        setTimeout(function() {
            if (window.)" + editor_js_var_name_ + R"() {
                window.)" + editor_js_var_name_ + R"(_current_text = `)" + text + R"(`;
                window.)" + editor_js_var_name_ + R"(.setValue(`)" + text + R"(`);
            } else {
                console.error("Editor instance is not initialized yet.");
            }
        }, 100);
    )");
    current_text_ = text;
    unsaved_text_ = text;
}

void MonacoEdditor::resetLayout()
{
    doJavaScript("setTimeout(function() { window." + editor_js_var_name_ + ".layout() }, 1);");
}

void MonacoEdditor::setDarkTheme(bool dark)
{
    if(dark)
        doJavaScript("if(monaco.editor) monaco.editor.setTheme('vs-dark');");
    else
        doJavaScript("if(monaco.editor) monaco.editor.setTheme('vs');");
}

std::string MonacoEdditor::getFileText(std::string file_path)
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
