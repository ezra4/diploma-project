//
// Created by vladn on 6/15/2023.
//

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/wifi-mac-header.h"
#include "ns3/wifi-net-device.h"
#include "ns3/yans-wifi-helper.h"
#include <cmath>
#include <iomanip>

using namespace ns3;
NS_LOG_COMPONENT_DEFINE("Positioning_Algorithms");

// Constants
const double SPEED_OF_LIGHT = 3e8;
const double PI = 3.14159265358979323846;
const double transmitPower = 20;


Mac48Address* receivingAddresses;
int numberOfAPs;
std::string algorithm;
Vector stationPos;
std::map<Mac48Address,uint32_t> macToNodeID;

double computeRSSI(double Pt, double freqMHZ, double distance)
{
    double frequency_Hz = freqMHZ * 1000000;
    double wavelength = SPEED_OF_LIGHT/frequency_Hz;
    double Pr = Pt - 20 * log10(wavelength/(4*PI*distance));
    return Pr;
}

double computeToA(double distance)
{
    double ToA = distance/SPEED_OF_LIGHT;// * 1.0e9;
    return ToA;
}
double computeToF(double distance)
{
    double ToF = 2*computeToA(distance);
    return ToF;
}

void SetAPPower(Ptr<WifiPhy> phy, double power)
{
    phy->SetAttribute("TxPowerStart", DoubleValue(power));
    phy->SetAttribute("TxPowerEnd", DoubleValue(power));
}

double times[10];
Ptr<Packet> CreateTimestampPacket()
{
    // Create a packet
    Packet::EnablePrinting();
    Ptr<Packet> packet = Create<Packet>(1000);
    return packet;
}

int getIDfromAddress(Mac48Address address)
{
    std::stringstream ss;
    ss<<address;
    std::string macAddrStr = ss.str();
    std::string lastTwoChars = macAddrStr.substr(macAddrStr.size()-2);
    int ID = std::stoi(lastTwoChars, nullptr, 16);
    return ID;
}

void SendPacket(Ptr<NetDevice> apDevice, Mac48Address staAddress)
{
    Mac48Address apMacAddress = Mac48Address::ConvertFrom(apDevice->GetAddress());

    double now = Simulator::Now().GetSeconds();

    times[getIDfromAddress(apMacAddress)] = now;
    Ptr<Packet> packet = CreateTimestampPacket();

    std::cout<<"------------------------------------------------------------"<<std::endl;
    std::cout<<"Packet sent succesfully!"<<std::endl;
    std::cout<<"Sender: "<<apMacAddress<<" (AP)"<<std::endl;
    std::cout<<"Receiver: "<<staAddress<<" (STA)"<<std::endl;
    std::cout<<"------------------------------------------------------------"<<std::endl;

    Ptr<WifiNetDevice> wifiAP = DynamicCast<WifiNetDevice>(apDevice);
    SetAPPower(wifiAP->GetPhy(),20);
    std::cout<<"Transmit power start: "<<wifiAP->GetPhy()->GetTxPowerStart()<<std::endl;
    std::cout<<"Transmit power end: "<<wifiAP->GetPhy()->GetTxPowerEnd()<<std::endl;

    wifiAP->Send(packet, staAddress, 1);
}

double computeDistance(Ptr<Node> sender, Ptr<Node> receiver)
{
    Ptr<MobilityModel> mobilitySender = sender->GetObject<MobilityModel>();
    Ptr<MobilityModel> mobilityReceiver = receiver->GetObject<MobilityModel>();
    Vector posSender = mobilitySender->GetPosition();
    Vector posReceiver = mobilityReceiver->GetPosition();
    double distance = sqrt(pow(posSender.x-posReceiver.x,2)+pow(posSender.y-posReceiver.y,2));
    return distance;
}

