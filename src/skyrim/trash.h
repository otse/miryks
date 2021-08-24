#pragma once

#include <skyrim/record.h>


namespace skyrim
{
	class Container;
	typedef Container Cont;

	class Container {
	public:
		static Cont *current;
		Record wrcd;
		Container(Record wrcd) : wrcd(wrcd)
		{

		};
		~Container() {}
		void Render();
		void Activate() {
			current = this;
		}
		static void Hide() {
			current = nullptr;
		}
		static void Step() {
			if (current)
				current->Render();
		}
	};
    
}