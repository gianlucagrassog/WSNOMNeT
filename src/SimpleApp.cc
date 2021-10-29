#include "SimpleApp.h"
#include "inet/linklayer/common/MacAddressTag_m.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/packet/Packet.h"

Define_Module(SimpleApp);

void SimpleApp::initialize(int stage) {

    if(stage == INITSTAGE_LOCAL) {

        payload = par("payload");
        destAddr = par("destAddress").stringValue();
        period = par("period");
    } else if(stage == INITSTAGE_APPLICATION_LAYER) {
        if(period > 0) {
            cMessage *tim = new cMessage("TxTimer");
            scheduleAt(period, tim);
        }
    }
}

void SimpleApp::handleMessage(cMessage *msg) {
    if(msg->isSelfMessage()) {
        if(strcmp(msg->getName(), "TxTimer") == 0) {
            transmitPacket();
            scheduleAt(simTime()+period, msg);
            return;
        }
        error("Timer non conosciuto");
    }

    EV << "Arrived Packet: " << msg->getName() << endl;
    delete msg;
}

void SimpleApp::transmitPacket() {
    auto data = makeShared<ByteCountChunk>(B(payload));
    Packet *pkt = new Packet("DataPacket", data);
    MacAddressReq *macAddressTag = pkt->addTag<MacAddressReq>();
    macAddressTag->setDestAddress(resolveDestMacAddress());
    pkt->addTag<PacketProtocolTag>()->setProtocol(&Protocol::nextHopForwarding);
    send(pkt, "lowerLayerOut");
}

MacAddress SimpleApp::resolveDestMacAddress() {
    MacAddress destMacAddress;
    if (destAddr[0]) {
        if (!destMacAddress.tryParse(destAddr))
            destMacAddress = L3AddressResolver().resolve(destAddr, L3AddressResolver::ADDR_MAC).toMac();
    }
    return destMacAddress;
}

