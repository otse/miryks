#pragma once

#include <dark/dark.h>

#include <renderer/aabb.h>
#include <renderer/types.h>

#include <renderer/camera.h>
#include <renderer/rendertarget.h>

namespace dark
{
	void itemrenderer_init();

	class ItemRenderer
	{
	public:
		Camera *camera;
		RenderTarget *render_target;

        ItemRenderer();
        
	};
}