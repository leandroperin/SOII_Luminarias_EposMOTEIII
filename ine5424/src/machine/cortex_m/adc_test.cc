// EPOS Cortex-M Analog to Digital Converter (ADC) Mediator Test Program

#include <adc.h>
#include <alarm.h>
#include <utility/ostream.h>

using namespace EPOS;

int main()
{
    OStream cout;

    cout << "ADC Test" << endl;

    ADC adc0(ADC::SINGLE_ENDED_ADC0);
    ADC adc1(ADC::SINGLE_ENDED_ADC1);
    ADC adc2(ADC::SINGLE_ENDED_ADC2);
    ADC adc3(ADC::SINGLE_ENDED_ADC3);
    ADC adc4(ADC::SINGLE_ENDED_ADC4);
    ADC adc5(ADC::SINGLE_ENDED_ADC5);
    ADC adc6(ADC::SINGLE_ENDED_ADC6);
    ADC adc7(ADC::SINGLE_ENDED_ADC7);

    cout << "========================" << endl;

    while(true) {
        int raw, converted;

        raw = adc0.read();
        converted = adc0.convert(raw);
        cout << "ADC pin 0 = " << raw << " = " << converted / 1000 << "." << converted % 1000 << "V" << endl;

        raw = adc1.read();
        converted = adc1.convert(raw);
        cout << "ADC pin 1 = " << raw << " = " << converted / 1000 << "." << converted % 1000 << "V" << endl;

        raw = adc2.read();
        converted = adc2.ADC::convert(raw);
        cout << "ADC pin 2 = " << raw << " = " << converted / 1000 << "." << converted % 1000 << "V" << endl;

        raw = adc3.read();
        converted = adc3.ADC::convert(raw);
        cout << "ADC pin 3 = " << raw << " = " << converted / 1000 << "." << converted % 1000 << "V" << endl;

        raw = adc4.read();
        converted = adc4.ADC::convert(raw);
        cout << "ADC pin 4 = " << raw << " = " << converted / 1000 << "." << converted % 1000 << "V" << endl;

        raw = adc5.read();
        converted = adc5.ADC::convert(raw);
        cout << "ADC pin 5 = " << raw << " = " << converted / 1000 << "." << converted % 1000 << "V" << endl;

        raw = adc6.read();
        converted = adc6.ADC::convert(raw);
        cout << "ADC pin 6 = " << raw << " = " << converted / 1000 << "." << converted % 1000 << "V" << endl;

        raw = adc7.read();
        converted = adc7.convert(raw);
        cout << "ADC pin 7 = " << raw << " = " << converted / 1000 << "." << converted % 1000 << "V" << endl;

        cout << "========================" << endl;

        Alarm::delay(1000000);
    }

    return 0;
}
