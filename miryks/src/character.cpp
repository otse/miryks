#include <miryks/miryks.hpp>
#include <miryks/actors.h>

#include <ogl/scene_graph.hpp>

#include <opengl/camera.h>

namespace miryks {
    Char::Char(const char *raceId)
	{
		anim = nullptr;
		race = dig_race(raceId, 0);
		skel = new skeleton(race.data<char *>("ANAM"));
		hat = head = body = hands = feet = nullptr;
		const bool beggar = true;
		if (beggar)
		{
			//hat = new SkinnedMesh("clothes\\beggarclothes\\hatm_0.nif");
			//head = new BodyPart("actors\\character\\character assets\\malehead.nif");
			body = new SkinnedMesh("clothes\\prisoner\\prisonerclothes_0.nif");
			hands = new SkinnedMesh("clothes\\prisoner\\prisonercuffs_0.nif");
			feet = new SkinnedMesh("clothes\\prisoner\\prisonershoes_0.nif");
		}
		//hat->skel = skel;
		body->skel = skel;
		hands->skel = skel;
		feet->skel = skel;
		/*
			//head = new BodyPart("ImperialRace", "clothes\\graybeardrobe\\greybeardhat_0.nif");
			//body = new BodyPart("ImperialRace", "clothes\\graybeardrobe\\greyboardrobe_0.nif");
			//feet = new BodyPart("ImperialRace", "clothes\\graybeardrobe\\greybeardboots_0.nif");
		*/
		groupDrawer = new GroupDrawer(nullptr, mat4(1.0));
		if (hat)
			groupDrawer->Add(hat->groupDrawer);
		if (head)
			groupDrawer->Add(head->groupDrawer);
		if (body)
			groupDrawer->Add(body->groupDrawer);
		if (hands)
			groupDrawer->Add(hands->groupDrawer);
		if (feet)
			groupDrawer->Add(feet->groupDrawer);
		groupDrawer->UpdateSideways();
	};

	void Char::Place(const char *q)
	{
		if (!ginterior)
			return;
		auto ref = ginterior->ids.find(q);
		if (ref == ginterior->ids.end())
			return;
		printf("place at %s\n", q);
		groupDrawer->matrix = ref->second->matrix;
		groupDrawer->UpdateSideways();
		sceneDef->bigGroup->Add(groupDrawer);
		// Create an offsetted mirror of Man
		/*GroupDrawer *mirror = new GroupDrawer(group, mat4());
		mirror->matrix = groupDrawer->matrix;
		mirror->matrix = glm::translate(mirror->matrix, vec3(50, 0, 0));*/
		//sceneDef->Add(mirror);
	}

	void Char::SetAnim(const char *path)
	{
		//if (anim)
		//	delete anim;
		anim->skel = skel;
		anim = new animation(get_keyframes(path));
		skel->anim = anim;
	}

	void Char::Step()
	{
		if (skel)
			skel->Step();
		if (hat)
			hat->Step();
		if (head)
			head->Step();
		if (body)
			body->Step();
		if (hands)
			hands->Step();
		if (feet)
			feet->Step();
		//if (csphere)
		//{
			//groupDrawer->matrix = translate(groupDrawer->matrix, csphere->GetWorldTransform());
			//groupDrawer->matrix = translate(mat4(1.0), csphere->GetWorldTransform());
			//groupDrawer->Reset();
		//}
	}
}