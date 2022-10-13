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
		groupDrawer = new GroupDrawer(modelSkinned->baseGroup, mat4(1.0));
		groupDrawer->name = "Monster";
		GroupDrawer *bones = new GroupDrawer(skel->root, mat4(1.0));
		bones->name = "Bones";
		groupDrawer->Add(bones);
		/*helmet2 = new SkinnedMesh("actors\\draugr\\character assets\\helmet03.nif");
		helmet2->skel = skel;
		groupDrawer->Add(helmet2->groupDrawer);*/
	}

	void Monster::SetAnim(const char *path)
	{
		keyframes *keyf = get_keyframes(path);
		anim = new animation(keyf);
		anim->skel = skel;
		skel->anim = anim;
	}

	void Monster::Place(const char *name)
	{
		std::string str(name);
		auto refe = ginterior->ids.find(name);
		if (refe != ginterior->ids.end())
		{
			groupDrawer->matrix = refe->second->matrix;
			groupDrawer->UpdateSideways();
			sceneDef->bigGroup->Add(groupDrawer);
		}
		else
			printf("!! cant place creature %s\n", name);
	}

	void Monster::Step()
	{
		if (skel)
			skel->Step();
		// if (helmet2)
		//	helmet2->Step();
		if (modelSkinned)
			modelSkinned->Step(skel);
		// const float merry = 0.002;
		// if (groupDrawer)
		// groupDrawer->matrix = glm::rotate(groupDrawer->matrix, merry, vec3(0, 0, 1));
	}

	Draugr::Draugr(const char *path) : Monster("DraugrRace", path)
	{
		helmet = nullptr;
		alcove = false;
		wake = false;
		sleeping = 0;
		wearHelmet = false;
	}

	void Draugr::Setup()
	{
		keyframes *keyf;
		if (alcove)
		{
			keyf = get_keyframes("anims/draugr/alcove_idle.kf");
			alcove_idle = new animation(keyf);
			alcove_idle->skel = skel;
			alcove_idle->loop = true;
			skel->anim = alcove_idle;
			keyf = get_keyframes("anims/draugr/alcove_wake.kf");
			animation *alcove_wake = new animation(keyf);
			alcove_wake->skel = skel;
			alcove_wake->ratio = 1;
			alcove_wake->loop = false;
			alcove_idle->next = alcove_wake;
			keyf = get_keyframes("anims/draugr/idle.kf");
			animation *idle = new animation(keyf);
			idle->skel = skel;
			idle->loop = true;
			alcove_wake->next = idle;
		}
		else
		{
			keyf = get_keyframes("anims/draugr/_h2hidle.kf");
			animation *idle = new animation(keyf);
			idle->skel = skel;
			idle->loop = true;
			skel->anim = idle;
		}
		if (wearHelmet)
		{
			helmet = new SkinnedMesh("actors\\draugr\\character assets\\helmet03.nif");
			helmet->skel = skel;
			groupDrawer->Add(helmet->groupDrawer);
		}
	}

	void Draugr::Step()
	{
		Monster::Step();
		if (helmet)
			helmet->Step();
		if (alcove)
		{
			if (sleeping < 1)
			{
				sleeping += delta / 5;
			}
			else if (!wake)
			{
				alcove_idle->loop = false;
				wake = true;
			}
		}
	}

} // namespace dark