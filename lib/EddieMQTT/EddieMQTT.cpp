#include <Arduino.h>
#include <WiFiClient.h>
#include <MQTT.h>
#include <Regexp.h>

#include "EddieMQTT.h"

bool
EddieMQTTSubscriber::TopicCompare(const EddieMQTTSubscriber& sub, const EddieMQTTSubscriber& in)
{
    char top[MQTT_BUFFER_LENGTH];
    strncpy_P(top, sub.topic.c_str(), sizeof(top));

    MatchState ms(top);

    // Replace MQTT Wildcards with matching RegExp
    char therest[] = ".*";
    char exp[] = MQTT_REGEXP;
    ms.GlobalReplace("#", therest);
    ms.GlobalReplace("\\+", exp);

    // Pad the topic for RegExp boundries: "^TOPIC$"
    size_t len = strlen(top);
    memmove(top + 1, top, len + 1);
    top[0] = '^';
    strcat_P(top, "$");

    // Match subscribed topic with incomming
    char in_top[MQTT_BUFFER_LENGTH];
    strncpy_P(in_top, in.topic.c_str(), sizeof(in_top));

    ms.Target(in_top);

    bool res = ms.Match(top) == REGEXP_MATCHED;

    return res;
}

bool
EddieMQTTSubscriber::PlainTopicCompare(const EddieMQTTSubscriber& a, const EddieMQTTSubscriber& b)
{
    return (a.topic == b.topic);
}
bool
EddieMQTTSubscriber::FullCompare(const EddieMQTTSubscriber& a, const EddieMQTTSubscriber& b)
{
    return (a.topic == b.topic) && (a.callback == b.callback);
}
bool
EddieMQTTSubscriber::operator== (EddieMQTTSubscriber b)
{
    return (topic == b.topic) && (callback == b.callback);
}


EddieMQTT* EddieMQTT::_instance = nullptr;

#if EDDIEMQTT_USE_INFO
EddieMQTT::EddieMQTT(IPAddress ip, String user, String dev_id, String prefix, String prefix_info, ushort port) : _client(1024), _subscribers()
#else
EddieMQTT(IPAddress ip, String user, String dev_id, String prefix, ushort port) : _client(1024), _subscribers()
#endif
{
    EddieMQTT::_instance = this;

    _server_ip = ip;
    _server_port = port;
    _server_host = "";

    _prefix = prefix;
    _add_slash = true;

#if EDDIEMQTT_USE_INFO
    _prefix_info = prefix_info;
    _add_slash_info = true;
#endif

    _dev_id = dev_id;
    _user = user;
}
#if EDDIEMQTT_USE_INFO
EddieMQTT::EddieMQTT(String host, String user, String dev_id, String prefix, String prefix_info, ushort port) : _client(1024), _subscribers()
#else
EddieMQTT(String host, String user, String dev_id, String prefix, ushort port) : _client(1024), _subscribers()
#endif
{
    EddieMQTT::_instance = this;

    //_server_ip = 0;
    _server_host = host;
    _server_port = port;

    _prefix = prefix;
    _add_slash = true;

#if EDDIEMQTT_USE_INFO
    _prefix_info = prefix_info;
    _add_slash_info = true;
#endif

    _dev_id = dev_id;
    _user = user;
}

bool
EddieMQTT::init(bool doConnect)
{
    if (_server_host == "")
        _client.begin(_server_ip, _server_port, _net);
    else
        _client.begin(_server_host.c_str(), _server_port, _net);

    _client.onMessage(_onMessageReceived);

    bool ret = true;
    
    if (doConnect)
        ret = connect();

    /* TODO?? */

    return ret;
}
bool
EddieMQTT::connect()
{
    if (_server_host == "")
        Serial.println("Connecting MQTT to ip: " + _server_ip.toString());
    else
        Serial.println("Connecting MQTT to host: " + _server_host);

    return _client.connect(_user.c_str(), _dev_id.c_str());
}
bool
EddieMQTT::Connect()
{
    if (_instance == nullptr)
        return false;

    return _instance->connect();
}


