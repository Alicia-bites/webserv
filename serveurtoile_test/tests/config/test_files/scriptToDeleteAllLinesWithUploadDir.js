const fs = require('fs');

// Define the directory containing the files to process
const directory = '/mnt/nfs/homes/amarchan/Documents/serveurtoile/serveurtoile_test/tests/config/test_files';

// Define the regex to match lines containing "upload_dir"
const regex = /^.*upload_dir.*$/gm;

// Read each file in the directory
fs.readdir(directory, (err, files) => {
  if (err) throw err;

  // Process each file
  files.forEach(file => {
    const path = `${directory}/${file}`;

    // Read the file contents
    fs.readFile(path, 'utf8', (err, data) => {
      if (err) throw err;


      const cleanedData = data.replace(regex, '');

      // Overwrite the file with the cleaned data
      fs.writeFile(path, cleanedData, 'utf8', err => {
        if (err) throw err;
        console.log(`Processed file: ${file}`);
      });
    });
  });
});
