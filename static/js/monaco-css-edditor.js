let edditor_css; // Declare the global variable
let current_text = ""; // Initialize current_text variable
require.config({ paths: { 'vs': 'https://unpkg.com/monaco-editor@0.34.1/min/vs' } });

function initializeCssEditor(elementId, edditor_content) {
  require(['vs/editor/editor.main'], function () {
    edditor_css = monaco.editor.create(document.getElementById(elementId), { // Assign to global variable
      language: 'css',
      value: edditor_content,
      theme: 'vs-dark',
      wordWrap: 'on',
      lineNumbers: 'on',
      tabSize: 4, // Set the tab size
      insertSpaces: false, // Use tabs instead of spaces
      detectIndentation: false, // Disable automatic indentation detection
      trimAutoWhitespace: false, // Prevent trimming of whitespace
      lineEnding: '\n' // Ensure consistent line endings
    });

    // Event listener for content changes
    edditor_css.onDidChangeModelContent(function (event) {
      if(current_text !== edditor_css.getValue()) { // Check if the text has changed`
        current_text = edditor_css.getValue(); // Update current_text with the new value
        Wt.emit(elementId, 'cssEdditorTextChanged', edditor_css.getValue());
      }
    });
    // Override the Ctrl+S command
    edditor_css.getDomNode().addEventListener('keydown', function(e) {
      if ((e.ctrlKey || e.metaKey) && e.key === 's') {
        e.preventDefault(); // Prevent the browser's default save dialog
        // Wt.emit(elementId, 'cssEdditorSaveText');
      }
    });
  });
}


function updateCssEditorValue(newValue) {
  if (edditor_css) {
    current_text = newValue; // Store the current text before updating
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