bool
EddieMQTT::loop()
{
    return _client.loop();
}
bool
EddieMQTT::Loop()
{
    if (_instance == nullptr)
        return false;

    return _instance->loop();
}
bool
EddieMQTT::disconnect()
{
    return _client.disconnect();
}
bool
EddieMQTT::Disconnect()
{
    if (_instance == nullptr)
        return false;

    return _instance->disconnect();
}

bool
EddieMQTT::isConnected()
{
    return _client.connected();
}
bool
EddieMQTT::IsConnected()
{
    if (_instance == nullptr)
        return false;

    return _instance->isConnected();
}

void
EddieMQTT::setPrefix(const String &prefix, bool add_slash) { _prefix = prefix; _add_slash = true; }
bool
EddieMQTT::SetPrefix(const String &prefix, bool add_slash)
{
    if (_instance == nullptr)
        return false;

    _instance->setPrefix(prefix, add_slash);

    return true;
}
const String 
EddieMQTT::getPrefix() { return _prefix; }
const String 
EddieMQTT::GetPrefix()
{
    if (_instance == nullptr)
        return "N/A";

    return _instance->getPrefix();
}

#if EDDIEMQTT_USE_INFO
void
EddieMQTT::setInfoPrefix(const String &prefix, bool add_slash) { _prefix_info = prefix; _add_slash_info = true; }
bool
EddieMQTT::SetInfoPrefix(const String &prefix, bool add_slash)
{
    if (_instance == nullptr)
        return false;

    _instance->setInfoPrefix(prefix, add_slash);

    return true;
}
const String 
EddieMQTT::getInfoPrefix() { return _prefix_info; }
const String 
EddieMQTT::GetInfoPrefix()
{
    if (_instance == nullptr)
        return "N/A";

    return _instance->getInfoPrefix();
}
#endif

void
EddieMQTT::setKeepAlive(int keepAlive)
{
    _client.setKeepAlive(keepAlive);
}
void
EddieMQTT::setCleanSession(bool cleanSession)
{
    _client.setCleanSession(cleanSession);
}
void
EddieMQTT::setTimeout(int timeout)
{
    _client.setTimeout(timeout);
}
bool
EddieMQTT::SetKeepAlive(int keepAlive)
{
    if (_instance == nullptr)
        return false;

    _instance->setKeepAlive(keepAlive);

    return true;
}
bool
EddieMQTT::SetCleanSession(bool cleanSession)
{
    if (_instance == nullptr)
        return false;

    _instance->setCleanSession(cleanSession);

    return true;
}
bool
EddieMQTT::SetTimeout(int timeout)
{
    if (_instance == nullptr)
        return false;

    _instance->setTimeout(timeout);

    return true;
}
void
EddieMQTT::setOptions(int _keepAlive, bool _cleanSession, int _timeout)
{
    this->setKeepAlive(_keepAlive);
    this->setCleanSession(_cleanSession);
    this->setTimeout(_timeout);
}
bool
EddieMQTT::SetOptions(int _keepAlive, bool _cleanSession, int _timeout)
{
    if (_instance == nullptr)
        return false;

    _instance->setOptions(_keepAlive, _cleanSession, _timeout);

    return true;
}

void
EddieMQTT::setWill(const String &topic, const String &payload, bool include_prefix, bool retained, int qos)
{
    _client.setWill(_pf(topic, include_prefix).c_str(), payload.c_str(), retained, qos);
}
bool
EddieMQTT::SetWill(const String &topic, const String &payload, bool include_prefix, bool retained, int qos)
{
    if (_instance == nullptr)
        return false;

    _instance->setWill(topic, payload, include_prefix, retained, qos);

    return true;
}
#if EDDIEMQTT_USE_INFO
void
EddieMQTT::setWillInfo(const String &topic, const String &payload, bool include_prefix, bool retained, int qos)
{
    _client.setWill(_pfi(topic, include_prefix).c_str(), payload.c_str(), retained, qos);
}
bool
EddieMQTT::SetWillInfo(const String &topic, const String &payload, bool include_prefix, bool retained, int qos)
{
    if (_instance == nullptr)
        return false;

    _instance->setWillInfo(topic, payload, include_prefix, retained, qos);

    return true;
}
#endif
void
EddieMQTT::clearWill()
{
    return _client.clearWill();
}
bool
EddieMQTT::ClearWill()
{
    if (_instance == nullptr)
        return false;

    _instance->clearWill();

    return true;
}

