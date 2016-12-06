// EPOS TI CC2538 IEEE 802.15.4 NIC Mediator Declarations

#ifndef __cc2538_h
#define __cc2538_h

#include <ieee802_15_4.h>
#include "../common/ieee802_15_4_mac.h"
#include "../common/tstp_mac.h"

__BEGIN_SYS

// IT CC2538 IEEE 802.15.4 RF Transceiver
class CC2538RF: private Cortex_M_Model
{
protected:
    typedef CPU::Reg8 Reg8;
    typedef CPU::Reg16 Reg16;
    typedef CPU::Reg32 Reg32;
    typedef CPU::IO_Irq IO_Irq;
    typedef MMU::DMA_Buffer DMA_Buffer;
    typedef RTC::Microsecond Microsecond;

public:
    // Bases
    enum
    {
        FFSM_BASE       = 0x40088500,
        XREG_BASE       = 0x40088600,
        SFR_BASE        = 0x40088800,
        MACTIMER_BASE   = SFR_BASE,
        ANA_BASE        = SFR_BASE,
        RXFIFO          = 0x40088000,
        TXFIFO          = 0x40088200,
    };

    // Useful FFSM register offsets
    enum
    {
        SRCRESINDEX = 0x8c,
        PAN_ID0     = 0xc8,
        PAN_ID1     = 0xcc,
        SHORT_ADDR0 = 0xd0,
        SHORT_ADDR1 = 0xd4,
    };

    // Useful XREG register offsets
    enum
    {
        FRMFILT0    = 0x000,
        FRMFILT1    = 0x004,
        SRCMATCH    = 0x008,
        FRMCTRL0    = 0x024,
        FRMCTRL1    = 0x028,
        RXMASKSET   = 0x030,
        FREQCTRL    = 0x03C,    // RF carrier frequency (f = 11 + 5 (k – 11), with k [11, 26])  ro      0x0000000b
        FSMSTAT1    = 0x04C,    // Radio status register                                        ro      0x00000000
        FIFOPCTRL   = 0x050,
        RXFIRST     = 0x068,
        RXFIFOCNT   = 0x06C,    // Number of bytes in RX FIFO                                   ro      0x00000000
        TXFIFOCNT   = 0x070,    // Number of bytes in TX FIFO                                   ro      0x00000000
        RXFIRST_PTR = 0x074,
        RXLAST_PTR  = 0x078,
        RFIRQM0     = 0x08c,
        RFIRQM1     = 0x090,
        RFERRM      = 0x094,
        CSPT        = 0x194,
        AGCCTRL1    = 0x0c8,
        TXFILTCFG   = 0x1e8,
        FSCAL1      = 0x0b8,
        CCACTRL0    = 0x058,
        TXPOWER     = 0x040,
        RSSI        = 0x060,
        RSSISTAT    = 0x064,    // RSSI valid status register                                   ro      0x00000000
    };

    // Useful SFR register offsets
    enum
    {
        RFDATA  = 0x28,
        RFERRF  = 0x2c,
        RFIRQF1 = 0x30,
        RFIRQF0 = 0x34,
        RFST    = 0x38,         // RF CSMA-CA/strobe processor                                  rw       0x000000d0
    };

    // Useful MACTIMER offsets
    enum {       //Offset   Description                               Type    Value after reset
        MTCSPCFG = 0x00, // MAC Timer event configuration              RW     0x0
        MTCTRL   = 0x04, // MAC Timer control register                 RW     0x2
        MTIRQM   = 0x08, // MAC Timer interrupt mask                   RW     0x0
        MTIRQF   = 0x0C, // MAC Timer interrupt flags                  RW     0x0
        MTMSEL   = 0x10, // MAC Timer multiplex select                 RW     0x0
        MTM0     = 0x14, // MAC Timer multiplexed register 0           RW     0x0
        MTM1     = 0x18, // MAC Timer multiplexed register 1           RW     0x0
        MTMOVF2  = 0x1C, // MAC Timer multiplexed overflow register 2  RW     0x0
        MTMOVF1  = 0x20, // MAC Timer multiplexed overflow register 1  RW     0x0
        MTMOVF0  = 0x24, // MAC Timer multiplexed overflow register 0  RW     0x0
    };

    // ANA_REGS register
    enum
    {
        IVCTRL    = 0x04,
    };

