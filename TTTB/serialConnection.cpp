#include <iostream>

#using <System.dll>

using namespace System;
using namespace System::IO::Ports;
using namespace std;

void connect(int nextMove) {
	int baudRate = 9600;
	String^ portName = "COM4";

	SerialPort^ arduino;
	arduino = gcnew SerialPort(portName, baudRate);

	try {
		arduino->Open();
		arduino->WriteLine(nextMove.ToString());

		//Next Move is defined by the position the which will be played next.
		//|1|2|3|
		//|4|5|6|
		//|7|8|9|
		cout << "Moving to position " << nextMove << endl;

		arduino->Close();
	}
	catch (IO::IOException^ e) {
		cout << "Port is not ready." << endl;
	}
	catch (ArgumentException^ e) {
		cout << "Incorrect Port Name." << endl;
	}
}