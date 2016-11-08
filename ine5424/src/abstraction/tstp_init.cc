// EPOS Trustful SpaceTime Protocol Initialization

#include <system/config.h>
#ifndef __no_networking__

#include <tstp.h>

__BEGIN_SYS

TSTP::TSTP()
{
    db<TSTP>(TRC) << "TSTP::TSTP()" << endl;
    _nic->attach(this, NIC::TSTP);
}

void TSTP::init(unsigned int unit)
{
    db<Init, TSTP>(TRC) << "TSTP::init(u=" << unit << ")" << endl;
    _nic = new (SYSTEM) NIC(unit);
    new (SYSTEM) TSTP;
}

__END_SYS

#endif
