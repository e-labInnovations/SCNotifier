/** Client class
 * @param {Object} e
 */
class Client {
  constructor(e) {
    this.e = e;
  }
  // Getter
  get updateNotification() {
    const name = this.e.parameter.name;
    const notification = this.e.parameter.notification;
    let result = {};
    if (name && notification) {
      result.status = updateNotification(name, notification);
      result.notifications = getNotifications();
    } else {
      result.status = false;
      result.message = "Required parameter is missing";
    }

    return result;
  }
}