    // Radio commands
    enum
    {
        STXON       = 0xd9,
        SFLUSHTX    = 0xde,
        ISSTART     = 0xe1,
        ISRXON      = 0xe3,
        ISTXON      = 0xe9,
        ISTXONCCA   = 0xea,
        ISSAMPLECCA = 0xeb,
        ISFLUSHRX   = 0xed,
        ISFLUSHTX   = 0xee,
        ISRFOFF     = 0xef,
        ISCLEAR     = 0xff,
    };

    // Useful bits in RSSISTAT
    enum
    {
        RSSI_VALID = 1 << 0,
    };
    // Useful bits in XREG_FRMFILT0
    enum
    {
        MAX_FRAME_VERSION = 1 << 2,
        PAN_COORDINATOR   = 1 << 1,
        FRAME_FILTER_EN   = 1 << 0,
    };
    // Useful bits in XREG_FRMFILT1
    enum
    {
        ACCEPT_FT3_MAC_CMD = 1 << 6,
        ACCEPT_FT2_ACK     = 1 << 5,
        ACCEPT_FT1_DATA    = 1 << 4,
        ACCEPT_FT0_BEACON  = 1 << 3,
    };
    // Useful bits in XREG_SRCMATCH
    enum
    {
        SRC_MATCH_EN   = 1 << 0,
    };

    // Useful bits in XREG_FRMCTRL0
    enum
    {
        APPEND_DATA_MODE = 1 << 7,
        AUTO_CRC         = 1 << 6,
        AUTO_ACK         = 1 << 5,
        ENERGY_SCAN      = 1 << 4,
        RX_MODE          = 1 << 2,
        TX_MODE          = 1 << 0,
    };
    enum RX_MODES
    {
        RX_MODE_NORMAL = 0,
        RX_MODE_OUTPUT_TO_IOC,
        RX_MODE_CYCLIC,
        RX_MODE_NO_SYMBOL_SEARCH,
    };

    // Bit set by hardware in FCS field when AUTO_CRC is set
    enum
    {
        AUTO_CRC_OK = 0x80,
    };

    // Useful bits in XREG_FRMCTRL1
    enum
    {
        PENDING_OR         = 1 << 2,
        IGNORE_TX_UNDERF   = 1 << 1,
        SET_RXENMASK_ON_TX = 1 << 0,
    };

    // Useful bits in XREG_FSMSTAT1
    enum
    {
        FIFO        = 1 << 7,
        FIFOP       = 1 << 6,
        SFD         = 1 << 5,
        CCA         = 1 << 4,
        SAMPLED_CCA = 1 << 3,
        LOCK_STATUS = 1 << 2,
        TX_ACTIVE   = 1 << 1,
        RX_ACTIVE   = 1 << 0,
    };

    // Useful bits in SFR_RFIRQF1
    enum
    {
        TXDONE = 1 << 1,
    };

    // RFIRQF0 Interrupts
    enum
    {
        INT_RXMASKZERO      = 1 << 7,
        INT_RXPKTDONE       = 1 << 6,
        INT_FRAME_ACCEPTED  = 1 << 5,
        INT_SRC_MATCH_FOUND = 1 << 4,
        INT_SRC_MATCH_DONE  = 1 << 3,
        INT_FIFOP           = 1 << 2,
        INT_SFD             = 1 << 1,
        INT_ACT_UNUSED      = 1 << 0,
    };

    // RFIRQF1 Interrupts
    enum
    {
        INT_CSP_WAIT   = 1 << 5,
        INT_CSP_STOP   = 1 << 4,
        INT_CSP_MANINT = 1 << 3,
        INT_RFIDLE     = 1 << 2,
        INT_TXDONE     = 1 << 1,
        INT_TXACKDONE  = 1 << 0,
    };

