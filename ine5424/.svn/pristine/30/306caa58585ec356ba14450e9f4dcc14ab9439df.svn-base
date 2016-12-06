// EPOS PC NIC Mediator Initialization

#include <machine/pc/nic.h>

__BEGIN_SYS

template<int unit>
inline static void call_init()
{
    typedef typename Traits<PC_Ethernet>::NICS::template Get<unit>::Result NIC;
    static const unsigned int OFFSET = Traits<PC_Ethernet>::NICS::template Find<NIC>::Result;

    if(Traits<NIC>::enabled)
        NIC::init(unit - OFFSET);

    call_init<unit + 1>();
};

template<>
inline void call_init<Traits<PC_Ethernet>::NICS::Length>()
{
};

void PC_Ethernet::init()
{
    call_init<0>();
}

__END_SYS
