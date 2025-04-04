let edditor_css; // Declare the global variable
require.config({ paths: { 'vs': 'https://unpkg.com/monaco-editor@0.34.1/min/vs' } });

function initializeCssEditor(elementId, edditor_content) {
    require(['vs/editor/editor.main'], function() {
      edditor_css = monaco.editor.create(document.getElementById(elementId), { // Assign to global variable
        language: 'css',
        value: edditor_content,
        theme: 'vs-dark',
        wordWrap: 'on' 
      });
  
      // Event listener for content changes
      edditor_css.onDidChangeModelContent(function(event) {
        Wt.emit(elementId, 'cssEdditorTextChanged', edditor_css.getValue());
      });
    });
}

  
function updateCssEditorValue(newValue) {
    if (edditor_css) {
      edditor_css.setValue(newValue);
    } else {
      console.error("Editor instance is not initialized yet.");
    }
}

// function toggleEditorTheme() {
//   const currentTheme = window.cssEditorInstance._themeService._theme.themeName;
//   console.log("Current theme:", currentTheme);
//   if (window.cssEditorInstance) {
//     // const newTheme = currentTheme === 'vs-dark' ? 'vs' : 'vs-dark';
//     // window.cssEditorInstance.setTheme(newTheme);
//   } else {
//     console.error("Editor instance is not initialized yet.");
//   }
// }