#include "SD_card_helper.h"

uint8_t dataBuf[DATA_BUF_LEN] = {0};
uint64_t readFilePointer = 0;
uint64_t writeFilePointer = 0;

bool SD_Initialize()
{
  SPIClass * hspi = NULL;
  hspi = new SPIClass(HSPI);
  hspi->begin(18, 19, 23, 5); //SCLK, MISO, MOSI, SS
  if (!SD.begin(SS, *hspi, 19000000, "/sd"))
  {
    Serial.println("Card Mount Failed");
    return false;
  }
  return true;
}

bool SD_Detect()
{
  SD.end();
  if (!SD.begin(SS, SPI, 18000000, "/sd"))
  {
    Serial.println("Card Mount Failed");
    return false;
  }
  uint64_t usedBy = SD.usedBytes ();
  uint64_t cardSize = SD.totalBytes() / (1024 * 1024);
  uint64_t freeSize = ( SD.cardSize () - SD.usedBytes () ) / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB Free Size %lluMB (%f %%)\n", cardSize, freeSize, (float)((freeSize * 100) / cardSize));
  return true;
}

bool SD_Exist(const char * path)
{
  File root = SD.open(path);
  if (!root)
  {
    // not a directory
    File file = SD.open(path);
    if (!file)
    {
      Serial.print(path);
      Serial.println(" Not Exist!!");
      return false;
    }
  }
  return true;
}

bool SD_mkDir(const char * path)
{
  Serial.printf("Creating Dir: %s\n", path);
  if (SD.mkdir(path)) {
    Serial.println("Dir created");
    return true;
  }
  else
  {
    Serial.println("mkdir failed");
    return false;
  }
}

bool SD_mkFile(const char * path)
{
  File file = SD.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return false;
  }
  /*
    if (file.print(message))
    {
    Serial.println("File written");
    }
    else
    {
    Serial.println("Write failed");
    }
  */
  file.close();
  return true;
}

bool SD_rmDir(const char * path)
{
  Serial.printf("Removing Dir: %s\n", path);
  if (SD.rmdir(path))
  {
    Serial.println("Dir removed");
    return true;
  }
  Serial.println("rmdir failed");
  return false;

}

bool SD_rmFile(const char * path)
{
  Serial.printf("Deleting file: %s\n", path);
  if (SD.remove(path))
  {
    Serial.println("File deleted");
    return true;
  }
  Serial.println("Delete failed");
  return false;
}

bool SD_CopyFile(const char * src , const char * dest)
{
  uint8_t ret;
  uint8_t buf[512];

  File filesrc = SD.open(src, FILE_READ);
  if (!filesrc)
  {
    Serial.println("Failed to open file for writing");
    return false;
  }
  File filedest = SD.open(dest, FILE_WRITE);
  if (!filesrc)
  {
    Serial.println("Failed to open file for writing");
    return false;
  }

  while ((ret = filesrc.read(buf, sizeof(buf))) > 0)
  {
    if (filedest.write(buf, ret) <= 0)
    {
      return false;
    }
  }
  return true;
}

uint8_t SD_ReadFile(uint64_t address, uint8_t *buf , uint8_t len, const char * path)
{
  File file = SD.open(path, FILE_READ);
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return false;
  }
  uint8_t fileSize = file.size();
  uint8_t ret = 0;

  if ( !((address + len) > fileSize))
  {
    if (file.seek(address))
    {
      ret = file.read(buf, len);
      readFilePointer = address + len;
      return ret;
    }
  }
  return 0;
}

uint8_t SD_WriteFile(uint8_t * buf , uint8_t len, const char * path)
{
  uint8_t ret = 0;
  File file = SD.open(path, FILE_APPEND);
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return 0;
  }
  ret = file.write(buf, len);
  return ret;
}

uint8_t Get_File_Size( const char * path)
{
  File file = SD.open(path, FILE_READ);
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return 0;
  }
  uint8_t fileSize = file.size();
  return fileSize;
}

void listDir(const char * dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = SD.open(dirname);
  if (!root)
  {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels)
      {
        listDir(file.name(), levels - 1);
      }
    }
    else
    {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void readFile(const char * path) {
  Serial.printf("Reading file: %s\n", path);

  File file = SD.open(path);
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available())
  {
    Serial.write(file.read());
  }
  file.close();
}

void writeFile(const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = SD.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = SD.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void renameFile(const char * path1, const char * path2) {
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (SD.rename(path1, path2)) {
    Serial.println("File renamed");
  } else {
    Serial.println("Rename failed");
  }
}

void testFileIO(const char * path) {
  File file = SD.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if (file) {
    len = file.size();
    size_t flen = len;
    start = millis();
    while (len) {
      size_t toRead = len;
      if (toRead > 512) {
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    Serial.printf("%u bytes read for %u ms\n", flen, end);
    file.close();
  } else {
    Serial.println("Failed to open file for reading");
  }


  file = SD.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }

  size_t i;
  start = millis();
  for (i = 0; i < 2048; i++) {
    file.write(buf, 512);
  }
  end = millis() - start;
  Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
  file.close();
}

