const fs = require("fs");
const PNG = require("pngjs").PNG;
var argv = require('yargs/yargs')(process.argv.slice(2)).argv;
const chalk = require('chalk');

var inputFile = argv.file;
var outputFile = "out.bin";

if(!inputFile) {
  console.log(chalk.blue.underline.bold("Convert a png image to bin"));
  console.log(chalk.green('node index --file filename.png'));
  return;
} else {
  outputFile = inputFile.replace(".png", ".bin")
}

fs.createReadStream(inputFile)
  .pipe(
    new PNG({
      filterType: 4,
    })
  )
  .on("parsed", function () {
    console.log(`Dimensions: ${this.width}x${this.height}px`)
    if(this.width > 255 || this.height > 255) {
      console.log(chalk.red("Error: Width and height are limited to 255x255px"));
      return;
    }
    
    var imgData = [];
    imgData.push(this.width);
    imgData.push(this.height);
    
    for (var y = 0; y < this.height; y++) {
      for (var x = 0; x < this.width; x++) {
        var idx = (this.width * y + x) << 2;
        
        var v;
        v  = parseInt(this.data[idx] / 255.0 * 3.0)            // R
        v |= parseInt(this.data[idx + 1] / 255.0 * 3.0) << 2   // G
        v |= parseInt(this.data[idx + 2] / 255.0 * 3.0) << 4   // B
        v |= parseInt(this.data[idx + 3] / 255.0 * 3.0) << 6   // A
        imgData.push(v);
        
        
        // 8 colors
        this.data[idx] = this.data[idx]>127.5?255:0;
        this.data[idx + 1] = this.data[idx + 1]>127.5?255:0;
        this.data[idx + 2] = this.data[idx + 2]>127.5?255:0;
        this.data[idx + 3] = this.data[idx + 3]>127.5?255:0;
        
      }
    }
    
    
    var b = Buffer.from(imgData);
    this.pack().pipe(fs.createWriteStream(outputFile.replace(".bin", "_out.png")));
    
    fs.writeFile(outputFile, b,  "hex",function(err) {
        if(err) {
            console.log(chalk.red(err));
        } else {
            console.log(chalk.green("The file was saved!"));
        }
    });
  });