// Copyright 2016 Google Inc. All rights reserved.

#ifndef FIREBASE_MESSAGING_CLIENT_CPP_INCLUDE_FIREBASE_MESSAGING_H_
#define FIREBASE_MESSAGING_CLIENT_CPP_INCLUDE_FIREBASE_MESSAGING_H_

#include <stdint.h>
#include <map>
#include <string>
#include <vector>
#include "firebase/app.h"
#include "firebase/internal/common.h"

#if !defined(DOXYGEN) && !defined(SWIG)
FIREBASE_APP_REGISTER_CALLBACKS_REFERENCE(messaging)
#endif  // !defined(DOXYGEN) && !defined(SWIG)

namespace firebase {

/// @brief Firebase Cloud Messaging API.
///
/// Firebase Cloud Messaging allows you to send data from your server to your
/// users' devices, and receive messages from devices on the same connection
/// if you're using a XMPP server.
///
/// The FCM service handles all aspects of queueing of messages and delivery
/// to client applications running on target devices.
namespace messaging {

/// Used for messages that display a notification.
///
/// On android, this requires that the app is using the Play Services client
/// library.
struct Notification {
  /// Indicates notification title. This field is not visible on iOS phones
  /// and tablets.
  std::string title;

  /// Indicates notification body text.
  std::string body;

  /// Indicates notification icon. Sets value to myicon for drawable resource
  /// myicon.
  std::string icon;

  /// Indicates a sound to play when the device receives the notification.
  /// Supports default, or the filename of a sound resource bundled in the
  /// app.
  ///
  /// Android sound files must reside in /res/raw/, while iOS sound files
  /// can be in the main bundle of the client app or in the Library/Sounds
  /// folder of the app’s data container.
  std::string sound;

  /// Indicates the badge on the client app home icon. iOS only.
  std::string badge;

  /// Indicates whether each notification results in a new entry in the
  /// notification drawer on Android. If not set, each request creates a new
  /// notification. If set, and a notification with the same tag is already
  /// being shown, the new notification replaces the existing one in the
  /// notification drawer.
  std::string tag;

  /// Indicates color of the icon, expressed in \#rrggbb format. Android only.
  std::string color;

  /// The action associated with a user click on the notification.
  ///
  /// On Android, if this is set, an activity with a matching intent filter is
  /// launched when user clicks the notification.
  ///
  /// If set on iOS, corresponds to category in APNS payload.
  std::string click_action;

  /// Indicates the key to the body string for localization.
  ///
  /// On iOS, this corresponds to "loc-key" in APNS payload.
  ///
  /// On Android, use the key in the app's string resources when populating this
  /// value.
  std::string body_loc_key;

  /// Indicates the string value to replace format specifiers in body string
  /// for localization.
  ///
  /// On iOS, this corresponds to "loc-args" in APNS payload.
  ///
  /// On Android, these are the format arguments for the string resource. For
  /// more information, see [Formatting strings][1].
  ///
  /// [1]: https://developer.android.com/guide/topics/resources/string-resource.html#FormattingAndStyling
  std::vector<std::string> body_loc_args;

  /// Indicates the key to the title string for localization.
  ///
  /// On iOS, this corresponds to "title-loc-key" in APNS payload.
  ///
  /// On Android, use the key in the app's string resources when populating this
  /// value.
  std::string title_loc_key;

  /// Indicates the string value to replace format specifiers in title string
  /// for localization.
  ///
  /// On iOS, this corresponds to "title-loc-args" in APNS payload.
  ///
  /// On Android, these are the format arguments for the string resource. For
  /// more information, see [Formatting strings][1].
  ///
  /// [1]: https://developer.android.com/guide/topics/resources/string-resource.html#FormattingAndStyling
  std::vector<std::string> title_loc_args;
};

/// @brief Data structure used to send messages to, and receive messages from,
/// cloud messaging.
struct Message {
  /// Initialize the message.
  Message()
      : time_to_live(0),
        notification(nullptr),
        notification_opened(false) {}

  /// Destructor.
  ~Message() { delete notification; }

  /// Copy constructor. Makes a deep copy of this Message.
  Message(const Message& other) : notification(nullptr) { *this = other; }

