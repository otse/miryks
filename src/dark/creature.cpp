#include <dark/dark.h>
#include <dark/creature.h>

#include <skyrim/record.h>
#include <skyrim/mesh.h>
#include <skyrim/interior.h>

#include <renderer/drawgroup.h>

using namespace skyrim;

namespace dark
{
    Keyframes *draugrAttack = nullptr;
	Keyframes *draugrIdle = nullptr;
    
    Creature::Creature(const char *editorId, const char *path)
	{
		animation = nullptr;
		drawGroup = nullptr;
		race = skyrim_get_race(editorId);
		Nif *model = load_model(load_rsc(path), true);
		skeleton = new Skeleton(race);
		skinnedMesh = new SkinnedMesh(model, skeleton);
	}

	void Creature::SetAnimation(Keyframes *keyframes)
	{
		animation = new Animation(keyframes);
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
			skinnedMesh->Forward();
		//const float merry = 0.002;
		//if (drawGroup)
		//drawGroup->matrix = glm::rotate(drawGroup->matrix, merry, vec3(0, 0, 1));
	}

} // namespace dark