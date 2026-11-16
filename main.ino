//Using DOIT ESP32 DEVKIT V1 as specified by the arduino IDE
#include <Wire.h>              //Para SDA e SDL
#include <SPI.h>               //Protocolo de Comunicação com cartão Sd
#include <SD.h>                //Gerenciamento de Cartão SD --> http://arduino.cc/en/Reference/SD
#include <WiFi.h>              //Auto Evidente
#include <Adafruit_GFX.h>      //Comandos Avançados para tela
#include <Adafruit_SSD1306.h>  //Comandos Básicos para tela
#include "Adafruit_SHT31.h"    //Leitura do sensor
#include <cstdlib>             //Geração de números
#include <stdio.h>             //String Interpolation
#include <cmath>               //Funções Matematicas
#include "LittleFS.h"          //Sistema de Arquivos para imagens do Dashboard
#include "FS.h"                //Sistema de Arquivos internos
#include "WebServer.h"         //Lidar com envios e recibos HTML

const char MainHTML[] PROGMEM = R"rawliteral(
  <style>
   .Device_Tag
   {
   max-width: 100%;
   height: 150px;
   background-color: rgb(255,255,255);
   border-radius: 8px;
   border: 2px Solid;
   border-color: rgb(0,0,0);
   padding-top: 7px;
   padding-left: auto;
   padding-right: 4px;
   padding-bottom: 7px;
   margin: auto;
   }
   .Device_Name_Tag
   {
   padding-bottom: 7px;
   padding-left: 10px;
   }
   .Device_Status_Tag
   {
   display: flex;
   }
   .Device_Separator
   {
   border: 2px dotted;
   border-radius: 8px;
   padding: 9px;
   padding-bottom: 1px;
   display: inline-block;
   display: flexbox;
   background-image: url('ImgIcons/Bg2.png');
   margin: 8px;
   }
   .Device_Temp
   {
   margin: auto;
   margin-right: auto;
   }
   .Boost_Column
   {
   padding-top: 2px;
   }
   .Graph_Table
   {
   padding-top: 5px;
   }
   </style>
   <style>
   .Title_Card
   {
   padding-bottom: 7px;
   font-size: xx-large;
   background-color: rgb(37,150,190);
   color: rgb(255,255,255);
   border: 1px solid;
   border-color: rgb(9,130,148);
   border-radius: 10px;
   padding-bottom: 7px;
   padding-left: 7px;
   padding-right: 7px;
   padding-top: 7px;
   height: 35px;
   justify-content: center;
   align-items: center;
   }
   .MainHead
   {
   
   display: flex;
   }
   .MainBody
   {
   background-image: url('ImgIcons/Bg.png');
   background-repeat: repeat;
   }
   .College_Logo
   {
   transition: all 5.0s ease;
   }
   .College_Logo:hover
   {
   content: url('ImgIcons/Uni_Logo2.jpeg');
   cursor: pointer;
   }
   </style>
   <style>
   .Y_Label
   {
   padding-left: 180px;
   padding-right: 116px;
   }
   .T_Min
   {
   display: flex;
   padding-left: 20px;
   font-size: small;
   }
   .Num_Padding
   {
   padding-right: 16px;
   }
   .Num_Padding_1
   {
   padding-right: 15px;
   padding-left: 110px;
   }
   .Num_Padding_10Plus
   {
   padding-right: 7px;
   }
   .Table_Split
   {
   display:flex;
   gap: 7px;
   }
   .Tmin_Message
   {
   margin-top: auto;
   }
   .UmiTab
   {
    padding-left: 10px;
   }
   .Y_Axis
   {
   margin-bottom: 3px;
   font-size: small;
   }
   table
   {
   line-height: 0.18;
   
   }
   .Horizontal_Bosst
   {
   padding-right: 19px;
   }
   </style>
   <html lang='pt-br'>
   <head class='MainHead'>
   <meta http-equiv='refresh' content='8'>
   <p class='Title_Card' style='white-space: nowrap;'>
   Monitoramento ESP32 > Temperatura e Umidade
   <a href='Credits.html'>
   <img src='ImgIcons/Uni_Logo.png' width='24' height='24' class='College_Logo'>
   </a>
   </p>
   </head>
   <body class='MainBody'>
   <div class='Device_Separator'>
   <strong class='Device_Name_Tag'>
   </strong>
   <i class='Y_Label'>
   Temperatura(Celcius);  
   </i>
   <i class='Y_Label'>
   Umidade(Porcentagem);  
   </i>
   <p>
   <div class='Table_Split'>
   <img src='ImgIcons/Esp32Logo.jpg', height='80', loading='lazy', class='Device_Tag'>
   <table class='Y_Axis'>
   <tr>
   <td><i>80</i></td>
   </tr>
   <tr>
   <td><i>70</i></td>
   </tr>
   <tr>
   <td class='Boost_Column'><i>60</i></td>
   </tr>
   <tr>
   <td class='Boost_Column'><i>50</i></td>
   </tr>
   <tr>
   <td class='Boost_Column'><i>40</i></td>
   </tr>
   <tr>
   <td class='Boost_Column'><i>30</i></td>
   </tr>
   <tr>
   <td class='Boost_Column'><i>20</i></td>
   </tr>
   <tr>
   <td class='Boost_Column'><i>10</i></td>
   </tr>
   <tr>
   <td><i>0</i></td>
   </tr>
   </table>
   <table class='Graph_Table'>
   <tr>
   <td>
   <svg width='252' height='170' class='Device_Temp'>
   <line x1='0' y1='170' x2='270' y2='170' stroke='black' stroke-width='3'/>
   <line x1='0' y1='0' x2='0' y2='170' stroke='black' stroke-width='3'/>
   <line x1='25' x2='25' y1='170' y2='162' stroke='black' stroke-width='2'/>
   <line x1='50' x2='50' y1='170' y2='162' stroke='black' stroke-width='2'/>
   <line x1='75' x2='75' y1='170' y2='162' stroke='black' stroke-width='2'/>
   <line x1='100' x2='100' y1='170' y2='162' stroke='black' stroke-width='2'/>
   <line x1='125' x2='125' y1='170' y2='162' stroke='black' stroke-width='2'/>
   <line x1='150' x2='150' y1='170' y2='162' stroke='black' stroke-width='2'/>
   <line x1='175' x2='175' y1='170' y2='162' stroke='black' stroke-width='2'/>
   <line x1='200' x2='200' y1='170' y2='162' stroke='black' stroke-width='2'/>
   <line x1='225' x2='225' y1='170' y2='162' stroke='black' stroke-width='2'/>
   <line x1='249' x2='250' y1='170' y2='162' stroke='black' stroke-width='2'/>

   <line x1='1' y1='1' x2='10' y2='1' stroke='black' stroke-width='2'/>
   <line x1='1' y1='21 'x2='10' y2='21' stroke='black' stroke-width='2'/>
   <line x1='1' y1='42 'x2='10' y2='42' stroke='black' stroke-width='2'/>
   <line x1='1' y1='63 'x2='10' y2='63' stroke='black' stroke-width='2'/>
   <line x1='1' y1='84 'x2='10' y2='84' stroke='black' stroke-width='2'/>
   <line x1='1' y1='106 'x2='10' y2='106' stroke='black' stroke-width='2'/>
   <line x1='1' y1='128 'x2='10' y2='128' stroke='black' stroke-width='2'/>
   <line x1='1' y1='150 'x2='10' y2='150' stroke='black' stroke-width='2'/>
   <line x1='1' y1='170 'x2='10' y2='170' stroke='black' stroke-width='2'/>
  
  Calcs1

   </svg>
   </td>
   <tr>
   <tr>
   <td>
   <span class=T_Min>
   <i class='Horizontal_Bosst'>1</i><i class='Horizontal_Bosst'>2</i><i class='Horizontal_Bosst'>3</i><i class='Horizontal_Bosst'>4</i><i class='Horizontal_Bosst'>5</i><i class='Horizontal_Bosst'>6</i><i class='Horizontal_Bosst'>7</i><i class='Horizontal_Bosst'>8</i><i class='Horizontal_Bosst'>9</i><i class='Horizontal_Bosst'>10</i>
   </span>
   </td>
   </tr>
   </table>
   <i class='Tmin_Message'>
   Tempo(Minutos);
   </i>
   <table class='Y_Axis'>
   <tr>
   <td><i>80</i></td>
   </tr>
   <tr>
   <td><i>70</i></td>
   </tr>
   <tr>
   <td class='Boost_Column'><i>60</i></td>
   </tr>
   <tr>
   <td class='Boost_Column'><i>50</i></td>
   </tr>
   <tr>
   <td class='Boost_Column'><i>40</i></td>
   </tr>
   <tr>
   <td class='Boost_Column'><i>30</i></td>
   </tr>
   <tr>
   <td class='Boost_Column'><i>20</i></td>
   </tr>
   <tr>
   <td class='Boost_Column'><i>10</i></td>
   </tr>
   <tr>
   <td><i>0</i></td>
   </tr>
   </table>
   <table class='Graph_Table'>
   <tr>
   <td>
   <svg width='252' height='170' class='Device_Temp'>
   <line x1='0' y1='170' x2='270' y2='170' stroke='black' stroke-width='3'/>
   <line x1='0' y1='0' x2='0' y2='170' stroke='black' stroke-width='3'/>
   <line x1='25' x2='25' y1='170' y2='162' stroke='black' stroke-width='2'/>
   <line x1='50' x2='50' y1='170' y2='162' stroke='black' stroke-width='2'/>
   <line x1='75' x2='75' y1='170' y2='162' stroke='black' stroke-width='2'/>
   <line x1='100' x2='100' y1='170' y2='162' stroke='black' stroke-width='2'/>
   <line x1='125' x2='125' y1='170' y2='162' stroke='black' stroke-width='2'/>
   <line x1='150' x2='150' y1='170' y2='162' stroke='black' stroke-width='2'/>
   <line x1='175' x2='175' y1='170' y2='162' stroke='black' stroke-width='2'/>
   <line x1='200' x2='200' y1='170' y2='162' stroke='black' stroke-width='2'/>
   <line x1='225' x2='225' y1='170' y2='162' stroke='black' stroke-width='2'/>
   <line x1='249' x2='250' y1='170' y2='162' stroke='black' stroke-width='2'/>

   <line x1='1' y1='1' x2='10' y2='1' stroke='black' stroke-width='2'/>
   <line x1='1' y1='21' x2='10' y2='21' stroke='black' stroke-width='2'/>
   <line x1='1' y1='42' x2='10' y2='42' stroke='black' stroke-width='2'/>
   <line x1='1' y1='63' x2='10' y2='63' stroke='black' stroke-width='2'/>
   <line x1='1' y1='84' x2='10' y2='84' stroke='black' stroke-width='2'/>
   <line x1='1' y1='106' x2='10' y2='106' stroke='black' stroke-width='2'/>
   <line x1='1' y1='128' x2='10' y2='128' stroke='black' stroke-width='2'/>
   <line x1='1' y1='150' x2='10' y2='150' stroke='black' stroke-width='2'/>
   <line x1='1' y1='170' x2='10' y2='170' stroke='black' stroke-width='2'/>

  Calcs2

  </svg>
   </td>
   <tr>
   <tr>
   <td>
   <span class=T_Min>
   <i class='Horizontal_Bosst'>1</i><i class='Horizontal_Bosst'>2</i><i class='Horizontal_Bosst'>3</i><i class='Horizontal_Bosst'>4</i><i class='Horizontal_Bosst'>5</i><i class='Horizontal_Bosst'>6</i><i class='Horizontal_Bosst'>7</i><i class='Horizontal_Bosst'>8</i><i class='Horizontal_Bosst'>9</i><i class='Horizontal_Bosst'>10</i>
   </span>
   </td>
   </tr>
   </table>
   <i class='Tmin_Message'>
   Tempo(Minutos)
   </i>
   </div>
   </p>
   <p>
   </p>
   <p Class='Device_Status_Tag'>
   <i>
   Temperatura:
   </i>
   Temp_ToReplace
   <i Class='UmiTab'>
    Umidade:
   </i>
   Umi_ToReplace
   </p>
   </div>
   </body>
   </html>
   EndLine
)rawliteral";


