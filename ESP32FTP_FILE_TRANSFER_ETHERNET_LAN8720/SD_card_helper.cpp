/** @file SD_card_helper.cpp
 *  @brief FFution implimentation for the SD card helpers.
 *
 * This contains the implimentation for the SD card
 *  functionalities.
 *
 *  @author Dhananjay Khairnar    
*/
#include "SD_card_helper.h"

/**File read pointer hold current file read location. need to clear manually*/
uint32_t readFilePointer = 0;

/**File write pointer hold current file write location. need to clear manually*/
uint32_t writeFilePointer = 0;

/**unsigned int varible for store in epprom*/
unsigned int epVariable = 0;

/**file read completion flag. true after read file reach to end*/
uint8_t fileReadFlag = 0;

SPIClass * hspi = NULL;

/** @brief initialize SD card with SPI configuration and initialise EEPROM
    for store file pointeers.

    Init this function you can configure spi pins , speed.
    EEPROM memory size can be change here

    @return Return true on successful initialization/false on unsuccessful initialization
*/
bool SD_Initialize()
{
  hspi = new SPIClass(HSPI);  
  hspi->begin(14,4,15,13); //SCLK, MISO, MOSI, SS
  if (!SD.begin(SS, *hspi, 18000000, "/sd"))
  {
    SDCARDLogLn("Card Mount Failed");
    return false;
  }
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    SDCARDLogLn("failed to initialise EEPROM");
  }
  SDCARDLogLn("Card Mount Success..");
  return true;
}

/** @brief This function detect if the SD card is inserted in slot or not

    This function ends SD card clss and reinitialize it. alternate way to inpliment this 
    by using reading CD pin

    @return Return true if sd card detect /false if no sd card detect
*/
bool SD_Detect()
{
  SD.end();
  delay(5);
  hspi->begin(14,4,15,13); //SCLK, MISO, MOSI, SS
  if (!SD.begin(SS, *hspi, 18000000, "/sd"))
  {
    SDCARDLogLn("Card Mount Failed in SD_Detect");
    return false;
  }
  uint64_t cardSize = SD.totalBytes() / (1024 * 1024);
  uint64_t freeSize = ( SD.cardSize () - SD.usedBytes () ) / (1024 * 1024);
  SDCARDLogF("SD Card Size: %lluMB Free Size %lluMB (%.2f %% free)\n", cardSize, freeSize, (float)((freeSize * 100) / cardSize));
  return true;
}

/** @brief This function look if given path file or directory exist

    Tests whether a file or directory exists on the SD card

    @param *path path to file or directory

    @return Return true if file or directory exist /false if no file or directory exist
*/
bool SD_Exist(const char * path)
{
  File root = SD.open(path);
  if (!root)
  {
    File file = SD.open(path);
    if (!file)
    {
      SDCARDLogLn((String)path + " Not Exist!!");
      return false;
    }
  }
  SDCARDLogLn((String)path + " Exist!!");
  return true;
}

/** @brief This function created directory with given path.

    Create Directory with Given path , If It is not there it will create it.
    if file already exixst will return false

    @param *path path to directory

    @return Return true if directory created /false if no directory created
*/
bool SD_mkDir(const char * path)
{
  char dirName[100];
  char separator = '/';
  char * sep_at = strchr(path, separator);

  for (int i = 0; sep_at != NULL; i++)
  {
    int len = strspn(sep_at, path);
    strncpy(dirName, path, strlen(path) - len);
    dirName[strlen(path) - len] = '\0';
    if (strlen(dirName) > 0)
    {
      if (SD_Exist(dirName))
      {

      }
      else
      {
        //create dir
        if (SD.mkdir(dirName)) {
          SDCARDLogLn((String)"Dir created" + dirName);
        } else {
          SDCARDLogLn("mkdir failed");
          return false;
        }
      }
    }
    sep_at = strchr(sep_at + 1, separator);
  }

  if (!SD_Exist(path))
  {
    if (SD.mkdir(path))
    {
      SDCARDLogLn((String)"file created" + path);
      return true;
    }
    else
    {
      SDCARDLogLn("SD_mkDir failed file already exist");
      return false;
    }
  }
  return false; //file already exist then return this
}

