#include <Adafruit_MPU6050.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <math.h>

Adafruit_MPU6050 mpu;
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);

float totalAcceleration = 0;

int pasos = 0;


const float umbral = 2; // Ajstar el umbral como sensibilidad
const int bufferLength = 15; // Numero de lecturas de la aceleracion en el bufer.
float buffer[bufferLength]; // Le asigna la longitud al buffer como matriz.
int bufferIndex = 0; // Indice inicial 
bool pasoDetectado = false;
const unsigned long tiempoPaso = 900; // El tiempo mas o menos a ojo de cuanto se demora entre paso y paso.
unsigned long pasoAnterior = 0;


void setup() {
  Serial.begin(115200);
  
  pinMode(18, OUTPUT);
  // while (!Serial);
  Serial.println("MPU6050 OLED demo");

  if (!mpu.begin()) {
    Serial.println("Sensor init failed");
    while (1)
      yield();
  }
  Serial.println("Found a MPU-6050 sensor");

  // SSD1306_SWITCHCAPVCC = segun documentacion, generar un voltage de 3.3v internamente
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // direccion 0x3C para 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // No continuar, quedarse en el loop para siempre
  }
  display.display();
  delay(500); // Pausa por 2 segundos
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setRotation(2);
}

void reset(){
  pasos = 0;
  delay(100);
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  display.clearDisplay();
  display.setCursor(29, 0);
  /*
  display.print(a.acceleration.x + 9.1, 1);
  display.print(", ");
  display.print(a.acceleration.y - 0.4, 1);
  display.print(", ");
  display.print(a.acceleration.z + 1.8, 1);
  display.println("");
*/
  totalAcceleration = sqrt(pow(a.acceleration.x + 9.1, 2)+pow(a.acceleration.y - 0.4, 2) + pow(a.acceleration.z + 1.8, 2));
  float accelerationY = a.acceleration.y - 0.4;
  
  //pisoton = reiniciar
  display.println("Pasos: ");
  if(pasos < 10){
    display.setCursor(55, 30);
  }
  else{
    display.setCursor(49, 30);
  }
  display.println(pasos);
  if(abs(a.acceleration.x + 0.4) >= 20){
    reset(); // Si la aceleracion (positva o negativa) es mayor a 40 (Pisotón) se resetea
  }

  buffer[bufferIndex] = accelerationY; //Una matriz que guarda los valores de la aceleración recientes.
  bufferIndex = (bufferIndex + 1) % bufferLength; 
  /*Posición actual de la matriz,
   utiliza el modulo para asegurarse de que 
   este dentro de la longitud de la matriz.
   En este caso la longitud es 15 (Definido al inicio)

   Va a ser 0 hasta que llegue a 15 basicamente y después se reinicia.
  */

  float aceleracionPromedio = 0; //Variable de promedio de la magnitud de la aceleracion.

  for (int i = 0; i < bufferLength; i++) {
    aceleracionPromedio += buffer[i];
  } // Este loop suma a la magnitud promedio todas las lecturas de la aceleración que se estaban guardando en el buffer
  aceleracionPromedio /= bufferLength; // Después de sumarlas todas, las divide entre 15, el número de lecturas.

  unsigned long millisRecorridos = millis(); //Numero de milisegundos que han pasado desde el update()

  if (accelerationY > (aceleracionPromedio + umbral)) { //SI la aceleracion es mayor al promedio + tiempoPaso arbitrario (definido al inicio)
    if (!pasoDetectado && (millisRecorridos - pasoAnterior) > tiempoPaso) { 
    /* Y SI NO ha detectado un paso previamente

       Y la diferencia entre el tiempo que ha pasado desde que inicio y el ultimo paso
       es mayor a 900ms (que es lo que se demora en dar un paso aprox. segun la gráfica)

       Detecta un paso.
       Súmale 1 paso

       Y ahora el tiempo que ha pasado desde el ultimo paso pasa a ser el tiempo que a transcurrido en total. 

    */
      pasoDetectado = true;
      pasoAnterior = millisRecorridos;
      pasos += 1;
    }
  } else { 
    /* Si la aceleración sobrepasa el tiempoPaso incluso después de promediarse, 
    pero no ha pasado suficiente tiempo desde el paso anterior, No se detecta como paso
    */
    pasoDetectado = false;
  }
  

  display.display();
  delay(100);
}