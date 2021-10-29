//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 
#include "inet/common/lifecycle/LifecycleController.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/common/ModuleAccess.h"
#include "inet/physicallayer/contract/packetlevel/IRadio.h"
#include "MyNodeStatus.h"
#include "MySimpleCcBattery.h"
namespace inet {

namespace power {
Define_Module(MySimpleCcBattery);


void MySimpleCcBattery::initialize(int stage)
{
    CcEnergyStorageBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        nominalCapacity = C(par("nominalCapacity"));
        nominalVoltage = V(par("nominalVoltage"));
        internalResistance = Ohm(par("internalResistance"));
        networkNode = findContainingNode(this);
        if (networkNode != nullptr) {
            nodeStatus = dynamic_cast<MyNodeStatus *>(networkNode->getSubmodule("status"));
            nodeStatus1 = dynamic_cast<MyNodeStatus *>(networkNode->getSubmodule("status"));
            cModule *radioModule = networkNode->getSubmodule("wlan")->getSubmodule("radio");
            radio = check_and_cast<inet::physicallayer::IRadio *>(radioModule);
            if (!nodeStatus)
                throw cRuntimeError("Cannot find node status");
        }
        setResidualCapacity(C(par("initialCapacity")));
        WATCH(residualCapacity);
    }
}
void MySimpleCcBattery::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

void MySimpleCcBattery::updateResidualCapacity()

{

    simtime_t currentTime = simTime();
    if (currentTime != lastResidualCapacityUpdate) {
        C newResidualCapacity = residualCapacity + s((currentTime - lastResidualCapacityUpdate).dbl()) * (totalCurrentGeneration - totalCurrentConsumption);

        if (newResidualCapacity < C(0)){
            newResidualCapacity = C(0);
                nodeStatus1->mySetState(NodeStatus::DOWN);
                radio->setRadioMode(inet::physicallayer::IRadio::RADIO_MODE_OFF);
                simsignal_t dischargeTimeId = registerSignal("dischargeTime");
                emit(dischargeTimeId,simTime());

        }
        else if (newResidualCapacity > nominalCapacity)
            newResidualCapacity = nominalCapacity;
            setResidualCapacity(newResidualCapacity);
    }
}
}}
