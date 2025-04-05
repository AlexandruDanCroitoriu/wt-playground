let edditor_css; // Declare the global variable
let edditor_css_content = "";
require.config({ paths: { 'vs': 'https://unpkg.com/monaco-editor@0.34.1/min/vs' } });

function initializeCssEditor(elementId, edditor_content) {
  edditor_css_content = edditor_content; // Initialize the global variable with the initial content
  require(['vs/editor/editor.main'], function () {
    edditor_css = monaco.editor.create(document.getElementById(elementId), { // Assign to global variable
      language: 'css',
      value: edditor_content,
      theme: 'vs-dark',
      wordWrap: 'on'
    });

    // Event listener for content changes
    edditor_css.onDidChangeModelContent(function (event) {
      if(edditor_css_content === edditor_css.getValue()) return;

      Wt.emit(elementId, 'cssEdditorTextChanged', edditor_css.getValue());
    });
    // Override the Ctrl+S command
    edditor_css.getDomNode().addEventListener('keydown', function(e) {
      if ((e.ctrlKey || e.metaKey) && e.key === 's') {
        e.preventDefault(); // Prevent the browser's default save dialog
        if(edditor_css_content === edditor_css.getValue()) {
          // Content hasn't changed, no need to emit the event
          return;
        }
        edditor_css_content = edditor_css.getValue(); // Update the global variable with the current content
        Wt.emit(elementId, 'cssEdditorTextChanged', edditor_css.getValue());
      }
    });
  });
}


function updateCssEditorValue(newValue) {
  if (edditor_css) {
    edditor_css_content = newValue; // Update the global variable with the new value
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