bool
EddieMQTT::publish(const String &topic, const String &payload, bool include_prefix, bool retained, int qos)
{
    if (_debug) {
        Serial.print("PUB:");
        Serial.println(_pf(topic, include_prefix) + " : " + payload);
    }

    return _client.publish(_pf(topic, include_prefix), payload, retained, qos);
}
bool
EddieMQTT::publishBool(const String &topic, const bool &payload, bool include_prefix, bool retained, int qos)
{
    String payload_s = BoolToStr(payload);

    if (_debug) {
        Serial.print("PUB bool:");
        Serial.println(_pf(topic, include_prefix) + " : " + payload_s);
    }

    return _client.publish(_pf(topic, include_prefix), payload_s, retained, qos);
}
bool
EddieMQTT::publishState(const String &topic, const bool &payload, bool include_prefix, bool retained, int qos)
{
    String payload_s = BoolToStr(payload, true);

    if (_debug) {
        Serial.print("PUB bool:");
        Serial.println(_pf(topic, include_prefix) + " : " + payload_s);
    }

    return _client.publish(_pf(topic, include_prefix), payload_s, retained, qos);
}
bool
EddieMQTT::publish(const String &topic, const float &payload, bool include_prefix, bool retained, int qos)
{
    String s_payload = String(payload);

    if (_debug) {
        Serial.print("PUB float:");
        Serial.println(_pf(topic, include_prefix) + " : " + s_payload);
    }

    return _client.publish(_pf(topic, include_prefix), s_payload, retained, qos);
}
bool
EddieMQTT::publish(const String &topic, const int &payload, bool include_prefix, bool retained, int qos)
{
    String s_payload = String(payload);

    if (_debug) {
        Serial.print("PUB int:");
        Serial.println(_pf(topic, include_prefix) + " : " + s_payload);
    }

    return _client.publish(_pf(topic, include_prefix), s_payload, retained, qos);
}
bool
EddieMQTT::publish(const String &topic, const unsigned int &payload, bool include_prefix, bool retained, int qos)
{
    String s_payload = String(payload);

    if (_debug) {
        Serial.print("PUB uint:");
        Serial.println(_pf(topic, include_prefix) + " : " + s_payload);
    }

    return _client.publish(_pf(topic, include_prefix), s_payload, retained, qos);
}
bool
EddieMQTT::publish(const String &topic, const long &payload, bool include_prefix, bool retained, int qos)
{
    String s_payload = String(payload);

    if (_debug) {
        Serial.print("PUB long:");
        Serial.println(_pf(topic, include_prefix) + " : " + s_payload);
    }

    return _client.publish(_pf(topic, include_prefix), s_payload, retained, qos);
}
bool
EddieMQTT::publish(const String &topic, const unsigned long &payload, bool include_prefix, bool retained, int qos)
{
    String s_payload = String(payload);

    if (_debug) {
        Serial.print("PUB ulong:");
        Serial.println(_pf(topic, include_prefix) + " : " + s_payload);
    }

    return _client.publish(_pf(topic, include_prefix), s_payload, retained, qos);
}
bool
EddieMQTT::Publish(const String &topic, const String &payload, bool include_prefix, bool retained, int qos)
{
    if (_instance == nullptr)
        return false;

    return _instance->publish(topic, payload, include_prefix, retained, qos);
}
bool
EddieMQTT::PublishBool(const String &topic, const bool &payload, bool include_prefix, bool retained, int qos)
{
    if (_instance == nullptr)
        return false;

    return _instance->publish(topic, payload, include_prefix, retained, qos);
}
bool
EddieMQTT::PublishState(const String &topic, const bool &payload, bool include_prefix, bool retained, int qos)
{
    if (_instance == nullptr)
        return false;

    return _instance->publishState(topic, payload, include_prefix, retained, qos);
}
bool
EddieMQTT::Publish(const String &topic, const float &payload, bool include_prefix, bool retained, int qos)
{
    if (_instance == nullptr)
        return false;

    return _instance->publish(topic, payload, include_prefix, retained, qos);
}
bool
EddieMQTT::Publish(const String &topic, const int &payload, bool include_prefix, bool retained, int qos)
{
    if (_instance == nullptr)
        return false;

    return _instance->publish(topic, payload, include_prefix, retained, qos);
}
bool
EddieMQTT::Publish(const String &topic, const unsigned int &payload, bool include_prefix, bool retained, int qos)
{
    if (_instance == nullptr)
        return false;

    return _instance->publish(topic, payload, include_prefix, retained, qos);
}
bool
EddieMQTT::Publish(const String &topic, const long &payload, bool include_prefix, bool retained, int qos)
{
    if (_instance == nullptr)
        return false;

    return _instance->publish(topic, payload, include_prefix, retained, qos);
}
bool
EddieMQTT::Publish(const String &topic, const unsigned long &payload, bool include_prefix, bool retained, int qos)
{
    if (_instance == nullptr)
        return false;

    return _instance->publish(topic, payload, include_prefix, retained, qos);
}

