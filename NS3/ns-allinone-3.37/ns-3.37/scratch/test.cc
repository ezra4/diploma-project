#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/mesh-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("WifiMeshExample");

int main (int argc, char *argv[])
{
    // Number of Mesh Points
    uint32_t meshPoints = 3;

    // Set up log level
    LogComponentEnable ("WifiMeshExample", LOG_LEVEL_INFO);

    NodeContainer meshNodes;
    meshNodes.Create (meshPoints);

    // Create single static node
    Ptr<Node> staticNode = CreateObject<Node> ();

    // Setup physical layer attributes
    YansWifiPhyHelper phy;
    YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
    phy.SetChannel (wifiChannel.Create ());

    // Configure mesh devices
    MeshHelper mesh = MeshHelper::Default();
    mesh.SetStandard(WIFI_STANDARD_80211a);
    mesh.SetStackInstaller ("ns3::Dot11sStack");
    mesh.SetSpreadInterfaceChannels (MeshHelper::ZERO_CHANNEL);
    mesh.SetMacType();

    // Install the mesh devices to mesh nodes
    NetDeviceContainer meshDevices = mesh.Install (phy, meshNodes);

    // Setup static device (non-mesh node)
    WifiHelper wifi;
    wifi.SetStandard (WIFI_STANDARD_80211a);
    WifiMacHelper wifiMac;
    wifiMac.SetType ("ns3::StaWifiMac",
                    "Ssid", SsidValue(Ssid("test-ssid")),
                    "ActiveProbing", BooleanValue (false));
    NetDeviceContainer staDevices;
    staDevices = wifi.Install (phy, wifiMac, NodeContainer (staticNode));

    // Configure mobility
    MobilityHelper mobility;
    mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue (0.0),
                                  "MinY", DoubleValue (0.0),
                                  "DeltaX", DoubleValue (5.0),
                                  "DeltaY", DoubleValue (10.0),
                                  "GridWidth", UintegerValue (3),
                                  "LayoutType", StringValue ("RowFirst"));
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (meshNodes);
    mobility.Install (staticNode);

    // Internet stack
    InternetStackHelper stack;
    stack.Install (meshNodes);
    stack.Install (staticNode);

    // Set IP address
    Ipv4AddressHelper address;
    address.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer meshInterfaces = address.Assign (meshDevices);
    address.Assign (staDevices);

    // Your applications and communication setup would go here.

    Simulator::Run ();
    Simulator::Destroy ();

    return 0;
}