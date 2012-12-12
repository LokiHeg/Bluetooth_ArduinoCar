// SaturdayDebug.cpp : main project file.
#include "stdafx.h"
#include <iostream>
#include <conio.h>
#pragma comment (lib, "user32.lib")
#include <Windows.h>
//Namespace we use to interact with keyboard events
namespace input {
	class key {
	public:
		static const int w = 0x57;
		static const int a = 0x41;
		static const int s = 0x53;
		static const int d = 0x44;

		static const int left = VK_LEFT;
		static const int right = VK_RIGHT;
		static const int up = 0x26;
		static const int down = VK_DOWN;

		static const int home = VK_HOME;
		static const int end = VK_END;

		static const int escape = VK_ESCAPE;
		static const int Ctrl = 0x11;
		static const int CapsLock = 0x14;
	};

	bool pressed(int key)
	{
		short result = GetAsyncKeyState(key);
		if(result & 0x8000) return true;
		else return false;
	};
	bool toogled(int key)
	{
		short result = GetKeyState(key);
		if(result & 0x8000) return false;
		else return true;
	};
};

using namespace input;
using namespace System;
using namespace System::IO::Ports;
using namespace std;
//Create a new class to interact with our Arduio throught COM-port
public ref class COM
{
private: SerialPort^  Arduino;
private: System::ComponentModel::IContainer^  components;
public:
	void Start (void)
	{
		this->components = (gcnew System::ComponentModel::Container());
		//Create new COM-port
		this->Arduino = (gcnew System::IO::Ports::SerialPort(this->components));
		//Set this name as "COM18" - this is the name of COM-port, which is connect our computer to Bluetooth
		this->Arduino->PortName = "COM18";
		//Open COM18 so that we can write some information to it
		this->Arduino->Open();
		//Send the "w" symbol
		this->Arduino->Write("w");
	}
	//Create the functions that we will use to drive our Car
	void Forward()
	{
		this->Arduino->Write("f");
	}
	void Back()
	{
		this->Arduino->Write("b");
	}
	void Left()
	{
		this->Arduino->Write("l");
	}
	void Right()
	{
		this->Arduino->Write("r");
	}
	void Auto()
	{
		this->Arduino->Write("s");
	}
	void Finish()
	{
		this->Arduino->Write("e");
		this->Arduino->Close();
	}
	void Break()
	{
		this->Arduino->Write("m");
	}
	void BackLeft()
	{
		this->Arduino->Write("o");
	}
	void BackRight()
	{
		this->Arduino->Write("p");
	}
};


int main(array<System::String ^> ^args)
{
	//Make an object of class COM named Car
    COM Car;
	//Open COM-port
	Car.Start();
	cout << "I'm ready to work, my master";
	do
	{
		//Go Forward ---------------------------------
		if(input::pressed(input::key::w))
			{
				Car.Forward();
				cout << "Forward" << endl;
				//What happens in case the "w" button was pressed and now it isn't
				if (!input::pressed(input::key::w))
				{
					Car.Break();
					cout << "break" << endl;
				};
				//What happens in case of pressing the "d" button the same time the "w" button was pressed
				if (input::pressed(key::d))
				{
					do
					{
						Car.Right();
						cout << "Right" << endl;
					}
					while(input::pressed(key::d));
					Car.Break();
					cout << "Break" << endl;
				};
				//Same thing for the "a" button
				if (input::pressed(key::a))
				{
					do
					{
						Car.Left();
						cout << "Left" << endl;
					}
					while(input::pressed(key::a));
					Car.Break();
					cout << "Break" << endl;
				};
			
			};		
		//Go back ---------------------------------
		if(input::pressed(input::key::s))
			{
				Car.Back();
				cout << "Back" << endl;
				//What happens in case the "w" button was pressed and now it isn't
				if (!input::pressed(input::key::s))
				{
					Car.Break();
					cout << "break" << endl;
				};
				//What happens in case of pressing the "d" button the same time the "s" button was pressed
				if (input::pressed(key::d))
				{
					do
					{
						Car.BackRight();
						cout << "Right" << endl;
					}
					while(input::pressed(key::d));
					Car.Break();
					cout << "Break" << endl;
				};
				//Same thing for the "a" button
				if (input::pressed(key::a))
				{
					do
					{
						Car.BackLeft();
						cout << "Left" << endl;
					}
					while(input::pressed(key::a));
					Car.Break();
					cout << "Break" << endl;
				};
			
			};
		//Go Right ---------------------------------
		if(input::pressed(input::key::d))
			{
				Car.Right();
				cout << "Right" << endl;
				//What happens in case the "d" button was pressed and now it isn't
				if (!input::pressed(input::key::d))
				{
					Car.Break();
					cout << "break" << endl;
				}
			};
		//Go Left ---------------------------------
		if(input::pressed(input::key::a))
			{
				Car.Left();
				cout << "Left" << endl;
				//What happens in case the "a" button was pressed and now it isn't
				if (!input::pressed(input::key::a))
				{
					Car.Break();
					cout << "break" << endl;
				}
			};
		//Turn on auto-mode
		if (input::pressed(key::Ctrl))
		{
			Car.Auto();
			cout << "Auto" << endl;
		};
		if(input::pressed(key::CapsLock))
		{
			Car.Break();
			cout << "Break" << endl;
		};
	}
	//Programm finishes when we press the "escape" button
	while(!input::pressed(input::key::escape));
	//Close COM-port
	Car.Finish();
    return 0;
}
