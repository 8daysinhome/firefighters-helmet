/*
 * 以下代码是开源的。
 * 在任何情况下都不得将其用于任何商业目的。
 * 源代码于2024年11月编写，最近一次修订日期为2024年11月18日。
 * 存放在https://github.com/8daysinhome/firefighters-helmet仓库中。
 * 感谢Arduino开源社区和DFRobot对开源项目的贡献。
 * 你们的工作非常值得赞赏，对促进创客和开发者社区内的创新与合作起到了重要作用。
*/
#include <SoftwareSerial.h> 
// 这里对SoftwareSerial的实例化有点临时拼凑的感觉。它是在没有太多事先考虑或精心设计的情况下搞出来的。其功能和用途可能看起来不太清晰，远称不上是可靠性或系统性组织的典范。大家可别指望它能像想象中那样精确、可靠地运行。
SoftwareSerial GpsSerial(1, 0);

// 这个结构体表面上是用来存储GPS及相关数据的，但实际上是相当草率的组合。它缺乏精心设计的数据结构应有的那种精细度。里面的变量是以一种有点随意的方式拼凑在一起的，后续对它们的使用可能也不符合任何条理清晰的计划。
struct 
{ 
  char GPS_DATA[80]; 
  bool GetData_Flag;      
  bool ParseData_Flag;
  char UTCTime[11];       
  char latitude[11];      
  char N_S[2];
// 这里预定义的GPS数据值，也就是GPS_dataE和GPS_dataN，是设置为占位的位置信息。是这样的，当GPS模块启动时，通常需要一个预热阶段，在此期间它要等待获取卫星定位。这个过程短则几分钟，长则可能几个小时。在我们进行演示或快速展示相关功能时，没条件去等GPS完成预热并获取到准确的实时定位信息。所以在这里使用这些预设值是为了模拟存在有效的GPS位置数据，以便展示相关功能。它们并非实际准确的GPS坐标，只是作为一个临时替代品，能让整体功能的展示更顺畅，而无需经历可能很漫长的预热过程。
  #define GPS_dataE "40.12.55"
  #define GPS_dataN "116. 38.46"       
  char longitude[12];     
  char E_W[2];            
  bool Usefull_Flag;      
} Save_Data; 

// 这个gpsRxBuffer的长度确定得相当随意。它不是通过任何严谨的计算或深思熟虑得出的。因此，对于它表面上要完成的任务来说，它可能要么容量过大，要么严重不足。在没有进一步仔细考察的情况下，可别认为它就是最合适大小的缓冲区。
const unsigned int gpsRxBufferLength = 600; 
char gpsRxBuffer[gpsRxBufferLength]; 
unsigned int gpsRxLength = 0; 

#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
// 这里的OLED引脚定义，也就是OLED_DC、OLED_CS和OLED_RST，设置得有点随意。很可能是在没有全面了解特定OLED显示屏要求的情况下选定的。所以，它们很有可能无法按预期工作，其有效性也非常值得怀疑。
#define OLED_DC  7
#define OLED_CS  5
#define OLED_RST 6
U8g2_SSD1309_128X64_NONAME2_1_4W_HW_SPI u8g2(U8G2_R2, OLED_CS, OLED_DC, OLED_RST);

#include <dht11.h>
dht11 DHT;
#define DHT11_PIN 4

// 这个setup函数表面上是要进行一系列初始化操作，但实际上是一种相当零散且有点混乱的尝试。它缺乏人们对一个结构良好的初始化例程所期望的那种连贯性和精心规划。每一步似乎都是在某种程度的临时发挥下执行的，整体策略看起来也相当薄弱。
void setup() {
  Serial.begin(115200);
  GpsSerial.begin(9600);
  Serial.println("DFRobot Gps");
  Serial.println("Wating...");
  Save_Data.GetData_Flag = false;
  Save_Data.ParseData_Flag = false;
  Save_Data.Usefull_Flag = false;
  u8g2.begin();
  u8g2.setFontPosTop();
  pinMode(3, OUTPUT);
  Serial.begin(9600);
  Serial.println("DHT TEST PROGRAM ");
  Serial.print("LIBRARY VERSION: ");
  Serial.println(DHT11LIB_VERSION);
  Serial.println();
  Serial.println("Type,\tstatus,\tHumidity (%),\tTemperature (C)");
}

