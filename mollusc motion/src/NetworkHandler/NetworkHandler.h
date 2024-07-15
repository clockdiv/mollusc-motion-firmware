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

    static IPAddress outIp;
    static const unsigned int outPort;   // the port to send to
    static const unsigned int localPort; // local port to listen on

    // buffers for receiving and sending data
    // char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; // buffer to hold incoming packet, size: 24
    // char ReplyBuffer[] = "acknowledged";       // a string to send back
    // char ReplyBuffer[16];       // a string to send back

    static String IpToString(IPAddress &ip);
    static void (*playCallback)(/*String filename, uint32_t frame_start, uint32_t frame_end*/);
    static void (*pauseCallback)();
    static void (*homeCallback)();

    static void MessageReceived(OSCMessage &msg);
    static void dispatchPause(OSCMessage &msg);
    static void dispatchPlay(OSCMessage &msg);
    static void dispatchHome(OSCMessage &msg);

public:
    static bool init();

    static void updateIncomingOSC();

    static void sendOSCString(String message);
    static bool HardwareStatus();
    static bool CableConnected();

    static void registerPlayCallback(void (*callback)(/*String filename, uint32_t startFrame, uint32_t frameCount*/));
    static void registerPauseCallback(void (*callback)());
    static void registerHomeCallback(void (*callback)());

    static String GetIp();
};
#endif