// EPOS Trustful SpaceTime Protocol Implementation

#include <system/config.h>
#ifndef __no_networking__

#include <tstp.h>

__BEGIN_SYS

// Class attributes
NIC * TSTP::_nic;
TSTP::Interests TSTP::_interested;
TSTP::Responsives TSTP::_responsives;
TSTP::Observed TSTP::_observed;

// Methods
TSTP::~TSTP()
{
    db<TSTP>(TRC) << "TSTP::~TSTP()" << endl;
    _nic->detach(this, 0);
}


void TSTP::update(NIC::Observed * obs, NIC::Protocol prot, Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP::update(obs=" << obs << ",buf=" << buf << ")" << endl;

    Packet * packet = buf->frame()->data<Packet>();
    switch(packet->type()) {
    case INTEREST: {
        Interest * interest = reinterpret_cast<Interest *>(packet);
        db<TSTP>(INF) << "TSTP::update:interest=" << interest << " => " << *interest << endl;
        // Check for local capability to respond and notify interested observers
        Responsives::List * list = _responsives[interest->unit()]; // TODO: What if sensor can answer multiple formats (e.g. int and float)
        if(list)
            for(Responsives::Element * el = list->head(); el; el = el->next()) {
                Responsive * responsive = el->object();
                if(interest->region().contains(responsive->origin(), now())) {
                    notify(responsive, buf);
                }
            }
    } break;
    case RESPONSE: {
        Response * response = reinterpret_cast<Response *>(packet);
        db<TSTP>(INF) << "TSTP::update:response=" << response << " => " << *response << endl;
        // Check region inclusion and notify interested observers
        Interests::List * list = _interested[response->unit()];
        if(list)
            for(Interests::Element * el = list->head(); el; el = el->next()) {
                Interested * interested = el->object();
                if(interested->region().contains(response->origin(), response->time()))
                    notify(interested, buf);
            }
    } break;
    case COMMAND: {
        Command * command = reinterpret_cast<Command *>(packet);
        db<TSTP>(INF) << "TSTP::update:command=" << command << " => " << *command << endl;
        // Check for local capability to respond and notify interested observers
        Responsives::List * list = _responsives[command->unit()]; // TODO: What if sensor can answer multiple formats (e.g. int and float)
        if(list)
            for(Responsives::Element * el = list->head(); el; el = el->next()) {
                Responsive * responsive = el->object();
                if(command->region().contains(responsive->origin(), now()))
                    notify(responsive, buf);
            }
    } break;
    case CONTROL: break;
    }

    _nic->free(buf);
}

__END_SYS

#endif
