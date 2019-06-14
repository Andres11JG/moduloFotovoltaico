/************* LCD *************/
#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 8, 9, 10, 11, 12); //    ( RS, EN, d4, d5, d6, d7)

/************* Sensor DHT  *************/
#include "DHT.h"
#define DHTTYPE DHT22   // Tipo de sensor a usar
//#define DHTTYPE DHT11   // Descomentar si se usa el DHT 11
#define DHTPIN 2 // Pin donde está conectado el sensor

DHT dht(DHTPIN, DHTTYPE);

/************* Sensor de voltaje *************/
// Para mayor precision medir y remplazar los valores reales de R1 y R2
double R1 = 10000;
double R2 = 100000;

/************* Calculo de potencia *************/
double I = 0.0;
double V = 0.0;
double P = 0.0;

/************ Cruce por cero  ****************/
volatile int pinCruce = 4; // OutputCruce
int pinCheck = 9;
volatile double tiempo1 = 0.0;
volatile double tiempo2 = 0.0;
volatile double periodo = 0.0;
volatile double frecuencia = 0.0;
void setup()

{
  Serial.begin(9600);
  /************* Config LCD *************/
  lcd.begin(16, 2); // Fijar el numero de columnas y de filas de la LCD
  lcd.clear();

  /************* Config DHT *************/
  dht.begin();

  /*************** Config Cruce por cero ********************/
  pinMode(pinCruce, OUTPUT); // Genera pulsos por cada cruce
  pinMode(2, INPUT); // Genera pulsos por cada cruce
  attachInterrupt(0, cruceCero, RISING); // Interrupción 0, pin 2.
  EIMSK = (0 << INT0);
}

void loop() {
  /*
    medirCorriente();
    delay(1000);
    medirVoltaje();
    delay(1000);
    medirPotencia();
    delay(1000);
    medirTemperatura();
    delay(1000);
    medirHumedad();
    delay(1000);*/
  medirFrecuencia();
  //delay(1000);
}

void medirCorriente() {
  double sensibilidad = 0.066;  // Sensibilidad del sensor ACS712
  double corriente = ( (analogRead(A0) * 5.0 / 1023) - 2.5) / sensibilidad;
  I = corriente;
  imprimirMedida("Corriente", corriente, "A");
}

void medirVoltaje() {
  double Vo = (analogRead(A1) * 5.0 / 1023);  // Voltaje a la salida del divisor
  double Vi = ((R1 + R2) / R1) * Vo; // Divisor de voltaje para conocer el voltaje de entrada
  V = Vi;
  imprimirMedida("Voltaje", Vi, "V");
}

void medirPotencia() {
  P = V * I;
  imprimirMedida("Potencia", P, "Watts");
}

void medirTemperatura() {
  double t = dht.readTemperature(); // Lee temperatura en grados Celsius
  Serial.print(t);
  //float t = dht.readTemperature(true); //Descomentar para leer la temperatura en grados Fahrenheit
  imprimirMedida("Temperatura", t, "Celsius");
}

void medirHumedad() {
  double h = dht.readHumidity(); // Lee temperatura en grados Celsius
  imprimirMedida("Humedad", h, "%RH");
}

void medirFrecuencia() {
  periodo = 0;
  frecuencia = 0;
  tiempo1 = 0;
  tiempo2 = 0;
  EIMSK = (1 << INT0);
  while (frecuencia == 0);
  EIMSK = (0 << INT0);
  imprimirMedida("Frecuencia", frecuencia, "Hz");
  Serial.println(frecuencia);
  //delay(1000);

}

void imprimirMedida(String medida, double valor, String unidad) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(medida);
  lcd.setCursor(0, 1);
  lcd.print(valor);
  lcd.print(" ");
  lcd.print(unidad);
}

void cruceCero() {
  digitalWrite(pinCruce, HIGH);
  digitalWrite(pinCruce, LOW);
  tiempo2 = tiempo1 == 0 ? tiempo2 : millis();
  if (tiempo1 && tiempo2 != 0) {
    periodo = (tiempo2 - tiempo1) * 2;
    frecuencia = 1000 / periodo;
    tiempo1 = 0;
    tiempo2 = 0;
  }
  tiempo1 = tiempo2 == 0 ? millis() : tiempo1;
}
