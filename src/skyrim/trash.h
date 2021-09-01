#pragma once

#include <dark/dark.h>
#include <dark/ref.h>

#include <skyrim/record.h>

#include <renderer/types.h>

using namespace dark;

namespace skyrim
{
	class Item;
	class Container;
	//class ItemRenderer;

	typedef Container Cont;

	//extern ItemRenderer *itemRenderer;

	class Item : public Record {
	public:
		RenderTarget *myRt;
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
			show_cursor();
			Refs::labelingEnabled = false;
			cur = this;
		}
		static void Hide() {
			hide_cursor();
			Refs::labelingEnabled = true;
			cur = nullptr;
		}
		static void Step() {
			if (cur)
				cur->Render();
		}
	};

	/*
	class ItemRenderer
	{
	public:
		// static std::map<const char *, RenderTarget *> renderTargets;
		const Item *item = nullptr;
		Scene *myScene = nullptr;
		ViewerCamera *myCam = nullptr;
		ItemRenderer();
		~ItemRenderer() {};
		void View(Item *);
	};
	*/
    
}