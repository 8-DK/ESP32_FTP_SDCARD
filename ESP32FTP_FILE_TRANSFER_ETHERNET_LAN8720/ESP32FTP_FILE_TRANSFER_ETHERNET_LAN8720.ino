#include <dummy.h>
#include <stdio.h>
#include "simple_ftp_client.h"
#include "SD_card_helper.h"
#include "button_helper.h"
#include "led.h"
#include "crc32.h"
#include "Ethernet.h"
#include "esp_system.h"

/**IP address of FTP server host*/
char* host = (char*)"192.168.31.43"; //local machine ip (esp32 -> router -> PC(File Zilla Server))
/**Port of FTP server*/
unsigned int ftpPort = 21;    //change 2221 for android(set in app)
/**Data port for receive incoming file data*/
unsigned int DataPort = 1;    //no need to change

/**Task handler*/
TaskHandle_t Task1;

void setup() {

  //initialize uart 1
  Serial.begin(115200);

  //initialize uart 2
  Serial2.begin(115200);

  //set uart 2 timeout
  Serial2.setTimeout(50);

  //initialize SD card with FAT file system and EEPROM
  while (!SD_Initialize()) {
    Serial.println("Card Mount Failed");
  }

  //initialize ethernet
  initEthernet();

  //initialize buttons
  initButtons();

  //initialize led
  initLED();

  //initialize task for main program to be run ( this task runs on core 1)
  xTaskCreatePinnedToCore(
    mainTask, /* Function to implement the task */
    "Task1", /* Name of the task */
    16384,  /* Stack size in words */
    NULL,  /* Task input parameter */
    4,  /* Priority of the task */
    &Task1,  /* Task handle. */
    1); /* Core where the task should run */
}

void loop()
{

}

/**
    @brief mainTask

    main test case
*/
void mainTask( void * parameter)
{
  while (1)
  {
    TestCase5();
  }
  vTaskDelete(NULL);
}


/**
    @brief mainApp

    main test case
*/

void mainApp( void * parameter)
{
  uint8_t buf[500];
  uint32_t ret = 0;

  readEEPROMData();     //read eeprom data
  Serial.print("Connecting to "+ String(host));  
  if (!connectToFTPServer())  //connect to ftp server
  {
    Serial.println("FTP connect Failed");
    return;
  }

  if (!loginFTPServer("pi", "raspberry")) //login to ftp server
  {
    Serial.println("Login Failed");
    return;
  }
  if (downloadFileFromFTP("/test.txt")) //download file from ftp server
  {
    while (1)
    {
      if (isButtonPressed(BOTTON_1))  //read 500 bytes from file
      {
        ret = SD_ReadFile(readFilePointer, buf, 500, "/test.txt");
        Serial.println((String)"Ret " + ret);
        writeEEPROMData();  //save readfilepointer
      }
      else if (isButtonPressed(BOTTON_2)) //send 500 bytes on uart2
      {
        Serial.println("Sending Data To UART2");
        Serial2.write(buf, ret);
        ret = 0;
      }

      if (Serial2.available() > 0)  //read if data received on uart2
      {
        unsigned int led = -1;
        unsigned int val = 0;
        if (8 >= Serial2.readBytes(buf, 8)) //read 8 bytes only
        {
          sscanf((char*)buf, "LED%u-%u", &led, &val);   //command format - "LED1-0" ,"LED1-1"
          Serial.printf("LED - %d , Val - %d\n", led, val); //print LED value
          setLed(led, val); // ON/OFF LED
        }
      }
    }
  }
}

/**
    @brief Testcasse1

    Transfer 5MB Size of .bmp File from Rpi FTPServer to SD Card. Check SD Card image in
    PC to verify Same File with Complete image is Stored or Not. Check Time Required to copy
*/
void TestCase1()
{
  while (1)
  {
    if (SD_Detect())
    {
      if (!connectToFTPServer())
      {
        Serial.println("Connect ftp Failed");
        return;
      }
      if (!loginFTPServer("pi", "raspberry"))
      {
        Serial.println("Login Failed");
        return;
      }
      getFile("/test.txt");  //dont use capital extentions for file eg. test.BMP
      Serial.printf("file Size %lld\n", Get_File_Size("/test1.txt"));
    }
    delay(1000);
  }
}

