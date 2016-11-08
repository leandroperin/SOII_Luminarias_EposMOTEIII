// Leandro Perin de Oliveira - 14100846
// Leonardo Vailatti Eichstaedt - 14100847

// 4.3: Controle de objetos inteligentes numa IoT (EPOS/EposMotesIII: Luminárias + Tomadas)

/*
 *	Possible commands:
 *
 *	un__: Use natural light
 *	upXX: Use progressive brightness with XX as the max value
 *	cYXX: Change intensity of the Y color to the XX value
*/

#include <machine.h>
#include <alarm.h>
#include <thread.h>
#include <mutex.h>

using namespace EPOS;

OStream cout;

Mutex mutUSB;

PWM * red_LED;
PWM * green_LED;
PWM * blue_LED;
PWM * white_LED; // EposMOTEIII LED

// Max brightness when progressive is on
int progRedBrightness = 100; bool incRedBrightness = true;
int progGreenBrightness = 100; bool incGreenBrightness = true;
int progBlueBrightness = 100; bool incBlueBrightness = true;
int progWhiteBrightness = 100; bool incWhiteBrightness = true;

ADC * temperatureSensor;
ADC * naturalLightSensor;

const int maxTemperature = 200; // Temperature at which the LEDs will be weakened
const int safeLEDIntensity = 10; // LED's intensity in overheating situations
bool deviceOverheating = false;

bool useNaturalLight = false;
bool useProgressiveBrightness = false;

int previousIntensityValues [4] = {10,10,10,10}; // Used to keep track of the intensity values

void setUp() {
	red_LED = new PWM(1, 1000000, 10, 'D', 4);
	green_LED = new PWM(2, 1000000, 10, 'A', 7);
	blue_LED = new PWM(3, 1000000, 10, 'A', 0);
	white_LED = new PWM(0, 1000000, 10, 'C', 3);

	temperatureSensor = new ADC(ADC::TEMPERATURE);
	naturalLightSensor = new ADC(ADC::SINGLE_ENDED_ADC5);
}

void setDown() {
	delete temperatureSensor;
	delete naturalLightSensor;
}

void setLEDColorIntensity(char color, int intensity) {
	switch (color) {
		case 'r': delete red_LED;	red_LED = new PWM(1, 1000000, intensity, 'D', 4);
							previousIntensityValues[1] = intensity;	break;
		case 'g': delete green_LED;	green_LED = new PWM(2, 1000000, intensity, 'A', 7);
							previousIntensityValues[2] = intensity;	break;
		case 'b': delete blue_LED;	blue_LED = new PWM(3, 1000000, intensity, 'A', 0);
							previousIntensityValues[3] = intensity;	break;
		default:	delete white_LED;	white_LED = new PWM(0, 1000000, intensity, 'C', 3);
							previousIntensityValues[0] = intensity;	break;
	}
}

void setLEDColorIntensity() {
	if (incRedBrightness) {
		if (previousIntensityValues[1] == progRedBrightness) {
			incRedBrightness = false;
		} else {
			int intensity = previousIntensityValues[1] + 1;
			setLEDColorIntensity('r', intensity);
		}
	} else {
		if (previousIntensityValues[1] == 05) {
			incRedBrightness = true;
		} else {
			int intensity = previousIntensityValues[1] - 1;
			setLEDColorIntensity('r', intensity);
		}
	}

	if (incGreenBrightness) {
		if (previousIntensityValues[2] == progGreenBrightness) {
			incGreenBrightness = false;
		} else {
			int intensity = previousIntensityValues[2] + 1;
			setLEDColorIntensity('g', intensity);
		}
	} else {
		if (previousIntensityValues[2] == 05) {
			incGreenBrightness = true;
		} else {
			int intensity = previousIntensityValues[2] - 1;
			setLEDColorIntensity('g', intensity);
		}
	}

	if (incBlueBrightness) {
		if (previousIntensityValues[3] == progBlueBrightness) {
			incBlueBrightness = false;
		} else {
			int intensity = previousIntensityValues[3] + 1;
			setLEDColorIntensity('b', intensity);
		}
	} else {
		if (previousIntensityValues[3] == 05) {
			incBlueBrightness = true;
		} else {
			int intensity = previousIntensityValues[3] - 1;
			setLEDColorIntensity('b', intensity);
		}
	}

	if (incWhiteBrightness) {
		if (previousIntensityValues[0] == progWhiteBrightness) {
			incWhiteBrightness = false;
		} else {
			int intensity = previousIntensityValues[0] + 1;
			setLEDColorIntensity('w', intensity);
		}
	} else {
		if (previousIntensityValues[0] == 05) {
			incWhiteBrightness = true;
		} else {
			int intensity = previousIntensityValues[0] - 1;
			setLEDColorIntensity('w', intensity);
		}
	}

	Delay(10000);
}

