#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Firebase_ESP_Client.h>
#include <AutoPID.h>
#include "Variables.h"
#include <cfloat>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// Iniciar WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

// Función que obtiene la hora de época actual
unsigned long getTime() {
  timeClient.update();
  unsigned long now = timeClient.getEpochTime();
  return now;
}

//Incializacion del sensor 
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

//Control PID
  AutoPID ledPID(&temperaturaActual, &temperaturaReferencia, &analogWriteValueLED, PWM_Min, PWM_Max, Kp, Ki, Kd);
  //AutoPID coolerPID(&temperaturaActual, &temperaturaReferencia, &analogWriteValueCooler, PWM_Min, PWM_Max, Kp, Ki, Kd);

// funcion de configuracion de Firebase
void FirebaseConfi() {
  // Assign the api key (required) | Asignar la clave de API (obligatorio)
  config.api_key = API_KEY;
  // Assign the user sign in credentials | Asignar las credenciales de inicio de sesión del usuario
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  // Assign the RTDB URL (required) | Asignar la URL de RTDB (obligatorio)
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  //Assign the callback function for the long running token generation task 
  config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h
  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;
  // Initialize the library with the Firebase authen and config  
  Firebase.begin(&config, &auth);
  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);
  // Update database path | Actualizar las rutas de acceso de la base de datos
  ActivateExperiment = "/UsersData/" + uid + "/Activar_Experimento";
  ResetExperiment = "/UsersData/" + uid + "/Resetear_Experimento"; 
  CalibrarExperiment = "/UsersData/" + uid + "/Calibrar_Experiment";
  exportdatabasePath = "/UsersData/" + uid + "/Muestras_Temperatura";
  importdatabasePath = "/UsersData/" + uid + "/Valores_Referencia";
}

