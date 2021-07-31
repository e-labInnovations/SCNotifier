const http = require('http'); // or 'https' for https:// URLs
const fs = require('fs');

var images = ["wintry_mix_rain_snow_dark_color_64dp.png", "haze_fog_dust_smoke_dark_color_64dp.png","cloudy_dark_color_64dp.png","snow_showers_snow_dark_color_64dp.png","flurries_dark_color_64dp.png","drizzle_dark_color_64dp.png","showers_rain_dark_color_64dp.png","heavy_rain_dark_color_64dp.png","strong_tstorms_dark_color_64dp.png","isolated_scattered_tstorms_day_dark_color_64dp.png","isolated_scattered_tstorms_night_dark_color_64dp.png","scattered_showers_day_dark_color_64dp.png","scattered_showers_night_dark_color_64dp.png","partly_cloudy_dark_color_64dp.png","partly_cloudy_night_dark_color_64dp.png","mostly_sunny_dark_color_64dp.png","mostly_clear_night_dark_color_64dp.png","mostly_cloudy_day_dark_color_64dp.png","mostly_cloudy_night_dark_color_64dp.png","sunny_dark_color_64dp.png","clear_night_dark_color_64dp.png","blizzard_dark_color_64dp.png"]

images.forEach(img => {
  const file = fs.createWriteStream("./icons/" + img.replace("_dark_color_64dp", ""));
  const request = http.get("http://www.gstatic.com/images/icons/material/apps/weather/1x/"+img, function(response) {
    response.pipe(file);
  });
})