/**
    @brief Testcasse2

    On Every Key Press Read 500 byte from file into buffer and Send it on UART till file
    Completes and start again. (Check this - 3 Times)

*/
void TestCase2()
{
  uint8_t buf[500];
  while (1)
  {
    //if (SD_Detect())
    {
      if (isButtonPressed(BOTTON_1))
      {
        uint32_t ret = SD_ReadFile(readFilePointer, buf, 500, "/test1.txt");
        Serial.println((String)"Ret " + ret);
        Serial2.write(buf, ret);
        if (fileReadFlag)
        {
          fileReadFlag = 0;
          Serial.println("------------File End Here------------");
        }
      }
      if (isButtonPressed(BOTTON_2))
      {
        Serial.println("Button 2 Pressed ");
      }
    }
  }
}

/**
    @brief Testcasse3

    Read file from in Between , I.e. Start Byte Address = 20,000, read till End of File and Cross
    Check the UART data with.Data in File on respective Address.

*/
void TestCase3()
{
  uint8_t buf[500];
  while (1)
  {
    //if (SD_Detect())
    {
      uint32_t ret = SD_ReadFile(20000, buf, 500, "/test.txt");
      while ( (ret = SD_ReadFile(readFilePointer, buf, 500, "/test.txt") ))
      {
        Serial.println((String)"Ret " + ret);
        Serial2.write(buf, ret);
        if (fileReadFlag)
        {
          fileReadFlag = 0;
          Serial.println("------------File End Here------------");
          break;
        }
      }
      //stop here
      while (1);
    }
  }
}

/**
    @brief Testcasse4

    On Press of Key, Save one long Global Variable ( File Pointer ) and one unsigned int
    variable into MCU Internal EEPROM memory. At each power ON, it should read data from
    EEPROM and Store into concern variable.

*/
void TestCase4()
{
  readEEPROMData();     //read eeprom data
  Serial.println("Read after restart from EEPROM");
  Serial.printf("readFilePointer-%ld, writeFilePointer-%ld, epVariable-%u\n", (long int)readFilePointer, (long int)writeFilePointer, epVariable);

  while (1)
  {
    if (isButtonPressed(BOTTON_1))
    {
      readFilePointer += 1;
      writeFilePointer += 2;
      epVariable += 3;
      Serial.println("Write to EEPROM");
      Serial.printf("readFilePointer-%ld, writeFilePointer-%ld, epVariable-%u\n", (long int)readFilePointer, (long int)writeFilePointer, epVariable);
      writeEEPROMData();
    }
  }
}

/**
    @brief Testcasse5

    this test case demonstrate the file download with CRC file.

*/
void TestCase5()
{
  uint32_t fileCRC = 0;
  if (SD_Detect())
  {
    if (!connectToFTPServer())  //connect to ftp server
    {
      Serial.println("FTP connect Failed");
      return;
    }

    if (!loginFTPServer("pi", "raspberry")) //login to ftp server
    {
      Serial.println("Login Failed");
      return;
    }
    ftpMkdir("NewDir");
    listDir();
    ftpChdir("NewDir");
    ftpChdir("/");
    ftpRmdir("NewDir");
    
    getFile("/test.txt");      //download Data file
    getFile("/test_crc.txt");  //download CRC file

    //read 4 byte CRC from crc file
    File file = SD.open("/test_crc.txt", FILE_READ);
    file.read((uint8_t*)&fileCRC, sizeof(uint32_t));
    file.close();

    if (fileCRC == getFileCrc("/test.txt"))
      Serial.println("CRC Match");
    else
      Serial.println("CRC mismatch");
  }
}