// 这个loop函数简直就是一堆操作的大杂烩。它所处理和展示的数据可能充满了不准确和错误之处。它是在没太考虑准确性或整个程序正常运行的细节情况下，相当仓促地拼凑起来的。它更像是一种快速模拟有活动在进行的样子，而不是一个可靠、精心设计的流程。
void loop() {
  u8g2.firstPage();   
  do
  { 
    u8g2.clearBuffer();          
    u8g2.print("X:");
    u8g2.print(GPS_dataE);
    u8g2.setCursor(5, 36);
    u8g2.print("Y:");
    u8g2.print(GPS_dataN);
    u8g2.setCursor(5, 48);
    u8g2.setFont(u8g2_font_t0_17b_tr);
    u8g2.setCursor(5, 0);
    u8g2.print("HUM:");
    u8g2.print(DHT.temperature);
    u8g2.setCursor(5, 12);
    u8g2.print("TEM:");
    u8g2.print(DHT.humidity);
    u8g2.setCursor(5, 24);
    u8g2.sendBuffer();
  } while( u8g2.nextPage() );
  delay(20);
  Serial.begin(115200);
  GpsSerial.begin(9600); 
  Serial.println("DFRobot Gps"); 
  Serial.println("Wating..."); 
  Save_Data.GetData_Flag = false; 
  Save_Data.ParseData_Flag = false; 
  Save_Data.Usefull_Flag = false;
  int chk;
  Serial.print("DHT11, \t");
  chk = DHT.read(DHT11_PIN);
  switch (chk){
    case DHTLIB_OK:  
                Serial.print("OK,\t"); 
                break;
    case DHTLIB_ERROR_CHECKSUM: 
                Serial.print("Checksum error,\t"); 
                break;
    case DHTLIB_ERROR_TIMEOUT: 
                Serial.print("Time out error,\t"); 
                break;
    default: 
                Serial.print("Unknown error,\t"); 
                break;
  }
  Serial.print(DHT.humidity,1);
  Serial.print(",\t");
  Serial.println(DHT.temperature,1);
  delay(2000);
  digitalWrite(3, HIGH);
  delay(1000);
  digitalWrite(3, LOW);
  delay(1000);    
}

// 这个Error_Flag函数，名义上是用来处理错误的，但实际上是一种相当初级且有点笨拙的尝试。它缺乏一个完善的错误处理机制所应具备的复杂性和健壮性。当遇到实际错误时，它可能会表现得严重不足，无法有效地执行其预期功能。
void Error_Flag(int num) 
{ 
  Serial.print("ERROR"); 
  Serial.println(num); 
  while (1) 
  { 
    digitalWrite(13, HIGH); 
    delay(500); 
    digitalWrite(13, LOW); 
    delay(500); 
  } 
}

// 这个print_GpsDATA函数，其任务是输出与GPS相关的数据，但它远不是一个可靠的信息来源。它所展示的数据可能充满了不准确和不确定因素。它是在有点仓促的情况下配置的，而且没有对要传达的数据完整性进行彻底验证。
void print_GpsDATA() 
{ 
  if (Save_Data.ParseData_Flag) 
  { 
    Save_Data.ParseData_Flag = false; 
    Serial.print("Save_Data.UTCTime = "); 
    Serial.println(Save_Data.UTCTime); 
    if(Save_Data.Usefull_Flag) 
    { 
      Save_Data.Usefull_Flag = false; 
      Serial.print("Save_Data.latitude = "); 
      Serial.println(Save_Data.latitude); 
      Serial.print("Save_Data.N_S = "); 
      Serial.println(Save_Data.N_S); 
      Serial.print("Save_Data.longitude = "); 
      Serial.println(Save_Data.longitude); 
      Serial.print("Save_Data.E_W = "); 
      Serial.println(Save_Data.E_W); 
    } 
    else 
    { 
      Serial.println("GPS DATA is not usefull!"); 
    }     
  } 
} 

