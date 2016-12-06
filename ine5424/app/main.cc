// Leandro Perin de Oliveira - 14100846
// Leonardo Vailatti Eichstaedt - 14100847

// 4.3: Controle de objetos inteligentes numa IoT (EPOS/EposMotesIII: Luminárias + Tomadas)

/*
 *	Possible commands:
 *
 *	un___:  Use natural light
 *	upXX_:  Use progressive brightness with XX as the max value
 *	uc___:	Use circadian cycle
 *	cYXX_:  Change intensity of the Y color to the XX value
 *	tXXYY:  Set the clock to XX hours and YY minutes
 *	suX__:  Switch user to the X user
 *	ss___:	Send settings values to another EposMoteIII
 *	XYYYY:  Change the time to change led intensity in circadian cycle
*/

#include <machine.h>
#include <alarm.h>
#include <thread.h>
#include <mutex.h>
#include <nic.h>

using namespace EPOS;

OStream cout;

Mutex mutUSB;
Mutex mutNIC;

NIC nic;

PWM * red_LED;
PWM * green_LED;
PWM * blue_LED;

GPIO * eMote3_LED;

// Max brightness when progressive is on
int progRedBrightness = 100; bool incRedBrightness = true;
int progGreenBrightness = 100; bool incGreenBrightness = true;
int progBlueBrightness = 100; bool incBlueBrightness = true;

ADC * temperatureSensor;
ADC * naturalLightSensor;

int timeInMinutes = 0;

int firstChange;
int secondChange;
int thirdChange;
int fourthChange;

const int maxTemperature = 200; // Temperature at which the LEDs will be weakened
const int safeLEDIntensity = 4; // LED's intensity in overheating situations
bool deviceOverheating = false;

bool useNaturalLight = true;
bool useProgressiveBrightness = false;
bool useCircadianCycle = false;

int previousIntensityValues [3] = {99,99,99}; // Used to keep track of the intensity values

char* user0;
char* user1;
char* user2;
char* user3;
char activeUser;

void setUp() {
	red_LED = new PWM(1, 1000000, 99, 'D', 4);
	green_LED = new PWM(2, 1000000, 99, 'A', 7);
	blue_LED = new PWM(3, 1000000, 99, 'A', 0);

	eMote3_LED = new GPIO('C', 3, GPIO::OUTPUT);

	temperatureSensor = new ADC(ADC::TEMPERATURE);
	naturalLightSensor = new ADC(ADC::SINGLE_ENDED_ADC5);

	activeUser = '0';

	user0 = user1 = user2 = user3 = "unxxx";

	firstChange = 480;
	secondChange = 720;
	thirdChange = 1080;
	fourthChange = 1260;
}

int sender() {
	NIC::Address dest(NIC::Address::BROADCAST);
	cout << "Dest: " << dest << endl;
	char msg[6] = "";
	strncpy(msg, user0, 5);
	cout << "Msg: " << msg << endl;
	mutNIC.lock();
	nic.send(dest, NIC::PTP, &msg, sizeof(char)*6);
	mutNIC.unlock();

	return 0;
}

void setLEDColorIntensity(char color, int intensity) {
	switch (color) {
		case 'r': delete red_LED;	red_LED = new PWM(0, 1000000, intensity, 'D', 4);
							previousIntensityValues[0] = intensity;	break;
		case 'g': delete green_LED;	green_LED = new PWM(1, 1000000, intensity, 'A', 7);
							previousIntensityValues[1] = intensity;	break;
		case 'b': delete blue_LED;	blue_LED = new PWM(2, 1000000, intensity, 'A', 0);
							previousIntensityValues[2] = intensity;	break;
	}
}

void setLEDColorIntensity() {
	if (incRedBrightness) {
		if (previousIntensityValues[0] == progRedBrightness) {
			incRedBrightness = false;
		} else {
			int intensity = previousIntensityValues[0] + 1;
			setLEDColorIntensity('r', intensity);
		}
	} else {
		if (previousIntensityValues[0] == 05) {
			incRedBrightness = true;
		} else {
			int intensity = previousIntensityValues[0] - 1;
			setLEDColorIntensity('r', intensity);
		}
	}

	if (incGreenBrightness) {
		if (previousIntensityValues[1] == progGreenBrightness) {
			incGreenBrightness = false;
		} else {
			int intensity = previousIntensityValues[1] + 1;
			setLEDColorIntensity('g', intensity);
		}
	} else {
		if (previousIntensityValues[1] == 05) {
			incGreenBrightness = true;
		} else {
			int intensity = previousIntensityValues[1] - 1;
			setLEDColorIntensity('g', intensity);
		}
	}

	if (incBlueBrightness) {
		if (previousIntensityValues[2] == progBlueBrightness) {
			incBlueBrightness = false;
		} else {
			int intensity = previousIntensityValues[2] + 1;
			setLEDColorIntensity('b', intensity);
		}
	} else {
		if (previousIntensityValues[2] == 05) {
			incBlueBrightness = true;
		} else {
			int intensity = previousIntensityValues[2] - 1;
			setLEDColorIntensity('b', intensity);
		}
	}

	Delay(10000);
}