void setLEDColorIntensity(int intensity, bool savePrevious) {
	delete red_LED; red_LED = new PWM(1, 1000000, intensity, 'D', 4);
	delete green_LED;	green_LED = new PWM(2, 1000000, intensity, 'A', 7);
	delete blue_LED;	blue_LED = new PWM(3, 1000000, intensity, 'A', 0);
	delete white_LED;	white_LED = new PWM(0, 1000000, intensity, 'C', 3);

	if (savePrevious) {
		previousIntensityValues[0] = intensity;
		previousIntensityValues[1] = intensity;
		previousIntensityValues[2] = intensity;
		previousIntensityValues[3] = intensity;
	}
}

bool watchTemperature() {
	short temperature = temperatureSensor->read();

	if (temperature > maxTemperature) { // Return true if the temperature is high
		if (!deviceOverheating) {
			cout << "WARNING: The device is overheating... The LEDs will be weakened." << endl;
			cout << "Do not worry, we will say when it is safe do continue!" << endl;
			deviceOverheating = true;
		}
		return true;
	}

	if (deviceOverheating) {
		deviceOverheating = false;
		cout << "Now it's safe, your device is not overheating anymore! Go ahead!" << endl;

		setLEDColorIntensity('r', previousIntensityValues[1]);
		setLEDColorIntensity('g', previousIntensityValues[2]);
		setLEDColorIntensity('b', previousIntensityValues[3]);
		setLEDColorIntensity('w', previousIntensityValues[0]);
	}

	return false;
}

short getNaturalLight() {
	return naturalLightSensor->read();
}

void naturalLightProcessing() {
	int naturalLight = 20470 / getNaturalLight(); // 20470 is the maximum value of the sensor * 10

	if (naturalLight > 3)
		setLEDColorIntensity(naturalLight, true);
	else
		setLEDColorIntensity(100, true); // Work around of an EPOS bug
}

int getAndroidCommands() {
	while (true) {
		mutUSB.lock();
		if (USB::ready_to_get()) {
			// USB::get();

			char command[4];
			for (int i = 0; i < 4; i++) {
				command[i] = USB::get();
			}
			// USB::get(command, 5);

			if (command[0] == 'u' && command[1] == 'n') { // Use natural light
				useNaturalLight = true;
				useProgressiveBrightness = false;

				cout << "Natural Light Activated!" << endl;
			}

			if (command[0] == 'u' && command[1] == 'p') { // Use progressive brightness
				progRedBrightness = progGreenBrightness = progBlueBrightness
				= progWhiteBrightness = command[2] * 10 + command[3];

				useNaturalLight = false;
				useProgressiveBrightness = true;

				cout << "Progressive Brighness Activated!" << endl;
			}

			if (command[0] == 'c') {
				useNaturalLight = false;
				useProgressiveBrightness = false;

				int intensity = command[2] * 10;
				intensity += command[3];

				setLEDColorIntensity(command[1], intensity);

				cout << "Color " << command[1] << " is now " << intensity << "!" << endl;
			}

		}
		mutUSB.unlock();

		Thread::yield();
	}
	return 0;
}

int mainExecution() {
	while (true) {
		if (watchTemperature())
			setLEDColorIntensity(safeLEDIntensity, false);
		else {
			if (useNaturalLight) {
				naturalLightProcessing();
			} else if (useProgressiveBrightness) {
				setLEDColorIntensity();
			}
		}

		Thread::yield();
	}

	return 0;
}

int main()
{
	setUp();

	Thread commandsThread(&getAndroidCommands);
	Thread executionThread(&mainExecution);

	commandsThread.join();
	executionThread.join();

	setDown();
	return 0;
}
