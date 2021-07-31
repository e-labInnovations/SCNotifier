const ss = SpreadsheetApp.getActiveSpreadsheet();
const timetableSheet = ss.getSheetByName("Timetable");
const notificationsSheet = ss.getSheetByName("Notifications");
const weatherSheet = ss.getSheetByName("Weather");

const doGet = (e) => {
  const action = e.parameter.action;
  const client = new Client(e);
  if (action == "getData") {
    return ContentService.createTextOutput(JSON.stringify({ currentPeriod: getCurrentPeriod(), notifications: getNotifications(), weather: getWeather() }))
      .setMimeType(ContentService.MimeType.JSON);
  } else if (action == 'updateNotification') {
    let result = client.updateNotification;
    return ContentService.createTextOutput(JSON.stringify(result))
      .setMimeType(ContentService.MimeType.JSON);
  } else {
    return ContentService.createTextOutput(JSON.stringify({ status: false, message: 'Error' }))
      .setMimeType(ContentService.MimeType.JSON);
  }
}

const test = () => {
  console.log(getWeather());
}

const test1 = (value) => {
  Logger.log(value);
  return value + "";
}
/** Return timetable object
 * @return {object}
 */
const getTimetable = () => {
  const ttRange = timetableSheet.getRange(2, 1, timetableSheet.getLastRow() - 1, timetableSheet.getMaxColumns());
  let ttArray = ttRange.getValues();
  let timetable = {
    Monday: [],
    Tuesday: [],
    Wednesday: [],
    Thursday: [],
    Friday: []
  };
  for (let iR = 0; iR < ttArray.length; iR++) {
    let rowArray = ttArray[iR];
    let rowObj = {};
    rowObj.id = rowArray[0];
    rowObj.startAt = rowArray[2];
    rowObj.endAt = rowArray[3];
    rowObj.period = rowArray[4];
    rowObj.subject = rowArray[5];
    rowObj.teacher = rowArray[6];
    let day = rowArray[1];

    timetable[day].push(rowObj);
  }

  return timetable
}

/** Return current period object
 * @return {object}
 */
const getCurrentPeriod = () => {
  const days = ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'];
  let today = new Date();
  let todayName = days[today.getDay()];
  let timetable = getTimetable();

  if (todayName === 'Sunday' || todayName === 'Saturday') {
    return { subject: '', startAt: '', endAt: '', id: '', period: '', teacher: '' }
  }

  let todayTT = timetable[todayName];
  let currentPeriod = todayTT.find((period) => {
    let startAt = format24(period.startAt);
    let endAt = format24(period.endAt);
    let todayTime = twoDigit(today.getHours()) + ':' + twoDigit(today.getMinutes());
    return startAt <= todayTime && endAt >= todayTime
  })

  return currentPeriod ? currentPeriod : { subject: '', startAt: '', endAt: '', id: '', period: '', teacher: '' }
}

/** Return list of notifications
 * @return {Object[]} 
 */
const getNotifications = () => {
  let nArray = notificationsSheet.getRange(1, 1, notificationsSheet.getLastRow(), notificationsSheet.getLastColumn()).getValues();
  let notifications = [];
  for (let i = 0; i < nArray.length; i++) {
    let nObj = {};
    nObj.name = nArray[i][0];
    nObj.notification = nArray[i][1];
    nObj.show = nArray[i][2];
    nObj.readingTime = nArray[i][3];
    notifications.push(nObj);
  }
  return notifications;
}

/** Update notification
 * @param {String} name
 * @param {String} notification
 */
const updateNotification = (name, notification) => {
  let row = null;
  if (name == "Class") {
    row = 4;
  } else if (name == "HOD") {
    row = 5;
  } else if (name == "College") {
    row = 6;
  }
  if (row != null) {
    notificationsSheet.getRange(row, 2).setValue(notification);
  }

  return row ? true : false;
}

const login = () => {

}

const getWeather = () => {
  const currentWeatherArray = weatherSheet.getRange(2,1,3,weatherSheet.getLastColumn()).getValues();
  let currentWeather = {}
  currentWeatherArray[0].forEach((key, i) => {
    currentWeather[key] = currentWeatherArray[2][i];
  })

  const hourlyForcastArray = weatherSheet.getRange(6,1,7,weatherSheet.getLastColumn()).getValues();
  let hourlyForcast = [];
  for(let i = 3;i<7;i++) {
    let hObj = {};
    hObj[hourlyForcastArray[0][1]] = formatAMPM(hourlyForcastArray[i][1]);
    hObj[hourlyForcastArray[0][2]] = hourlyForcastArray[i][2];
    hObj[hourlyForcastArray[0][4]] = Math.round(hourlyForcastArray[i][4] * 100)+'%';
    hObj[hourlyForcastArray[0][6]] = hourlyForcastArray[i][6];
    hObj[hourlyForcastArray[0][7]] = hourlyForcastArray[i][7];
    hourlyForcast.push(hObj)
  }

  let weather = {
    currentWeather,
    hourlyForcast
  }
  return weather
}