  /// Copy assignment operator. Makes a deep copy of this Message.
  Message& operator=(const Message& other) {
    this->from = other.from;
    this->to = other.to;
    this->collapse_key = other.collapse_key;
    this->data = other.data;
    this->raw_data = other.raw_data;
    this->message_id = other.message_id;
    this->message_type = other.message_type;
    this->priority = other.priority;
    this->time_to_live = other.time_to_live;
    this->error = other.error;
    this->error_description = other.error_description;
    delete this->notification;
    if (other.notification) {
      this->notification = new Notification(*other.notification);
    } else {
      this->notification = nullptr;
    }
    this->notification_opened = other.notification_opened;
    this->link = other.link;
    return *this;
  }

  /// Authenticated ID of the sender. This is a project number in most cases.
  ///
  /// Any value starting with google.com, goog. or gcm. are reserved.
  ///
  /// This field is only used for downstream messages received through
  /// Listener::OnMessage().
  std::string from;

  /// This parameter specifies the recipient of a message.
  ///
  /// For example it can be a registration token, a topic name, an Instance ID
  /// or project ID.
  ///
  /// For upstream messages use the format  PROJECT_ID@gcm.googleapis.com.
  ///
  /// This field is used for both upstream messages sent with
  /// firebase::messaging::Send() and downstream messages received through
  /// Listener::OnMessage(). For upstream messages,
  /// PROJECT_ID@gcm.googleapis.com or Instance ID are accepted.
  std::string to;

  /// This parameter identifies a group of messages (e.g., with collapse_key:
  /// "Updates Available") that can be collapsed, so that only the last message
  /// gets sent when delivery can be resumed.  This is intended to avoid sending
  /// too many of the same messages when the device comes back online or becomes
  /// active.
  ///
  /// Note that there is no guarantee of the order in which messages get sent.
  ///
  /// Note: A maximum of 4 different collapse keys is allowed at any given time.
  /// This means a FCM connection server can simultaneously store 4 different
  /// send-to-sync messages per client app. If you exceed this number, there is
  /// no guarantee which 4 collapse keys the FCM connection server will keep.
  ///
  /// This field is only used for downstream messages received through
  /// Listener::OnMessage().
  std::string collapse_key;

  /// The metadata, including all original key/value pairs. Includes some of the
  /// HTTP headers used when sending the message. `gcm`, `google` and `goog`
  /// prefixes are reserved for internal use.
  ///
  /// This field is used for both upstream messages sent with
  /// firebase::messaging::Send() and downstream messages received through
  /// Listener::OnMessage().
  std::map<std::string, std::string> data;

  /// Binary payload. This field is currently unused.
  std::string raw_data;

  /// Message ID. This can be specified by sender. Internally a hash of the
  /// message ID and other elements will be used for storage. The ID must be
  /// unique for each topic subscription - using the same ID may result in
  /// overriding the original message or duplicate delivery.
  ///
  /// This field is used for both upstream messages sent with
  /// firebase::messaging::Send() and downstream messages received through
  /// Listener::OnMessage().
  std::string message_id;

  /// Equivalent with a content-type.
  ///
  /// Defined values:
  ///   - "deleted_messages" - indicates the server had too many messages and
  ///     dropped some, and the client should sync with his own server.
  ///     Current limit is 100 messages stored.
  ///   - "send_event" - indicates an upstream message has been pushed to the
  ///     FCM server. It does not guarantee the upstream destination received
  ///     it.
  ///     Parameters: "message_id"
  ///   - "send_error" - indicates an upstream message expired, without being
  ///     sent to the FCM server.
  ///     Parameters: "message_id" and "error"
  ///
  /// If this field is missing, the message is a regular message.
  ///
  /// This field is only used for downstream messages received through
  /// Listener::OnMessage().
  std::string message_type;

