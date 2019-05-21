
using static System.Console;
using static CANAPE.Cli.ConsoleUtils;

var template = new FixedProxyTemplate();
template.LocalPort = 10025;
template.Host = "example.com";
template.Port = 80;

var service = template.Create();
service.Start();

WriteLine("Created {0}", service);
WriteLine("Press Enter to exit...");
ReadLine();
service.Stop();

var packets = service.Packets;
WriteLine("Captured {0} packets:", packets.Count);
WritePackets(packets);