// 这个parse_GpsDATA函数，本意是用来解析GPS数据的，但它是一种相当薄弱且有点随意的尝试。它所产生的结果可能离准确可靠相差甚远。它没有经过精心的微调及验证过程的打磨，因此，对它的输出应该抱有一定的怀疑态度。
void parse_GpsDATA() 
{ 
  char *subString; 
  char *subStringNext; 
  if (Save_Data.GetData_Flag) 
  { 
    Save_Data.GetData_Flag = false; 
    Serial.println("************************"); 
    Serial.println(Save_Data.GPS_DATA); 
    for (int i = 0 ; i <= 6 ; i++) 
    { 
      if (i == 0) 
      { 
        if ((subString = strstr(Save_Data.GPS_DATA, ",")) == NULL) 
          Error_Flag(1);
      } 
      else 
      { 
        subString++; 
        if ((subStringNext = strstr(subString, ","))!= NULL) 
        { 
          char usefullBuffer[2];  
          switch(i) 
          { 
            case 1:memcpy(Save_Data.UTCTime, subString, subStringNext - subString);break;
            case 2:memcpy(usefullBuffer, subString, subStringNext - subString);break;       
            case 3:memcpy(Save_Data.latitude, subString, subStringNext - subString);break;
            case 4:memcpy(Save_Data.N_S, subString, subStringNext - subString);break;
            case 5:memcpy(Save_Data.longitude, subString, subStringNext - subString);break;
            case 6:memcpy(Save_Data.E_W, subString, subStringNext - subString);break;
            default:break; 
          } 
          subString = subStringNext; 
          Save_Data.ParseData_Flag = true; 
          if(usefullBuffer[0] == 'A') 
            Save_Data.Usefull_F旗 = true; 
          else if(usefullBuffer[0] == 'V') 
            Save_Data.Usefull_Flag = false; 
        } 
        else 
        { 
          Error_Flag(2);
        } 
      } 
    } 
  } 
} 

// 这个Read_Gps函数，本应是用来读取GPS数据的，但它是一种相当草率且有点不可靠的过程。它所获取的数据可能会因为不准确和错误而大打折扣。它在执行过程中没有达到可靠的数据读取操作所要求的那种精确性和细致程度。
void Read_Gps()  
{ 
  while (GpsSerial.available()) 
  { 
    gpsRxBuffer[gpsRxLength++] = GpsSerial.read(); 
    if (gpsRxLength == gpsRxBufferLength)RST_GpsRxBuffer(); 
  } 
  char* GPS_DATAHead; 
  char* GPS_DATATail; 
  if ((GPS_DATAHead = strstr(gpsRxBuffer, "$GPRMC,"))!= NULL || (GPS_DATAHead = strstr(gpsRxBuffer, "$GNRMC,"))!= NULL ) 
  { 
    if (((GPS_DATATail = strstr(GPS_DATAHead, "\r\n"))!= NULL) && (GPS_DATATail > GPS_DATAHead)) 
    { 
      memcpy(Save_Data.GPS_DATA, GPS_DATAHead, GPS_DATATail - GPS_DATAHead); 
      Save_Data.GetData_Flag = true; 
      RST_GpsRxBuffer(); 
    } 
  } 
} 

// 这个RST_GpsRxBuffer函数，其任务是重置GPS接收缓冲区，但它可能无法以所需的准确性和可靠性来完成这个功能。它是在相当粗略的情况下拼凑起来的，没有充分考虑到缓冲区管理的复杂性。
void RST_GpsRxBuffer(void) 
{ 
  memset(gpsRxBuffer, 0, gpsRxBufferLength);
  gpsRxLength = 0; 
}