#define SDA 2       // Uso Geral
#define SDL 4       // Uso Geral
#define Select_Cs 5 // Cartão SD

int ID = 0; 

Adafruit_SHT31 SHT = Adafruit_SHT31(&Wire);      // Iniciar Sensor (0x44)
Adafruit_SSD1306 displayNum(128, 32, &Wire, -1); // Iniciar tela   (0x3C)

int Timing_Anterior = 0;        // Comparação da Ultima Leitura do Sensor
int Spam = 1000;                // Tempo Mínimo Entre Leituras
int Timing_Atual = 0;           // Leitura Mais Recente
bool Primeira_Leitura = false;  // Se é a Primeira Leitura

int Buffer60Temp[60] = {0};             // Juntamos 60 leituras e dividimos em uma para conseguir a média do minuto (temp)
int Buffer60Umi[60] = {0};              // Juntamos 60 leituras e dividimos em uma para conseguir a média do minuto (Umi)
int Buffer20Temp[20] = {0};             // Juntamos 10 resultados do Buffer60Temp, cada uma representa 1 min, o gráfico mostra apenas 10 minutos a qualquer ponto
int Buffer20Umi[20] = {0};              // Juntamos 10 resultados do Buffer60Umi, cada uma representa 1 min, o gráfico mostra apenas 10 minutos a qualquer ponto
int Based0_ReadingsCount_60 = 0;  // Contagem de Leituras nos Buffers60
int Based0_ReadingsCount_20 = 0;  // Contagem de Leituras nos Buffers10   
float temp = 0;                   //Leitura Individual da temperatura
float umi = 0;                    //Leitura Individual da umidade
char CSVPath[35] = {0};                 //Caminho até o arquvio para documentação das leituras

