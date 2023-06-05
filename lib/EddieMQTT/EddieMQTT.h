#include <Arduino.h>
#include <WiFiClient.h>
#include <MQTT.h>

#pragma once

typedef void (*EddieMQTTCallback)(const String &topic, const String &payload, const String &orig_topic);

#ifndef EDDIEMQTT_MAXSUBS
#define EDDIEMQTT_MAXSUBS 16
#endif

#ifndef EDDIEMQTT_USE_INFO
#define EDDIEMQTT_USE_INFO 1
#endif

#define LLIST_MAX EDDIEMQTT_MAXSUBS
#include "llist.h"


#ifndef MQTT_REGEXP
#define MQTT_REGEXP "[a-zA-Z0-9_$]+"
#endif
#ifndef MQTT_BUFFER_LENGTH
#define MQTT_BUFFER_LENGTH 100
#endif
class EddieMQTTSubscriber
{
  public:
    String topic;
    EddieMQTTCallback callback;
    bool with_prefix = true;

    EddieMQTTSubscriber(const String& top = "N/A")
    {
        topic = top;
        callback = nullptr;
        with_prefix = true;
    }
    EddieMQTTSubscriber(const String& top, EddieMQTTCallback cb)
    {
        topic = top;
        callback = cb;
        with_prefix = true;
    }
    #if EDDIEMQTT_USE_INFO
    EddieMQTTSubscriber(const String& top, bool info)
    {
        topic = top;
        callback = nullptr;
        infoTopic = info;
        with_prefix = true;
    }
    EddieMQTTSubscriber(const String& top, EddieMQTTCallback cb, bool info)
    {
        topic = top;
        callback = cb;
        infoTopic = info;
        with_prefix = true;
    }
    bool infoTopic = false;
    #endif

    static bool TopicCompare(const EddieMQTTSubscriber& sub, const EddieMQTTSubscriber& in);
    static bool PlainTopicCompare(const EddieMQTTSubscriber& sub, const EddieMQTTSubscriber& in);
    static bool FullCompare(const EddieMQTTSubscriber& a, const EddieMQTTSubscriber& b);

    bool operator== (EddieMQTTSubscriber b);
};

class EddieMQTT
{
  public:
#if EDDIEMQTT_USE_INFO
    EddieMQTT(IPAddress ip, String user = "EddieMQTT", String dev_id = "eddimqtt", String prefix = "", String info_prefix = "", ushort port = 1883);
    EddieMQTT(String host, String user = "EddieMQTT", String dev_id = "eddimqtt", String prefix = "", String info_prefix = "", ushort port = 1883);
#else
    EddieMQTT(IPAddress ip, String user = "EddieMQTT", String dev_id = "eddimqtt", String prefix = "", ushort port = 1883);
    EddieMQTT(String host, String user = "EddieMQTT", String dev_id = "eddimqtt", String prefix = "", ushort port = 1883);
#endif
    EddieMQTT(EddieMQTT &&) = default;
    EddieMQTT(const EddieMQTT &) = default;
    EddieMQTT &operator=(EddieMQTT &&) = default;
    EddieMQTT &operator=(const EddieMQTT &) = default;
    ~EddieMQTT() {}
    
    void setDebug(bool debug = true) { _debug = debug; }

    void setPrefix(const String &prefix = "", bool add_slash = true);
    static bool SetPrefix(const String &prefix = "", bool add_slash = true);
    const String getPrefix();
    static const String GetPrefix();

#if EDDIEMQTT_USE_INFO
    void setInfoPrefix(const String &prefix = "", bool add_slash = true);
    static bool SetInfoPrefix(const String &prefix = "", bool add_slash = true);
    const String getInfoPrefix();
    static const String GetInfoPrefix();
#endif
    
    void setKeepAlive(int keepAlive);
    void setCleanSession(bool cleanSession);
    void setTimeout(int timeout);
    static bool SetKeepAlive(int keepAlive);
    static bool SetCleanSession(bool cleanSession);
    static bool SetTimeout(int timeout);

    void setOptions(int _keepAlive, bool _cleanSession, int _timeout);
    static bool SetOptions(int _keepAlive, bool _cleanSession, int _timeout);

    bool init(bool connect = true);
    bool connect();
    bool disconnect();
    bool loop();

    bool isConnected();
    
    //void publishIntroduction();

    void setWill(const String &topic, const String &payload = "", bool include_prefix = true, bool retained = false, int qos = 0);
#if EDDIEMQTT_USE_INFO
    void setWillInfo(const String &topic, const String &payload = "", bool include_prefix = true, bool retained = false, int qos = 0);
#endif
    void clearWill();
    
