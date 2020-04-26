#include <stdio.h>
#include "simple_ftp_client.h"
#include "SD_card_helper.h"

const char* ssid     = "Restricted Zone";
//const char* ssid     = "raspi-webgui";
const char* password = "9028928349";


void setup() {

  Serial.begin(115200);
  Serial2.begin(115200);

  while (!SD_Initialize()) {
    Serial.println("Card Mount Failed");
    // return;
  }
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  /*
    listDir(."/", 0);
    createDir(."/mydir");
    listDir(."/", 0);
    removeDir(."/mydir");
    listDir(."/", 2);
    writeFile(."/hello.txt", "Hello ");
    appendFile(."/hello.txt", "World!\n");
    readFile(."/hello.txt");
    deleteFile(."/foo.txt");
    renameFile(."/hello.txt", "/foo.txt");
    readFile(."/foo.txt");
    testFileIO(."/test.txt");
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
  */
}

void loop() {

  connectToFTPServer();
  if(!loginFTPServer("admin","651151"))
  {
    Serial.println("Login Failed");
    return;
  }
  Serial.println(">Loged iN");

  sendFTPCommand("NOOP");
  sendFTPCommand("TYPE I");
  sendFTPCommand("TYPE I");
  delay(200);
  sendFTPCommand("EPSV");
  parseFTPDataPort();

  //getFile();

  downloadFileFromFTP("/test.txt");
  listDir("/", 0);

  Serial.println("Reading File...");

  delay(3000);
}

