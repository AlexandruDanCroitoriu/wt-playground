#include "003-Tailwind/MonacoCssEdditor.h"
#include <Wt/WText.h>
#include <Wt/WApplication.h>
#include <Wt/WPushButton.h>

MonacoCssEdditor::MonacoCssEdditor()
    : js_signal_text_changed_(this, "cssEdditorTextChanged")
{

    setStyleClass("min-w-[500px] min-h-[500px]");

    js_signal_text_changed_.connect(this, &MonacoCssEdditor::cssEdditorTextChanged);
    // doJavaScript("require.config({ paths: { 'vs': 'https://unpkg.com/monaco-editor@0.34.1/min/vs' } });");
    std::string js_member = "";
    js_member += "require(['vs/editor/editor.main'], function() {";
    js_member += "  window.cssEditorInstance = monaco.editor.create(document.getElementById('" + id() + "'), {";
    js_member += "    language: 'css',";
    js_member += "  });";
    js_member += "  window.cssEditorInstance.onDidChangeModelContent(function() {";
    js_member += js_signal_text_changed_.createCall({ " window.cssEditorInstance.getValue()" });
    // js_member += "    Wt.emit("+id()+", 'cssEdditorTextChanged', window.cssEditorInstance.getValue());";
    js_member += "  });";
    js_member += "});";   

    // setJavaScriptMember("initialize", js_member);
    // setJavaScriptMember("initializeCssEditor("+id()+")", js_member);
    // Wt::WApplication::instance()->doJavaScript("initializeCssEditor('" + id() + "', '"+"alkjdshfaijksdhf i"+"');", true);
    std::string member = "initializeCssEditor('" + id() + "', '"+"alkjdshfaijksdhf i"+"');";
    setJavaScriptMember("cssEdditorTextChanged", member);
    // Wt::WApplication::instance()->doJavaScript("updateCssEditorValue('body { background-color: red; }')", true);
}

void MonacoCssEdditor::cssEdditorTextChanged(const std::string text)
{
    std::cout << "\nText changed: " << text << "\n";
}

void MonacoCssEdditor::setCssEdditorText(std::string text)
{
    // doJavaScript("window.cssEditorInstance.setValue('" + text + "');");
    doJavaScript("updateCssEditorValue('" + text + "');");
}


void MonacoCssEdditor::setDarkTheme(bool dark)
{
    // doJavaScript("toggleEditorTheme()");
    if(dark)
        doJavaScript("if(monaco.editor) monaco.editor.setTheme('vs-dark');");
    else
        doJavaScript("if(monaco.editor) monaco.editor.setTheme('vs');");
}