#if EDDIEMQTT_USE_INFO
bool
EddieMQTT::publishInfo(const String &topic, const String &payload, bool include_prefix, bool retained, int qos)
{
    if (_debug) {
        Serial.print("PUB Info:");
        Serial.println(_pfi(topic, include_prefix) + " : " + payload);
    }

    return _client.publish(_pfi(topic, include_prefix), payload, retained, qos);
}
bool
EddieMQTT::publishInfoBool(const String &topic, const bool &payload, bool include_prefix, bool retained, int qos)
{
    String payload_s = BoolToStr(payload);

    if (_debug) {
        Serial.print("PUB Info Bool:");
        Serial.println(_pfi(topic, include_prefix) + " : " + payload_s);
    }

    return _client.publish(_pfi(topic, include_prefix), payload_s, retained, qos);
}
bool
EddieMQTT::publishInfoState(const String &topic, const bool &payload, bool include_prefix, bool retained, int qos)
{
    String payload_s = BoolToStr(payload, true);

    if (_debug) {
        Serial.print("PUB Info Bool:");
        Serial.println(_pfi(topic, include_prefix) + " : " + payload_s);
    }

    return _client.publish(_pfi(topic, include_prefix), payload_s, retained, qos);
}
bool
EddieMQTT::publishInfo(const String &topic, const float &payload, bool include_prefix, bool retained, int qos)
{
    String s_payload = String(payload);
    if (_debug) {
        Serial.print("PUB INFO float:");
        Serial.println(_pfi(topic, include_prefix) + " : " + s_payload);
    }

    return _client.publish(_pfi(topic, include_prefix), s_payload, retained, qos);
}
bool
EddieMQTT::publishInfo(const String &topic, const int &payload, bool include_prefix, bool retained, int qos)
{
    String s_payload = String(payload);

    if (_debug) {
        Serial.print("PUB INFO int:");
        Serial.println(_pfi(topic, include_prefix) + " : " + s_payload);
    }

    return _client.publish(_pfi(topic, include_prefix), s_payload, retained, qos);
}
bool
EddieMQTT::publishInfo(const String &topic, const unsigned int &payload, bool include_prefix, bool retained, int qos)
{
    String s_payload = String(payload);

    if (_debug) {
        Serial.print("PUB INFO uint:");
        Serial.println(_pfi(topic, include_prefix) + " : " + s_payload);
    }

    return _client.publish(_pfi(topic, include_prefix), s_payload, retained, qos);
}
bool
EddieMQTT::publishInfo(const String &topic, const long &payload, bool include_prefix, bool retained, int qos)
{
    String s_payload = String(payload);

    if (_debug) {
        Serial.print("PUB INFO long:");
        Serial.println(_pfi(topic, include_prefix) + " : " + s_payload);
    }

    return _client.publish(_pfi(topic, include_prefix), s_payload, retained, qos);
}
bool
EddieMQTT::publishInfo(const String &topic, const unsigned long &payload, bool include_prefix, bool retained, int qos)
{
    String s_payload = String(payload);

    if (_debug) {
        Serial.print("PUB INFO ulong:");
        Serial.println(_pfi(topic, include_prefix) + " : " + s_payload);
    }

    return _client.publish(_pfi(topic, include_prefix), s_payload, retained, qos);
}
bool
EddieMQTT::publishInfoF(const String &topic, const char *format, ...)
{
    String s_payload;

    char loc_buf[256];
    char * temp = loc_buf;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    size_t len = vsnprintf(NULL, 0, format, arg);
    va_end(copy);
    if(len >= sizeof(loc_buf)){
        temp = new char[len+1];
        if(temp == NULL) {
            return 0;
        }
    }
    len = vsnprintf(temp, len+1, format, arg);
    s_payload = temp;

    va_end(arg);
    if(len >= sizeof(loc_buf)){
        delete[] temp;
    }
    return _client.publish(_pfi(topic, true), s_payload, false, 0);
}


