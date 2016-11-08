// EPOS TI CC2538 IEEE 802.15.4 NIC Mediator Implementation


#include <machine/cortex_m/machine.h>
#include <machine/cortex_m/cc2538.h>
#include <utility/malloc.h>
#include <utility/random.h>

__BEGIN_SYS

// Class attributes
CC2538RF::Reg32 CC2538RF::Timer::_overflow_count;
CC2538RF::Reg32 CC2538RF::Timer::_interrupt_overflow_count;

CC2538::Device CC2538::_devices[UNITS];

// Methods
CC2538::~CC2538()
{
    db<CC2538>(TRC) << "~CC2538(unit=" << _unit << ")" << endl;
}

int CC2538::send(const Address & dst, const Type & type, const void * data, unsigned int size)
{
    db<CC2538>(TRC) << "CC2538::send(s=" << address() << ",d=" << dst << ",p=" << hex << type << dec << ",d=" << data << ",s=" << size << ")" << endl;

    Buffer * b;
    if((b = alloc(reinterpret_cast<NIC*>(this), dst, type, 0, 0, size))) {
        // Assemble the Frame Header and buffer Metainformation
        MAC::marshal(b, address(), dst, type, data, size);
        return send(b);
    }
    return 0;
}

int CC2538::receive(Address * src, Type * type, void * data, unsigned int size)
{
    db<CC2538>(TRC) << "CC2538::receive(s=" << *src << ",p=" << hex << *type << dec << ",d=" << data << ",s=" << size << ") => " << endl;

    unsigned int ret = 0;

    Buffer::Element * el = _received_buffers.remove_head();
    if(el) {
        Buffer * buf = el->object();
        Address dst;
        ret = MAC::unmarshal(buf, src, &dst, type, data, size);
        free(buf);
    }

    db<CC2538>(INF) << "CC2538::received " << ret << " bytes" << endl;

    return ret;
}

CC2538::Buffer * CC2538::alloc(NIC * nic, const Address & dst, const Type & type, unsigned int once, unsigned int always, unsigned int payload)
{
    db<CC2538>(TRC) << "CC2538::alloc(s=" << address() << ",d=" << dst << ",p=" << hex << type << dec << ",on=" << once << ",al=" << always << ",ld=" << payload << ")" << endl;

    // Initialize the buffer
    return new (SYSTEM) Buffer(nic, once + always + payload, once + always + payload); // the last parameter is passed to Phy_Frame as the length
}

int CC2538::send(Buffer * buf)
{
    db<CC2538>(TRC) << "CC2538::send(buf=" << buf << ")" << endl;
    db<CC2538>(INF) << "CC2538::send:frame=" << buf->frame() << " => " << *(buf->frame()) << endl;

    unsigned int size = MAC::send(buf);

    if(size) {
        _statistics.tx_packets++;
        _statistics.tx_bytes += size;
    } else
        db<CC2538>(WRN) << "CC2538::send(buf=" << buf << ")" << " => failed!" << endl;

    delete buf;

    return size;
}

void CC2538::free(Buffer * buf)
{
    db<CC2538>(TRC) << "CC2538::free(buf=" << buf << ")" << endl;

    _statistics.rx_packets++;
    _statistics.rx_bytes += buf->size();

    delete buf;
}

void CC2538::reset()
{
    db<CC2538>(TRC) << "CC2538::reset()" << endl;

    // Reset statistics
    new (&_statistics) Statistics;
}

void CC2538::handle_int()
{
    db<CC2538>(TRC) << "CC2538::handle_int()" << endl;

    Reg32 irqrf0 = sfr(RFIRQF0);
    Reg32 irqrf1 = sfr(RFIRQF1);
    Reg32 errf = sfr(RFERRF);
    sfr(RFIRQF0) = irqrf0 & INT_RXPKTDONE; //INT_RXPKTDONE is polled by rx_done()
    sfr(RFIRQF1) = irqrf1 & INT_TXDONE; //INT_TXDONE is polled by tx_done()
    sfr(RFERRF) = 0;
    db<CC2538>(INF) << "CC2538::handle_int:RFIRQF0=" << hex << irqrf0 << endl;
    db<CC2538>(INF) << "CC2538::handle_int:RFIRQF1=" << hex << irqrf1 << endl;
    db<CC2538>(INF) << "CC2538::handle_int:RFERRF=" << hex << errf << endl;

    if(irqrf0 & INT_FIFOP) { // Frame received
        db<CC2538>(TRC) << "CC2538::handle_int:receive()" << endl;
        if(CC2538RF::filter()) {
            Buffer * buf = new (SYSTEM) Buffer(0);
            if(MAC::copy_from_nic(buf)) {
                db<CC2538>(TRC) << "CC2538::handle_int:receive(b=" << buf << ") => " << *buf << endl;
                if(!notify(reinterpret_cast<Frame*>(buf->frame())->type(), buf)) {
                    // No one was waiting for this frame, so store it for receive()
                    if(_received_buffers.size() < RX_BUFS) {
                        _received_buffers.insert(buf->link());
                    } else {
                        db<CC2538>(WRN) << "CC2538::handle_int: frame dropped, too many buffers in queue!"  << endl;
                        delete buf;
                    }
                }
            } else {
                db<CC2538>(TRC) << "CC2538::handle_int: frame dropped by MAC"  << endl;
                delete buf;
            }
        }
    }
}

void CC2538::int_handler(const IC::Interrupt_Id & interrupt)
{
    CC2538 * dev = get_by_interrupt(interrupt);

    db<CC2538>(TRC) << "Radio::int_handler(int=" << interrupt << ",dev=" << dev << ")" << endl;

    if(!dev)
        db<CC2538>(WRN) << "Radio::int_handler: handler not assigned!" << endl;
    else
        dev->handle_int();
}

__END_SYS
