#include "NetworkHandler.h"

byte NetworkHandler::mac[] = {0x04, 0xE9, 0xE5, 0x0E, 0x03, 0x82};
EthernetUDP NetworkHandler::Udp;

IPAddress NetworkHandler::outIp = IPAddress(192, 168, 235, 20);
const unsigned int NetworkHandler::outPort = 9999;   // the port to send to
const unsigned int NetworkHandler::localPort = 8888; // local port to listen on

void (*NetworkHandler::playCallback)(/*String filename, uint32_t frame_start, uint32_t frame_end*/) = nullptr;
void (*NetworkHandler::pauseCallback)() = nullptr;
void (*NetworkHandler::homeCallback)() = nullptr;

bool NetworkHandler::init()
{
    Ethernet.begin(mac, 10000, 4000); // mac, timeout, responseTimeout

    if (HardwareStatus())
    {
        if (CableConnected())
        {
            NetworkHandler::Udp.begin(localPort);
        }
        else
        {
            Serial.println(F("No ethernet cable connected."));
            return false;
        }
    }
    else
    {
        Serial.println(F("Ethernet hardware not detected."));
        return false;
    }
    return true;
}

void NetworkHandler::MessageReceived(OSCMessage &msg)
{
    Serial.println(F("Message received."));
    if (msg.isFloat(0))
    {
        Serial.print("\nReceived a float, ");
        float f = msg.getFloat(0);
        Serial.println(f);
    }
    else if (msg.isInt(0))
    {
        Serial.print("\nReceived an int, ");
        int i = msg.getInt(0);
        Serial.println(i);
    }
    else if (msg.isString(0))
    {
        Serial.print("\nReceived a string, ");
        char str[20];
        msg.getString(0, str);
        String s = (char *)str;
        Serial.println(s);
    }
}

void NetworkHandler::dispatchPlay(OSCMessage &msg)
{
    // String filename = "";
    // uint32_t startFrame = 0, endFrame = 0;

    // if (msg.isString(0))
    // {
    //     char str[12];
    //     filename = msg.getString(0, str);
    //     filename = (char *)str;
    // }
    // if (msg.isInt(1))
    // {
    //     startFrame = msg.getInt(1);
    // }
    // if (msg.isInt(2))
    // {
    //     endFrame = msg.getInt(2);
    // }

    if (playCallback != nullptr)
    {
        playCallback(/*filename, startFrame, endFrame*/);
    }
}

void NetworkHandler::dispatchPause(OSCMessage &msg)
{
    if (pauseCallback != nullptr)
    {
        pauseCallback();
    }
}

void NetworkHandler::dispatchHome(OSCMessage &msg)
{
    if (homeCallback != nullptr)
    {
        homeCallback();
    }
}

bool NetworkHandler::HardwareStatus()
{
    if (Ethernet.hardwareStatus() == EthernetNoHardware)
    {
        return false;
    }
    return true;
}

bool NetworkHandler::CableConnected()
{
    if (Ethernet.linkStatus() == LinkON)
    {
        return true;
    }
    return false;
}

String NetworkHandler::GetIp()
{
    IPAddress ipaddress = Ethernet.localIP();
    return IpToString(ipaddress);
}

String NetworkHandler::IpToString(IPAddress &ip)
{
    // IP v4 only
    String ips;
    ips.reserve(16);
    ips = "";
    ips += ip[0];
    ips += '.';
    ips += ip[1];
    ips += '.';
    ips += ip[2];
    ips += '.';
    ips += ip[3];
    return ips;
}

void NetworkHandler::updateIncomingOSC()
{
    if (!CableConnected())
    {
        return;
    }

    OSCBundle bundleIN;
    int osc_bundle_size;

    if ((osc_bundle_size = NetworkHandler::Udp.parsePacket()) > 0)
    {
        while (osc_bundle_size--)
        {
            bundleIN.fill(NetworkHandler::Udp.read());
        }

        if (!bundleIN.hasError())
        {
            bundleIN.dispatch("/test", NetworkHandler::MessageReceived);

            bundleIN.dispatch("/play", NetworkHandler::dispatchPlay);
            // bundleIN.dispatch("/loop", NetworkHandler::MessageReceived);
            bundleIN.dispatch("/pause", NetworkHandler::dispatchPause);
            bundleIN.dispatch("/home", NetworkHandler::dispatchHome);
            bundleIN.empty();
        }
    }
}

void NetworkHandler::sendOSCString(String message)
{
    int str_len = message.length() + 1;
    char osc_message_char[str_len];
    message.toCharArray(osc_message_char, str_len);

    // build the bundle and send
    OSCBundle bundleOut;
    bundleOut.add("/test").add(osc_message_char);
    Udp.beginPacket(outIp, outPort);
    bundleOut.send(Udp);
    Udp.endPacket();
    bundleOut.empty();
}

void NetworkHandler::registerPlayCallback(void (*callback)(/*String filename, uint32_t frame_start, uint32_t frame_end*/))
{
    playCallback = callback;
}

void NetworkHandler::registerPauseCallback(void (*callback)())
{
    pauseCallback = callback;
}

void NetworkHandler::registerHomeCallback(void (*callback)())
{
    homeCallback = callback;
}