    bool publish(const String &topic, const String &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    bool publishBool(const String &topic, const bool &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    bool publishState(const String &topic, const bool &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    bool publish(const String &topic, const float &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    bool publish(const String &topic, const int &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    bool publish(const String &topic, const unsigned int &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    bool publish(const String &topic, const long &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    bool publish(const String &topic, const unsigned long &payload, bool include_prefix = true, bool retained = false, int qos = 0);


#if EDDIEMQTT_USE_INFO
    bool publishInfo(const String &topic, const String &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    bool publishInfoBool(const String &topic, const bool &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    bool publishInfoState(const String &topic, const bool &payload, bool include_prefix = true, bool retained = false, int qos = 0); 
    bool publishInfo(const String &topic, const float &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    bool publishInfo(const String &topic, const int &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    bool publishInfo(const String &topic, const unsigned int &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    bool publishInfo(const String &topic, const long &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    bool publishInfo(const String &topic, const unsigned long &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    bool publishInfoF(const String &topic, const char *format, ...);
#endif

    void subscribe(const String &topic, EddieMQTTCallback callback, bool include_prefix = true, int qos = 0);
#if EDDIEMQTT_USE_INFO
    void subscribeInfo(const String &topic, EddieMQTTCallback callback, bool include_prefix = true, int qos = 0);
#endif
    void unsubscribe(const String &topic, EddieMQTTCallback callback, bool include_prefix = true);

    static bool Loop();
    static bool Connect();
    static bool Disconnect();
    static bool IsConnected();

    static bool SetWill(const String &topic, const String &payload = "", bool include_prefix = true, bool retained = false, int qos = 0);
#if EDDIEMQTT_USE_INFO
    static bool SetWillInfo(const String &topic, const String &payload = "", bool include_prefix = true, bool retained = false, int qos = 0);
#endif
    static bool ClearWill();
    
    static bool Publish(const String &topic, const String &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    static bool PublishBool(const String &topic, const bool &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    static bool PublishState(const String &topic, const bool &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    static bool Publish(const String &topic, const float &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    static bool Publish(const String &topic, const int &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    static bool Publish(const String &topic, const unsigned int &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    static bool Publish(const String &topic, const long &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    static bool Publish(const String &topic, const unsigned long &payload, bool include_prefix = true, bool retained = false, int qos = 0);

#if EDDIEMQTT_USE_INFO
    static bool PublishInfo(const String &topic, const String &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    static bool PublishInfoBool(const String &topic, const bool &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    static bool PublishInfoState(const String &topic, const bool &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    static bool PublishInfo(const String &topic, const float &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    static bool PublishInfo(const String &topic, const int &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    static bool PublishInfo(const String &topic, const unsigned int &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    static bool PublishInfo(const String &topic, const long &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    static bool PublishInfo(const String &topic, const unsigned long &payload, bool include_prefix = true, bool retained = false, int qos = 0);
    static bool PublishInfoF(const String &topic, const char *format, ...);
#endif

    static bool Subscribe(const String &topic, EddieMQTTCallback callback, bool include_prefix = true, int qos = 0);
#if EDDIEMQTT_USE_INFO
    static bool SubscribeInfo(const String &topic, EddieMQTTCallback callback, bool include_prefix = true, int qos = 0);
#endif
    static bool Unsubscribe(const String &topic, EddieMQTTCallback callback, bool include_prefix = true);

    static String BoolToStr(bool value, bool as_onoff_state = false);
    static bool StrToBool(String value);

    String cleanTopic(const String &incomming, const String &prefix);

    MQTTClient _client;

    static String AddPrefix(const String &topic) { if (_instance == nullptr) { return topic; } return _instance->_pf(topic); }
#if EDDIEMQTT_USE_INFO
    static String AddInfoPrefix(const String &topic) { if (_instance == nullptr) { return topic; } return _instance->_pfi(topic); }
#endif

  private:
    static EddieMQTT* _instance;

    void _messageReceived(String &topic, String &payload);
    static void _onMessageReceived(String &topic, String &payload);

    IPAddress _server_ip;
    String _server_host;
    ushort _server_port;
    String _dev_id;
    String _user;

    bool _debug = false;

    String _pf(const String &str, bool add = true);
    String _prefix;
    bool _add_slash;

#if EDDIEMQTT_USE_INFO
    String _pfi(const String &str, bool add = true);
    String _prefix_info;
    bool _add_slash_info;
#endif

    WiFiClient _net;

    LList<EddieMQTTSubscriber> _subscribers; //[EDDIEMQTT_MAXSUBS];
};
