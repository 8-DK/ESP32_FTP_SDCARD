/** @file Ethernet.cpp
 *  @brief Function implimentation for the Ethernet.
 *
 * This contains the implimentation for the Ethernet
 *  functionalities.
 *
 *  @author Dhananjay Khairnar    
*/
#include "Ethernet.h"

bool eth_connected = false;
IPAddress staticIp(192,168,31,44);
IPAddress gateway(192,168,31,1);
IPAddress subnet(255,255,255,0);
IPAddress dns1(8,8,8,8);
IPAddress dns2(8,8,4,4);

void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    
    case SYSTEM_EVENT_ETH_START:
      EthernetLogLn("ETH Started");
      //set eth hostname here
      ETH.setHostname("esp32-ethernet");
      //confihure static ip
//      ETH.config(staticIp, gateway,subnet,dns1,dns2);      
      break;
      
    case SYSTEM_EVENT_ETH_CONNECTED:
      EthernetLogLn("ETH Connected");
      break;
      
    case SYSTEM_EVENT_ETH_GOT_IP:
      EthernetLog("ETH MAC: ");
      EthernetLog(ETH.macAddress());
      EthernetLog(", IPv4: ");
      EthernetLog(ETH.localIP());
      if (ETH.fullDuplex()) {
        EthernetLog(", FULL_DUPLEX");
      }
      EthernetLog(", ");
      (ETH.linkSpeed());
      EthernetLogLn("Mbps");
      eth_connected = true;
      break;
      
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      EthernetLogLn("ETH Disconnected");
      eth_connected = false;
      break;
      
    case SYSTEM_EVENT_ETH_STOP:
      EthernetLogLn("ETH Stopped");
      eth_connected = false;
      break;
      
    default:
      break;
      
  }
}

/**
    @brief initEthernet
        
   initialise Ethernet interface
*/
void initEthernet()
{
  WiFi.onEvent(WiFiEvent);  
  ETH.begin(ETH_ADDR, ETH_PHY_POWER, ETH_MDC_PIN, ETH_MDIO_PIN, ETH_TYPE, ETH_CLK_MODE);
 
  while(eth_connected == false)
  {
    EthernetLog(".");
    delay(1000);
  }  
}
