//#include <SoftwareDBG_OUTPUT_PORT.h>
#include "LedControl.h"

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
WebServer server(80);

const char* ssid = "41a iPhone";
const char* password = "19880911";
const char* host = "esp32";
#define DBG_OUTPUT_PORT Serial
#include <ArduinoJson.h>

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 10 is connected to the DataIn 
 pin 8 is connected to the CLK 
 pin 9 is connected to LOAD 
 ***** Please set the number of devices you have *****
 But the maximum default of 8 MAX72XX wil also work.
 */
int PIN_CLK = 13;
int PIN_CS = 14; // chip select
int PIN_DIN = 12;

/* we always wait a bit between updates of the display */
int DevNum = 8;
int Light = 3;

LedControl lc(PIN_DIN, PIN_CLK, PIN_CS, DevNum);

int WalkSize = 7;
int WalkPos = 0;

int WordSize = 4;
int WordPos = 0;
unsigned long oldTime = 0;
unsigned long oldWalkTime = 0;
unsigned long oldLoopTime = 0;
int loopDelay = 10;
int StartTime = 50;
int StopTime = 0;
int OffsetPos = 0;
int Pos = 0;
byte LedBuf[2][32] = {0};
int loopPos = 0;

bool StopLoop = false;

// DBG_OUTPUT_PORT control
int bufCount = 0;
char inputString[300] = {0}; //650
bool stringComplete = false;
int SetMsgSize = 0;
byte MaxMsgSize = 20;
byte MsgBuf[20][32] = {0}; 

void setup()  
{
  // Open DBG_OUTPUT_PORT communications and wait for port to open:
  DBG_OUTPUT_PORT.begin(115200);
  DBG_OUTPUT_PORT.print("\n");
  DBG_OUTPUT_PORT.setDebugOutput(true);
  SPIFFS.begin();
  listDir(SPIFFS, "/", 0);
  for(int addr = 0; addr < DevNum; addr ++)
  {
      /*The MAX72XX is in power-saving mode on startup*/
      lc.shutdown(addr,false);
      /* Set the brightness to a medium values */
      lc.setIntensity(addr, Light);
      /* and clear the display */
      lc.clearDisplay(addr);
  }

  const byte* val1 = NULL;
  int len = 32;
  for(int i = 0; i < 4; ++ i)
  {
    val1 = lc.GetArrayByIndex(i);
    memcpy(MsgBuf[i], val1, len);
  }

  
  //WIFI INIT
  DBG_OUTPUT_PORT.printf("Connecting to %s\n", ssid);
  if (String(WiFi.SSID()) != String(ssid)) {
    WiFi.begin(ssid, password);
  }
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DBG_OUTPUT_PORT.print(".");
  }
  DBG_OUTPUT_PORT.println("");
  DBG_OUTPUT_PORT.print("Connected! IP address: ");
  DBG_OUTPUT_PORT.println(WiFi.localIP());

  MDNS.begin(host);
  DBG_OUTPUT_PORT.print("Open http://");
  DBG_OUTPUT_PORT.print(host);
  DBG_OUTPUT_PORT.println(".local/demo.html to access this device");
  
  
  //SERVER INIT
  //list directory
  server.on("/set", handleSet);
  server.on("/demo", HTTP_GET, showMainPage);
  server.onNotFound([](){
    if(!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });
  server.begin();
  DBG_OUTPUT_PORT.println("HTTP server started");

}

void loop() // run over and over
{
    server.handleClient();
    if(StopLoop)
      return;

    switch(loopPos) {
      case 0:
        if( WordPos == WordSize )
          ++loopPos;
        loopDelay = 10;
        ShiftLeftWord();
        break;
      case 1:
        loopDelay = 300;
        if(SignleWord())
          loopPos = 0;
        break;
    }
}

bool decideLoopTime()
{
    unsigned long currentTime = millis();
    bool hit = false;
    if( currentTime - oldLoopTime >= loopDelay )
    {
        hit = true;
        oldLoopTime = currentTime;
    }

    return hit;  
}

