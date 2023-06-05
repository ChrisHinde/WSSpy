

#if DEBUG_LVL > 3
#define clog_d(x) Serial.print("D: "); Serial.print(x)
#define clogln_d(x) Serial.print("D: "); Serial.println(x)
#define clogf_d(...) Serial.print("D: "); Serial.printf(__VA_ARGS__)
#define clogf_d_(...) Serial.printf(__VA_ARGS__)
#else
#define clog_d(x)
#define clogln_d(x)
#define clogf_d(...)
#endif

#if DEBUG_LVL > 2
#define clog_i(x) Serial.print("I: "); Serial.print(x)
#define clogln_i(x) Serial.print("I: "); Serial.println(x)
#define clogf_i(...) Serial.print("I: "); Serial.printf(__VA_ARGS__)
#else
#define clog_i(x)
#define clogln_i(x)
#define clogf_i(...)
#endif

#if DEBUG_LVL > 1
#define clog_w(x) Serial.print("W: "); Serial.print(x)
#define clogln_w(x) Serial.print("W: "); Serial.println(x)
#define clogf_w(...) Serial.print("W: "); Serial.printf(__VA_ARGS__)
#else
#define clog_w(x)
#define clogln_w(x)
#define clogf_w(...)
#endif

#if DEBUG_LVL > 0
#define clog_e(x) Serial.print("ERROR: "); Serial.print(x)
#define clogln_e(x) Serial.print("ERROR: "); Serial.println(x)
#define clogf_e(...) Serial.print("ERROR: "); Serial.printf(__VA_ARGS__)
#else
#define clog_e(x)
#define clogln_e(x)
#define clogf_e(x)
#endif