#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//Variaveis de Personalizações
bool AllBigScreenElements = true;//Modifica o estilo das letras da tela dentre dois padrões
uint8_t CAL = 0;//Regula a temperatura, adicionando ou retirando do resultado final

// I2C compartilhado
#define PIN_OLED_SDA 21
#define PIN_OLED_SCL 22
#define PIN_ONEWIRE  4

// Um display: NUM (0x3C)
Adafruit_SSD1306 displayNum(128, 64, &Wire, -1); // 0x3C

OneWire oneWire(PIN_ONEWIRE);
DallasTemperature sensors(&oneWire);

// Modelo didático
float A_cal = 10.0f, B_cal = -1.8f;

// Histórico p/ gráfico
static const uint8_t W = 128, H = 64, H_TOP = 64, H_BOT = 32;
float histTemp[W];
uint8_t head = 0;

const uint32_t SAMPLE_MS = 250;
uint32_t lastSample = 0;

// Escalas do gráfico
const float TEMP_MIN_C = -10.0f, TEMP_MAX_C = 50.0f;

void setup() {
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);

  // Inicia displays em endereços distintos
  displayNum.begin(SSD1306_SWITCHCAPVCC, 0x3C);
 

  displayNum.clearDisplay(); displayNum.setTextColor(SSD1306_WHITE);
  

  sensors.begin();
  for (int i=0;i<W;i++){ histTemp[i]=0;}
}

void loop() {
  static float tempC= 0;
  uint32_t now = millis();

  // Amostras
  if (now - lastSample >= SAMPLE_MS) {
    lastSample = now;
    sensors.requestTemperatures();
    tempC = sensors.getTempCByIndex(0) - CAL;//Adicionar um subtrair aqui para modificar o resultado
    histTemp[head] = tempC;
    head = (head + 1) % W;
  }

  // -------- TELA 1 (NUM) em 0x3C --------
  if(AllBigScreenElements == true)
  {
      displayNum.clearDisplay();
      displayNum.setTextSize(2);
   //displayNum.setCursor(100,1); displayNum.print("NUM"); // rótulo debug
      displayNum.setCursor(0,16);
      displayNum.print("Temp:");  
      displayNum.print(tempC,1);//segundo argumento define as casas decimais
      displayNum.setTextSize(1);
      displayNum.setCursor(112,23);
      displayNum.print("C");  
  }
  else
  {
    displayNum.clearDisplay();
    displayNum.setTextSize(1);
   //displayNum.setCursor(100,1); displayNum.print("NUM"); // rótulo debug
    displayNum.setCursor(0,0);
    displayNum.print("Temperatura (Celcius)");  
    displayNum.setCursor(20,20);
    displayNum.setTextSize(4);
    displayNum.print(tempC,1);
    displayNum.setTextSize(1);
    displayNum.setCursor(0,32);
  }
  
 // displayNum.println("Unidade de Medida:   Celcius");
  displayNum.display();
 
}
