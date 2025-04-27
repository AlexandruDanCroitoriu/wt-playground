function handleTailwindCSSUpload(file) {
  // Check if the uploaded file's name starts with "tailwind.css"
  if (file.name.startsWith("tailwind.css")) {
    // Check if there is already a file with similar name in browser memory
    const existingFiles = Object.values(sessionStorage).filter(key => key.startsWith('tailwind.css'));
    if (existingFiles.length > 0) {
        console.log('Tailwind.css file found in browser memory and is being deleted')
        // Delete the existing file by removing it from the session storage
        existingFiles.forEach(fileName => {
            sessionStorage.removeItem(fileName)
        })
      }
      console.log('New Tailwind.css file is being uploaded')
      // Save new file in session storage
      sessionStorage.setItem(file.name, file)
    }
}