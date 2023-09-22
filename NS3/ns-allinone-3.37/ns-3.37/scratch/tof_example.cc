#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/wifi-module.h"
#include "ns3/yans-wifi-helper.h"

// Default Network Topology
//              10.1.1.0/24
//  AP  --------------------------------------  AP
//  |                                           |
//  |                                           |
//  |                                           |
//  |                  |---|                    |
//  |                  | MD|                    |
//  |                  |---|                    |
//  |                                           |
//  |                                           |
//  AP --------------------------------------- AP

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("ToF_example");

class TimestampTag : public Tag
{
  public:
    TimestampTag()
    {
    }

    TimestampTag(double time)
        : m_timestamp(time)
    {
    }

    static TypeId GetTypeId()
    {
        static TypeId tid =
            TypeId("ns3::TimestampTag").SetParent<Tag>().AddConstructor<TimestampTag>();
        return tid;
    }

    TypeId GetInstanceTypeId() const override
    {
        return GetTypeId();
    }

    uint32_t GetSerializedSize() const override
    {
        return sizeof(m_timestamp);
    }

    void Serialize(TagBuffer i) const override
    {
        i.WriteDouble(m_timestamp);
    }

    void Deserialize(TagBuffer i) override
    {
        m_timestamp = i.ReadDouble();
    }

    void Print(std::ostream& os) const override
    {
        os << "Timestamp: " << m_timestamp;
    }

    double GetTimestamp() const
    {
        return m_timestamp;
    }

  private:
    double m_timestamp;
};

class CustomApplication : public Application
{
  public:
    CustomApplication()
        : m_running(false)
    {
    }

    CustomApplication(Ptr<Socket> socket, Address address)
        : m_socket(socket),
          m_peer(address),
          m_running(false)
    {
    }

    static TypeId GetTypeId()
    {
        static TypeId tid = TypeId("CustomApplication")
                                .SetParent<Application>()
                                .AddConstructor<CustomApplication>();
        return tid;
    }

    void StartApplication() override
    {
        NS_LOG_UNCOND("Starting application!");
        m_running = true;
        m_socket->Bind(m_local);
        m_socket->Connect(m_peer);
        for (double ct = 1; ct <= 9; ct++)
        {
            Simulator::Schedule(Seconds(ct+1), &CustomApplication::SendPacket, this);
        }
    }

    void StopApplication() override
    {
        m_running = false;
        if (m_socket)
        {
            m_socket->Close();
        }
    }

    void SetSocket(Ptr<Socket> socket)
    {
        m_socket = socket;
    }

    void SetPeer(Address address)
    {
        m_peer = address;
    }
    void SetLocal(Address address)
    {
        m_local = address;
    }

  private:

    void SendPacket()
    {
        Ptr<Packet> packet = Create<Packet>(512);

        // Attach a timestamp tag
        TimestampTag timestampTag(Simulator::Now().GetSeconds());
        packet->AddPacketTag(timestampTag);

        m_socket->Send(packet);
        if (m_running)
        {
            NS_LOG_UNCOND("Scheduling next packet for 1 second later.");
        }
    }

    Ptr<Socket> m_socket;
    Address m_local;
    Address m_peer;
    bool m_running;
};

class CustomSinkApplication : public Application
{
  public:
    CustomSinkApplication()
    {
    }

    ~CustomSinkApplication() override
    {
    }

    static TypeId GetTypeId()
    {
        static TypeId tid = TypeId("CustomSinkApplication")
                                .SetParent<Application>()
                                .AddConstructor<CustomSinkApplication>();
        return tid;
    }

    void StartApplication() override
    {
        NS_LOG_UNCOND("Starting sink application!");
        //m_socket->Bind(m_local);
        m_socket->SetRecvCallback(MakeCallback(&CustomSinkApplication::HandleRead, this));
    }

    void StopApplication() override
    {
        if (m_socket)
        {
            m_socket->Close();
        }
    }

    void SetSocket(Ptr<Socket> socket)
    {
        m_socket = socket;
    }

    void SetLocal(Address local)
    {
        m_local = local;
    }

  private:
    void HandleRead ()
    {
        std::cout<<"MERG IN SFARSIT";
    }
    void ReceivePacket(Ptr<Socket> socket)
    {
        Address remoteAddr;
        Ptr<Packet> packet;
        while ((packet = socket->RecvFrom(remoteAddr)))
        {
            TimestampTag timestampTag;
            if (packet->PeekPacketTag(timestampTag))
            {
                double timestamp = timestampTag.GetTimestamp();
                double delay = Simulator::Now().GetSeconds() - timestamp;
                std::cout << "Time of flight: " << delay << "s" << std::endl;
            }
            else
            {
                std::cerr << "No timestamp tag found!" << std::endl;
            }
        }
    }