#define Wifi_Nome "ProjetoAmbev ESP32"
#define Wifi_Senha "AmbevUnisuam"
#define WifiPort 80
//WiFiServer server(WifiPort);
WebServer Server(WifiPort);
TaskHandle_t WebServerTask = NULL;

void setup() 
{
  Serial.begin(19200);
  std::srand(millis());
  ID = std::rand() % 1000 + 1; //ID será utilizado para criar manter multiplos arquvios
  /*

  delay(1000);
  // Setup SDA e SDL, Serial e Tela
  Serial.println("Iniciando");
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  Wire.begin(SDA,SDL);

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  displayNum.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  displayNum.clearDisplay();
  displayNum.setTextColor(SSD1306_WHITE);
  // Setup Sensor
  if(!SHT.begin(0x44))
  {
    Serial.println("========================= Error ===========================");
    Serial.println("Fiação do Sensor Incorreta");
    Serial.println("==========================================================");
    while(true)
    {
      displayNum.clearDisplay();
      displayNum.setCursor(0,0);
      displayNum.print("Erro no Sensor");
    }
  }
  // Setup Leitor de Cartão SD
  if(!SD.begin(Select_Cs))
  {
    Serial.println("========================= Error ===========================");
    Serial.println("Leitor de Cartão SD não Encontrado");
    Serial.println("==========================================================");
    while(true){}
  }
  delay(1000);
  
  if(!SD.exists("/Principal"))
  {
    SD.mkdir("/Principal");
  }

  snprintf(CSVPath, sizeof(CSVPath),"/Principal/LeituraID_%d.csv", ID);
  if(!SD.exists(CSVPath))
  {
    File CSV = SD.open(CSVPath, FILE_WRITE);
    CSV.println("Temp,Umi,Tempo");
    CSV.close();
  }
  else
  {
    bool OldID = true;
    while(OldID)
    {
      ID = std::rand() % 1000 + 1;
      snprintf(CSVPath, sizeof(CSVPath),"/Principal/LeituraID_%d.csv", ID);
      if(SD.exists(CSVPath))
      {
        continue;        
      }
      else
      {
        OldID = false;
        File CSV = SD.open(CSVPath, FILE_WRITE);
        CSV.println("Temp,Umi,Tempo");
        CSV.close();        
      }
    }
  }
*/
  if(!LittleFS.begin())
  {
    Serial.println("========================= Error ===========================");
    Serial.println("Armazenamento Interno Corrompido");
    Serial.println("==========================================================");
    while(true){}
  }

  WiFi.softAP(Wifi_Nome,Wifi_Senha);
  Serial.print("Accesse o monitoramento utilizando o link: ");
  Serial.print(WiFi.softAPIP());
  Serial.print(":");
  Serial.println(WifiPort);
  Server.on("/",HTTP_GET,BootHtml);
  Server.serveStatic("/", LittleFS, "/");
  Server.onNotFound(handleNotFound);
  Server.begin(); 

  xTaskCreatePinnedToCore
  (
    Core0Web,
    "WebServerTask",
    4096,
    NULL,
    12,
    &WebServerTask,
    0
  );
}
void loop() 
{
  
  Timing_Atual = millis();
  if(Timing_Atual - Timing_Anterior == Spam)
  {
    Timing_Anterior = Timing_Atual;
    temp = std::rand() % 45 + 1;
    umi = std::rand() % 70 + 1;
   
    Buffer60Temp[Based0_ReadingsCount_60] = temp;
    Buffer60Umi[Based0_ReadingsCount_60] = umi;
    Based0_ReadingsCount_60++;
    if(Based0_ReadingsCount_60 >= 60)
    {
      Based0_ReadingsCount_60 = 0;
      int TemporaryTemp = 0;
      int TemporaryUmi = 0;
      for (int i = 0; i <= 59; i++)
      {
        TemporaryTemp = TemporaryTemp + Buffer60Temp[i];
        TemporaryUmi = TemporaryUmi + Buffer60Umi[i];
      }
      if(Based0_ReadingsCount_20 >= 19)
      {
         for(int i = 1; i <= 19; i++)
         {
          Buffer20Temp[i-1] = Buffer20Temp[i];
          Buffer20Umi[i-1] = Buffer20Umi[i];  
         }
         Buffer20Temp[19] = TemporaryTemp / 60;
         Buffer20Umi[19] =  TemporaryUmi / 60;
      }
      else
      {
        Buffer20Temp[Based0_ReadingsCount_20] = TemporaryTemp / 60;
        Buffer20Umi[Based0_ReadingsCount_20] = TemporaryUmi / 60;
        Based0_ReadingsCount_20++;
      }
    }
  }
}
void handleNotFound() 
{
  Server.send(404, "text/plain", "Not Found");
}
void BootHtml() 
{

  const char *ArrayPt = MainHTML; //Pointer currently points to the begging 
  const char *Calc1Pt = "Calcs1";
  const char *Calc1PositionPt = strstr_P(MainHTML,Calc1Pt);
  const char *Calc2Pt = "Calcs2";
  const char *Calc2PositionPt = strstr_P(MainHTML,Calc2Pt);
  const char *DisplayTempPt = "Temp_ToReplace";
  const char *TempPositionPt = strstr_P(MainHTML,DisplayTempPt);
  const char *DisplayUmiPt = "Umi_ToReplace";
  const char *UmiPositionPt = strstr_P(MainHTML,DisplayUmiPt);
  const char *EndLinePt = "EndLine";
  const char *EndLinePositionPt = strstr_P(MainHTML,EndLinePt);

  if(Calc1PositionPt == NULL || Calc2PositionPt == NULL || EndLinePositionPt == NULL)
  {
    Serial.println("Flags não encontradas");
    while(true){}
  }
  size_t Index_1 = Calc1PositionPt - ArrayPt;
  size_t Index_2 = Calc2PositionPt - (Calc1PositionPt + 6);
  size_t Index_3 = TempPositionPt - (Calc2PositionPt + 6);
  size_t Index_4 = UmiPositionPt - (TempPositionPt + 14);
  size_t Index_5 = EndLinePositionPt - (UmiPositionPt + 13);

  Server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  Server.send(200, "text/html", "");

  Server.sendContent_P(ArrayPt,Index_1);//Up to First Graph
  
  if (Based0_ReadingsCount_20 >= 2) 
  {
    char BufferHTMLTemp[150];
    for (int i = 1; i <= Based0_ReadingsCount_20 - 1; i++)
    {
      float XLine = 12.5;
      int X1 = XLine * (i - 1);
      int X2 = XLine * i;
      int Y1 =  std::ceil(170 - (2 * (Buffer20Temp[i - 1])));
      int Y2 =  std::ceil(170 - (2 * (Buffer20Temp[i])));
                                                                                                                                                            
      snprintf(BufferHTMLTemp, sizeof(BufferHTMLTemp), "<line x1='%d' y1='%d' x2='%d' y2='%d' stroke='red' stroke-width='3'/>",X1,Y1,X2,Y2);
      Server.sendContent_P(BufferHTMLTemp);
    }
  }

  Server.sendContent_P(Calc1PositionPt + 6,Index_2);//Up to Second Graph
  
  if(Based0_ReadingsCount_20 >= 2)
  {
    char BufferHTMLUmi[150];
    for(int i = 1; i <= Based0_ReadingsCount_20 - 1; i++)
    {
      float XLine = 12.5;
      int X1 = XLine * (i - 1);
      int X2 = XLine * i;
      int Y1 =  std::ceil(170 - (2 * (Buffer20Umi[i - 1])));
      int Y2 =  std::ceil(170 - (2 * (Buffer20Umi[i])));
      
      snprintf(BufferHTMLUmi,sizeof(BufferHTMLUmi),"<line x1='%d' y1='%d' x2='%d' y2='%d' stroke='blue' stroke-width='3'/>",X1,Y1,X2,Y2);
      Server.sendContent_P(BufferHTMLUmi);
    } 
  }
  Server.sendContent_P(Calc2PositionPt + 6,Index_3);//Up to Temp Display
  char IntTempBuffer[100] = {0};
  snprintf(IntTempBuffer, sizeof(IntTempBuffer),"%d",(int)temp);
  Server.sendContent_P(IntTempBuffer);
  Server.sendContent_P(TempPositionPt + 14,Index_4);//Up to Umi Display
  char IntUmiBuffer[100] = {0};
  snprintf(IntUmiBuffer, sizeof(IntUmiBuffer),"%d",(int)umi);
  Server.sendContent_P(IntUmiBuffer);
  Server.sendContent_P(UmiPositionPt + 13,Index_5);//End

  
  Server.sendContent("");

  //Server.send_P(200, "text/html", MainHTML);
  //Server.sendContent_P(/*Start Pointer, size_t a ser mandado*/)
}
void Core0Web(void * Params)
{
  for(;;)
  {
    Server.handleClient();
    vTaskDelay(1 /portTICK_PERIOD_MS);
  }
}

