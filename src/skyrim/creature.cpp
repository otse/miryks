#include <dark/dark.h>

#include <skyrim/actors.h>
#include <skyrim/record.h>
#include <skyrim/model.h>
#include <skyrim/cell.h>

#include <renderer/drawgroup.h>

using namespace skyrim;

namespace skyrim
{
	Monster::Monster(const char *raceId, const char *path)
	{
		anim = nullptr;
		drawGroup = nullptr;
		race = GetRace(raceId);
		skel = new Skel(race);
		modelSkinned = new ModelSkinned(path);
	}

	void Monster::SetAnim(const char *path)
	{
		anim = new Anim(get_keyframes(path));
		anim->skel = skel;
		skel->anim = anim;
	}

	void Monster::Place(const char *q)
	{
		auto ref = dungeon->edIds.find(q);
		if (ref != dungeon->edIds.end())
		{
			drawGroup = new DrawGroup(
				modelSkinned->baseGroup, ref->second->matrix);
			//drawGroup->matrix = scale(drawGroup->matrix, vec3(1, 1, .5));
			//drawGroup->Add(new DrawGroup(skeleton->baseBone->group, mat4(1.0));
			sceneDef->bigGroup->Add(drawGroup);
		}
		else
			printf("cant place creature %s\n", q);
	}

	void Monster::Step()
	{
		if (skel)
			skel->Step();
		if (modelSkinned)
			modelSkinned->Step(skel);
		//const float merry = 0.002;
		//if (drawGroup)
		//drawGroup->matrix = glm::rotate(drawGroup->matrix, merry, vec3(0, 0, 1));
	}

} // namespace dark