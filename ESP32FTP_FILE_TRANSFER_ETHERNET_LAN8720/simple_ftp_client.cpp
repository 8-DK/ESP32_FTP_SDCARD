/** @file simple_ftp_client.cpp
    @brief Function implimentation for ftp client library.

    This contains the implimentation for the ftp client library
    functionalities.

    @author Dhananjay Khairnar
*/

/*
 * FTP Raw commands ABOR  base  Abort s m [RFC959]
 * 
 * 
        ABOR Abort
        ACCT  Account
        ADAT  Authentication/Security Data
        ALGS  FTP64 ALG status
        ALLO  Allocate
        APPE  Append (with create)
        AUTH  Authentication/Security Mechanism
        AUTH+ Authentication/Security Mechanism
        CCC Clear Command Channel
        CDUP  Change to Parent Directory
        CONF  Confidentiality Protected Command
        CWD Change Working Directory
        DELE  Delete File
        ENC Privacy Protected Command
        EPRT  Extended Port
        EPSV  Extended Passive Mode
        FEAT  Feature Negotiation
        HELP  Help
        HOST  Hostname
        LANG  Language (for Server Messages)
        LIST  List
        LPRT  Data Port {FOOBAR}
        LPSV  Passive Mode {FOOBAR}
        MDTM  File Modification Time
        MIC Integrity Protected Command
        MKD Make Directory
        MLSD  List Directory (for machine)
        MLST  List Single Object
        MODE  Transfer Mode
        NLST  Name List
        NOOP  No-Op
        OPTS  Options
        PASS  Password
        PASV  Passive Mode
        PBSZ  Protection Buffer Size
        PBSZ+ Protection Buffer Size
        PORT  Data Port
        PROT  Data Channel Protection Level
        PROT+ Data Channel Protection Level
        PWD Print Directory
        QUIT  Logout
        REIN  Reinitialize
        REST  Restart
        REST+ Restart (for STREAM mode)
        RETR  Retrieve
        RMD Remove Directory
        RNFR  Rename From
        RNTO  Rename From
        SITE  Site Parameters
        SIZE  File Size
        SMNT  Structure Mount
        STAT  Status
        STOR  Store
        STOU  Store Unique
        STRU  File Structure
        SYST  System
        TYPE  Representation Type
        USER  User Name
        XCUP  {precursor for CDUP}
        XCWD  {precursor for CWD}
        XMKD  {precursor for MKD}
        XPWD  {precursor for PWD}
        XRMD  {precursor for RMD}
        -N/A- Trivial Virtual File Store
 */

#include "simple_ftp_client.h"
#include "SD_card_helper.h"

WiFiClient client[2];

/**FTP response receive buffer String format*/
String recvBuf;

