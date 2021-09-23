#pragma once

#include <dark/dark.h>
#include <dark/ref.h>

#include <skyrim/record.h>

#include <renderer/renderer.h>

using namespace dark;

namespace skyrim
{
	class SKItem;
	class SKContainer;
	//class ItemRenderer;

	typedef SKContainer Cont;

	//extern ItemRenderer *itemRenderer;

	class SKItem : public SKRecord {
	public:
		RenderTarget *myRt;
		SKItem(SKRecord rcd) : SKRecord(rcd) {
			
		}
	};

	class SKContainer : public SKRecord {
	public:
		static Cont *cur;
		SKContainer(SKRecord rcd) : SKRecord(rcd)
		{
			Init();
		};
		~SKContainer() {}
		std::vector<SKItem> items;
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