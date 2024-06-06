//Define Firebase Data object | Definir el objeto de datos de Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;
FirebaseJsonData jsonData;
// Insert your network credentials | Inserte sus credenciales de red
#define WIFI_SSID "MARIA"
#define WIFI_PASSWORD "M@r!@2023"
/*#define WIFI_SSID "ANDREA CAROLINA"
#define WIFI_PASSWORD "1095790680"*/
// Insert your network credentials | Inserte sus credenciales de red
#define WIFI_SSID1 "po"
#define WIFI_PASSWORD1 "endexe11"
// Insert your network credentials | Inserte sus credenciales de red

// Insert Firebase project API Key | Inserta la clave de API del proyecto de Firebase
#define API_KEY "AIzaSyChB8SAFKTAh7y4I6RdsW_RgdwDSJh1fEA"
// Inserte los valores de identificacion 
#define USER_EMAIL "endexe11@gmail.com"
#define USER_PASSWORD "Fabian1105"

// Insert RTDB URLefine the RTDB URL 
#define DATABASE_URL "https://led1-a76d6-default-rtdb.firebaseio.com/"
// Variable para guardar el UID de usuario
String uid;
//Ruta principal de la base de datos (que se actualizará en la configuración con el UID de usuario)
String ActivateExperiment;
String ResetExperiment;
String CalibrarExperiment;

String exportdatabasePath;
String exportExperiment;
int RuteExportExperiment = 0;

String importdatabasePath;
String importExperiment;
int RuteImportExperiment = 0;
// Database child nodes | Nodos secundarios de base de datos
String tempPath = "Temperatura_Actual";
String refPath = "Temperatura_Referencia";
//Nodos principal (que se actualizará en cada bucle)
String parentPath;

//Variables Activar Experimento 

int ExperimentoActivo = 0;
bool reset;
// variable tiempo real
int timestamp;
// Variabñes para la esp 32, Puente H y sensor de temperatura
#define enablePin  2
#define enablePin1 19
#define HBRIDGE_PIN_IN1 4
#define HBRIDGE_PIN_IN2 21
#define oneWireBus 5
//Variables para la funcion Millis
unsigned long previousMillis = 0;
const long interval = 5000; // Intervalo de retardo en milisegundos
unsigned long currentMillis = 0;

float temperaturaAmbiente = 27.0;
//Variables para el control PID
double temperaturaReferencia = 0.0; //Variable de temperatura de referencia 
double temperaturaActual = 0.0; // Variable de temperatura actual
double analogWriteValueCooler = 0.0; // Valor PWM para el ventilador
double analogWriteValueLED = 0.0; // Valor PWM para el LED
//Valores de operacion del PWM 
#define PWM_Min 0 //Valor de pwm minimo
#define PWM_Max 1024 //Valor del pwm maximo
//Variables para realizar el control PID
float Kp, Ki, Kd;

#define PID_Interval 1000

const int numReadings  = 15;
int readings [numReadings];
int readIndex  = 0;
float total  = 0.0;
double average;



#define Kp1  1.0 // Constante proporcional (ajústala según tus necesidades)
#define Ki1  0.5 // Constante integral (ajústala según tus necesidades)
#define Kd1  0.1 // Constante derivativa (ajústala según tus necesidades)

int CompExperimento; //Variable para comprobar cual experimento esta en funcionamiento

enum State {
  COOLING,
  PID_CONTROL
};

State currentState = COOLING;

enum ExperimentoEstado {
  Inactivo,
  Ejecutando,
  Completado,
  Error
};
ExperimentoEstado estadoExperimento1 = Inactivo;
ExperimentoEstado estadoExperimento2 = Inactivo;
ExperimentoEstado estadoExperimento3 = Inactivo;