// if ture than exit loop
bool SignleWord()
{
    if(! decideLoopTime())
      return false;

    const byte* val1 = NULL;
    const byte* val2 = NULL;
    int wordOffset = 18;
    int len = 32;
    if(WordPos >= WordSize)
    {
        WordPos = 0;
        return true;
    }
    val1 = MsgBuf[WordPos++];
    memcpy(LedBuf[0], val1, len);
    if(WordPos >= WordSize)
      memset(LedBuf[1], 0, len);
    else
    {
      val2 = MsgBuf[WordPos++];
      memcpy(LedBuf[1], val2, len);
    }

    drawTwoRowScreen();
    return false;
}

void ShiftLeftWord()
{
    if(! decideLoopTime())
      return;

    const byte* val1 = NULL;
    byte tmp[32] = {0};
    int len = 32;
    int wordOffset = 18;
    if(WordPos >= WordSize)
      WordPos = 0;

    val1 = MsgBuf[WordPos];
    memcpy(tmp, val1, len);

    // shift two word from memory first
     for (byte k = 0; k < 16; k += 2)
     {
        // Up-Left word with high byte
        LedBuf[0][k] <<= 1;
        LedBuf[0][k] |= ((LedBuf[0][k+1] & 0x80) >> 7);
        // Up-Left word with low byte
        LedBuf[0][k+1] <<= 1;
        // middle word with last bit
        LedBuf[0][k+1] |= ((LedBuf[1][k] & 0x80) >> 7);

        // Down-Left word with high byte
        LedBuf[0][k+16] <<= 1;
        LedBuf[0][k+16] |= ((LedBuf[0][k+16+1] & 0x80) >> 7);
        // Down-Left word with low byte
        LedBuf[0][k+16+1] <<= 1;
        // middle word with last bit
        LedBuf[0][k+16+1] |= ((LedBuf[1][k+16] & 0x80) >> 7);
    
        // Up-Right word with high byte
        LedBuf[1][k] <<= 1;
        LedBuf[1][k] |= ((LedBuf[1][k+1] & 0x80) >> 7);
        LedBuf[1][k+1] <<= 1;

        // Down-Right word with high byte
        LedBuf[1][k+16] <<= 1;
        LedBuf[1][k+16] |= ((LedBuf[1][k+16+1] & 0x80) >> 7);
        LedBuf[1][k+16+1] <<= 1;

        // get single word and shift 1 bit ony by one
        if(Pos <= 7) // high byte
        {
          tmp[k] <<= OffsetPos;
          tmp[k+16] <<= OffsetPos;
        }
        else //low byte
        {
          tmp[k] = tmp[k+1] << OffsetPos;
          tmp[k+16] = tmp[k+16+1] << OffsetPos;
        }

        // Up-Right word with last bit
        LedBuf[1][k+1] |= ((tmp[k] & 0x80) >> 7);
        // Down-Right word with last bit
        LedBuf[1][k+16+1] |= ((tmp[k+16] & 0x80) >> 7);
     }
    
     ++ OffsetPos;
     ++ Pos;
    
     // get next world
     if(OffsetPos >= 8)
        OffsetPos = 0;
     if(Pos >= 16)
     {
        ++ WordPos;
        Pos = 0;
     }

     drawTwoRowScreen();
}

void drawTwoRowScreen()
{
    // Turn on each row in series
    // 12x32 matrix order as below:
    // [3][2][1][0]  0:head, 7:tail
    // [7][6][5][4]
     for (byte k = 0, i = 0; k < 16; k += 2, ++ i)
     {
          lc.resetStatus();
          lc.setRow(1, i, LedBuf[1][k]);
          lc.setRow(0, i, LedBuf[1][k+1]);
          lc.setRow(5, i, LedBuf[1][k+16]);
          lc.setRow(4, i, LedBuf[1][k+16+1]);
  
          lc.setRow(3, i, LedBuf[0][k]);
          lc.setRow(2, i, LedBuf[0][k+1]);
          lc.setRow(7, i, LedBuf[0][k+16]);
          lc.setRow(6, i, LedBuf[0][k+16+1]);
          lc.action();
     }
}