bool checkIfReceiver(Mac48Address address)
{
    for(int i=0;i<numberOfAPs;i++)
    {
         if(address == receivingAddresses[i])
         {
             return true;
         }
    }
    return false;
}
int v[16]; // Frequency vector for each APs ID
void MonitorSnifferRx_ToF(std::string context, Ptr<const Packet> packet, uint16_t channelFreqMhz, WifiTxVector txVector, MpduInfo aMpdu, SignalNoiseDbm signalNoise, uint16_t staId)
{

    WifiMacHeader hdr;
    packet->PeekHeader(hdr);

    Mac48Address senderAddress = hdr.GetAddr2();
    Mac48Address receiverAddress = hdr.GetAddr1();
    if(senderAddress=="00:00:00:00:00:00") {
        return;
    }
    if(!checkIfReceiver(receiverAddress))
    {
        return;
    }
    if (senderAddress == Mac48Address("ff:ff:ff:ff:ff:ff") || receiverAddress == Mac48Address("ff:ff:ff:ff:ff:ff")) {
        // This is a broadcast packet, so we can return or skip processing this packet
        return;
    }

    int senderID = getIDfromAddress(senderAddress);
    Ptr<Node> sender = NodeList::GetNode(macToNodeID[senderAddress]);
    Ptr<Node> receiver = NodeList::GetNode(0);
    double distance = computeDistance(sender,receiver);

    if(v[senderID]!=0)
    {
        return;
    }
    v[senderID]++;

    std::cout<<"------------------------------------------------------------"<<std::endl;
    std::cout<<"\tDistance between devices: "<<distance<<std::endl;
    std::cout<<"\tChannel(MHZ): "<<channelFreqMhz<<std::endl;

    double idealToF = computeToF(distance);
    double now = Simulator::Now().GetSeconds()-times[senderID];
    std::cout<<"\tIdeal Time of Flight: "<<std::fixed<<std::setprecision(9)<<idealToF<<std::endl;
    std::cout<<"\tActual Time of Flight: "<<now*2<<"s "<<"from senderID: "<<senderID<<std::endl;
    std::cout<<"------------------------------------------------------------"<<std::endl;
}

void MonitorSnifferRx_RSSI(std::string context, Ptr<const Packet> packet, uint16_t channelFreqMhz, WifiTxVector txVector, MpduInfo aMpdu, SignalNoiseDbm signalNoise, uint16_t staId)
{
    WifiMacHeader hdr;
    packet->PeekHeader(hdr);

    Mac48Address senderAddress = hdr.GetAddr2();
    Mac48Address receiverAddress = hdr.GetAddr1();
    if(senderAddress=="00:00:00:00:00:00") {
        return;
    }
    if(!checkIfReceiver(receiverAddress))
    {
        return;
    }
    if (senderAddress == Mac48Address("ff:ff:ff:ff:ff:ff") || receiverAddress == Mac48Address("ff:ff:ff:ff:ff:ff")) {
        // This is a broadcast packet, so we can return or skip processing this packet
        return;
    }

    int senderID = getIDfromAddress(senderAddress);
    Ptr<Node> sender = NodeList::GetNode(macToNodeID[senderAddress]);
    Ptr<Node> receiver = NodeList::GetNode(0);
    double distance = computeDistance(sender,receiver);

    if(v[senderID]!=0)
    {
        return;
    }
    v[senderID]++;

    double idealSignal = computeRSSI(transmitPower,channelFreqMhz,distance);

    std::cout<<"------------------------------------------------------------"<<std::endl;
    std::cout<<"\tDistance between devices: "<<distance<<std::endl;
    std::cout<<"\tChannel(MHZ): "<<channelFreqMhz<<std::endl;
    std::cout<<"\tIdeal Signal: "<<std::fixed<<std::setprecision(9)<<idealSignal<<std::endl;
    std::cout<<"\tActual Signal: "<<signalNoise.signal<<std::endl;
    std::cout<<"\tNoise: "<<signalNoise.noise<<std::endl;
    std::cout<<"------------------------------------------------------------"<<std::endl;
}
double* diffs;
double* idealDiffs;
/*//void MonitorSnifferRx_TDoA(std::string context, Ptr<const Packet> packet, uint16_t channelFreqMhz, WifiTxVector txVector, MpduInfo aMpdu, SignalNoiseDbm signalNoise, uint16_t staId)
{
    WifiMacHeader hdr;
    packet->PeekHeader(hdr);

    Mac48Address senderAddress = hdr.GetAddr2();
    Mac48Address receiverAddress = hdr.GetAddr1();
    if(senderAddress=="00:00:00:00:00:00") {
        return;
    }
    if(!checkIfReceiver(receiverAddress))
    {
        return;
    }
    if (senderAddress == Mac48Address("ff:ff:ff:ff:ff:ff") || receiverAddress == Mac48Address("ff:ff:ff:ff:ff:ff")) {
        // This is a broadcast packet, so we can return or skip processing this packet
        return;
    }

    int senderID = getIDfromAddress(senderAddress);
    Ptr<Node> sender = NodeList::GetNode(macToNodeID[senderAddress]);
    Ptr<Node> receiver = NodeList::GetNode(0);
    //double distance = computeDistance(sender,receiver);

    if(v[senderID]!=0)
    {
        return;
    }
    v[senderID]++;
}*/