/** @brief This function created file with given path.

    Create file with Given path , If It is not there it will create it.
    if file already exixst will return false

    @param *path path to file

    @return Return true if file created /false if no file created
*/
bool SD_mkFile(const char * path)
{

  char dirName[100];
  char separator = '/';
  char * sep_at = strchr(path, separator);

  for (int i = 0; sep_at != NULL; i++)
  {
    int len = strspn(sep_at, path);
    strncpy(dirName, path, strlen(path) - len);
    dirName[strlen(path) - len] = '\0';
    if (strlen(dirName) > 0)
    {
      if (SD_Exist(dirName))
      {

      }
      else
      {
        //create dir
        if (SD.mkdir(dirName)) {
          SDCARDLogLn((String)"Dir created" + dirName);
        } else {
          SDCARDLogLn("mkdir failed");
          return false;
        }
      }
    }
    sep_at = strchr(sep_at + 1, separator);
  }

  if (!SD_Exist(path))
  {
    File file = SD.open(path, FILE_WRITE);
    if (file)
    {
      SDCARDLogLn((String)"Dir created" + path);
      return true;
      file.close();
    }
  }
  return false; //file already exist then return this
}

/** @brief This function remove directory with given path.

    remove directory with Given path , If It is not there it will return false.

    @param *path path to file

    @return Return true if directory removed /false if no directory removed
*/
bool SD_rmDir(const char * path)
{
  SDCARDLogF("Removing Dir: %s\n", path);
  if (SD.rmdir(path))
  {
    SDCARDLogLn("Dir removed");
    return true;
  }
  SDCARDLogLn("rmdir failed");
  return false;

}


/** @brief This function remove file with given path.

    remove file with Given path , If It is not there it will return false.

    @param *path path to file

    @return Return true if file removed /false if no file removed
*/
bool SD_rmFile(const char * path)
{
  SDCARDLogF("Deleting file: %s\n", path);
  if (SD.remove(path))
  {
    SDCARDLogLn("File deleted");
    return true;
  }
  SDCARDLogLn("Delete failed");
  return false;
}

/** @brief This function copy file with source path to destination path.

    Copy file from one path to other path.

    @param *src path to source file
    @param *dest path to destination file

    @return Return true if file copied /false if no file copied
*/
bool SD_CopyFile(const char * src , const char * dest)
{
  uint8_t buf[5000];

  File filesrc = SD.open(src, FILE_READ);
  if (!filesrc)
  {
    SDCARDLogLn("Failed to open file for Copying");
    return false;
  }
  File filedest = SD.open(dest, FILE_WRITE);
  if (!filesrc)
  {
    SDCARDLogLn("Failed to open file for Pasting");
    return false;
  }
  SDCARDLogLn("File copying ...");
  uint32_t t = millis();
  while (filesrc.available()) {
    //int ret = filesrc.read(buf, 5000);
    filedest.write(buf, filesrc.read(buf, 5000));
  }
  SDCARDLogLn((String)"Copy Completed. Time Taken " + (millis() - t));
  return true;
}

/** @brief This function read file with given parameters.

  Read Number of Byte from File and Save it to the RAM Buffer.
  Every Call of this Function will auto increment \a readFilePointer to the Next Read Byte
  Array.It Automatically starts from File Origin if File Reading is Complete. When Complete File
  read is done it will set \a fileReadFlag to true.
    @param address address from which file to be read.
    @param *buf buffer pointer to store read data
    @param len length to read number of bytes from file
    @param *path path of file to be read

    @return Return number of bytes read from file
*/
uint32_t SD_ReadFile(uint64_t address, uint8_t *buf , uint32_t len, const char *path)
{
  File file = SD.open(path, FILE_READ);
  if (!file)
  {
    SDCARDLogLn("Failed to open file for writing");
    return false;
  }
  uint64_t fileSize = file.size();
  uint16_t ret = 0;
  if (file.seek(address))
  {
    ret = file.read(buf, len);
    readFilePointer = address + ret;
    SDCARDLogF("readFilePointer - %ld , fileReadFlag %d - fileSize - %ld\n", (long int)readFilePointer, fileReadFlag, (long int)fileSize);
    if (readFilePointer >= fileSize)
    {
      fileReadFlag = 1;
      readFilePointer = 0;
    }
    else
      fileReadFlag = 0;
    return ret;
  }
  else
  {
    readFilePointer = 0;
    SDCARDLogLn("Not Valid Adress");
  }

  return 0;
}

