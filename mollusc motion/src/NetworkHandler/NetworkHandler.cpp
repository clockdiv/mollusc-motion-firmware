#include "NetworkHandler.h"

byte NetworkHandler::mac[] = {0x04, 0xE9, 0xE5, 0x0E, 0x03, 0x82};
EthernetUDP NetworkHandler::Udp;
// const unsigned int NetworkHandler::outPort = 9999;   // the port to send to
// const unsigned int NetworkHandler::localPort = 8888; // local port to listen on

NetworkHandler::NetworkHandler()
{
    outIp = IPAddress(192, 168, 235, 20);
    // ReplyBuffer = "acknowledged";       // a string to send back

    Ethernet.begin(mac);
    if (HardwareStatus() && CableConnected())
    {
        NetworkHandler::Udp.begin(localPort);
    }
}

void NetworkHandler::MessageReceived(OSCMessage &msg)
{
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
    if (Ethernet.linkStatus() == LinkOFF)
    {
        return false;
    }
    return true;
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