void setLEDColorIntensity(int intensity, bool savePrevious) {
	delete red_LED; red_LED = new PWM(0, 1000000, intensity, 'D', 4);
	delete green_LED;	green_LED = new PWM(1, 1000000, intensity, 'A', 7);
	delete blue_LED;	blue_LED = new PWM(2, 1000000, intensity, 'A', 0);

	if (savePrevious) {
		previousIntensityValues[0] = intensity;
		previousIntensityValues[1] = intensity;
		previousIntensityValues[2] = intensity;
	}
}

bool watchTemperature() {
	short temperature = temperatureSensor->read();

	if (temperature > maxTemperature) { // Return true if the temperature is high
		if (!deviceOverheating) {
			cout << "WARNING: The device is overheating... The LEDs will be weakened." << endl;
			cout << "Do not worry, we will say when it is safe do continue!" << endl;
			deviceOverheating = true;
			eMote3_LED->set(1);
		}
		return true;
	}

	if (deviceOverheating) {
		deviceOverheating = false;
		cout << "Now it's safe, your device is not overheating anymore! Go ahead!" << endl;

		setLEDColorIntensity('r', previousIntensityValues[0]);
		setLEDColorIntensity('g', previousIntensityValues[1]);
		setLEDColorIntensity('b', previousIntensityValues[2]);

		eMote3_LED->set(0);
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

void circadianCycleProcessing() {
	if (timeInMinutes < firstChange) {
		setLEDColorIntensity(10, true);
	} else if (timeInMinutes < secondChange) {
		setLEDColorIntensity(50, true);
	} else if (timeInMinutes < thirdChange) {
		setLEDColorIntensity(99, true);
	} else if (timeInMinutes < fourthChange) {
		setLEDColorIntensity(50, true);
	} else {
		setLEDColorIntensity(10, true);
	}
}

int increaseTime() {
	while (true) {
		Delay(60000000); // One minute delay

		if (timeInMinutes >= 1439) { // 23:59 hrs
			timeInMinutes = 0;
		} else {
			timeInMinutes++;
		}
	}

	return 0;
}

void updateUserPreferences(char* _intensity = "99\0", char* _color = "r\0") {
	if (useNaturalLight) {
		if (activeUser == '0') {
			user0 = "unxxx";
		} else if (activeUser == '1') {
			user1 = "unxxx";
		} else if (activeUser == '2') {
			user2 = "unxxx";
		} else {
			user3 = "unxxx";
		}
	} else if (useProgressiveBrightness) {
		char* _c = new char[6];

		strncpy(_c, "up", 2);
		strcat(_c, _intensity);
		strcat(_c, "x\0");

		if (activeUser == '0') {
			user0 = _c;
		} else if (activeUser == '1') {
			user1 = _c;
		} else if (activeUser == '2') {
			user2 = _c;
		} else {
			user3 = _c;
		}
	} else if (useCircadianCycle) {
		if (activeUser == '0') {
			user0 = "ucxxx";
		} else if (activeUser == '1') {
			user1 = "ucxxx";
		} else if (activeUser == '2') {
			user2 = "ucxxx";
		} else {
			user3 = "ucxxx";
		}
	} else {
		char* _c = new char[6];

		strncpy(_c, "c", 1);
		strcat(_c, _color);
		strcat(_c, _intensity);
		strcat(_c, "x\0");

		if (activeUser == '0') {
			user0 = _c;
		} else if (activeUser == '1') {
			user1 = _c;
		} else if (activeUser == '2') {
			user2 = _c;
		} else {
			user3 = _c;
		}
	}
}

void executeCommands(char command[5]) {
	char* _n = new char[3];
	strncpy(_n, &command[2], 2);

	char* _color = new char[2];
	strncpy(_color, &command[1], 1);

	if (command[0] == 'u' && command[1] == 'n') { // Use natural light
		useNaturalLight = true;
		useProgressiveBrightness = false;
		useCircadianCycle = false;

		cout << "Natural Light Activated!" << endl;

		updateUserPreferences();
	}

	else if (command[0] == 'u' && command[1] == 'p') { // Use progressive brightness
		progRedBrightness = progGreenBrightness = progBlueBrightness = atoi(_n);

		useNaturalLight = false;
		useProgressiveBrightness = true;
		useCircadianCycle = false;

		cout << "Progressive Brightness Activated!" << endl;

		updateUserPreferences(_n);
	}

	else if (command[0] == 'u' && command[1] == 'c') { // Use circadian cycle
		useNaturalLight = false;
		useProgressiveBrightness = false;
		useCircadianCycle = true;

		cout << "Circadian Cycle Activated!" << endl;

		updateUserPreferences();
	}

	else if (command[0] == '1') {
		int hours = (command[1]-'0') * 10;
		hours += (command[2]-'0');

		int minutes = (command[3]-'0') * 10;
		minutes += (command[4]-'0');

		firstChange = (hours*60) + minutes;

		cout << "Às " << hours << ":" << minutes << " a intensidade será aumentada" << endl;
	}

	else if (command[0] == '2') {
		int hours = (command[1]-'0') * 10;
		hours += (command[2]-'0');

		int minutes = (command[3]-'0') * 10;
		minutes += (command[4]-'0');

		secondChange = (hours*60) + minutes;

		cout << "Às " << hours << ":" << minutes << " a intensidade será bastante aumentada" << endl;
	}

	else if (command[0] == '3') {
		int hours = (command[1]-'0') * 10;
		hours += (command[2]-'0');

		int minutes = (command[3]-'0') * 10;
		minutes += (command[4]-'0');

		thirdChange = (hours*60) + minutes;

		cout << "Às " << hours << ":" << minutes << " a intensidade será diminuída" << endl;
	}

	else if (command[0] == '4') {
		int hours = (command[1]-'0') * 10;
		hours += (command[2]-'0');

		int minutes = (command[3]-'0') * 10;
		minutes += (command[4]-'0');

		fourthChange = (hours*60) + minutes;

		cout << "Às " << hours << ":" << minutes << " a intensidade será bastante diminuída" << endl;
	}

	else if (command[0] == 't') { // Set the current time
		int hours = (command[1]-'0') * 10;
		hours += (command[2]-'0');

		int minutes = (command[3]-'0') * 10;
		minutes += (command[4]-'0');

		timeInMinutes = (hours * 60) + minutes;

		cout << "Time set to " << hours << ":" << minutes << "!" << endl;
	}

	else if (command[0] == 'c') { // Change LED color
		useNaturalLight = false;
		useProgressiveBrightness = false;
		useCircadianCycle = false;

		setLEDColorIntensity(command[1], atoi(_n));

		cout << "Color " << command[1] << " is now " << _n << "!" << endl;

		updateUserPreferences(_n, _color);
	}

	else if (command[0] == 's' && command[1] == 'u') { // Switch user
		cout << "User switched to " << command[2] << endl;

		if (command[2] == '0') {
			activeUser = '0';
			executeCommands(user0);
		}	else if (command[2] == '1') {
			activeUser = '1';
			executeCommands(user1);
		}	else if (command[2] == '2') {
			activeUser = '2';
			executeCommands(user2);
		} else {
			activeUser = '3';
			executeCommands(user3);
		}
	}

	else if (command[0] == 's' && command[1] == 's') { // Send settings
		sender();

		cout << "Settings sent!" << endl;
	}

	else {
		cout << "Invalid command!" << endl;
	}

	delete _n;
	delete _color;
}

int getInputCommands() {
	while (true) {
		mutUSB.lock();
		if (USB::ready_to_get()) {

			char command[5];
			for (int i = 0; i < 5; i++) {
				command[i] = USB::get();
			}

			executeCommands(command);
		}
		mutUSB.unlock();

		Thread::yield();
	}
	return 0;
}

int mainExecution() {
	while (true) {
		if (watchTemperature())
			setLEDColorIntensity();
		else {
			if (useNaturalLight) {
				naturalLightProcessing();
			} else if (useProgressiveBrightness) {
				setLEDColorIntensity();
			} else if (useCircadianCycle) {
				circadianCycleProcessing();
			}
		}

		Thread::yield();
	}

	return 0;
}

int receiver() {
	NIC::Protocol prot;
	NIC::Address src;

	int ret;
	while (true) {
		char msg[6];
		do {
				mutNIC.lock();
				ret = nic.receive(&src, &prot, &msg, sizeof(char)*6);
				mutNIC.unlock();
				Delay(100);
		} while (ret <= 0);

		Delay(100000);

		executeCommands(msg);

		Thread::yield();
	}
	return 0;
}

int main()
{
	setUp();

	Thread commandsThread(&getInputCommands);
	Thread executionThread(&mainExecution);
	Thread clockThread(&increaseTime);
	Thread receiverThread(&receiver);

	commandsThread.join();
	executionThread.join();
	clockThread.join();
	receiverThread.join();

	return 0;
}
