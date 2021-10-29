package simplewireless;

import inet.common.lifecycle.NodeStatus;
import inet.applications.ethernet.EtherTrafGen;
import inet.linklayer.ieee802154.Ieee802154NarrowbandInterface;
import inet.power.contract.IEnergyConsumer;
import inet.power.contract.IEnergyGenerator;
import inet.power.contract.IEnergyManagement;
import inet.power.contract.IEnergyStorage;
import inet.mobility.contract.IMobility;
import inet.networklayer.common.InterfaceTable;

module Node
{
    parameters:
        @networkNode;
        @display("bgb=466,408;i=misc/sensor2");
        bool hasStatus = default(false);
        mobility.typename = default("StationaryMobility");
        mobility.subjectModule = default("^");
        *.mobilityModule = default(absPath(".mobility"));
        **.energySourceModule = default(absPath(".energyStorage"));
        *.energyGeneratorModule = default(exists(energyGenerator) ? absPath(".energyGenerator") : "");
        *.interfaceTableModule = default(absPath(".interfaceTable"));
        wlan.radio.antenna.mobilityModule = default("^.^.^.mobility");

    gates:
        input radioIn @directIn;
    submodules:
        status: MyNodeStatus if hasStatus {
            parameters:
                @display("p=157,31;is=s");
        }
        wlan: Aloha_Ieee802154NarrowbandInterface {
            @display("p=267,305");
        }
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=82,361;is=s");
        }
        energyStorage: <default("")> like IEnergyStorage if typename != "" {
            parameters:
                @display("p=82,101;is=s");
        }
        energyManagement: <default("")> like IEnergyManagement if typename != "" {
            parameters:
                @display("p=82,166;is=s");
        }
        energyGenerator: <default("")> like IEnergyGenerator if typename != "" {
            parameters:
                @display("p=82,239;is=s");
        }
        mobility: <default("")> like IMobility if typename != "" {
            parameters:
                @display("p=82,31;is=s");
        }
        app: SimpleApp {
            @display("p=267,132");
        }

    connections:
        wlan.radioIn <-- radioIn;
        app.lowerLayerOut --> wlan.upperLayerIn;
        wlan.upperLayerOut --> app.lowerLayerIn;
}
