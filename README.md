# ESP32 FTP client with SDCARD interface
Refer **ESP32FTP_FILE_TRANSFER_ETHERNET_LAN8720** for full functionality.
## Description:
 This repo contain code for ftp client library written in C. This library is implemented for Wifi and Ethernet interface on esp32. On Ethernet interface this library have code to support following Ethernet peripherals
| Interface  |
| :------------: |
| LAN8720  |
| ENC28j60  |
| W5100 |

## File tree


    │   ESp32 ethernet specification.pdf
    │   ESP32FTP_FILE_TRANSFER_ETHERNET.zip
    │   ESP32FTP_FILE_TRANSFER_ETHERNET_ENC28j60_.rar
    │   ESP32FTP_FILE_TRANSFER_ETHERNET_LAN8720.zip
    │   ESP32FTP_FILE_TRANSFER_ETHERNET_W5100.rar
    │   ESP32FTP_FILE_TRANSFER_wifi.zip
    │   How To install FTP server and Hots pot on Raspberry PI.mp4
    │   install board.docx
    │   install ftp server on raspberry pi and android.docx
    │   New OpenDocument Spreadsheet.ods
    │   README.md
    │
    ├───ESP32FTP_FILE_TRANSFER_ETHERNET
    │
    ├───ESP32FTP_FILE_TRANSFER_ETHERNET_LAN8720
    │
    ├───ESP32FTP_SIMPLE_FILEZILLA
    │
    ├───ESP32FTP_SIMPLE_FILEZILLA_WIFI
    │
    └───Hardware Connections
            connections diagram.jpg
            Connections.txt
            ESP32 Log.txt
            ESP32 static IP setting .txt
            ethernet connection.png
            ethernet cross cable connections.jpg
            Raspberry PI ftp server setup.txt
            SDCARD CONNECTION.png
			
## FTP client functionality
	bool connectToFTPServer();
	uint8_t sendFTPCommand(String cmd);
	bool loginFTPServer(String username , String password);
	void readFTPResponse();
	bool parseFTPDataPort();
	bool initFileDownloadSequence();
	bool ftpMkdir(String dirName);
	bool ftpRmdir(String dirName);
	bool ftpChdir(String dirName);
	bool listDir();
	bool renameFile(String fromName, String toName);
	bool deleteFile(String fileName);
	bool downloadFileFromFTP(String path);
	bool getFile(String path);

## SDCARD functionality
	bool SD_Initialize();
	bool SD_Detect();
	bool SD_Exist(const char * path);
	bool SD_mkDir(const char * path);
	bool SD_mkFile(const char * path);
	bool SD_rmDir(const char * path);
	bool SD_rmFile(const char * path);
	bool SD_CopyFile(const char * src , const char * dest);
	uint32_t SD_ReadFile(uint64_t address, uint8_t *buf , uint32_t len, const char * path);
	uint32_t SD_WriteFile(uint8_t * buf , uint32_t len, const char * path);
	uint64_t Get_File_Size( const char * path);
	void SD_ListDir(const char * dirname, uint8_t levels);
	bool SD_RenameFile(const char * path1, const char * path2);
## Button driver functionality
	void initButtons();
	bool isButtonPressed(uint8_t button);

## LED driver functionality
	void initLED();
	void setLed(uint8_t led, uint8_t val);

## Test cases
Refer **ESP32FTP_FILE_TRANSFER_ETHERNET_LAN8720.ino** for full functionality
1. Transfer 5MB Size of .bmp File from Rpi FTPServer to SD Card. Check SD Card image in
PC to verify Same File with Complete image is Stored or Not. Check Time Required to Copy.
Repeat above transfer with Mobile Phone App. (Check this all - 3 Times)

2. On Every Key Press Read 500 byte from file into buffer and Send it on UART till file
Completes and start again. (Check this - 3 Times)

3. Read file from in Between , I.e. Start Byte Address = 20,000, read till End of File and Cross
Check the UART data with Data in File on respective Address.
(Check this - 3 Times)

4. On Press of Key, Save one long Global Variable ( File Pointer ) and one unsigned int
variable into MCU Internal EEPROM memory. At each power ON, it should read data from
EEPROM and Store into concern variable.
