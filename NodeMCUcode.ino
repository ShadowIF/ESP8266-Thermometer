#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_INA219.h> // Correct library for INA219

// Wi-Fi credentials
const char* ssid = "RESPAWN_ZULI";
const char* password = "kwakwa5!";

// INA219 sensor setup
Adafruit_INA219 ina219;

int Rs = 9900;        // Fixed resistor in voltage divider
double Vcc = 5.07;    // Supply voltage
double Beta = 4600;   // Thermistor beta value

double AverageTemp() {
  double av = 0;
  for (int i = 0; i < 150; i++) {
    av += Thermister(ina219.getBusVoltage_V());
  }
  av = av / 150;
  return av;
}

double Thermister(double val) {
  //Serial.print("Bus Voltage (val): ");
  //Serial.println(val);
  double V_NTC = Vcc - val; // Voltage across thermistor
  //Serial.print("V_NTC: ");
  //Serial.println(V_NTC);
  double R_NTC = (Rs * V_NTC) / (Vcc - V_NTC); // Thermistor resistance
  //Serial.print("R_NTC: ");
  //Serial.println(R_NTC);
  long double Temp = (1 / ((1 / 298.15) + log(R_NTC / 200000) * (1 / Beta))) - 273.15;
  return Temp;
}

String getTemperature() {
  float temperature = AverageTemp();
  return String(temperature);
}

void ina219_init() {
  if (!ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }
  ina219.setCalibration_16V_400mA(); // Calibration for bus voltage up to 16V
  Serial.println("Measuring voltage with INA219 ...");
}

// WebSocket setup
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.println("Client connected");
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.println("Client disconnected");
  }
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Start INA219 sensor
  ina219_init();

  // Configure WebSocket
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.begin();
}

unsigned long lastTempReadTime = 0;
void loop() {
  unsigned long currentTime = millis();
  if (currentTime - lastTempReadTime >= 200) {
    String tempStr = getTemperature();
    ws.textAll(tempStr);
    //Serial.println("Temperature: " + tempStr + " °C");
    lastTempReadTime = currentTime;
  }
}