#include <miryks/miryks.hpp>

#include <miryks/actors.h>
#include <miryks/model.h>

#include <ogl/scene_graph.hpp>

using namespace miryks;

namespace miryks
{
	Monster::Monster(const char *raceId, const char *path)
	{
		anim = nullptr;
		groupDrawer = nullptr;
		race = dig_race(raceId, 0);
		skel = new skeleton(race.data<char *>("ANAM"));
		modelSkinned = new ModelSkinned(path);
	}

	void Monster::SetAnim(const char *path)
	{
		anim = new animation(get_keyframes(path));
		anim->skel = skel;
		skel->anim = anim;
	}

	void Monster::Place(const char *name)
	{
		std::string str(name);
		auto refe = ginterior->ids.find(name);
		if (refe != ginterior->ids.end())
		{
			groupDrawer = new GroupDrawer(
				modelSkinned->baseGroup, refe->second->matrix);
			groupDrawer->Add(new GroupDrawer(skel->root, mat4(1.0)));
			sceneDef->bigGroup->Add(groupDrawer);
		}
		else
			printf("!! cant place creature %s\n", name);
	}

	void Monster::Step()
	{
		if (skel)
			skel->Step();
		if (modelSkinned)
			modelSkinned->Step(skel);
		// const float merry = 0.002;
		// if (groupDrawer)
		// groupDrawer->matrix = glm::rotate(groupDrawer->matrix, merry, vec3(0, 0, 1));
	}

} // namespace dark