    // Useful bits in MTCTRL
    enum {           //Offset   Description                                                             Type    Value after reset
        MTCTRL_LATCH_MODE = 1 << 3, // 0: Reading MTM0 with MTMSEL.MTMSEL = 000 latches the high               RW      0
        // byte of the timer, making it ready to be read from MTM1. Reading
        // MTMOVF0 with MTMSEL.MTMOVFSEL = 000 latches the two
        // most-significant bytes of the overflow counter, making it possible to
        // read these from MTMOVF1 and MTMOVF2.
        // 1: Reading MTM0 with MTMSEL.MTMSEL = 000 latches the high
        // byte of the timer and the entire overflow counter at once, making it
        // possible to read the values from MTM1, MTMOVF0, MTMOVF1, and MTMOVF2.
        MTCTRL_STATE      = 1 << 2, // State of MAC Timer                                                      RO      0
        // 0: Timer idle
        // 1: Timer running
        MTCTRL_SYNC       = 1 << 1, // 0: Starting and stopping of timer is immediate; that is, synchronous    RW      1
        // with clk_rf_32m.
        // 1: Starting and stopping of timer occurs at the first positive edge of
        // the 32-kHz clock. For more details regarding timer start and stop,
        // see Section 22.4.
        MTCTRL_RUN        = 1 << 0, // Write 1 to start timer, write 0 to stop timer. When read, it returns    RW      0
        // the last written value.
    };

    // Useful bits in MSEL
    enum {
        MSEL_MTMOVFSEL = 1 << 4, // See possible values below
        MSEL_MTMSEL    = 1 << 0, // See possible values below
    };
    enum {
        OVERFLOW_COUNTER  = 0x00,
        OVERFLOW_CAPTURE  = 0x01,
        OVERFLOW_PERIOD   = 0x02,
        OVERFLOW_COMPARE1 = 0x03,
        OVERFLOW_COMPARE2 = 0x04,
    };
    enum {
        TIMER_COUNTER  = 0x00,
        TIMER_CAPTURE  = 0x01,
        TIMER_PERIOD   = 0x02,
        TIMER_COMPARE1 = 0x03,
        TIMER_COMPARE2 = 0x04,
    };
    enum {
        INT_OVERFLOW_COMPARE2 = 1 << 5,
        INT_OVERFLOW_COMPARE1 = 1 << 4,
        INT_OVERFLOW_PER      = 1 << 3,
        INT_COMPARE2          = 1 << 2,
        INT_COMPARE1          = 1 << 1,
        INT_PER               = 1 << 0
    };

    class Timer
    {
    private:
        const static unsigned int CLOCK = 32 * 1000 * 1000; // 32MHz

    public:
        typedef long long Time_Stamp;

    public:
        Timer() {}

        static Time_Stamp read() { return read((OVERFLOW_COUNTER * MSEL_MTMOVFSEL) | (TIMER_COUNTER * MSEL_MTMSEL)); }
        static Time_Stamp sfd() { return read(TIMER_CAPTURE * MSEL_MTMSEL); }
        static Time_Stamp now() { return read() + _offset; }

        static void set(const Time_Stamp & t) {
            mactimer(MTCTRL) |= MTCTRL_RUN; // Stop counting

            mactimer(MTMSEL) = (OVERFLOW_COUNTER * MSEL_MTMOVFSEL) | (TIMER_COUNTER * MSEL_MTMSEL);

            mactimer(MTMOVF0) = t >> 16ll;
            mactimer(MTMOVF1) = t >> 24ll;
            mactimer(MTMOVF2) = t >> 32ll; // MOVF2 must be written last
            _overflow_count = t >> 40ll;

            mactimer(MTM0) = t; // M0 must be written first
            mactimer(MTM1) = t >> 8ll;

            mactimer(MTCTRL) &= ~MTCTRL_RUN; // Restart counting
        }

        static void interrupt(const Time_Stamp & when, const IC::Interrupt_Handler & h) {
            mactimer(MTIRQM) = 0; // Mask interrupts
            _handler = h;
            mactimer(MTMSEL) = (OVERFLOW_COMPARE1 * MSEL_MTMOVFSEL) | (TIMER_COMPARE1 * MSEL_MTMSEL);
            mactimer(MTM0) = when;
            mactimer(MTM1) = when >> 8;
            mactimer(MTMOVF0) = when >> 16;
            mactimer(MTMOVF1) = when >> 24;
            mactimer(MTMOVF2) = when >> 32;
            _interrupt_overflow_count = when >> 40ll;

            mactimer(MTIRQF) = 0;
            Time_Stamp now = read();
            if(when <= now) {
                int_enable(INT_OVERFLOW_PER);
                _handler(49); // FIXME: this cannot be right!
            } else if((when >> 16ll) > (now >> 16ll))
                int_enable(INT_OVERFLOW_COMPARE1 | INT_OVERFLOW_PER);
            else if(when > now)
                int_enable(INT_COMPARE1 | INT_OVERFLOW_PER);
        }

