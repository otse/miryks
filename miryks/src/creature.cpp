#include <miryks/miryks.hpp>

#include <miryks/actors.h>
#include <miryks/model.h>

#include <opengl/drawgroup.h>

using namespace miryks;

namespace miryks
{
	Monster::Monster(const char *raceId, const char *path)
	{
		anim = nullptr;
		drawGroup = nullptr;
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
			drawGroup = new DrawGroup(
				modelSkinned->baseGroup, refe->second->matrix);
			// drawGroup->matrix = scale(drawGroup->matrix, vec3(1, 1, .5));
			drawGroup->Add(new DrawGroupFlatSorted(skel->baseBone, mat4(1.0)));
			sceneDef->bigGroup->Add(drawGroup);
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
		// if (drawGroup)
		// drawGroup->matrix = glm::rotate(drawGroup->matrix, merry, vec3(0, 0, 1));
	}

} // namespace dark