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

#ifndef __SIMPLEWIRELESS_ALOHA_IEEE802154MAC_H_
#define __SIMPLEWIRELESS_ALOHA_IEEE802154MAC_H_
#include "inet/queueing/contract/IPacketQueue.h"
#include "inet/linklayer/base/MacProtocolBase.h"
#include "inet/linklayer/common/MacAddress.h"
#include "inet/linklayer/contract/IMacProtocol.h"
#include "inet/physicallayer/contract/packetlevel/IRadio.h"


namespace inet {


class Aloha_Ieee802154Mac : public MacProtocolBase, public IMacProtocol {
  public:
    Aloha_Ieee802154Mac()
        : MacProtocolBase()
        , txTime(0)
        , idleTime(0)
        , rxTime(0)
        , lastTxTime(0)
        , lastRxTime(0)
        ,lastBusyRxTime(0)
        , nbTxFrames(0)
        , nbRxFrames(0)
        , nbMissedAcks(0)
        , nbRecvdAcks(0)
        , nbDroppedFrames(0)
        , nbTxAcks(0)
        , nbDuplicates(0)
        , nbBackoffs(0)
        , backoffValues(0)
        , backoffTimer(nullptr), ccaTimer(nullptr), sifsTimer(nullptr), rxAckTimer(nullptr)
        , macState(IDLE_1)
        , status(STATUS_OK)
        , radio(nullptr)
        , transmissionState(physicallayer::IRadio::TRANSMISSION_STATE_UNDEFINED)
        , sifs()
        , macAckWaitDuration()
        , headerLength(0)
        , transmissionAttemptInterruptedByRx(false)
        , ccaDetectionTime()
        , rxSetupTime()
        , aTurnaroundTime()
        , macMaxCSMABackoffs(0)
        , macMaxFrameRetries(0)
        , aUnitBackoffPeriod()
        , useMACAcks(false)
        , backoffMethod(CONSTANT)
        , macMinBE(0)
        , macMaxBE(0)
        , initialCW(0)
        , txPower(0)
        , NB(0)
        , txAttempts(0)
        , bitrate(0)
        , ackLength(0)
        , ackMessage(nullptr)
        , SeqNrParent()
        , SeqNrChild()
    {}

    virtual ~Aloha_Ieee802154Mac();

    /** @brief Initialization of the module and some variables*/
    virtual void initialize(int) override;

    /** @brief Delete all dynamically allocated objects of the module*/
    virtual void finish() override;

    /** @brief Handle messages from lower layer */
    virtual void handleLowerPacket(Packet *packet) override;

    /** @brief Handle messages from upper layer */
    virtual void handleUpperPacket(Packet *packet) override;

    /** @brief Handle self messages such as timers */
    virtual void handleSelfMessage(cMessage *) override;

    /** @brief Handle control messages from lower layer */
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, intval_t value, cObject *details) override;

  protected:
    /** @name Different tracked statistics.*/
    /*@{*/

    simtime_t txTime;
    simtime_t rxTime;
    simtime_t idleTime;
    simtime_t lastRxTime;
    simtime_t lastTxTime;
    simtime_t lastBusyRxTime;
    long nbTxFrames;
    long nbRxFrames;
    long nbMissedAcks;
    long nbRecvdAcks;
    long nbDroppedFrames;
    long nbTxAcks;
    long nbDuplicates;
    long nbBackoffs;
    double backoffValues;
    /*@}*/

    /** @brief MAC states
     * see states diagram.
     */
    enum t_mac_states {
        IDLE_1 = 1,
        BACKOFF_2,
        CCA_3,
        TRANSMITFRAME_4,
        WAITACK_5,
        WAITSIFS_6,
        TRANSMITACK_7
    };

    /*************************************************************/
    /****************** TYPES ************************************/
    /*************************************************************/

    /** @brief Kinds for timer messages.*/
    enum t_mac_timer {
        TIMER_NULL = 0,
        TIMER_BACKOFF,
        TIMER_CCA,
        TIMER_SIFS,
        TIMER_RX_ACK,
    };

    /** @name Pointer for timer messages.*/
    /*@{*/
    cMessage *backoffTimer, *ccaTimer, *sifsTimer, *rxAckTimer;
    /*@}*/

    /** @brief MAC state machine events.
     * See state diagram.*/
    enum t_mac_event {
        EV_SEND_REQUEST = 1,    // 1, 11, 20, 21, 22
        EV_TIMER_BACKOFF,    // 2, 7, 14, 15
        EV_FRAME_TRANSMITTED,    // 4, 19
        EV_ACK_RECEIVED,    // 5
        EV_ACK_TIMEOUT,    // 12
        EV_FRAME_RECEIVED,    // 15, 26
        EV_DUPLICATE_RECEIVED,
        EV_TIMER_SIFS,    // 17
        EV_BROADCAST_RECEIVED,    // 23, 24
        EV_TIMER_CCA
    };

    /** @brief Types for frames sent by the CSMA.*/
    enum t_csma_frame_types {
        DATA,
        ACK
    };

