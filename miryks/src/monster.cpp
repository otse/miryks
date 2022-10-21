#include <miryks/miryks.hpp>

#include <miryks/actors.h>
#include <miryks/model.h>
#include <miryks/collision.h>

#include <opengl/scene_graph.hpp>

using namespace miryks;

namespace miryks
{
	Monster::Monster(const char *raceId, const char *path)
	{
		groupDrawer = nullptr;
		placement = nullptr;
		capsule = nullptr;
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
		animation *anim = new animation(keyf);
		anim->skel = skel;
		skel->anim = anim;
	}

	void Monster::Place(const char *name)
	{
		auto reference = ginterior->ids.find(name);
		if (reference != ginterior->ids.end())
		{
			placement = reference->second;
			orientation = placement->orientation;
			groupDrawer->matrix = placement->matrix;
			groupDrawer->UpdateSideways();
			sceneDef->bigGroup->Add(groupDrawer);
		}
		else
			printf("  cant place monster at ref %s  \n", name);
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
		idle = nullptr;
		wearHelmet = false;
	}

	DraugrAlcove::DraugrAlcove(const char *path) : Draugr(path)
	{
		wake = false;
		sleeping = 0;
		transition = 0;
	}

	void Draugr::SetupCollision()
	{
		if (capsule)
			return;
		capsule = new collision::capsule;
		capsule->make(vec3(groupDrawer->matrix[3]) + vec3(0, 0, capsule->half * 2 + capsule->height / 2));
	}

	void Draugr::Setup()
	{
		keyframes *keyf;
		keyf = get_keyframes("anims/draugr/_h2hidle.kf");
		keyf->loop = true;
		animation *idle = new animation(keyf);
		idle->skel = skel;
		skel->anim = idle;
	}

	void DraugrAlcove::Setup()
	{
		CreateFinish();
		keyframes *keyf;
		keyf = get_keyframes("anims/draugr/alcove_idle.kf");
		keyf->loop = false;
		alcove_idle = new animation(keyf);
		alcove_idle->skel = skel;
		skel->anim = alcove_idle;
		keyf = get_keyframes("anims/draugr/alcove_wake.kf");
		keyf->loop = false;
		alcove_wake = new animation(keyf);
		alcove_wake->skel = skel;
		alcove_wake->ratio = 1;
		alcove_wake->proceed = true;
		alcove_idle->next = alcove_wake;
		keyf = get_keyframes("anims/draugr/idle.kf");
		keyf->loop = true;
		idle = new animation(keyf);
		idle->skel = skel;
		alcove_wake->next = idle;
		if (wearHelmet)
		{
			helmet = new SkinnedMesh("actors\\draugr\\character assets\\helmet03.nif");
			helmet->skel = skel;
			groupDrawer->Add(helmet->groupDrawer);
		}
	}

	void DraugrAlcove::CreateFinish()
	{
		auto refe = ginterior->ids.find("gloomgendraugr2");
		if (refe != ginterior->ids.end())
		{
			printf("got end marker");
			end_marker = refe->second;
			// finish = refe->second->matrix;
		}
	}

	void Draugr::Step()
	{
		Monster::Step();
		if (helmet)
			helmet->Step();
	}

	float easeInOutCirc(float t)
	{

		float scaledTime = t * 2;
		float scaledTime1 = scaledTime - 2;

		if (scaledTime < 1)
		{
			return -0.5 * (sqrt(1 - scaledTime * scaledTime) - 1);
		}

		return 0.5 * (sqrt(1 - scaledTime1 * scaledTime1) + 1);
	}

	float easeInOutQuad(float t)
	{
		return t < 0.5 ? 2 * t * t : -1 + (4 - 2 * t) * t;
	}
	float easeInOutQuart(float t)
	{
		auto t1 = t - 1;
		return t < 0.5 ? 8 * t * t * t * t : 1 - 8 * t1 * t1 * t1 * t1;
	}

	void DraugrAlcove::Step()
	{
		Draugr::Step();

		if (sleeping < 1)
		{
			sleeping += delta / 6; // 5 seconds ?
		}
		else if (!wake)
		{
			alcove_idle->proceed = true;
			wake = true;
		}
		else if (skel->anim == alcove_wake)
		{
			transition += delta / 3.f;
			if (transition > 1)
				transition = 1;
			float easing = easeInOutQuart(transition);
			orientation.position = glm::mix(placement->orientation.position, end_marker->orientation.position, easing);
			orientation.Compose();
			groupDrawer->matrix = orientation.matrix;
			groupDrawer->UpdateSideways();
		}
		else if (skel->anim == idle)
		{
			SetupCollision();
		}

		if (capsule)
		{
			vec3 origin = collision::bt_to_glm(capsule->get_world_transform().getOrigin());
			origin = origin - vec3(0, 0, capsule->half * 2 + capsule->height / 2);
			// vec3 vec = vec3(drawer->matrix[3]);
			groupDrawer->matrix[3] = vec4(origin, 1); // = glm::translate(groupDrawer->matrix, origin);
			groupDrawer->UpdateSideways();
		}
	}

} // namespace dark