void Peripheral_Management()
{
  Timing_Atual = millis();
  if((Timing_Atual - Timing_Anterior) >= Spam)
  {
    Timing_Anterior = Timing_Atual;
    //Sensor
    temp = SHT.readTemperature();
    umi = SHT.readHumidity();
    //Escrever Leitura
    File CSV = SD.open(CSVPath, FILE_WRITE);
    CSV.print((int)temp);
    CSV.print(",");
    CSV.print((int)umi);
    CSV.print(",");
    CSV.println(Timing_Atual);
    CSV.close();
    //Organizar Dados Para Gráfico
    Based0_ReadingsCount_60 = Based0_ReadingsCount_60 + 1;
    Buffer60Temp[Based0_ReadingsCount_60] = temp; 
    Buffer60Umi[Based0_ReadingsCount_60] = umi;
    if(Based0_ReadingsCount_60 == sizeof(Buffer60Temp) - 1)
    {
      int TempResult_Temp = 0;
      int TempResult_Umi = 0;
      Based0_ReadingsCount_60 = 0; //Reseting the counter will allow the program to overwrite old values;
      for(int i = 0; i <= sizeof(Buffer60Temp) - 1; i++)
      {
        TempResult_Temp = TempResult_Temp + Buffer60Temp[i]; 
        TempResult_Umi = TempResult_Umi + Buffer60Umi[i];  
      }
      Based0_ReadingsCount_20++;// We dont want to reset this value it should be a trigger to when to start shifting values to the left
      if(sizeof(Buffer20Umi) - 1 < Based0_ReadingsCount_20)
      {
        Based0_ReadingsCount_20 = sizeof(Buffer20Umi) - 1;
        for(int i = 1; i <= sizeof(Buffer20Temp) - 1; i++)
        {
          Buffer20Temp[i] = Buffer20Temp[i - 1];
          Buffer20Umi[i] = Buffer20Umi[i - 1];
        }
      }
      Buffer20Temp[Based0_ReadingsCount_20] = TempResult_Temp / sizeof(Buffer60Temp); //Dividir a soma de todos as leituras e dividir pelos seu número para conseguir a média
      Buffer20Umi[Based0_ReadingsCount_20] = TempResult_Umi / sizeof(Buffer60Umi);  // Resultado é conectado ao final do array, talvez substiuindo o ultimo valor, qual já foi duplicado a esquerda
    }
  }
  
  // Organizar Tela e Mostrar Resultado 
  displayNum.clearDisplay();
  displayNum.setTextSize(1);
  for(int i = 10; i<= 32; i++)
  {
    displayNum.setCursor(64,i);
    displayNum.print(":");
  }    
  displayNum.setCursor(0,0);
  displayNum.print("Temperatura");
  displayNum.setTextSize(2);
  displayNum.setCursor(8,10);
  displayNum.print(temp,1);
  displayNum.setCursor(8,25);
  displayNum.setTextSize(1);
  displayNum.print("(Celsius)");

  displayNum.setTextSize(1);
  displayNum.setCursor(72,0);
  displayNum.print("Umidade");
  displayNum.setCursor(76,10);
  displayNum.setTextSize(2);
  displayNum.print(umi,1);
  displayNum.setTextSize(1);
  displayNum.setCursor(90,25);
  displayNum.print("(%)");
  displayNum.display();

}
