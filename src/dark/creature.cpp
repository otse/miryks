#include <dark/dark.h>
#include <dark/creature.h>

#include <skyrim/record.h>
#include <skyrim/mesh.h>
#include <skyrim/interior.h>

#include <renderer/drawgroup.h>

using namespace skyrim;

namespace dark
{    
    Creature::Creature(const char *raceId, const char *path)
	{
		animation = nullptr;
		drawGroup = nullptr;
		race = skyrim_get_race(raceId);
		Nif *model = load_model(load_res(path));
		skeleton = new Skeleton(race);
		skinnedMesh = new SkinnedMesh(model, skeleton);
	}

	void Creature::SetAnimation(const char *path)
	{
		animation = new Animation(skyrim_get_keyframes(path));
		animation->skeleton = skeleton;
		skeleton->animation = animation;
	}

	void Creature::Place(const char *q)
	{
		auto ref = dungeon->edIds.find(q);
		if (ref != dungeon->edIds.end())
		{
			drawGroup = new DrawGroup(
				skinnedMesh->baseGroup, ref->second->matrix);
			//drawGroup->matrix = scale(drawGroup->matrix, vec3(1, 1, .5));
			//drawGroup->Add(new DrawGroup(skeleton->baseBone->group, mat4(1.0));
			sceneDef->bigGroup->Add(drawGroup);
		}
		else
		{
			printf("actor put down cant find %s\n", q);
		}
	}

	void Creature::Step()
	{
		if (skinnedMesh)
			skinnedMesh->Step();
		//const float merry = 0.002;
		//if (drawGroup)
		//drawGroup->matrix = glm::rotate(drawGroup->matrix, merry, vec3(0, 0, 1));
	}

} // namespace dark