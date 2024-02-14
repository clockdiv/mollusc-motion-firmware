#ifndef NETWORKHANDLER_H
#define NETWORKHANDLER_H

#include <Arduino.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <OSCBundle.h>

class NetworkHandler
{
private:
    static EthernetUDP Udp;
    static byte mac[];

    IPAddress outIp;
    const unsigned int outPort = 9999;   // the port to send to
    const unsigned int localPort = 8888; // local port to listen on

    // buffers for receiving and sending data
    // char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; // buffer to hold incoming packet, size: 24
    // char ReplyBuffer[] = "acknowledged";       // a string to send back
    // char ReplyBuffer[16];       // a string to send back

    String IpToString(IPAddress &ip);

public:
    NetworkHandler();

    static void MessageReceived(OSCMessage &msg);
    static void updateIncomingOSC();

    void sendOSCString(String message);
    bool HardwareStatus();
    bool CableConnected();
    String GetIp();
};
#endif