void MonitorSnifferRx_ToA(std::string context, Ptr<const Packet> packet, uint16_t channelFreqMhz, WifiTxVector txVector, MpduInfo aMpdu, SignalNoiseDbm signalNoise, uint16_t staId)
{
    WifiMacHeader hdr;
    packet->PeekHeader(hdr);

    Mac48Address senderAddress = hdr.GetAddr2();
    Mac48Address receiverAddress = hdr.GetAddr1();
    if(senderAddress=="00:00:00:00:00:00") {
        return;
    }
    if(!checkIfReceiver(receiverAddress))
    {
        return;
    }
    if (senderAddress == Mac48Address("ff:ff:ff:ff:ff:ff") || receiverAddress == Mac48Address("ff:ff:ff:ff:ff:ff")) {
        // This is a broadcast packet, so we can return or skip processing this packet
        return;
    }

    int senderID = getIDfromAddress(senderAddress);
    Ptr<Node> sender = NodeList::GetNode(macToNodeID[senderAddress]);
    Ptr<Node> receiver = NodeList::GetNode(0);
    double distance = computeDistance(sender,receiver);

    if(v[senderID]!=0)
    {
        return;
    }
    v[senderID]++;

    double idealToA = computeToA(distance);
    double ToA = Simulator::Now().GetSeconds()-times[senderID];
    std::cout<<"------------------------------------------------------------"<<std::endl;
    std::cout<<"\tDistance between devices: "<<distance<<std::endl;
    std::cout<<"\tChannel(MHZ): "<<channelFreqMhz<<std::endl;
    std::cout<<"\tIdeal Time of Arrival: "<<std::fixed<<std::setprecision(9)<<idealToA<<"s"<<std::endl;
    std::cout<<"\tActual Time of Arrival: "<<ToA<<"s"<<std::endl;
    std::cout<<"------------------------------------------------------------"<<std::endl;
}

std::vector<Vector> parseFile(std::string filename, std::string &alg, int &nr)
{
    std::ifstream file(filename);
    if(!file.is_open())
    {
        std::cerr<<"Unable to open file!"<<std::endl;
    }
    file>>alg;

    double x;
    double y;
    file>>x>>y;
    stationPos = Vector(x,y,0.0);
    file>>nr;
    std::vector<Vector> APs;
    while(!file.eof())
    {
        file>>x>>y;
        Vector pos = Vector(x,y,0.0);
        APs.push_back(pos);
    }
    return APs;
}


void
oldSetup(std::string filename)
{
    Packet::EnablePrinting();
    Packet::EnableChecking();

    std::vector<Vector> APs = parseFile(filename,algorithm,numberOfAPs);

    NodeContainer stationNode;
    stationNode.Create(1);

    NodeContainer apNodes;
    apNodes.Create(numberOfAPs);

    WifiHelper wifi;
    wifi.SetStandard(WifiStandard::WIFI_STANDARD_80211a);
    wifi.SetRemoteStationManager("ns3::AarfWifiManager");

    YansWifiPhyHelper phy = YansWifiPhyHelper();
    phy.SetErrorRateModel("ns3::YansErrorRateModel");
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    phy.SetChannel(channel.Create());

    WifiMacHelper mac;

    NetDeviceContainer apDevices;

    for(uint32_t i=0;i<apNodes.GetN();++i)
    {
        // Get SSID of the AP by its number
        std::ostringstream  oss;
        oss << "ns3-ssid-"<<i;
        Ssid ssid = Ssid(oss.str());

        // Set AP properties
        mac.SetType("ns3::ApWifiMac",
                    "Ssid", SsidValue(ssid),
                    "EnableBeaconJitter", BooleanValue(false),
                    "BeaconGeneration", BooleanValue(true));

        NetDeviceContainer apDevice = wifi.Install(phy,mac,apNodes.Get(i));
        apDevices.Add(apDevice);
    }
    receivingAddresses = (Mac48Address*)calloc(numberOfAPs,sizeof(Mac48Address));
    NetDeviceContainer staInterfaces;
    for(uint32_t i=0;i<apNodes.GetN();++i)
    {
        // Retrieve SSID of the AP corresponding to the STA interface number
        std::ostringstream oss;
        oss<<"ns3-ssid-"<<i;
        Ssid ssid = Ssid(oss.str());

        // Set STA properties
        mac.SetType("ns3::StaWifiMac",
                    "Ssid", SsidValue(ssid),
                    "ActiveProbing", BooleanValue(true));

        phy.Set("RxSensitivity", DoubleValue(-100));

        NetDeviceContainer staInterface = wifi.Install(phy,mac,stationNode);
        Mac48Address staAddress = Mac48Address::ConvertFrom(staInterface.Get(0)->GetAddress());
        receivingAddresses[i] = staAddress;
        staInterfaces.Add(staInterface);
    }

    MobilityHelper mobility;

    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    for(std::size_t i = 0; i<APs.size();i++)
    {
        Vector ap = APs[i];
        positionAlloc->Add(ap);
    }
    positionAlloc->Add(Vector(5.0,5.0,0.0)); // Position for STA

    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(apNodes);
    mobility.Install(stationNode);

    for (uint32_t i = 0; i < apDevices.GetN(); ++i)
    {
        Mac48Address dest = Mac48Address::ConvertFrom(staInterfaces.Get(i)->GetAddress());
        SendPacket(apDevices.Get(i),dest);
    }
}