  /// Sets the priority of the message. Valid values are "normal" and "high." On
  /// iOS, these correspond to APNs priority 5 and 10.
  ///
  /// By default, messages are sent with normal priority. Normal priority
  /// optimizes the client app's battery consumption, and should be used unless
  /// immediate delivery is required. For messages with normal priority, the app
  /// may receive the message with unspecified delay.
  ///
  /// When a message is sent with high priority, it is sent immediately, and the
  /// app can wake a sleeping device and open a network connection to your
  /// server.
  ///
  /// For more information, see [Setting the priority of a message][1].
  ///
  /// This field is only used for downstream messages received through
  /// Listener::OnMessage().
  ///
  /// [1]: https://firebase.google.com/docs/cloud-messaging/concept-options#setting-the-priority-of-a-message
  std::string priority;

  /// This parameter specifies how long (in seconds) the message should be kept
  /// in FCM storage if the device is offline. The maximum time to live
  /// supported is 4 weeks, and the default value is 4 weeks. For more
  /// information, see [Setting the lifespan of a message][1].
  ///
  /// This field is only used for downstream messages received through
  /// Listener::OnMessage().
  ///
  /// [1]: https://firebase.google.com/docs/cloud-messaging/concept-options#ttl
  int32_t time_to_live;

  /// Error code. Used in "nack" messages for CCS, and in responses from the
  /// server.
  /// See the CCS specification for the externally-supported list.
  ///
  /// This field is only used for downstream messages received through
  /// Listener::OnMessage().
  std::string error;

  /// Human readable details about the error.
  ///
  /// This field is only used for downstream messages received through
  /// Listener::OnMessage().
  std::string error_description;

  /// Optional notification to show. This only set if a notification was
  /// received with this message, otherwise it is null.
  ///
  /// The notification is only guaranteed to be valid during the call to
  /// Listener::OnMessage(). If you need to keep it around longer you will need
  /// to make a copy of either the Message or Notification. Copying the Message
  /// object implicitly makes a deep copy of the notification (allocated with
  /// new) which is owned by the Message.
  ///
  /// This field is only used for downstream messages received through
  /// Listener::OnMessage().
  Notification* notification;

  /// A flag indicating whether this message was opened by tapping a
  /// notification in the OS system tray. If the message was received this way
  /// this flag is set to true.
  bool notification_opened;

  /// The link into the app from the message.
  ///
  /// This field is only used for downstream messages received through
  /// Listener::OnMessage().
  std::string link;
};

/// @brief Base class used to receive messages from Firebase Cloud Messaging.
///
/// You need to override base class methods to handle any events required by the
/// application. Methods are invoked asynchronously and may be invoked on other
/// threads.
class Listener {
 public:
  virtual ~Listener();

  /// Called on the client when a message arrives.
  ///
  /// @param[in] message The data describing this message.
  virtual void OnMessage(const Message& message) = 0;

