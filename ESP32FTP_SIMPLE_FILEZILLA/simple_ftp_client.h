#ifndef __SIMPLE_FTP_CLIENT_H
#define __SIMPLE_FTP_CLIENT_H
#include <Arduino.h>
#include <WiFi.h>
#include <lwip/sockets.h>


bool connectToFTPServer();
uint8_t sendFTPCommand(String cmd);
bool loginFTPServer(String username , String password);
void readFTPResponse();
bool parseFTPDataPort();
bool downloadFileFromFTP(const char *path);
void getFile();
#endif