//
// DBG_OUTPUT_PORT message process
//

void SetMsgBuf(char* str, int len)
{
  //DBG_OUTPUT_PORT.println("SetMsgBuf");
  //DBG_OUTPUT_PORT.print("len=");
  //DBG_OUTPUT_PORT.print(len);

    int MsgSize = len/32;

    if( MsgSize > MaxMsgSize )
        MsgSize = MaxMsgSize;

    int index = 0;
    for(int i = 0; i < MsgSize; ++ i)
    {
        for(int j = 0; j < 32; ++ j)
        {
            index = i * 32 + j;
            if(index >= len)
                break;

            MsgBuf[i][j] = (byte)str[index]; 
        }
    }

    WordSize = MsgSize;
    //DBG_OUTPUT_PORT.print("WordSize=");
    //DBG_OUTPUT_PORT.print(WordSize);
}

void AddMsgBuf(char* str, int len)
{
  //DBG_OUTPUT_PORT.println("AddMsgBuf");
  //DBG_OUTPUT_PORT.print("len=");
  //DBG_OUTPUT_PORT.print(len);
    int MsgSize = len/32;

    if( MsgSize + WordSize > MaxMsgSize )
        MsgSize = MaxMsgSize;

    int index = 0;
    for(int i = 0; i < MsgSize; ++ i)
    {
        for(int j = 0; j < 32; ++ j)
        {
            index = i * 32 + j;
            if(index >= len)
                break;

            MsgBuf[i+WordSize][j] = (byte)str[index]; 
        }
    }

    WordSize += MsgSize;
    //DBG_OUTPUT_PORT.print("WordSize=");
    //DBG_OUTPUT_PORT.print(WordSize);
}

void ParseMsg(void)
{
  if(stringComplete)
  {
     char* key = "SetMsg=";
     char* key2 = "AddMsg=";
     char* ptr = NULL;
     ptr = strstr(inputString, key);
     if(ptr)
     {
         DBG_OUTPUT_PORT.println("str SetMsg");
         int offset = strlen(key) + 2; // without 2byte message size
         SetMsgBuf(ptr+offset, bufCount-offset);
     }
     else// if(ptr2)
     {
         ptr = strstr(inputString, key2);
         if(ptr)
         {
             DBG_OUTPUT_PORT.println("str AddMsg");
             int offset = strlen(key2) + 2; // without 2byte message size
             AddMsgBuf(ptr+offset, bufCount-offset);
         }
     }

     bufCount = 0;
     stringComplete = false;
     memset(inputString, 0, sizeof(inputString));
  }
}

void DBG_OUTPUT_PORTEvent()
{
  char* key = "SetMsg=";
  int keyLen = strlen(key);

  while(DBG_OUTPUT_PORT.available())
  {
      char inChar = (char)DBG_OUTPUT_PORT.read();
      //DBG_OUTPUT_PORT.print((unsigned char)inChar, HEX);
      //DBG_OUTPUT_PORT.print(",");
      if(bufCount >= sizeof(inputString) - 1)
      {
          SetMsgSize = 0;
          stringComplete = true;
          continue;
      }

      inputString[bufCount++] = inChar;

      // with 2 byte message size
      if( SetMsgSize == 0 && bufCount >= keyLen + 2 )
      {
          SetMsgSize = (int)((int)inputString[keyLen] * 256);
          SetMsgSize += (int)(unsigned char)(inputString[keyLen+1]);
          //DBG_OUTPUT_PORT.print("SetMsgSize=");
          //DBG_OUTPUT_PORT.println(SetMsgSize);
      }

      if( SetMsgSize && bufCount >= (SetMsgSize+keyLen) )
      {
          /*DBG_OUTPUT_PORT.print("keyLen=");
          DBG_OUTPUT_PORT.println(keyLen);
          DBG_OUTPUT_PORT.print("SetMsgSize=");
          DBG_OUTPUT_PORT.println(SetMsgSize);
          DBG_OUTPUT_PORT.print("bufCount=");
          DBG_OUTPUT_PORT.println(bufCount);
          */
          SetMsgSize = 0;
          stringComplete = true;
          break;
      }
  }
//DBG_OUTPUT_PORT.print("bufCount=");
//DBG_OUTPUT_PORT.println(bufCount);
  if(stringComplete)
      ParseMsg();
}



