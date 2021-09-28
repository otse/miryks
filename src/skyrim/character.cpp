#include <skyrim/actors.h>
#include <skyrim/interior.h>

#include <renderer/drawgroup.h>

#include <renderer/camera.h>>

namespace skyrim {
    Char::Char(const char *raceId)
	{
		anim = nullptr;
		race = get_race(raceId);
		skel = new Skel(race);
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
		drawGroup = new DrawGroup(nullptr, mat4(1.0));
		if (hat)
			drawGroup->Add(hat->drawGroup);
		if (head)
			drawGroup->Add(head->drawGroup);
		if (body)
			drawGroup->Add(body->drawGroup);
		if (hands)
			drawGroup->Add(hands->drawGroup);
		if (feet)
			drawGroup->Add(feet->drawGroup);
		drawGroup->Update();
	};

	void Char::Place(const char *q)
	{
		auto ref = dungeon->edIds.find(q);
		if (ref == dungeon->edIds.end())
			return;
		printf("place at %s\n", q);
		drawGroup->matrix = ref->second->matrix;
		drawGroup->Update();
		sceneDef->bigGroup->Add(drawGroup);
		// Create an offsetted mirror of Man
		/*DrawGroup *mirror = new DrawGroup(group, mat4());
		mirror->matrix = drawGroup->matrix;
		mirror->matrix = glm::translate(mirror->matrix, vec3(50, 0, 0));*/
		//sceneDef->Add(mirror);
	}

	void Char::SetAnimation(const char *path)
	{
		anim = new Anim(get_keyframes(path));
		anim->skel = skel;
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
			//drawGroup->matrix = translate(drawGroup->matrix, csphere->GetWorldTransform());
			//drawGroup->matrix = translate(mat4(1.0), csphere->GetWorldTransform());
			//drawGroup->Reset();
		//}
	}
}