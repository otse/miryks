#pragma once

#include <miryks/reference.h>
#include <miryks/record.h>

#include <opengl/renderer.h>

namespace miryks
{
	class SKItem;
	class Container;
	//class ItemRenderer;

	typedef Container Cont;

	//extern ItemRenderer *itemRenderer;

	class SKItem : public record_copy {
	public:
		RenderTarget *myRt;
		SKItem(record_copy rcd) : record_copy(rcd) {
			
		}
	};

	class Container : public record_copy {
	public:
		static Cont *cur;
		Container(record_copy rcd) : record_copy(rcd)
		{
			Init();
		};
		~Container() {}
		std::vector<SKItem> items;
		void Init();
		void Render();
		void Activate() {
			//now_you_see_me();
			itemfinder::detect = false;
			cur = this;
		}
		static void Hide() {
			//now_you_dont();
			itemfinder::detect = true;
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