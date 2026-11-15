#include <Wire.h>
#include <Adafruit_INA219.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT22
#define RELAY_GRID 7
#define RELAY_LOAD 8
#define PWM_TEG_PIN 9
#define PWM_SOLAR_PIN 10

Adafruit_INA219 ina_teg(0x40);
Adafruit_INA219 ina_solar(0x41);
Adafruit_INA219 ina_batt(0x44);
DHT dht(DHTPIN, DHTTYPE);

const float V_BATT_LOW = 11.82;
const float V_BATT_HIGH = 12.10;
bool gridBackup = false;
int tegDuty = 128;
int solarDuty = 128;
float prevSolarPower = 0.0;

void setup() {
  Serial.begin(115200);
  ina_teg.begin();
  ina_solar.begin();
  ina_batt.begin();
  dht.begin();
  pinMode(RELAY_GRID, OUTPUT);
  pinMode(RELAY_LOAD, OUTPUT);
  pinMode(PWM_TEG_PIN, OUTPUT);
  pinMode(PWM_SOLAR_PIN, OUTPUT);
  digitalWrite(RELAY_GRID, LOW);
  digitalWrite(RELAY_LOAD, HIGH);
}

void loop() {
  float vTeg = ina_teg.getBusVoltage_V();
  float pTeg = ina_teg.getPower_mW() / 1000.0;
  float vSol = ina_solar.getBusVoltage_V();
  float pSol = ina_solar.getPower_mW() / 1000.0;
  float vBatt = ina_batt.getBusVoltage_V();
  float iBatt = ina_batt.getCurrent_mA() / 1000.0;
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (vTeg < 12.4 && tegDuty < 230) tegDuty += 2;
  else if (vTeg > 12.6 && tegDuty > 20) tegDuty -= 2;
  analogWrite(PWM_TEG_PIN, tegDuty);

  if (pSol > prevSolarPower && solarDuty < 240) solarDuty += 2;
  else if (pSol < prevSolarPower && solarDuty > 20) solarDuty -= 2;
  prevSolarPower = pSol;
  analogWrite(PWM_SOLAR_PIN, solarDuty);

  if (vBatt <= V_BATT_LOW && !gridBackup) {
    digitalWrite(RELAY_GRID, HIGH);
    gridBackup = true;
  } else if (vBatt >= V_BATT_HIGH && gridBackup) {
    digitalWrite(RELAY_GRID, LOW);
    gridBackup = false;
  }

  Serial.print("{\"v_batt\":"); Serial.print(vBatt, 2);
  Serial.print(",\"i_batt\":"); Serial.print(iBatt, 2);
  Serial.print(",\"p_teg\":"); Serial.print(pTeg, 2);
  Serial.print(",\"p_sol\":"); Serial.print(pSol, 2);
  Serial.print(",\"temp\":"); Serial.print(isnan(temp) ? 28.0 : temp, 1);
  Serial.print(",\"hum\":"); Serial.print(isnan(hum) ? 62.0 : hum, 1);
  Serial.print(",\"grid\":"); Serial.print(gridBackup ? 1 : 0);
  Serial.println("}");

  delay(1000);
}
