const fs = require("fs");
const PNG = require("pngjs").PNG;
var argv = require('yargs/yargs')(process.argv.slice(2)).argv;
const chalk = require('chalk');

var images = ["wintry_mix_rain_snow_dark_color_64dp.png", "haze_fog_dust_smoke_dark_color_64dp.png","cloudy_dark_color_64dp.png","snow_showers_snow_dark_color_64dp.png","flurries_dark_color_64dp.png","drizzle_dark_color_64dp.png","showers_rain_dark_color_64dp.png","heavy_rain_dark_color_64dp.png","strong_tstorms_dark_color_64dp.png","isolated_scattered_tstorms_day_dark_color_64dp.png","isolated_scattered_tstorms_night_dark_color_64dp.png","scattered_showers_day_dark_color_64dp.png","scattered_showers_night_dark_color_64dp.png","partly_cloudy_dark_color_64dp.png","partly_cloudy_night_dark_color_64dp.png","mostly_sunny_dark_color_64dp.png","mostly_clear_night_dark_color_64dp.png","mostly_cloudy_day_dark_color_64dp.png","mostly_cloudy_night_dark_color_64dp.png","sunny_dark_color_64dp.png","clear_night_dark_color_64dp.png","blizzard_dark_color_64dp.png"]

images.forEach(img => {
var inputFile = "./icons/" + img.replace("_dark_color_64dp", "");
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
            console.log(chalk.green("The file was saved! ->" + inputFile));
        }
    });
  });
})