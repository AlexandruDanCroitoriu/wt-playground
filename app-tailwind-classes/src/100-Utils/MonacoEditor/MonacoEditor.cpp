#include "100-Utils/MonacoEditor/MonacoEditor.h"
#include <Wt/WApplication.h>
#include <Wt/WRandom.h>

MonacoEditor::MonacoEditor(std::string language)
    : js_signal_text_changed_(this, "editorTextChanged"),
        unsaved_text_(""),
        current_text_("")
{
    setLayoutSizeAware(true);
    
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
                minimap: { enabled: true },
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

    keyWentDown().connect([=](Wt::WKeyEvent e){ 
        Wt::WApplication::instance()->globalKeyWentDown().emit(e); // Emit the global key event
        if (e.modifiers().test(Wt::KeyboardModifier::Control))
        {
            if (e.key() == Wt::Key::S)
            {
                if(unsavedChanges()){
                    save_file_signal_.emit(unsaved_text_);
                }
            }
        } 
    });
    setEditorReadOnly(true);
}

void MonacoEditor::layoutSizeChanged(int width, int height)
{
    resetLayout(); // This is not needed as it is already called in setEditorText
    if(width >= 240) {
        width_changed_.emit(Wt::WString(std::to_string(width)));
    }
}


void MonacoEditor::editorTextChanged(std::string text)
{
    unsaved_text_ = text;
    avalable_save_.emit();
}

void MonacoEditor::textSaved()
{
    current_text_ = unsaved_text_;
    avalable_save_.emit();
}

void MonacoEditor::setEditorReadOnly(bool read_only) { 
    doJavaScript("setTimeout(function() { if(window." + editor_js_var_name_ + ") window." + editor_js_var_name_ + ".updateOptions({ readOnly: " + std::to_string(read_only) + " }); }, 200);");
}

bool MonacoEditor::unsavedChanges()
{
    if (current_text_.compare(unsaved_text_) == 0)
    {
        return false;
    }
    return true;
}

void MonacoEditor::setEditorText(std::string resource_path, std::string file_content)
{
    resetLayout();
    auto resource_path_url = resource_path + "?v=" + Wt::WRandom::generateId();
    doJavaScript(
        R"(
            setTimeout(function() {
                if(!window.)" + editor_js_var_name_ + R"() {
                    setTimeout(function() {
                        console.log("Setting editor text to: )" + resource_path_url + R"(");
                        if (window.)" + editor_js_var_name_ + R"() {
                            fetch(')" + resource_path_url + R"(')
                            .then(response => response.text())
                            .then(css => {
                                window.)" + editor_js_var_name_ + R"(_current_text = css;
                                window.)" + editor_js_var_name_ + R"(.setValue(css);
                            });
                        } else {
                            console.error("Editor instance is stil l not initialized.");
                        }
                    }, 2000);
                    return;
                }
                console.log("Setting editor text to: )" + resource_path_url + R"(");
                fetch(')" + resource_path_url + R"(')
                    .then(response => response.text())
                    .then(css => {
                        window.)" + editor_js_var_name_ + R"(_current_text = css;
                        window.)" + editor_js_var_name_ + R"(.setValue(css);
                    });
            }, 10); // Delay to ensure the editor is ready
        )");
    current_text_ = file_content;
    unsaved_text_ = current_text_;
}

void MonacoEditor::resetLayout()
{
    doJavaScript("setTimeout(function() { window." + editor_js_var_name_ + ".layout() }, 200);");
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