/** @brief flush receive buffer

    @return void
*/
void ftpflush()
{
  recvBuf = "";
  for (int i = 0 ; i < 50 ; i++)
  {
    while (client[0].available())
    {
      char ch = client[0].read();
      ch = '0';
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

/** @brief establis connection to ftp server

    @return true if success / false if  failed
*/
bool connectToFTPServer()
{
  if (!client[0].connect(host, ftpPort)) {
    FTPLogLn("FTP Server connection failed");
    return false;
  }
  readFTPResponse();
  return true;
}

/** @brief This function read responce comming from ftp server

    read responses from ftp server and stores it in buffre \a recvBuf

    @return void
*/
void readFTPResponse()
{
  String temp;
  char ch;
  for (int i = 0 ; i < 100 ; i++)
  {
    while (client[0].available())
    {
      ch = client[0].read();
      recvBuf += ch;
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

/** @brief This function sends command to ftp server

    sends command to ftp server and get responce from ftp server,
     then it parse that respoce to get responce code.

    @param cmd ftp command

    @return ftp responce code
*/
uint8_t sendFTPCommand(String cmd)
{
  int ret = 0;
  FTPLog("< ");
  FTPLogLn(cmd);
  ftpflush();
  client[0].flush();
  client[0].println(cmd);
  vTaskDelay(DELAY_BETWEEN_COMMAND / portTICK_PERIOD_MS);
  volatile unsigned long t = millis();
  while ( ((millis() - t) < COMMAND_TIME_OUT) && (recvBuf.length() < 3))
    readFTPResponse();
  FTPLog("> ");
  FTPLogLn(recvBuf);
  if (recvBuf.length())
    sscanf(recvBuf.c_str(), "%d ", &ret);
  char *ptr;
  ptr = (char*)recvBuf.c_str();
  while (*ptr)
  {
    if (isdigit (*ptr) ) {
      ret = strtoul (ptr, &ptr, 10);
      break;
    }
    ptr++;
  }

  return (uint8_t)ret;
}

/** @brief This function sends login sequence to ftp serce

    sends login sequeto ftp server and return status of login

    @param username username for login
    @param password password for login

    @return if login success then true / if login fails then false
*/
bool loginFTPServer(String username , String password)
{
  volatile uint8_t ret = 0;
  volatile uint8_t retry = LOGIN_RETRY;
  vTaskDelay(50 / portTICK_PERIOD_MS);
  while (retry > 0)
  {
    ftpflush();
    ret = sendFTPCommand("USER " + username);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    ftpflush();
    ret = sendFTPCommand("PASS " + password);
    if ( (LOGIN_SUCCESS == ret) | (ALREADY_LOGIN == ret) )
    {
      return true;
    }
    retry--;
  }
  return false;
}

/** @brief This function parse data port send by ftp server

    parse data port used for transfer data from ftp server and stores it in \a DataPort

    @return if parse success then true / if parse fails then false
*/
bool parseFTPDataPort()
{
  DataPort = 0;
  char *ptr;
  ptr = (char*)recvBuf.c_str();
  ptr += 3;
  while (*ptr)
  {
    if (isdigit (*ptr) ) {
      DataPort = strtoul (ptr, &ptr, 10);
    }
    ptr++;
  }

  FTPLogLn(String("Port Parsed ") + DataPort);
  if (DataPort > 0)
  {
    return true;
  }
  return false;
}

/** @brief This function initialize file download sequence

    @return if success then true / if fails then false
*/
bool initFileDownloadSequence()
{
  sendFTPCommand("NOOP");
  sendFTPCommand("TYPE I");
  sendFTPCommand("TYPE I");
  vTaskDelay(200 / portTICK_PERIOD_MS);
  sendFTPCommand("EPSV");
  return parseFTPDataPort();
}

/** @brief This function create directory on remote location

    create directory are remote location if write permission is given at ftp server

    @param dirName directory name 
    
    @return bool true is success
*/
bool ftpMkdir(String dirName)
{
  uint8_t resp = sendFTPCommand("XMKD " + dirName);
  if (resp == CREATED_SUCCESSFULLY)
  {
    FTPLogLn("Directory Created Successfuly");
    return true;
  }
  else if (resp == FILE_NOT_FOUND)
  {
    FTPLogLn("Can't create directory.");
  }
  return false;
}

/** @brief This function remove directory on remote location

    remove directory are remote location if write permission is given at ftp server

    @param dirName directory name 
    
    @return bool true is success
*/

bool ftpRmdir(String dirName)
{
  uint8_t resp = sendFTPCommand("XRMD " + dirName);
  if (resp == CREATED_SUCCESSFULLY)
  {
    FTPLogLn("Directory Deleted Successfuly");
    return true;
  }
  else if (resp == FILE_NOT_FOUND)
  {
    FTPLogLn("Directory not found");
  }
  return false;
}

/** @brief This function change directory on remote location

    change directory are remote location if write permission is given at ftp server

    @param dirName directory name 
    
    @return bool true is success
*/

bool ftpChdir(String dirName)
{
  uint8_t resp = sendFTPCommand("XCWD " + dirName);
  if (resp == CREATED_SUCCESSFULLY)
  {
    FTPLogLn("Directory Deleted Successfuly");
    return true;
  }
  else if (resp == FILE_NOT_FOUND)
  {
    FTPLogLn("Directory not found");
  }
  return false;
}

/** @brief This function list the directory on remote location

    list the directory are remote location if write permission is given at ftp server
    
    @return bool true is success
*/
bool listDir()
{
  uint8_t resp = sendFTPCommand("NLST");
  if (resp == SUCCESSFULY_TRANSFERED)
  {
    FTPLogLn("Command Successfully executed");
    return true;
  }
  return false;
}

/** @brief This function rename file on remote location

    rename file if write permission is given at ftp server

    @param file name 
    @param file name 
    
    @return bool true is success
*/
bool renameFile(String fromName, String toName)
{
  uint8_t resp = sendFTPCommand("RNFR " + fromName);
  if (resp == FILE_EXIST)
  {
    uint8_t resp = sendFTPCommand("RNTO " + toName);
    if (resp == FILE_COMMAND_SUCCESS)
    {
      FTPLogLn("Command Successfully executed");
      return true;
    }
  }
  return false;
}

/** @brief This function delete file on remote location

    delete file if write permission is given at ftp server

    @param file name 
    
    @return bool true is success
*/
bool deleteFile(String fileName)
{
  uint8_t resp = sendFTPCommand("DELE " + fileName);
  if (resp == FILE_COMMAND_SUCCESS)
  {
      FTPLogLn("Command Successfully executed");
      return true;
  }
  return false;
}

/** @brief This function download file from ftp server(Using socket api)

    function download file from ftp server. before calling this function you have to
    connect to ftp saerver then login using connectToFTPServer() and loginFTPServer() functions

    @param path path of file on ftp folder

    @return true if success / false if fails
*/
bool downloadFileFromFTP(String path)
{
  int socket_fd;
  struct sockaddr_in ra;
  File file;
  int recvDataCount = 0;
  uint8_t data_buffer[5000];

  if (!initFileDownloadSequence())
  {
    return false;
  }

  socket_fd = socket(PF_INET, SOCK_STREAM, 0);
  if ( socket_fd < 0 )
  {
    printf("socket call failed");
    return false;
  }

  memset(&ra, 0, sizeof(struct sockaddr_in));
  ra.sin_family = AF_INET;
  ra.sin_addr.s_addr = inet_addr(host);
  ra.sin_port = htons(DataPort);

  if (connect(socket_fd, (const sockaddr*)&ra, sizeof(struct sockaddr_in)) < 0)
  {
    printf("connect failed \n");
    close(socket_fd);
    return false;
  }

  int recvCount = 0;

  if (OPENING_FILE == sendFTPCommand("RETR " + path))
  {
    SD_rmFile(path.c_str());
    file = SD.open(path, FILE_WRITE);
    if (!file) {
      FTPLogLn("Failed to open file for writing");
      close(socket_fd);
      return false;
    }
  }
  else
  {
    close(socket_fd);
    return false;
  }

  FTPLogLn("Downloading File");
  uint32_t t = millis();
  while ( (recvDataCount = recv(socket_fd, data_buffer, sizeof(data_buffer), 0)) > 0)
  {
    file.write(data_buffer, recvDataCount);
    recvCount += recvDataCount;
  }
  uint32_t ttotal = (millis() - t);
  FTPLogLn((String)"File Download Completed " + recvCount);
  FTPLogF("Time required Download File %d ms, File Size %d bytes\n" , ttotal , recvCount);
  file.close();
  close(socket_fd);
  return true;
}

/** @brief This function download file from ftp server (Using wifi client api)

    function download file from ftp server. before calling this function you have to
    connect to ftp saerver then login using connectToFTPServer() and loginFTPServer() functions

    @param path path of file on ftp folder

    @return true if success / false if fails
*/
bool getFile(String path)
{
  char ch;
  File file;
  unsigned int i = 0, j = 0;
  uint8_t *dptr;
  recvBuf = "";
  initFileDownloadSequence();
  if (!DataPort)
    return false;
  client[1].connect(host, DataPort);

  if (OPENING_FILE == sendFTPCommand("RETR " + path)) //FTP will start file transfer on DataPort
  {
    SD_rmFile(path.c_str());
    file = SD.open(path, FILE_WRITE);
    if (!file) {
      FTPLogLn("Failed to open file for writing");
      client[1].stop();
      return false;
    }
  }
  uint32_t t = millis();
  uint32_t cnt = 0;
  uint8_t dataBuf[8192];
  while (client[1].connected())
  {
    while (client[1].available() > 0)
    {
      j = client[1].read(dataBuf, 8192);
      file.write(dataBuf, j);
      i += j;
    }
  }
  uint32_t ttotal = (millis() - t);
  FTPLogLn("File Download Completed");
  FTPLogLn(String("Time required Download File ") + (ttotal / 1000.0) + "sec File Size " + i + "kb " + " Speed of file transfer " + ((float)(i / 1024.0) / (float)(ttotal / 1000.0)) + " kbps");
  vTaskDelay(2 / portTICK_PERIOD_MS);
  client[1].stop();
  file.close();
  return true;
}
