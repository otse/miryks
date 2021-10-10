#pragma once

#include <dark/dark.h>
#include <dark/reference.h>

#include <skyrim/record.h>

#include <renderer/renderer.h>

using namespace dark;

namespace skyrim
{
	class SKItem;
	class Container;
	//class ItemRenderer;

	typedef Container Cont;

	//extern ItemRenderer *itemRenderer;

	class SKItem : public Record {
	public:
		RenderTarget *myRt;
		SKItem(Record rcd) : Record(rcd) {
			
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
		std::vector<SKItem> items;
		void Init();
		void Render();
		void Activate() {
			now_you_see_me();
			Refs::labelingEnabled = false;
			cur = this;
		}
		static void Hide() {
			now_you_dont();
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