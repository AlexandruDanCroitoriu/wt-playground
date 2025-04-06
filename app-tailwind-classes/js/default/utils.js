window.addEventListener("keydown", function(e) {
  if(e.altKey && ["ArrowUp","ArrowDown","ArrowLeft","ArrowRight"].indexOf(e.code) > -1) {
      e.preventDefault();
  }
}, false);

console.log("Hello world!");