        static void int_enable(const Reg32 & interrupt) { mactimer(MTIRQM) |= interrupt; }
        static void int_disable() { mactimer(MTIRQM) = INT_OVERFLOW_PER; }

        static Time_Stamp us_to_ts(const Microsecond & us) { return static_cast<long long>(us) * CLOCK / 1000000; }
        static Time_Stamp ts_to_us(const Microsecond & ts) { return static_cast<long long>(ts) * 1000000 / CLOCK; }

    private:
        static Time_Stamp read(unsigned int sel) {
            mactimer(MTMSEL) = sel;
            Time_Stamp ts = mactimer(MTM0); // M0 must be read first
            ts += mactimer(MTM1) << 8;
            ts += static_cast<long long>(mactimer(MTMOVF2)) << 32ll;
            ts += static_cast<long long>(mactimer(MTMOVF1)) << 24ll;
            ts += static_cast<long long>(mactimer(MTMOVF0)) << 16ll;
            ts += static_cast<long long>(_overflow_count) << 40ll;
            return ts;
        }

        static void int_handler(const IC::Interrupt_Id & interrupt) {
            Reg32 ints = mactimer(MTIRQF);
            mactimer(MTIRQF) = 0;
            if(ints & INT_OVERFLOW_PER) {
                _overflow_count++;
                if(_interrupt_overflow_count == _overflow_count)
                    int_enable(INT_OVERFLOW_COMPARE1);
            }
            if(ints & INT_OVERFLOW_COMPARE1)
                mactimer(MTIRQM) = (INT_COMPARE1 | INT_OVERFLOW_PER);
            else if(ints & INT_COMPARE1) {
                int_disable();
                _handler(interrupt);
            }
        }

        static void init() {
            mactimer(MTCTRL) |= MTCTRL_RUN; // Stop counting
            mactimer(MTIRQM) = 0; // Mask interrupts
            mactimer(MTIRQF) = 0; // Clear interrupts
            mactimer(MTCTRL) &= ~MTCTRL_SYNC; // We can't use the sync feature because we want to change the count and overflow values when the timer is stopped
            mactimer(MTCTRL) |= MTCTRL_LATCH_MODE; // count and overflow will be latched at once
            IC::int_vector(IC::INT_MACTIMER, &int_handler);
            IC::enable(33);
            int_enable(INT_OVERFLOW_PER);
        }

    private:
        static Time_Stamp _offset;
        static IC::Interrupt_Handler _handler;
        static Reg32 _overflow_count;
        static Reg32 _interrupt_overflow_count;
    };

public:
    CC2538RF() {
        // Enable clock to RF module
        ieee802_15_4_power(FULL);

        // Disable interrupts
        xreg(RFIRQM0) = 0;
        xreg(RFIRQM1) = 0;

        // Change recommended in the user guide (CCACTRL0 register description)
        xreg(CCACTRL0) = 0xf8;

        // Changes recommended in the user guide (Section 23.15 Register Settings Update)
        xreg(TXFILTCFG) = 0x09;
        xreg(AGCCTRL1) = 0x15;
        ana(IVCTRL) = 0x0b;
        xreg(FSCAL1) = 0x01;

        // Clear FIFOs
        sfr(RFST) = ISFLUSHTX;
        sfr(RFST) = ISFLUSHRX;

        // Reset result of source matching (value undefined on reset)
        ffsm(SRCRESINDEX) = 0;

        // Set FIFOP threshold to maximum
        xreg(FIFOPCTRL) = 0xff;

        // Maximize transmission power
        xreg(TXPOWER) = 0xff;

        // Disable counting of MAC overflows
        xreg(CSPT) = 0xff;

        // Clear interrupts
        sfr(RFIRQF0) = 0;
        sfr(RFIRQF1) = 0;

        // Clear error flags
        sfr(RFERRF) = 0;
    }

    void address(const IEEE802_15_4::Address & address) {
        ffsm(SHORT_ADDR0) = address[0];
        ffsm(SHORT_ADDR1) = address[1];
    }

    bool cca(const Microsecond & time) {
        Timer::Time_Stamp end = Timer::read() + Timer::us_to_ts(time);
        while(!(xreg(RSSISTAT) & RSSI_VALID));
        bool channel_free;
        while((channel_free = xreg(FSMSTAT1) & CCA) && (Timer::read() < end));
        return channel_free;
    }

    bool transmit() { sfr(RFST) = ISTXONCCA; return (xreg(FSMSTAT1) & SAMPLED_CCA); }