void
currentSetup(std::string filename)
{
    Packet::EnablePrinting();
    Packet::EnableChecking();

    std::vector<Vector> APs = parseFile(filename,algorithm,numberOfAPs);

    NodeContainer stationNode;
    stationNode.Create(1);

    NodeContainer apNodes;
    apNodes.Create(numberOfAPs);

    WifiHelper wifi;
    wifi.SetStandard(WifiStandard::WIFI_STANDARD_80211a);
    wifi.SetRemoteStationManager("ns3::AarfWifiManager");

    YansWifiPhyHelper phy = YansWifiPhyHelper();
    phy.SetErrorRateModel("ns3::YansErrorRateModel");

    WifiMacHelper mac;

    NetDeviceContainer apDevices;

    receivingAddresses = (Mac48Address*)calloc(numberOfAPs,sizeof(Mac48Address));
    NetDeviceContainer staInterfaces;

    for(uint32_t i=0;i<apNodes.GetN();++i)
    {
        YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
        phy.SetChannel(channel.Create());

        // Get SSID of the AP by its number
        std::ostringstream  oss;
        oss << "ns3-ssid-"<<i;
        Ssid ssid = Ssid(oss.str());

        // Set AP properties
        mac.SetType("ns3::ApWifiMac",
                    "Ssid", SsidValue(ssid),
                    "EnableBeaconJitter", BooleanValue(false),
                    "BeaconGeneration", BooleanValue(true));

        NetDeviceContainer apDevice = wifi.Install(phy,mac,apNodes.Get(i));
        Mac48Address apAddress = Mac48Address::ConvertFrom(apDevice.Get(0)->GetAddress());
        macToNodeID[apAddress] = apNodes.Get(i)->GetId();

        apDevices.Add(apDevice);

        mac.SetType("ns3::StaWifiMac",
                    "Ssid", SsidValue(ssid),
                    "ActiveProbing", BooleanValue(true));

        //phy.Set("RxSensitivity", DoubleValue(-100));
        NetDeviceContainer staInterface = wifi.Install(phy,mac,stationNode);
        Mac48Address staAddress = Mac48Address::ConvertFrom(staInterface.Get(0)->GetAddress());
        receivingAddresses[i] = staAddress;
        staInterfaces.Add(staInterface);
    }

    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    for(std::size_t i = 0; i<APs.size();i++)
    {
        Vector ap = APs[i];
        positionAlloc->Add(ap);
    }
    positionAlloc->Add(stationPos); // Position for STA

    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(apNodes);
    mobility.Install(stationNode);

    for (uint32_t i = 0; i < apDevices.GetN(); ++i)
    {
        Mac48Address dest = Mac48Address::ConvertFrom(staInterfaces.Get(i)->GetAddress());
        SendPacket(apDevices.Get(i),dest);
    }

}

void connectTraceSourceFromAlgorithm(std::string alg)
{
    std::string phyPath = "/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Phy/MonitorSnifferRx";
    if(alg == "tof")
    {
        Config::Connect(phyPath,MakeCallback(&MonitorSnifferRx_ToF));
    }
    else if(alg == "toa")
    {
        Config::Connect(phyPath,MakeCallback(&MonitorSnifferRx_ToA));
    }
    else if(alg == "rssi")
    {
        Config::Connect(phyPath,MakeCallback(&MonitorSnifferRx_RSSI));
    }
    /*else if(alg == "tdoa")
    {
        diffs = (double*)calloc(numberOfAPs-1,sizeof(double));
        idealDiffs = (double*)calloc(numberOfAPs-1,sizeof(double));
        Config::Connect(phyPath,MakeCallback(&MonitorSnifferRx_TDoA));
    }*/
    else
    {
        std::cout<<"Cannot connect to anything!";
    }
}

int main(int argc, char* argv[])
{
    std::string filename;
    CommandLine cmd;
    cmd.AddValue("inputFile", "Path to the input file", filename);
    cmd.Parse(argc, argv);

    currentSetup(filename);
    connectTraceSourceFromAlgorithm(algorithm);

    Simulator::Stop(Seconds(1.0));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}