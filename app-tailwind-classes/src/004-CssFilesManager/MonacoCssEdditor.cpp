#include "004-CssFilesManager/MonacoCssEdditor.h"
#include <Wt/WText.h>
#include <Wt/WApplication.h>
#include <Wt/WPushButton.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <regex>

MonacoCssEdditor::MonacoCssEdditor(std::string text_content)
    : js_signal_text_changed_(this, "cssEdditorTextChanged")
{

    setStyleClass("!text-start");

    js_signal_text_changed_.connect(this, &MonacoCssEdditor::cssEdditorTextChanged);
    // doJavaScript("require.config({ paths: { 'vs': 'https://unpkg.com/monaco-editor@0.34.1/min/vs' } });");
    // std::string js_member = "";
    // js_member += "require(['vs/editor/editor.main'], function() {";
    // js_member += "  window.cssEditorInstance = monaco.editor.create(document.getElementById('" + id() + "'), {";
    // js_member += "    language: 'css',";
    // js_member += "  });";
    // js_member += "  window.cssEditorInstance.onDidChangeModelContent(function() {";
    // js_member += js_signal_text_changed_.createCall({ " window.cssEditorInstance.getValue()" });
    // // js_member += "    Wt.emit("+id()+", 'cssEdditorTextChanged', window.cssEditorInstance.getValue());";
    // js_member += "  });";
    // js_member += "});";   

    // setJavaScriptMember("initialize", js_member);
    // setJavaScriptMember("initializeCssEditor("+id()+")", js_member);
    // Wt::WApplication::instance()->doJavaScript("initializeCssEditor('" + id() + "', '"+"alkjdshfaijksdhf i"+"');", true);
    std::string member = "initializeCssEditor('" + id() + "', '"+text_content+"');";
    setJavaScriptMember("cssEdditorTextChanged", member);
    // Wt::WApplication::instance()->doJavaScript("updateCssEditorValue('body { background-color: red; }')", true);
}


void MonacoCssEdditor::cssEdditorTextChanged(const std::string text)
{
    if(text.compare(current_text_) == 0)
    {
        std::cout << "\n\n Text Did Not Change as it was the SAME \n\n";
        return;
    }
    std::cout << "\nText changed: " << text << "\n";
}

void MonacoCssEdditor::setCssEdditorText(std::string text)
{
    if(text.compare(current_text_) == 0) {
        std::cout << "\n\n Text Did Not Change as it was the SAME \n\n";
        return;
    }
    // replace all new lines with \n
    text = std::regex_replace(text, std::regex("\n"), "\\n");
    text = std::regex_replace(text, std::regex("\r"), "\\r");
    
    doJavaScript("updateCssEditorValue('" + text + "');");
    current_text_ = text;
}

void MonacoCssEdditor::setDarkTheme(bool dark)
{
    // doJavaScript("toggleEditorTheme()");
    if(dark)
        doJavaScript("if(monaco.editor) monaco.editor.setTheme('vs-dark');");
    else
        doJavaScript("if(monaco.editor) monaco.editor.setTheme('vs');");
}