/** @brief This function write file with given parameters.

  Write Number of Byte from buffer and Save it to file.
  Every Call of this Function will auto increment \a writeFilePointer to the write Byte.

    @param *buf buffer pointer to write into file
    @param len length to write number of bytes to file
    @param *path path of file to be write

    @return Return number of bytes write to file
*/
uint32_t SD_WriteFile(uint8_t * buf , uint32_t len, const char * path)
{
  uint8_t ret = 0;
  File file = SD.open(path, FILE_APPEND);
  if (!file)
  {
    SDCARDLogLn("Failed to open file for writing");
    return 0;
  }
  if (file.seek(writeFilePointer))
  {
    ret = file.write(buf, len);
    writeFilePointer += ret;
    return ret;
  }
  SDCARDLogLn("file seek failed");
  return false;
}

/** @brief This function gives file size.

    return file size in bytes.

    @param *path path of file

    @return Return size of file in bytes
*/
uint64_t Get_File_Size( const char * path)
{
  File file = SD.open(path, FILE_READ);
  if (!file)
  {
    SDCARDLogLn("Failed to open file for get file size");
    return 0;
  }
  uint64_t fileSize = file.size();
  file.close();
  return fileSize;
}

/** @brief This function writes parameters to eeprom

    Store 3 parameters in EEPROM \a readFilePointer ,\a writeFilePointer ,\a epVariable

    @return Void
*/
void writeEEPROMData()
{
  int address = 0;
  EEPROM.writeUInt(address, readFilePointer);           // 2^32 - 1

  address += sizeof(readFilePointer);

  EEPROM.writeUInt(address, writeFilePointer);         // 2^32 - 1

  address += sizeof(writeFilePointer);

  EEPROM.writeUInt(address, epVariable);               // 2^32 - 1

  EEPROM.commit();
}

/** @brief This function read parameters from eeprom

    Read 3 parameters from EEPROM \a readFilePointer ,\a writeFilePointer ,\a epVariable

    @return Void
*/
void readEEPROMData ()
{
  int address = 0;
  readFilePointer = EEPROM.readUInt(address);           // 2^32 - 1
  address += sizeof(readFilePointer);

  writeFilePointer = EEPROM.readUInt(address);        // 2^32 - 1
  address += sizeof(writeFilePointer);

  epVariable  = EEPROM.readUInt(address);             // 2^32 -1
}


/** @brief This function list the directory and files in sdcard

    List directories and file in given path. leve is depth of list

    @param *dirname path of directory to be list
    @param *levels depth of list

    @return Void
*/
void SD_ListDir(const char * dirname, uint8_t levels)
{
  SDCARDLogF("Listing directory: %s\n", dirname);

  File root = SD.open(dirname);
  if (!root)
  {
    SDCARDLogLn("Failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    SDCARDLogLn("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      SDCARDLog("  DIR : ");
      SDCARDLogLn(file.name());
      if (levels)
      {
        SD_ListDir(file.name(), levels - 1);
      }
    }
    else
    {
      SDCARDLog("  FILE: ");
      SDCARDLog(file.name());
      SDCARDLog("  SIZE: ");
      SDCARDLogLn(file.size());
    }
    file = root.openNextFile();
  }
}

/** @brief This function rename the file

    remane the file with provided parameters

    @param *path1 path of e file which is rename
    @param *path2 new name of the file with path

    @return true if rename is done / false if rename failed
*/
bool SD_RenameFile(const char * path1, const char * path2)
{
  SDCARDLogF("Renaming file %s to %s\n", path1, path2);
  if (SD.rename(path1, path2))
  {
    SDCARDLogLn("File renamed");
    return true;
  }
  SDCARDLogLn("Rename failed");
  return false;
}
