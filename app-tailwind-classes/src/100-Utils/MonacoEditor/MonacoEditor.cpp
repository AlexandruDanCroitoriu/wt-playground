#include "100-Utils/MonacoEditor/MonacoEditor.h"
#include <Wt/WText.h>
#include <Wt/WApplication.h>
#include <Wt/WPushButton.h>
#include <Wt/WRandom.h>

#include <filesystem>
#include <iostream>
#include <fstream>
#include <regex>


MonacoEditor::MonacoEditor(std::string language)
    : js_signal_text_changed_(this, "editorTextChanged"),
        unsaved_text_(""),
        current_text_("")
{
    setLayoutSizeAware(true);
    setMinimumSize(Wt::WLength(240, Wt::LengthUnit::Pixel), Wt::WLength(100, Wt::LengthUnit::ViewportHeight));
    // setMinimumSize(Wt::WLength(240, Wt::LengthUnit::Pixel), Wt::WLength(100, Wt::LengthUnit::Pixel));
    setMaximumSize(Wt::WLength::Auto, Wt::WLength(100, Wt::LengthUnit::ViewportHeight));
    
    js_signal_text_changed_.connect(this, &MonacoEditor::editorTextChanged);
    doJavaScript(R"(require.config({ paths: { 'vs': 'https://unpkg.com/monaco-editor@0.34.1/min/vs' } });)");
    editor_js_var_name_ = language + Wt::WRandom::generateId() + "_editor";

    std::string initializer =
            R"(
        require(['vs/editor/editor.main'], function () {
                window.)" + editor_js_var_name_ + R"(_current_text = `)" + current_text_ + R"(`;
                window.)" + editor_js_var_name_ + R"( = monaco.editor.create(document.getElementById(')" + id() + R"('), {
                language: ')" + language + R"(',
                theme: 'vs-light',
                wordWrap: 'off',
                lineNumbers: 'on',
                tabSize: 2,
                insertSpaces: false,
                detectIndentation: false,
                trimAutoWhitespace: false,
                lineEnding: '\n',
                minimap: { enabled: false },
            });

            window.)" + editor_js_var_name_ + R"(.onDidChangeModelContent(function (event) {
                if (window.)" + editor_js_var_name_ + R"(_current_text !== window.)" + editor_js_var_name_ + R"(.getValue()) {
                    window.)" + editor_js_var_name_ + R"(_current_text = window.)" + editor_js_var_name_ + R"(.getValue();
                    Wt.emit(')" + id() + R"(', 'editorTextChanged', window.)" + editor_js_var_name_ + R"(.getValue());
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

    setJavaScriptMember("editorTextChanged", initializer);
    // setFile(file_path);
    // setJavaScriptMember("editorTextChanged", "initializeEditor('" + id() + "', '"+current_text_+"');");

    keyWentDown().connect([=](Wt::WKeyEvent e)
                            { 
    Wt::WApplication::instance()->globalKeyWentDown().emit(e); // Emit the global key event
    if (e.modifiers().test(Wt::KeyboardModifier::Control))
    {
        if (e.key() == Wt::Key::S)
        {
            // textSaved();
            std::cout << "\n\n MonacoEditor::keyWentDown() Ctrl+S pressed \n\n";
            save_file_signal_.emit(unsaved_text_);
        }
    } });
}

void MonacoEditor::layoutSizeChanged(int width, int height)
{
    // resetLayout(); // This is not needed as it is already called in setEditorText
    setEditorText(unsaved_text_); // This is not needed as some times the text is not set when the app server serves the first app instance
    if(width >= 240) {
        width_changed_.emit(Wt::WString(std::to_string(width)));
    }
}


void MonacoEditor::editorTextChanged(std::string text)
{
    std::cout << "\n\n redived text: " << "\n\n";
    if (text.compare(unsaved_text_) == 0)
        return;
    if (text.compare(current_text_) == 0)
    {
        avalable_save_.emit(false);
        unsaved_text_ = text;
        std::cout << "\n\n Text Did Not Change as it was the SAME \n\n";
        return;
    }
    std::cout << "\n\n MonacoEditor::editorTextChanged()\n\n";
    unsaved_text_ = text;
    avalable_save_.emit(true);
}

void MonacoEditor::textSaved()
{
    current_text_ = unsaved_text_;
    avalable_save_.emit(false);
}

void MonacoEditor::setEditorReadOnly(bool read_only) { 
    doJavaScript("setTimeout(function() { if(window." + editor_js_var_name_ + ") window." + editor_js_var_name_ + ".updateOptions({ readOnly: " + std::to_string(read_only) + " }); }, 200);");
}

bool MonacoEditor::unsavedChanges()
{
    // std::cout << "\n\n MonacoEditor::unsavedChanges()";
    if (current_text_.compare(unsaved_text_) == 0)
    {
        std::cout << " No unsaved changes \n\n";
        return false;
    }
    std::cout << "unsaved changes \n\n";
    return true;
}

void MonacoEditor::setFile(std::string file_path)
{
    setEditorText(getFileText(file_path));
}

void MonacoEditor::setEditorText(std::string text)
{
    resetLayout();
    doJavaScript(
    R"(
        setTimeout(function() {
            if (window.)" + editor_js_var_name_ + R"() {
                window.)" + editor_js_var_name_ + R"(_current_text = `)" + text + R"(`;
                window.)" + editor_js_var_name_ + R"(.setValue(`)" + text + R"(`);
            } else {
                console.error("Editor instance is not initialized yet.");
                setTimeout(function() {
                    if (window.)" + editor_js_var_name_ + R"() {
                        window.)" + editor_js_var_name_ + R"(_current_text = `)" + text + R"(`;
                        window.)" + editor_js_var_name_ + R"(.setValue(`)" + text + R"(`);
                    } else {
                        console.error("Editor instance is still not initialized.");
                    }
                }, 1000);
            }
        }, 100);
    )");
    current_text_ = text;
    unsaved_text_ = text;
}

void MonacoEditor::resetLayout()
{
    doJavaScript("setTimeout(function() { window." + editor_js_var_name_ + ".layout() }, 1);");
}
void MonacoEditor::setDarkTheme(bool dark)
{
    doJavaScript(R"(
        (function() {
            var interval = setInterval(function() {
                if (window.)" + editor_js_var_name_ + R"() {
                    clearInterval(interval);
                    if ()" + (dark ? "true" : "false") + R"() {
                        window.)" + editor_js_var_name_ + R"(.updateOptions({ theme: 'vs-dark' });
                    } else {
                        window.)" + editor_js_var_name_ + R"(.updateOptions({ theme: 'vs' });
                    }
                }
            }, 100);
        })();
    )");
}

std::string MonacoEditor::getFileText(std::string file_path)
{
    std::ifstream file(file_path);
    if (!file.is_open())
    {
        std::cout << "\n\n Failed to read file: " << file_path << "\n\n";
        return "";
    }

    std::string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    Wt::WString file_content_wt = Wt::WString::fromUTF8(file_content);
    // file_content = std::regex_replace(file_content, std::regex("\n"), "\\n");
    // file_content = std::regex_replace(file_content, std::regex("\r"), "\\r");
    // file_content = std::regex_replace(file_content, std::regex("\t"), "\\t");
    return file_content;
}