  /// Called on the client when a registration token arrives. This function
  /// will eventually be called in response to a call to
  /// firebase::messaging::Initialize(...).
  ///
  /// @param[in] token The registration token.
  virtual void OnTokenReceived(const char* token) = 0;
};

/// @brief Initialize Firebase Cloud Messaging.
///
/// After Initialize is called, the implementation may call functions on the
/// Listener provided at any time.
///
/// @param[in] app The Firebase App object for this application.
/// @param[in] listener A Listener object that listens for events from the
///            Firebase Cloud Messaging servers.
///
/// @return kInitResultSuccess if initialization succeeded, or
/// kInitResultFailedMissingDependency on Android if Google Play services is
/// not available on the current device.
InitResult Initialize(const App& app, Listener* listener);

/// @brief Terminate Firebase Cloud Messaging.
///
/// Frees resources associated with Firebase Cloud Messaging.
///
/// @note On Android, the services will not be shut down by this method.
void Terminate();

/// @brief Set the listener for events from the Firebase Cloud Messaging
/// servers.
///
/// A listener must be set for the application to receive messages from
/// the Firebase Cloud Messaging servers.  The implementation may call functions
/// on the Listener provided at any time.
///
/// @param[in] listener A Listener object that listens for events from the
///            Firebase Cloud Messaging servers.
///
/// @return Pointer to the previously set listener.
Listener* SetListener(Listener* listener);

/// Send an upstream ("device to cloud") message. You can only use the upstream
/// feature if your FCM implementation uses the XMPP-based Cloud Connection
/// Server. The current limits for max storage time and number of outstanding
/// messages per application are documented in the [FCM Developers Guide].
///
/// [FCM Developers Guide]: https://firebase.google.com/docs/cloud-messaging/
///
/// @param[in] message The message to send upstream.
void Send(const Message& message);

/// @brief Subscribe to receive all messages to the specified topic.
///
/// Subscribes an app instance to a topic, enabling it to receive messages
/// sent to that topic.
///
/// Call this function from the main thread. FCM is not thread safe.
///
/// @param topic The topic to subscribe to. Should be of the form
///              `"/topics/<topic-name>"`.
void Subscribe(const char* topic);

/// @brief Unsubscribe from a topic.
///
/// Unsubscribes an app instance from a topic, stopping it from receiving
/// any further messages sent to that topic.
///
/// Call this function from the main thread. FCM is not thread safe.
///
/// @param topic The topic to unsubscribe from.
void Unsubscribe(const char* topic);

class PollableListenerImpl;

/// @brief A listener that can be polled to consume pending `Message`s.
///
/// This class is intended to be used with applications that have a main loop
/// that frequently updates, such as in the case of a game that has a main
/// loop that updates 30 to 60 times a second. Rather than respond to incoming
/// messages and tokens via the `OnMessage` virtual function, this class will
/// queue up the message internally in a thread-safe manner so that it can be
/// consumed with `PollMessage`. For example:
///
///     ::firebase::messaging::PollableListener listener;
///     ::firebase::messaging::Initialize(app, &listener);
///
///     while (true) {
///       std::string token;
///       if (listener.PollRegistrationToken(&token)) {
///         LogMessage("Received a registration token");
///       }
///
///       ::firebase::messaging::Message message;
///       while (listener.PollMessage(&message)) {
///         LogMessage("Received a new message");
///       }
///
///       // Remainder of application logic...
///     }
class PollableListener : public Listener {
 public:
  /// @brief The default constructor.
  PollableListener();

  /// @brief The required virtual destructor.
  virtual ~PollableListener();

  /// @brief An implementation of `OnMessage` which adds the incoming messages
  /// to a queue, which can be consumed by calling `PollMessage`.
  virtual void OnMessage(const Message& message);

  /// @brief An implementation of `OnTokenReceived` which stores the incoming
  /// token so that it can be consumed by calling `PollRegistrationToken`.
  virtual void OnTokenReceived(const char* token);

  /// @brief Returns the first message queued up, if any.
  ///
  /// If one or more messages has been received, the first message in the
  /// queue will be popped and used to populate the `message` argument and the
  /// function will return `true`. If there are no pending messages, `false` is
  /// returned. This function should be called in a loop until all messages have
  /// been consumed, like so:
  ///
  ///     ::firebase::messaging::Message message;
  ///     while (listener.PollMessage(&message)) {
  ///       LogMessage("Received a new message");
  ///     }
  ///
  /// @param[out] message The `Message` struct to be populated. If there were no
  /// pending messages, `message` is not modified.
  ///
  /// @return Returns `true` if there was a pending message, `false` otherwise.
  bool PollMessage(Message* message);

  /// @brief Returns the registration key, if a new one has been received.
  ///
  /// When a new registration token is received, it is cached internally and can
  /// be retrieved by calling `PollRegistrationToken`. The cached registration
  /// token will be used to populate the `token` argument, then the cache will
  /// be cleared and the function will return `true`. If there is no cached
  /// registration token this function retuns `false`.
  ///
  ///     std::string token;
  ///     if (listener.PollRegistrationToken(&token)) {
  ///       LogMessage("Received a registration token");
  ///     }
  ///
  /// @param[out] token A string to be populated with the new token if one has
  /// been recieved. If there were no new token, the string is left unmodified.
  ///
  /// @return Returns `true` if there was a new token, `false` otherwise.
  bool PollRegistrationToken(std::string* token);

 private:
  // The implementation of the `PollableListener`.
  PollableListenerImpl* impl_;
};

}  // namespace messaging
}  // namespace firebase

#endif  // FIREBASE_MESSAGING_CLIENT_CPP_INCLUDE_FIREBASE_MESSAGING_H_