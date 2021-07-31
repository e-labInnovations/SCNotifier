/** Return 2 digit number from single digit number
 * @param {number | string} num
 * @return {string}
 */
const twoDigit = (num) => {
  return formattedNumber = ("0" + num).slice(-2);
}

/** Return 24 hours time from 12 hours time
 * @param {string} time
 * @return {string}
 */
const format24 = (time) => {
  var hours = Number(time.match(/^(\d+)/)[1]);
  var minutes = Number(time.match(/:(\d+)/)[1]);
  var AMPM = time.match(/\s(.*)$/)[1];
  if (AMPM == "PM" && hours < 12) hours = hours + 12;
  if (AMPM == "AM" && hours == 12) hours = hours - 12;
  var sHours = hours.toString();
  var sMinutes = minutes.toString();
  if (hours < 10) sHours = "0" + sHours;
  if (minutes < 10) sMinutes = "0" + sMinutes;
  return sHours + ":" + sMinutes;
}

/** File for include html file
 * @param {string} filename
 * @return string
 */
const include = (filename) => {
  return HtmlService.createHtmlOutputFromFile(filename)
    .getContent();
}

/** Return reading time in milli seconds
 * @param {string} notification
 * @return Number
 */
const getReadingTime = (notification) => {
  const WORDS_PER_MIN = 275;
  const wordCount = wordsCount(notification);
  const wordTime = wordCount / WORDS_PER_MIN;
  const wordTimeMilliSec =  wordTime * 60 * 1000;
  return Math.ceil(wordTimeMilliSec);
}

/** Return word count
 * @param {string} notification
 * @return Number
 */
const wordsCount = (notification) => {
  const pattern = '\\w+';
  const reg = new RegExp(pattern, 'g');
  return (notification.match(reg) || []).length;
}

/** Formate date to 12 hour time
 * @param {date} date
 * @return String
 */
const formatAMPM = date => {
  date = new Date(date);
  var hours = date.getHours();
  var minutes = date.getMinutes();
  var ampm = hours >= 12 ? 'PM' : 'AM';
  hours = hours % 12;
  hours = hours ? hours : 12; // the hour '0' should be '12'
  minutes = minutes < 10 ? '0'+minutes : minutes;
  var strTime = hours + ':' + minutes + ' ' + ampm;
  return strTime;
}

/** Return icon name from skycode
 * @param {String | Number} skycode
 * @return String
 */
const skycodeToIcon = skycode => {
  const icons = {
    "0" 	:	"",
    "1" 	:	"/strong_tstorms.png",
    "2" 	:	"/strong_tstorms.png",
    "3" 	:	"/strong_tstorms.png",
    "4" 	:	"/strong_tstorms.png",
    "5" 	:	"/wintry_mix_rain_snow.png",
    "6" 	:	"/wintry_mix_rain_snow.png",
    "7" 	:	"/wintry_mix_rain_snow.png",
    "8" 	:	"/drizzle.png",
    "9" 	:	"/drizzle.png",
    "10"	:	"/wintry_mix_rain_snow.png",
    "11"	:	"/drizzle.png",
    "12"	:	"/showers_rain.png",
    "13"	:	"/flurries.png",
    "14"	:	"/snow_showers_snow.png",
    "15"	:	"/snow_showers_snow.png",
    "16"	:	"/snow_showers_snow.png",
    "17"	:	"/wintry_mix_rain_snow.png",
    "18"	:	"/wintry_mix_rain_snow.png",
    "19"	:	"/haze_fog_dust_smoke.png",
    "20"	:	"/haze_fog_dust_smoke.png",
    "21"	:	"/haze_fog_dust_smoke.png",
    "22"	:	"/haze_fog_dust_smoke.png",
    "23"	:	"/haze_fog_dust_smoke.png",
    "24"	:	"/haze_fog_dust_smoke.png",
    "25"	:	"/blizzard.png",
    "26"	:	"/cloudy.png",
    "27"	:	"/mostly_cloudy_night.png",
    "28"	:	"/mostly_cloudy_day.png",
    "29"	:	"/partly_cloudy_night.png",
    "30"	:	"/partly_cloudy.png",
    "31"	:	"/clear_night.png",
    "32"	:	"/sunny.png",
    "33"	:	"/mostly_clear_night.png",
    "34"	:	"/mostly_sunny.png",
    "35"	:	"/wintry_mix_rain_snow.png",
    "36"	:	"/sunny.png",
    "37"	:	"/i_s_tstorms_day.png",//isolated_scattered_tstorms
    "38"	:	"/i_s_tstorms_day.png",//isolated_scattered_tstorms
    "39"	:	"/scattered_showers_day.png",
    "40"	:	"/heavy_rain.png",
    "41"	:	"/snow_showers_snow.png",
    "42"	:	"/blizzard.png",
    "43"	:	"/blizzard.png",
    "44"	:	"",
    "45"	:	"/scattered_showers_night.png",
    "46"	:	"/snow_showers_snow.png",
    "47"	:	"/i_s_tstorms_night.png"//isolated_scattered_tstorms
  }
  let icon = icons[skycode+""];
  icon = icon?icon:"";
  return icon
}