#ifndef __SIMPLEWIRELESS_SIMPLEAPP_H_
#define __SIMPLEWIRELESS_SIMPLEAPP_H_

#include <omnetpp.h>
#include "inet/common/INETDefs.h"
#include "inet/linklayer/common/MacAddress.h"
#include "inet/physicallayer/contract/packetlevel/IRadio.h"


using namespace omnetpp;
using namespace inet;

class SimpleApp : public cSimpleModule {
  protected:
    virtual void initialize(int) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg);

    virtual void transmitPacket();
    virtual MacAddress resolveDestMacAddress();

    int payload;
    const char *destAddr;
    simtime_t period;


};

#endif