bool
EddieMQTT::PublishInfo(const String &topic, const String &payload, bool include_prefix, bool retained, int qos)
{
    if (_instance == nullptr)
        return false;

    return _instance->publishInfo(topic, payload, include_prefix, retained, qos);
}
bool
EddieMQTT::PublishInfoBool(const String &topic, const bool &payload, bool include_prefix, bool retained, int qos)
{
    if (_instance == nullptr)
        return false;

    return _instance->publishInfo(topic, payload, include_prefix, retained, qos);
}
bool
EddieMQTT::PublishInfoState(const String &topic, const bool &payload, bool include_prefix, bool retained, int qos)
{
    if (_instance == nullptr)
        return false;

    return _instance->publishInfoState(topic, payload, include_prefix, retained, qos);
}
bool
EddieMQTT::PublishInfo(const String &topic, const float &payload, bool include_prefix, bool retained, int qos)
{
    if (_instance == nullptr)
        return false;

    return _instance->publishInfo(topic, payload, include_prefix, retained, qos);
}
bool
EddieMQTT::PublishInfo(const String &topic, const int &payload, bool include_prefix, bool retained, int qos)
{
    if (_instance == nullptr)
        return false;

    return _instance->publishInfo(topic, payload, include_prefix, retained, qos);
}
bool
EddieMQTT::PublishInfo(const String &topic, const unsigned int &payload, bool include_prefix, bool retained, int qos)
{
    if (_instance == nullptr)
        return false;

    return _instance->publishInfo(topic, payload, include_prefix, retained, qos);
}
bool
EddieMQTT::PublishInfo(const String &topic, const long &payload, bool include_prefix, bool retained, int qos)
{
    if (_instance == nullptr)
        return false;

    return _instance->publishInfo(topic, payload, include_prefix, retained, qos);
}
bool
EddieMQTT::PublishInfo(const String &topic, const unsigned long &payload, bool include_prefix, bool retained, int qos)
{
    if (_instance == nullptr)
        return false;

    return _instance->publishInfo(topic, payload, include_prefix, retained, qos);
}
bool
EddieMQTT::PublishInfoF(const String &topic, const char *format, ...)
{
    if (_instance == nullptr)
        return false;

    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    bool r = _instance->publishInfoF(topic, format, arg);
    va_end(copy);

    return r;
}
#endif

