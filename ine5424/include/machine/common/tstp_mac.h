// EPOS Trustful SpaceTime Protocol MAC Declarations

#ifndef __tstp_mac_h
#define __tstp_mac_h

#define __tstp_h
#include <tstp.h>
#undef __tstp_h

__BEGIN_SYS

template<typename Radio>
class TSTP_MAC: public TSTP_Common, public TSTP_Common::Observed, public Radio
{
public:
    using TSTP_Common::Address;

protected:
    TSTP_MAC() {}

public:
    unsigned int marshal(IEEE802_15_4::Phy_Frame * frame, const Address & src, const Address & dst, const Type & type, const void * data, unsigned int size) {
         if(size > Frame::MTU)
             size = Frame::MTU;
         new (frame) Frame(type, src, dst, data, size);
         return size;
     }

     unsigned int unmarshal(IEEE802_15_4::Phy_Frame * f, Address * src, Address * dst, Type * type, void * data, unsigned int size) {
         Frame * frame = reinterpret_cast<Frame *>(f);
         unsigned int data_size = frame->length() - sizeof(Header) - sizeof(CRC) + sizeof(Phy_Header); // Phy_Header is included in Header, but is already discounted in frame_length
         if(size > data_size)
             size = data_size;
         *type = frame->type();
         memcpy(data, frame->data<void>(), size);
         return size;
     }

    int send() { return 0; }
    void receive() {}

    bool backoff_and_send();
};

__END_SYS

#endif
