#include <utility/ostream.h>
#include <nic.h>
#include <gpio.h>
#include <alarm.h>
#include <chronometer.h>
#include <periodic_thread.h>

using namespace EPOS;

OStream cout;

bool led_value;
GPIO * led;

class Receiver : public IEEE802_15_4::Observer
{
public:
    typedef char data_type;

    typedef IEEE802_15_4::Protocol Protocol;
    typedef IEEE802_15_4::Buffer Buffer;
    typedef IEEE802_15_4::Frame Frame;
    typedef IEEE802_15_4::Observed Observed;

    Receiver(const Protocol & p, NIC * nic) : _prot(p), _nic(nic) {
        _chrono.start();
        _nic->attach(this, _prot);
    }

    void update(Observed * o, Protocol p, Buffer * b)
    {
        led_value = !led_value;
        led->set(led_value);
        Frame * f = reinterpret_cast<Frame *>(b->frame());
        auto d = f->data<data_type>();
        cout << "[" << _chrono.read() << "] {" << f->src() << " => " << f->dst() << "} (" << b->size() << ")" << endl;
        for(int i=0; i<b->size()/sizeof(data_type); i++)
            cout << d[i] << " ";
        cout << endl;
        _nic->free(b);
    }

private:
    Protocol _prot;
    NIC * _nic;
    Chronometer _chrono;
};

int main()
{
    Alarm::delay(1000000);

    cout << "IEEE802_15_4 Protocol Analyzer" << endl;
    cout << "Log format:" << endl;
    cout << "[Timestamp] {src => dst} (size)" << endl;
    cout << "message contents, space-separated" << endl;

    led = new GPIO('C',3, GPIO::OUTPUT);
    led_value = true;
    led->set(led_value);

    NIC * nic = new NIC();
    Receiver * r = new Receiver(NIC::PTP, nic);

    while(true);

    return 0;
}
