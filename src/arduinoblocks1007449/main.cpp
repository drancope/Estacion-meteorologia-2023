/* arduinoblocks.com */
/* project: Educacont IoT Easy Plug v2alm255 */
/* author: Antonio Contreras */

#include <SoftwareSerial.h>
#include "ABlocksIOTMQTTESP8266.h"
#include <HardwareSerial.h>
#include "ABlocks_DHT.h"
#include "priv_datareal.h"

double humedad;
double temperatura;
double no2_ug_m3;
double particulas_ug_m3;
double co_mg_m3;
const char mqtt_broker[]=BROKER;
const int mqtt_port=1883;
const char mqtt_user[]="estacion";
const char mqtt_pass[]="meteorologica";
const char mqtt_clientid[]="estacion2";
const char mqtt_wifi_ssid[]= SSIDD;
const char mqtt_wifi_pass[]= PASSS;
HardwareSerial &mqtt_esp8266_serial=Serial;
ESP8266 mqtt_esp8266_wifi(&mqtt_esp8266_serial);
char mqtt_payload[64];
DHT dht9(9,DHT22);

void subir_datos_a_la_nube() {
	ABlocksIOT.Publish(String("channels/1741307/publish/fields/field1/A7AMO9PNVFX8NKK2"),String(humedad));
	delay(2000);
	ABlocksIOT.Publish(String("channels/1741307/publish/fields/field2/A7AMO9PNVFX8NKK2"),String(temperatura));
	delay(2000);
	ABlocksIOT.Publish(String("channels/1741307/publish/fields/field3/A7AMO9PNVFX8NKK2"),String(no2_ug_m3));
	delay(2000);
	ABlocksIOT.Publish(String("channels/1741307/publish/fields/field4/A7AMO9PNVFX8NKK2"),String(particulas_ug_m3));
	delay(2000);
	ABlocksIOT.Publish(String("channels/1741307/publish/fields/field5/A7AMO9PNVFX8NKK2"),String(co_mg_m3));
	delay(2000);
}

double mqtt_payload2double(unsigned char *_payload, int _length){
	int i;
	for (i = 0; i<_length && i<64; i++){
		mqtt_payload[i] = _payload[i];
	}
	mqtt_payload[i] = 0;
	return atof(mqtt_payload);
}

String mqtt_payload2string(unsigned char *_payload, int _length){
	int i;
	for (i = 0; i<_length && i<64; i++){
		mqtt_payload[i] = _payload[i];
	}
	mqtt_payload[i] = 0;
	return String(mqtt_payload);
}
void mqtt_callback(char* _topic, unsigned char* _payload, unsigned int _payloadlength){
	double v=mqtt_payload2double(_payload,_payloadlength);
	String vt=mqtt_payload2string(_payload,_payloadlength);
}

void mqtt_subscribe(){
}

void subir_a_mosquitto() {
	ABlocksIOT.Publish(String("estacion1/temperatura"), String(temperatura));
	ABlocksIOT.Publish(String("estacion1/humedad"), String(humedad));
	ABlocksIOT.Publish(String("estacion1/no2"), String(no2_ug_m3));
	ABlocksIOT.Publish(String("estacion1/particulas"), String(particulas_ug_m3));
	ABlocksIOT.Publish(String("estacion1/co"), String(co_mg_m3));
}
void detener_medicion_nn_minutos() {
	for (int count = 0; count < (60 * 1); count++) { // segundos * minutos
		delay(1000);
	}
}
void fnc_mics4514_preheat(int _prepin)
{
	pinMode(_prepin, OUTPUT);
	digitalWrite(_prepin, 1);
	delay(50000);
	digitalWrite(_prepin, 0);
}

double fnc_mics4514(int _prepin, int _noxpin, int _redpin, int _result, double _temp)
{
	double vnox_value = analogRead(_noxpin);
	double VPIN_NO2 = vnox_value*5.0/1023.0;
	double R0_NO2 = 2200.0;
	double RS_NO2 = ((5.0*22000.0)/VPIN_NO2)-22000.0;
	double ppbNO2 = 0.0473538*(RS_NO2/R0_NO2)-60.4778+50.8654* pow( (RS_NO2/R0_NO2), 0.327898 );
	double ug_m3NO2 = ppbNO2*1.88;

	double vred_value = analogRead(_redpin);
	double VPIN_CO = vred_value*5.0/1023.0;
	double R0_CO = 750000.0;
	double RS_CO = ((5.0*47000.0)/VPIN_CO)-47000.0;
	double FactorT = 250.0/(3313.44/_temp+194.11-2.51*_temp);
	double RS_COT = RS_CO;
	if(_temp > -999) RS_COT = FactorT*RS_CO;
	double ppmCO = -75.6518+exp(-143.735*(RS_COT/R0_CO))+41.1398/(RS_COT/R0_CO)+27.3354*(RS_COT/R0_CO);
	double mg_m3CO = ppmCO*1.14;

	if(_result==0)return max(0.0,ppbNO2);
	if(_result==1 && (VPIN_CO >= 0.4))return max(0.0,ppmCO);
	if(_result==2)return max(0.0,ug_m3NO2);
	if(_result==3 && (VPIN_CO >= 0.4))return max(0.0,mg_m3CO);
	if(_result==4)return max(0.0,VPIN_NO2);
	if(_result==5)return max(0.0,VPIN_CO);
	return 0.0;
}

double fnc_pm25(int _ledpin, int _outpin, int _result)
{
	digitalWrite(_ledpin,LOW);
	delayMicroseconds(280);
	double voMeasured = analogRead(_outpin);
	delayMicroseconds(40);
	digitalWrite(_ledpin,HIGH);
	delayMicroseconds(9680);
	double calcVoltage = voMeasured * (5.0/ (1023.0+1.0) );
	double ug_m3PM = 1000*(0.172*calcVoltage - 0.0999);
	if(_result==1)return max(0.0,ug_m3PM);
	if(_result==2)return calcVoltage;
	return 0.0;
}

void leer_sensores() {
	humedad = dht9.readHumidity();
	temperatura = dht9.readTemperature();
	no2_ug_m3 = fnc_mics4514(8,A0,A1,2,-999);
	particulas_ug_m3 = fnc_pm25(2,A6,1);
	co_mg_m3 = fnc_mics4514(8,A0,A1,3,temperatura);
}

void setup()
{
  	pinMode(9, INPUT);
	pinMode(8, OUTPUT);
	pinMode(A0, INPUT );
	pinMode(A1, INPUT);
	pinMode(A6, INPUT);
	pinMode(2, OUTPUT);

	Serial.begin(9600);
	Serial.flush();
	while(Serial.available()>0)Serial.read();

	dht9.begin();
	fnc_mics4514_preheat(8);

	mqtt_esp8266_serial.begin(115200);
	ABlocksIOT.begin(mqtt_broker,mqtt_port, mqtt_user,mqtt_pass, mqtt_clientid, mqtt_esp8266_wifi, mqtt_wifi_ssid, mqtt_wifi_pass, mqtt_callback, mqtt_subscribe);

}


void loop()
{
	ABlocksIOT.loop();
  	leer_sensores();
  	Serial.println(temperatura);
  	subir_a_mosquitto();
  	detener_medicion_nn_minutos();

}