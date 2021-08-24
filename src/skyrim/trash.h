#pragma once

#include <skyrim/record.h>


namespace skyrim
{
	class Item;
	class Container;
	class ItemRenderer;

	typedef Container Cont;

	class Item : public Record {
	public:
		Item(Record rcd) : Record(rcd) {

		}
	};

	class Container : public Record {
	public:
		static Cont *cur;
		Container(Record rcd) : Record(rcd)
		{
			Init();
		};
		~Container() {}
		std::vector<Item> items;
		void Init();
		void Render();
		void Activate() {
			cur = this;
		}
		static void Hide() {
			cur = nullptr;
		}
		static void Step() {
			if (cur)
				cur->Render();
		}
	};

	class ItemRenderer
	{
		ItemRenderer() {};
		~ItemRenderer() {}
	};
    
}