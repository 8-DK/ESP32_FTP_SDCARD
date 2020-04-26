#include "simple_ftp_client.h"
#include "SD_card_helper.h"

WiFiClient client;
WiFiClient dataclient;
WiFiClient ftpClient;

char* host = "192.168.31.43";
//char* host = "10.3.141.1";
unsigned int ftpPort = 21;
unsigned int DataPort = 1;


String recvBuf;
char ftpRespDataBuf[100] = {0};

bool connectToFTPServer()
{
  if (!ftpClient.connect(host, ftpPort)) {
    Serial.println("FTP Server connection failed");
    return false;
  }
  return true;
}

void readFTPResponse()
{
  recvBuf = "";
  String temp;
  char ch;
  for (int i = 0 ; i < 3 ; i++)
  {
    while (ftpClient.available())
    {
      ch = ftpClient.read();//readStringUntil('\r');
      recvBuf += ch;
    }
  }
  Serial.print("> ");
  Serial.print(recvBuf);
  ftpClient.flush();
}

uint8_t sendFTPCommand(String cmd)
{
  int ret = 0;
  Serial.print("< ");
  Serial.println(cmd);
  ftpClient.println(cmd);
  delay(200);
  readFTPResponse();
  if (recvBuf.length())
    sscanf(recvBuf.c_str(), "%d ", &ret);
  return (uint8_t)ret;
}

bool loginFTPServer(String username , String password)
{
  uint8_t ret = 0;
  ret = sendFTPCommand("USER "+username);  
  ret = sendFTPCommand("PASS "+password);
  if (ret == 230 )
  {
    return true;
  }
  return false;
}

bool parseFTPDataPort()
{
  char str[] = "";
  DataPort = 0;
  sscanf(recvBuf.c_str(), "229 Entering Extended Passive Mode (|||%d|)", &DataPort);
  Serial.printf("Port Parsed %d\n", DataPort);
  if (DataPort)
  {
    return true;
  }
  return false;
}

bool initFileDownloadSequence()
{
  return 0;
}

bool downloadFileFromFTP(const char *path)
{
  int socket_fd;
  struct sockaddr_in sa, ra;

  int recv_data = 0;
  uint8_t data_buffer[5000];

  SD_rmFile(path);
  File file = SD.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return false;
  }

  socket_fd = socket(PF_INET, SOCK_STREAM, 0);
  if ( socket_fd < 0 )
  {
    printf("socket call failed");
    return false;
  }

  /* Receiver connects to server ip-address. */

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
  uint32_t t = millis();
  sendFTPCommand("RETR /test.txt");
  Serial.println("Downloading File");
  while ( (recv_data = recv(socket_fd, data_buffer, sizeof(data_buffer), 0)) > 0)
  {
    file.write(data_buffer, recv_data);
    recvCount += recv_data;
  }
  uint32_t ttotal = (millis() - t);
  Serial.println("File Download Completed CRC");
  Serial.println(String("Time required Download File ") + (ttotal / 1000) + "sec File Size " + recvCount + " byte " + " Speed of file transfer " + ((recvCount / 1024) / (ttotal / 1000)) + " kbps");
  file.close();
  close(socket_fd);
  return true;
}

void getFile()
{
  char ch;
  recvBuf = "";
  //memset(dataBuf, 0, sizeof(dataBuf));
  SD_rmFile("/test.txt");
  File file = SD.open("/test.txt", FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  unsigned int i = 0, j = 0;
  uint8_t dataBuf[4096];
  client.connect(host, DataPort);
  //sendFTPCommand("RETR /home/pi/Downloads/test.txt");
  sendFTPCommand("RETR /test.txt");
  uint32_t t = millis();
  client.flush();
  while (client.connected())
  {
    while (client.available() > 0)
    {
      j = client.read(dataBuf, 4096);
      file.write(dataBuf, j);
      i += j;
    }
  }

  uint32_t ttotal = (millis() - t);
  delay(2);
  Serial.println("File Download Completed");
  Serial.println(String("Time required Download File ") + (ttotal / 1000) + "sec File Size " + i + "kb " + " Speed of file transfer " + ((i / 1024) / (ttotal / 1000)) + " kbps");
  client.stop();
  file.close();
}