    bool wait_for_ack(const Microsecond & timeout) {
        // Disable and clear FIFOP int. We'll poll the interrupt flag
        xreg(RFIRQM0) &= ~INT_FIFOP;
        sfr(RFIRQF0) &= ~INT_FIFOP;

        // Save radio configuration
        Reg32 saved_filter_settings = xreg(FRMFILT1);
        bool was_promiscuous = promiscuous();

        // Accept only ACK frames now
        promiscuous(false);
        xreg(FRMFILT1) = ACCEPT_FT2_ACK;

        while(!tx_done());

        // Wait for either ACK or timeout
        bool acked = false;
        for(Timer::Time_Stamp end = Timer::read() + Timer::us_to_ts(timeout); (Timer::read() < end) && !(acked = sfr(RFIRQF0) & INT_FIFOP););

        // Restore radio configuration
        if(acked) {
            sfr(RFST) = ISFLUSHRX;
            sfr(RFIRQF0) &= ~INT_FIFOP;
        }
        xreg(FRMFILT1) = saved_filter_settings;
        promiscuous(was_promiscuous);
        xreg(RFIRQM0) |= INT_FIFOP;

        return acked;
    }

    void listen() { sfr(RFST) = ISRXON; }

    //FIXME: this doesn't work without the noinline attribute
    bool tx_done()__attribute__((noinline)) {
        bool ret = (sfr(RFIRQF1) & INT_TXDONE);
        if(ret)
            sfr(RFIRQF1) &= ~INT_TXDONE;
        return ret;
    }

    //FIXME: this doesn't work without the noinline attribute
    bool rx_done()__attribute__((noinline)) {
        bool ret = (sfr(RFIRQF0) & INT_RXPKTDONE);
        if(ret)
            sfr(RFIRQF0) &= ~INT_RXPKTDONE;
        return ret;
    }

    void promiscuous(bool on) {
        if(on)
            xreg(FRMFILT0) &= ~FRAME_FILTER_EN;
        else
            xreg(FRMFILT0) |= FRAME_FILTER_EN;
    }

    bool promiscuous() { return !(xreg(FRMFILT0) & FRAME_FILTER_EN); }

    void channel(unsigned int c) {
        assert((c > 10) && (c < 27));
        xreg(FREQCTRL) = 11 + 5 * (c - 11);
    }

    void copy_to_nic(IEEE802_15_4::Phy_Frame * frame) {
        // Clear TXFIFO
        sfr(RFST) = ISFLUSHTX;
        while(xreg(TXFIFOCNT) != 0);

        // Copy Frame to TXFIFO
        const char * f = reinterpret_cast<const char *>(frame);
        sfr(RFDATA) = f[0]; // len
        for(unsigned int i = 1; i <= f[0] - sizeof(IEEE802_15_4::CRC); i++)
            sfr(RFDATA) = f[i];
    }

    void copy_from_nic(IEEE802_15_4::Phy_Frame * frame) {
        char * f = reinterpret_cast<char *>(frame);
        f[0] = sfr(RFDATA);  // First byte is the length of MAC frame
        for(unsigned int i = 1; i <= f[0]; ++i)
            f[i] = sfr(RFDATA);
        sfr(RFST) = ISFLUSHRX;
    }

    bool filter() {
        bool valid_frame = false;
        if(xreg(RXFIFOCNT) > sizeof(IEEE802_15_4::Phy_Header) + sizeof(IEEE802_15_4::CRC)) {
            volatile unsigned int * rxfifo = reinterpret_cast<volatile unsigned int*>(RXFIFO);
            unsigned char mac_frame_size = rxfifo[0];
            // On RX, last two bytes in the frame are replaced by info like CRC result
            // (obs: mac frame is preceded by one byte containing the frame length,
            // so total RXFIFO data size is 1 + mac_frame_size)
            valid_frame = (mac_frame_size <= 127) && (rxfifo[mac_frame_size] & AUTO_CRC_OK);
        }
        if(!valid_frame)
            sfr(RFST) = ISFLUSHRX;

        return valid_frame;
    }

