#pragma once
#include <iostream>

namespace _testdll
{
	class __declspec(dllexport) PCIControl
	{
	public:
		PCIControl();
		int _create();



	};
}