// define only one of these two. Definir solamente uno de estos dos:
//#define CASA
#define INSTI

// define or not:
//#define TEST


#ifdef CASA
  const String ssidd = "vodafone6E30";
  const String passs = "EWF8MSVUFXL3WJ";
#define BROKER "192.168.0.26"
#endif
#ifdef INSTI
  const String ssidd = "Andared_Corporativo";
  const String userss = "18001147-sala-af"
  const String passs = "af12.jdm";
#define BROKER "192.168.13.169"
#endif
#ifdef TEST
const String ID = "prueba_13243";
const String CLIENT_NAME = ID + "test";
const String CLIENT_TELEMETRY_TOPIC = ID + "/telemetry";
#define BROKER "test.mosquitto.org"
#else
const String ID = "aa6ba08d-0ca2-41ec-8d4c-9e56112ff81f";
const String CLIENT_NAME = ID + "nightlight_client";
const String CLIENT_TELEMETRY_TOPIC = ID + "/telemetry";
#endif