    static void power(const Power_Mode & mode) {
         switch(mode) {
         case FULL: // Able to receive and transmit
             ieee802_15_4_power(FULL);
             xreg(FRMCTRL0) = (xreg(FRMCTRL0) & ~(3 * RX_MODE)) | (RX_MODE_NORMAL * RX_MODE);
             break;
         case LIGHT: // Able to sense channel and transmit
             xreg(FRMCTRL0) = (xreg(FRMCTRL0) & ~(3 * RX_MODE)) | (RX_MODE_NO_SYMBOL_SEARCH * RX_MODE);
             ieee802_15_4_power(LIGHT);
             break;
         case SLEEP: // Receiver off
             sfr(RFST) = ISRFOFF;
             ieee802_15_4_power(SLEEP);
             break;
         case OFF: // Radio unit shut down
             sfr(RFST) = ISRFOFF;
             ieee802_15_4_power(OFF);
             break;
         }
     }

protected:
    static volatile Reg32 & ana     (unsigned int offset) { return *(reinterpret_cast<volatile Reg32 *>(ANA_BASE + offset)); }
    static volatile Reg32 & xreg    (unsigned int offset) { return *(reinterpret_cast<volatile Reg32 *>(XREG_BASE + offset)); }
    static volatile Reg32 & ffsm    (unsigned int offset) { return *(reinterpret_cast<volatile Reg32 *>(FFSM_BASE + offset)); }
    static volatile Reg32 & sfr     (unsigned int offset) { return *(reinterpret_cast<volatile Reg32 *>(SFR_BASE  + offset)); }
    static volatile Reg32 & mactimer(unsigned int offset) { return *(reinterpret_cast<volatile Reg32 *>(MACTIMER_BASE + offset)); }
};

// CC2538 IEEE 802.15.4 EPOSMote III NIC Mediator
class CC2538: public IF<EQUAL<Traits<Network>::NETWORKS::Get<Traits<Cortex_M_IEEE802_15_4>::NICS::Find<CC2538>::Result>::Result, TSTP>::Result, TSTP_MAC<CC2538RF>, IEEE802_15_4_MAC<CC2538RF>>::Result
{
    template <int unit> friend void call_init();

private:
    typedef IF<EQUAL<Traits<Network>::NETWORKS::Get<Traits<Cortex_M_IEEE802_15_4>::NICS::Find<CC2538>::Result>::Result, _SYS::TSTP>::Result, TSTP_MAC<CC2538RF>, IEEE802_15_4_MAC<CC2538RF>>::Result MAC;

    // Transmit and Receive Ring sizes
    static const unsigned int UNITS = Traits<CC2538>::UNITS;
    static const unsigned int RX_BUFS = Traits<CC2538>::RECEIVE_BUFFERS;

    // Size of the DMA Buffer that will host the ring buffers
    static const unsigned int DMA_BUFFER_SIZE = RX_BUFS * sizeof(Buffer);

    // Interrupt dispatching binding
    struct Device {
        CC2538 * device;
        unsigned int interrupt;
    };

protected:
    CC2538(unsigned int unit);

public:
    ~CC2538();

    int send(const Address & dst, const Protocol & prot, const void * data, unsigned int size);
    int receive(Address * src, Protocol * prot, void * data, unsigned int size);

    Buffer * alloc(NIC * nic, const Address & dst, const Protocol & prot, unsigned int once, unsigned int always, unsigned int payload);
    void free(Buffer * buf);
    int send(Buffer * buf);

    const Address & address() { return _address; }
    void address(const Address & address) { _address = address; CC2538RF::address(address); }

    unsigned int channel() { return _channel; }
    void channel(unsigned int channel) {
        if((channel > 10) && (channel < 27)) {
            _channel = channel;
            CC2538RF::channel(_channel);
        }
    }

    const Statistics & statistics() { return _statistics; }

    void reset();

    static CC2538 * get(unsigned int unit = 0) { return get_by_unit(unit); }

private:
    void handle_int();

    static void int_handler(const IC::Interrupt_Id & interrupt);

    static CC2538 * get_by_unit(unsigned int unit) {
        assert(unit < UNITS);
        return _devices[unit].device;
    }

    static CC2538 * get_by_interrupt(unsigned int interrupt) {
        CC2538 * tmp = 0;
        for(unsigned int i = 0; i < UNITS; i++)
            if(_devices[i].interrupt == interrupt)
                tmp = _devices[i].device;
        return tmp;
    };

    static void init(unsigned int unit);

private:
    unsigned int _unit;

    Address _address;
    unsigned int _channel;
    Statistics _statistics;

    Buffer::List _received_buffers;
    static Device _devices[UNITS];
};

__END_SYS

#endif
