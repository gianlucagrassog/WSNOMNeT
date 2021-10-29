package simplewireless.simulations;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.physicallayer.energyconsumer.SensorStateBasedCcEnergyConsumer;
import inet.physicallayer.ieee802154.packetlevel.Ieee802154NarrowbandScalarRadioMedium;
import inet.physicallayer.pathloss.LogNormalShadowing;
import inet.visualizer.integrated.IntegratedCanvasVisualizer;
import simplewireless.Node;


network TestNet
{
    parameters:
        @display("bgb=910,371");
        int NumHost;
    submodules:
        source[NumHost]: Node {
            @display("p=90,95");
        }
        sink: Node {
            @display("p=95,95");
        }
        radioMedium: Ieee802154NarrowbandScalarRadioMedium {
            @display("p=77,34");
        }

        configurator: Ipv4NetworkConfigurator {
            @display("p=297.05,34.275");
        }
}