    Ptr<Socket> m_socket;
    Address m_local;
};

int
main(int argc, char* argv[])
{
    bool verbose = true;
    // uint8_t nAP = 0;

    CommandLine cmd(__FILE__);
    // cmd.AddValue("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
    // cmd.AddValue("nWifi", "Number of wifi STA devices", nWifi);
    // cmd.AddValue("nAP, "Number of AP devices in the system, nAP);
    cmd.AddValue("verbose", "Tell echo applications to log if true", verbose);

    cmd.Parse(argc, argv);

    NodeContainer apNodes;
    apNodes.Create(4);
    NodeContainer mobileNode;
    mobileNode.Create(1);

    // Create a Wifi helper and set it up
    WifiHelper wifi;
    wifi.SetStandard(WifiStandard::WIFI_STANDARD_80211a);

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy = YansWifiPhyHelper();
    phy.SetChannel(channel.Create());

    double txPower = 90;
    phy.Set("TxPowerStart", DoubleValue(txPower)); // in dBm
    phy.Set("TxPowerEnd", DoubleValue(txPower)); // in dBm

    WifiMacHelper mac;
    mac.SetType("ns3::ApWifiMac");

    NetDeviceContainer apDevices = wifi.Install(phy, mac, apNodes);

    mac.SetType("ns3::StaWifiMac");
    NetDeviceContainer staDevice = wifi.Install(phy, mac, mobileNode);

    MobilityHelper mobility;

    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));   // Position for AP1
    positionAlloc->Add(Vector(10.0, 0.0, 0.0));  // Position for AP2
    positionAlloc->Add(Vector(0.0, 10.0, 0.0));  // Position for AP3
    positionAlloc->Add(Vector(10.0, 10.0, 0.0)); // Position for AP4

    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(apNodes);

    Ptr<ListPositionAllocator> positionAllocSta = CreateObject<ListPositionAllocator>();
    positionAllocSta->Add(Vector(5.0, 5.0, 0.0)); // Position for mobileNode

    mobility.SetPositionAllocator(positionAllocSta);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(mobileNode);

    InternetStackHelper stack;
    stack.Install(apNodes);
    stack.Install(mobileNode);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");

    // First assign IP addresses to the APs
    Ipv4InterfaceContainer apInterfaces = address.Assign(apDevices);
    Ipv4InterfaceContainer staInterfaces = address.Assign(staDevice);

    int port = 5000;

    /*PacketSinkHelper sinkHelper("ns3::UdpSocketFactory",InetSocketAddress(staInterfaces.GetAddress(0),port));
    ApplicationContainer sinkApp = sinkHelper.Install(mobileNode);
    sinkApp.Start(Seconds(0.0));
    sinkApp.Stop(Seconds(10.0));*/

    ApplicationContainer apps;
    for (NodeContainer::Iterator i = apNodes.Begin(); i != apNodes.End(); ++i)
    {
        Ptr<Node> node = *i;
        Ptr<CustomApplication> app = CreateObject<CustomApplication>();
        Ptr<Socket> socket = Socket::CreateSocket(apNodes.Get(node->GetId()), UdpSocketFactory::GetTypeId());
        app->SetSocket(socket);
        app->SetPeer(InetSocketAddress(staInterfaces.GetAddress(0),port));
        app->SetLocal(InetSocketAddress(apInterfaces.GetAddress(node->GetId()),port));
        node->AddApplication(app);
        apps.Add(app);
    }
    apps.Start(Seconds(0.0));
    apps.Stop(Seconds(10.0));

    // Create a packet sink on the mobile node to receive data
    Ptr<Socket> sinkSocket = Socket::CreateSocket(mobileNode.Get(0), UdpSocketFactory::GetTypeId());
    Ptr<CustomSinkApplication> sinkApp = CreateObject<CustomSinkApplication>();
    sinkSocket->Bind(InetSocketAddress(staInterfaces.GetAddress(0),port));
    sinkApp->SetStartTime(Seconds(0.0));
    sinkApp->SetStopTime(Seconds(10.0));;

    sinkApp->SetSocket(sinkSocket);
    sinkApp->SetLocal(InetSocketAddress(Ipv4Address::GetAny(),port));
    mobileNode.Get(0)->AddApplication(sinkApp);


    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // Install PacketSocketHelper on all nodes
    /*PacketSocketHelper packetSocket;
    packetSocket.Install (apNodes);
    packetSocket.Install (mobileNode);*/

    Simulator::Stop(Seconds(15.0));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