void
EddieMQTT::subscribe(const String &topic, EddieMQTTCallback callback, bool include_prefix, int qos)
{
    if (_debug) {
        Serial.print("SUB:");
        Serial.println(_pf(topic, include_prefix));
    }

    EddieMQTTSubscriber sub = EddieMQTTSubscriber(_pf(topic, include_prefix), callback);
    sub.with_prefix = include_prefix;
    // Store Subscriber
    _subscribers.Add(sub);

    // Subscribe to the topic
    _client.subscribe(_pf(topic, include_prefix), qos);
}
#if EDDIEMQTT_USE_INFO
void
EddieMQTT::subscribeInfo(const String &topic, EddieMQTTCallback callback, bool include_prefix, int qos)
{
    if (_debug) {
        Serial.print("SUB Info:");
        Serial.println(_pfi(topic, include_prefix));
    }

    EddieMQTTSubscriber sub = EddieMQTTSubscriber(_pfi(topic, include_prefix), callback, true);
    sub.with_prefix = include_prefix;
    // Store Subscriber
    _subscribers.Add(sub);

    // Subscribe to the topic
    _client.subscribe(_pfi(topic, include_prefix), qos);
}
bool
EddieMQTT::SubscribeInfo(const String &topic, EddieMQTTCallback callback, bool include_prefix, int qos)
{
    if (_instance == nullptr)
        return false;

    _instance->subscribeInfo(topic, callback, include_prefix, qos);

    return true;
}
#endif
bool
EddieMQTT::Subscribe(const String &topic, EddieMQTTCallback callback, bool include_prefix, int qos)
{
    if (_instance == nullptr)
        return false;

    _instance->subscribe(topic, callback, include_prefix, qos);

    return true;
}

void
EddieMQTT::unsubscribe(const String &topic, EddieMQTTCallback callback, bool include_prefix)
{
    EddieMQTTSubscriber sub = EddieMQTTSubscriber(topic, callback);

    _subscribers.Remove(sub);

    // If there aren't any more subscribers with for the same topic (pattern) 
    //  unsubscribe from the server.
    if (_subscribers.GetIndex(sub, EddieMQTTSubscriber::PlainTopicCompare) == -1) {
        _client.unsubscribe(_pf(topic, include_prefix));
    }
}
bool
EddieMQTT::Unsubscribe(const String &topic, EddieMQTTCallback callback, bool include_prefix)
{
    if (_instance == nullptr)
        return false;

    _instance->unsubscribe(topic, callback, include_prefix);

    return true;
}

String
EddieMQTT::_pf(const String &str, bool add)
{
    if ( (_prefix != "") && add )
        return _prefix + "/" + str;
    return str;
}
#if EDDIEMQTT_USE_INFO
String
EddieMQTT::_pfi(const String &str, bool add)
{
    if ( (_prefix_info != "") && add )
        return _prefix_info + "/" + str;
    else if (add)
        return _pf(str);
    else
        return str; 
}
#endif

String
EddieMQTT::BoolToStr(bool value, bool as_onoff_state)
{
    if (as_onoff_state)
        return (value ? "on" : "off");
    else
        return (value ? "true" : "false");
}
bool
EddieMQTT::StrToBool(String value)
{
    value.toLowerCase();

    if ( (value == "on") || (value == "true") || (value == "1") )
        return true;
    else
        return false;
}

String
EddieMQTT::cleanTopic(const String &incomming, const String &prefix)
{
    String topic = incomming.substring(prefix.length());

    if (topic[0] == '/')
        topic = topic.substring(1);

    return topic;
}

void
EddieMQTT::_messageReceived(String &topic, String &payload)
{
    EddieMQTTSubscriber top(topic);
    String clean_topic;
    
    if (_debug) {
        Serial.print("MSG:");
        Serial.print(topic + " ");
        Serial.println(payload);
    } 

    // Get all subscribers with a matching topic
    LList<EddieMQTTSubscriber> subs = _subscribers.FindAll(top, EddieMQTTSubscriber::TopicCompare);

    Atom<EddieMQTTSubscriber> *ptr = subs.First;
    
    while (ptr != nullptr) {
#if EDDIEMQTT_USE_INFO
        if (ptr->Data.infoTopic)
            clean_topic = cleanTopic(topic, _prefix_info);
        else
            clean_topic = cleanTopic(topic, _prefix);
#else
        clean_topic = cleanTopic(topic, _prefix);
#endif

        ptr->Data.callback(clean_topic, payload, topic);
        
        ptr = ptr->Next;
    }
}
void
EddieMQTT::_onMessageReceived(String &topic, String &payload)
{
    if (_instance == nullptr)
        return;

    _instance->_messageReceived(topic, payload);
}