void handleSet() { //Handler for the body path

  //StaticJsonBuffer<2048> jsonBuffer;
  DynamicJsonBuffer jsonBuffer; // 自動管理json buffer
  
  if (server.hasArg("plain")== false){ //Check if body received, 反正hasArg("plain")就是能拿到json，不知道原因。
    server.send(200, "text/json", "{\"\result\":\"Error\",\"error\":\"Body not received\"}");
    return;
  }

  Serial.println(server.arg("plain"));
  JsonObject& root = jsonBuffer.parseObject( server.arg("plain") );
  if (!root.success()) {
    Serial.println("Deserialization failed!");
    return;
  }
  
  JsonArray& jsonArray = root["data"]; //array在data底下
  int len = root["len"];
  char buf[2048];            // 16*128=2048
  jsonArray.copyTo(buf);
  SetMsgBuf(buf, len);
  server.send(200, "text/json", "{\"result\":\"OK\"}");



//  因為無法解析ascii code為0，暫時停止開發
//  if (server.hasArg("plain")== false){ //Check if body received, 反正hasArg("plain")就是能拿到json，不知道原因。
//    DBG_OUTPUT_PORT.println("Nothing recieved.");
//    return;
//  }
//  String temp = server.arg("plain");
//  int len = temp.length();
//  DBG_OUTPUT_PORT.print("Data recieved, data numbers:");  
//  DBG_OUTPUT_PORT.println(len);
//  
//  char buf[len+1];  // +1 for \0
//  temp.toCharArray(buf, len);
//  for (int i = 0 ; i < len; i++){
//    DBG_OUTPUT_PORT.println(buf[i], DEC);
//  }
//  SetMsgBuf(buf, len);
//  server.send(200, "text/plain", "Success  " + message);


}

void showMainPage()
{
  DBG_OUTPUT_PORT.println("Main!");
  if(!handleFileRead("/demo.html")) server.send(404, "text/plain", "FileNotFound");
}

bool handleFileRead(String path){
  DBG_OUTPUT_PORT.println("handleFileRead: " + path);
  if(path.endsWith("/")) path += "index.htm";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

String getContentType(String filename){
  if(server.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

void handleFileList() {
  if(!server.hasArg("dir")) {
    returnFail("BAD ARGS");
    return;
  }
  String path = server.arg("dir");
  if(path != "/" && !SPIFFS.exists((char *)path.c_str())) {
    returnFail("BAD PATH");
    return;
  }
  File dir = SPIFFS.open((char *)path.c_str());
  path = String();
  if(!dir.isDirectory()){
    dir.close();
    returnFail("NOT DIR");
    return;
  }
  dir.rewindDirectory();

  String output = "[";
  for (int cnt = 0; true; ++cnt) {
    File entry = dir.openNextFile();
    if (!entry)
    break;

    if (cnt > 0)
      output += ',';

    output += "{\"type\":\"";
    output += (entry.isDirectory()) ? "dir" : "file";
    output += "\",\"name\":\"";
    // Ignore '/' prefix
    output += entry.name()+1;
    output += "\"";
    output += "}";
    entry.close();
  }
  output += "]";
  server.send(200, "text/json", output);
  dir.close();
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  DBG_OUTPUT_PORT.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    DBG_OUTPUT_PORT.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    DBG_OUTPUT_PORT.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      DBG_OUTPUT_PORT.print("  DIR : ");
      DBG_OUTPUT_PORT.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      DBG_OUTPUT_PORT.print("  FILE: ");
      DBG_OUTPUT_PORT.print(file.name());
      DBG_OUTPUT_PORT.print("  SIZE: ");
      DBG_OUTPUT_PORT.println(file.size());
    }
    file = root.openNextFile();
  }
}

void returnFail(String msg) {
  server.send(500, "text/plain", msg + "\r\n");
}
