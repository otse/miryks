#include <dark/dark.h>

#include <skyrim/actors.h>
#include <skyrim/record.h>
#include <skyrim/model.h>
#include <skyrim/interior.h>

#include <renderer/drawgroup.h>

using namespace skyrim;

namespace skyrim
{
	SKCreature::SKCreature(const char *raceId, const char *path)
	{
		animation = nullptr;
		drawGroup = nullptr;
		race = SkyrimGetRace(raceId);
		skeleton = new SKSkeleton(race);
		modelSkinned = new SKModelSkinned(path);
	}

	void SKCreature::SetAnimation(const char *path)
	{
		animation = new SKAnimation(skyrim_get_keyframes(path));
		animation->skeleton = skeleton;
		skeleton->animation = animation;
	}

	void SKCreature::Place(const char *q)
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

	void SKCreature::Step()
	{
		if (skeleton)
			skeleton->Step();
		if (modelSkinned)
			modelSkinned->Step(skeleton);
		//const float merry = 0.002;
		//if (drawGroup)
		//drawGroup->matrix = glm::rotate(drawGroup->matrix, merry, vec3(0, 0, 1));
	}

} // namespace dark