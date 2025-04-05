// // let window.css_editor; // Declare the global variable
// let current_text = ""; // Initialize current_text variable
// require.config({ paths: { 'vs': 'https://unpkg.com/monaco-editor@0.34.1/min/vs' } });

// function initializeCssEditor(elementId, edditor_content) {
//   require(['vs/editor/editor.main'], function () {
//     window.css_editor = monaco.editor.create(document.getElementById(elementId), {
//       language: 'css',
//       value: edditor_content,
//       theme: 'vs-dark',
//       wordWrap: 'on',
//       lineNumbers: 'on', 
//       tabSize: 4, 
//       insertSpaces: false, 
//       detectIndentation: false, 
//       trimAutoWhitespace: false,
//       lineEnding: '\n'
//     });

//     window.css_editor.onDidChangeModelContent(function (event) {
//       if(current_text !== window.css_editor.getValue()) {
//         current_text = window.css_editor.getValue(); 
//         Wt.emit(elementId, 'cssEdditorTextChanged', window.css_editor.getValue());
//       }
//     });
//     // Override the Ctrl+S command
//     window.css_editor.getDomNode().addEventListener('keydown', function(e) {
//       if ((e.ctrlKey || e.metaKey) && e.key === 's') {
//         e.preventDefault(); 
//       }
//     });
//   });
// }


// // function updateCssEditorValue(newValue) {
// //   if (window.css_editor) {
// //     current_text = newValue;
// //     window.css_editor.setValue(newValue);
// //   } else {
// //     console.error("Editor instance is not initialized yet.");
// //   }
// // }