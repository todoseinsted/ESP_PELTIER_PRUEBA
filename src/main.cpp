#include <WiFi.h>
#include <WebServer.h>

// Datos de tu red WiFi
const char* ssid = "FiberCorp WiFi758";
const char* password = "30716644789";

// IP fija del ESP32
IPAddress local_IP(192, 168, 0, 200);

// Datos típicos del router
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

// DNS
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

// Servidor web en puerto 80
WebServer server(80);

// LED azul integrado del ESP32 DevKit
const int LED_AZUL = 2;

// Variables para parpadeo sin bloquear el programa
unsigned long tiempoAnteriorLed = 0;
const unsigned long intervaloLed = 500; // milisegundos
bool estadoLed = false;

void handleRoot() {
  String html = "";

  html += "<!DOCTYPE html>";
  html += "<html>";
  html += "<head>";
  html += "<meta charset='UTF-8'>";
  html += "<title>ESP32 WiFi Test</title>";
  html += "<style>";
  html += "body { font-family: Arial; background: #f2f2f2; padding: 30px; }";
  html += ".card { background: white; padding: 25px; border-radius: 12px; max-width: 500px; margin: auto; box-shadow: 0 0 10px #ccc; }";
  html += "h1 { color: #1b7f3a; }";
  html += "p { font-size: 18px; }";
  html += "</style>";
  html += "</head>";
  html += "<body>";
  html += "<div class='card'>";
  html += "<h1>ESP32 WiFi funcionando</h1>";
  html += "<p>El ESP32 se conectó correctamente a la red WiFi.</p>";
  html += "<p><b>Red WiFi:</b> ";
  html += ssid;
  html += "</p>";
  html += "<p><b>IP del ESP32:</b> ";
  html += WiFi.localIP().toString();
  html += "</p>";
  html += "<p><b>Señal WiFi RSSI:</b> ";
  html += String(WiFi.RSSI());
  html += " dBm</p>";
  html += "<p><b>Tiempo encendido:</b> ";
  html += String(millis() / 1000);
  html += " segundos</p>";
  html += "</div>";
  html += "</body>";
  html += "</html>";

  server.send(200, "text/html", html);
}

void handleNotFound() {
  server.send(404, "text/plain", "Pagina no encontrada");
}

void parpadearLedWifi() {
  if (WiFi.status() == WL_CONNECTED) {
    unsigned long tiempoActual = millis();

    if (tiempoActual - tiempoAnteriorLed >= intervaloLed) {
      tiempoAnteriorLed = tiempoActual;

      estadoLed = !estadoLed;
      digitalWrite(LED_AZUL, estadoLed);
    }
  } else {
    digitalWrite(LED_AZUL, LOW);
    estadoLed = false;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_AZUL, OUTPUT);
  digitalWrite(LED_AZUL, LOW);

  Serial.println();
  Serial.println("Iniciando ESP32...");

  WiFi.mode(WIFI_STA);

  // Configurar IP fija antes de conectarse
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("Error configurando IP fija");
  }

  Serial.print("Conectando a la red WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int intentos = 0;

  while (WiFi.status() != WL_CONNECTED && intentos < 30) {
    delay(500);
    Serial.print(".");
    intentos++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi conectado correctamente");
    Serial.print("IP del ESP32: ");
    Serial.println(WiFi.localIP());
    Serial.print("RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");

    // Parpadeo rápido inicial para indicar conexión exitosa
    for (int i = 0; i < 6; i++) {
      digitalWrite(LED_AZUL, HIGH);
      delay(150);
      digitalWrite(LED_AZUL, LOW);
      delay(150);
    }

  } else {
    Serial.println("No se pudo conectar al WiFi");
    Serial.println("Revisar nombre de red, contraseña o configuración de IP");
    digitalWrite(LED_AZUL, LOW);
    return;
  }

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Servidor web iniciado");
  Serial.println("Abrir en el navegador:");
  Serial.println("http://192.168.0.200");
}

void loop() {
  server.handleClient();

  parpadearLedWifi();

  // Si se desconecta del WiFi, intenta reconectar
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado. Reintentando...");
    digitalWrite(LED_AZUL, LOW);
    WiFi.reconnect();
    delay(5000);
  }
}