    enum t_mac_carrier_sensed {
        CHANNEL_BUSY = 1,
        CHANNEL_FREE
    };

    enum t_mac_status {
        STATUS_OK = 1,
        STATUS_ERROR,
        STATUS_RX_ERROR,
        STATUS_RX_TIMEOUT,
        STATUS_FRAME_TO_PROCESS,
        STATUS_NO_FRAME_TO_PROCESS,
        STATUS_FRAME_TRANSMITTED
    };

    /** @brief The different back-off methods.*/
    enum backoff_methods {
        /** @brief Constant back-off time.*/
        CONSTANT = 0,
        /** @brief Linear increasing back-off time.*/
        LINEAR,
        /** @brief Exponentially increasing back-off time.*/
        EXPONENTIAL,
    };

    /** @brief keep track of MAC state */
    t_mac_states macState;
    t_mac_status status;

    /** @brief The radio. */
    physicallayer::IRadio *radio;
    physicallayer::IRadio::TransmissionState transmissionState;
    physicallayer::IRadio::ReceptionState receptionState;

    /** @brief Maximum time between a packet and its ACK
     *
     * Usually this is slightly more then the tx-rx turnaround time
     * The channel should stay clear within this period of time.
     */
    simtime_t sifs;

    /** @brief The amount of time the MAC waits for the ACK of a packet.*/
    simtime_t macAckWaitDuration;

    /** @brief Length of the header*/
    int headerLength;

    bool transmissionAttemptInterruptedByRx;
    /** @brief CCA detection time */
    simtime_t ccaDetectionTime;
    /** @brief Time to setup radio from sleep to Rx state */
    simtime_t rxSetupTime;
    /** @brief Time to switch radio from Rx to Tx state */
    simtime_t aTurnaroundTime;
    /** @brief maximum number of extra backoffs (excluding the first unconditional one) before frame drop */
    int macMaxCSMABackoffs;
    /** @brief maximum number of frame retransmissions without ack */
    unsigned int macMaxFrameRetries;
    /** @brief base time unit for calculating backoff durations */
    simtime_t aUnitBackoffPeriod;
    /** @brief Stores if the MAC expects Acks for Unicast packets.*/
    bool useMACAcks;

    /** @brief Defines the backoff method to be used.*/
    backoff_methods backoffMethod;

    /**
     * @brief Minimum backoff exponent.
     * Only used for exponential backoff method.
     */
    int macMinBE;
    /**
     * @brief Maximum backoff exponent.
     * Only used for exponential backoff method.
     */
    int macMaxBE;

    /** @brief initial contention window size
     * Only used for linear and constant backoff method.*/
    int initialCW;

    /** @brief The power (in mW) to transmit with.*/
    double txPower;

    /** @brief number of backoff performed until now for current frame */
    int NB;

    /** @brief count the number of tx attempts
     *
     * This holds the number of transmission attempts for the current frame.
     */
    unsigned int txAttempts;

    /** @brief the bit rate at which we transmit */
    double bitrate;

    /** @brief The bit length of the ACK packet.*/
    int ackLength;

  protected:
    /** @brief Generate new interface address*/
    virtual void configureInterfaceEntry() override;
    virtual void handleCommand(cMessage *msg) {}

    // FSM functions
    void fsmError(t_mac_event event, cMessage *msg);
    void executeMac(t_mac_event event, cMessage *msg);
    void updateStatusIdle(t_mac_event event, cMessage *msg);
    void updateStatusBackoff(t_mac_event event, cMessage *msg);
    void updateStatusCCA(t_mac_event event, cMessage *msg);
    void updateStatusTransmitFrame(t_mac_event event, cMessage *msg);
    void updateStatusWaitAck(t_mac_event event, cMessage *msg);
    void updateStatusSIFS(t_mac_event event, cMessage *msg);
    void updateStatusTransmitAck(t_mac_event event, cMessage *msg);
    void updateStatusNotIdle(cMessage *msg);
    void manageQueue();
    void updateMacState(t_mac_states newMacState);

    void attachSignal(Packet *mac, simtime_t_cref startTime);
    void manageMissingAck(t_mac_event event, cMessage *msg);
    void startTimer(t_mac_timer timer);

    virtual simtime_t scheduleBackoff();

    virtual void decapsulate(Packet *packet);

    Packet *ackMessage;

    //sequence number for sending, map for the general case with more senders
    //also in initialisation phase multiple potential parents
    std::map<MacAddress, unsigned long> SeqNrParent;    //parent -> sequence number

    //sequence numbers for receiving
    std::map<MacAddress, unsigned long> SeqNrChild;    //child -> sequence number

  private:
    /** @brief Copy constructor is not allowed.
     */
    Aloha_Ieee802154Mac(const Aloha_Ieee802154Mac&);
    /** @brief Assignment operator is not allowed.
     */
    Aloha_Ieee802154Mac& operator=(const Aloha_Ieee802154Mac&);
};

} // namespace inet

#endif // ifndef __INET_IEEE802154MAC_H