//Funcion configuracion de pines Esp 32
void ConfigPins() {
  pinMode(HBRIDGE_PIN_IN1, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(HBRIDGE_PIN_IN2, OUTPUT);
  pinMode(enablePin1, OUTPUT);

  // Inicialmente, establecer IN1 y Enable a LOW (ventilador apagado)
  digitalWrite(HBRIDGE_PIN_IN1, LOW);
  analogWrite(enablePin, 0); // Habilitar el puente H con PWM (0% inicialmente)
  // Inicialmente, establecer IN2 y Enable1 a LOW (LED apagado)
  digitalWrite(HBRIDGE_PIN_IN2, LOW);
  analogWrite(enablePin1, 0); // Habilitar el puente H con PWM (0% inicialmente)
}

void setup() {
  Serial.begin(115200);
  //iniciar Wifi
  initWiFi();
  //Inciar Obtencion de la Hora
  timeClient.begin();
  //Iniciar Configuracion con firebase
  FirebaseConfi();
  //Configurar pines del microcontrolador
  ConfigPins();
  //Configurar Intervalos del controlador PID
  ledPID.setTimeStep(PID_Interval);
  // Iniciar el sensor DS18B20
  sensors.begin();
}

//Funcion Que realiza la lectura de la temperatura
void LeerTemperatura() {
  // Solicitar la temperatura actual
  sensors.requestTemperatures();
  temperaturaActual = sensors.getTempCByIndex(0);
  total = total - readings[readIndex];
  // read the sensor:
  readings[readIndex] = temperaturaActual;
  // add value to total:
  total = total + readings[readIndex];
  // handle index
  readIndex = readIndex + 1;
  if (readIndex >= numReadings) {
    readIndex = 0;
  }
  // calculate the average:
  average = total / numReadings;
}
//Exportacion de las rutas definidas por cada experimento
void ExportLineRute(int Exp) {
  RuteExportExperiment = Exp;
  if (RuteExportExperiment == 1) {
    exportExperiment = exportdatabasePath + "/Experimento_1";
    parentPath = exportExperiment + "/" + String(timestamp);
  } else if (RuteExportExperiment == 2) {
    exportExperiment = exportdatabasePath + "/Experimento_2";
    parentPath = exportExperiment + "/" + String(timestamp);
  } else if (RuteExportExperiment == 3) {
    exportExperiment = exportdatabasePath + "/Experimento_3";
    parentPath = exportExperiment + "/" + String(timestamp);
  } else if (RuteExportExperiment < 1 || RuteExportExperiment > 3) {
    Serial.println("Error en la asignacion de ruta");
  }
}
//Lectura de la base de datos donde se almacenan los valores de referencia
void LeerFirebase() {
  //Solicitar a la base de datos la temperatura de referencia Kp, Ki y Kd
  if (Firebase.RTDB.getJSON(&fbdo, importExperiment)) {
    if (fbdo.dataType() == "json") {
      FirebaseJson &jsonObject = fbdo.jsonObject();
      if (jsonObject.get(jsonData, "temperaturaReferencia")) {
        temperaturaReferencia = jsonData.floatValue;
      }
      if (jsonObject.get(jsonData, "Kp")) {
        Kp = jsonData.floatValue; 
      }
      if (jsonObject.get(jsonData, "Ki")) {
        Ki = jsonData.floatValue;
      }
      if (jsonObject.get(jsonData, "Kd")) {
        Kd = jsonData.floatValue;
      } else {
        Serial.println(fbdo.errorReason());
      }
    }
  } else {
    Serial.println(fbdo.errorReason());
  }
}
//Funcion en la que se importan las rutas donde se guardara la informacion muestreada
void ImportLineRute(int Exp) {
  RuteImportExperiment = Exp;
  if (RuteImportExperiment == 1) {
    importExperiment = importdatabasePath + "/Experimento1";
  } else if (RuteImportExperiment == 2) {
    importExperiment = importdatabasePath + "/Experimento2";
  } else if (RuteImportExperiment == 3) {
    importExperiment = importdatabasePath + "/Experimento3";
  } else if (RuteImportExperiment < 1 || RuteImportExperiment > 3) {
    Serial.println("Error en la asignacion de ruta");
  }
}
//funcion donde se le manda a la base de datos los valores muestreados
void EscribirFirebase() {
  //Mandar a la base de datos la températura Actual
  json.set(tempPath.c_str(), float (temperaturaActual));
  json.set(refPath.c_str(), float (temperaturaReferencia));
  Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
}
//Funcion donde se realica el control PID, por medio de la libreria AutoPID
void ControlPID(){
    //insertar Valores de Kp, Ki y Kd en el control PID
  ledPID.setGains(Kp, Ki, Kd);
  ledPID.run(); // Controlador PID para el LED
  //coolerPID.run(); // Controlador PID para el cooler
  analogWrite(enablePin1, analogWriteValueLED);
  digitalWrite(HBRIDGE_PIN_IN2, HIGH);

  /*if(temperaturaActual > (temperaturaReferencia+0.5)){
  analogWrite(enablePin, PWM_Max);
  digitalWrite(HBRIDGE_PIN_IN1, HIGH);
  }
  else{
  analogWrite(enablePin, PWM_Min);
  digitalWrite(HBRIDGE_PIN_IN1, LOW);
  }*/
}
//Funcion donde se realiza el control del ventilador para reestablecer el sistema 
void ControlCooler() {
   if (temperaturaActual > temperaturaAmbiente) {
    analogWrite(enablePin, PWM_Max);
    digitalWrite(HBRIDGE_PIN_IN1, HIGH);
  } else {
    analogWrite(enablePin, PWM_Min);
    digitalWrite(HBRIDGE_PIN_IN1, LOW);
    currentState = PID_CONTROL;
  }
}
//Funcion que controla los actuadores, que son el cooler y el led
void controlActuador() {
switch (currentState) {
      case COOLING:
        ControlCooler();
        break;
      case PID_CONTROL:
        ControlPID();
        break;
    }
}
//Funcion donde se ejecuta el experimento asignado
void experimento(bool comprob, int ActiExperimento) {
  bool comprobacion = comprob;
  CompExperimento = ActiExperimento;
  if (comprobacion == true) {
    ExportLineRute(CompExperimento);
    ImportLineRute(CompExperimento);
    LeerFirebase();
    LeerTemperatura();
    controlActuador();
    EscribirFirebase();
  } else {
    Serial.println("Experimento Desactivado");
  }
}
//Funcion donde se elige el experimento que se va a realizar
void ElegirExperiment() {
  if (Firebase.RTDB.getInt(&fbdo, ActivateExperiment)) {
    if (fbdo.dataType() == "int") {
      ExperimentoActivo = fbdo.intData();
      if (ExperimentoActivo == 1 || ExperimentoActivo == 2 || ExperimentoActivo == 3) {
        experimento(true, ExperimentoActivo);
      } else {
        experimento(false, ExperimentoActivo);
      }
    } else {
      Serial.println("Error en el tipo de dato de Firebase");
    }  
  } else {
    Serial.println(fbdo.errorReason());
  }
}
//Funcion donde se inicia el experimento por medio de el boton de iniciar de la app web
void Activate() {
  if (Firebase.ready()) {
    if (Firebase.RTDB.getBool(&fbdo, ResetExperiment)) {
      reset = fbdo.boolData();
      if (reset == true) {
        ElegirExperiment();
      } else {
        reset = false;
        Firebase.RTDB.setBool(&fbdo, ResetExperiment, &reset);
        ESP.restart();
      }
    } else {
      Serial.println(fbdo.errorReason());
    }
  }
}

void loop() {
  //Get current timestamp | Obtener la marca de tiempo actual
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    timestamp = getTime();
    Activate();
  }
}





