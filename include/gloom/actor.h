#ifndef GLOOM_ACTOR_H
#define GLOOM_ACTOR_H

#include <gloom/dark2.h>

#include "object.h"

namespace gloom
{
	Object GetRace(const char *);
	//void Export(const char *);
	void ExportRaceHkxToKf(const char *);

	class Actor
	{
	public:
		Actor(const char *, const char *);

		Mesh *mesh = nullptr;
		SkinnedMesh *smesh = nullptr;
		Skeleton *skeleton = nullptr;
		Animation *animation = nullptr;

		DrawGroup *drawGroup = nullptr;

		float merry = 0;

		void Step();

		void PutDown(const char *);
	};

	class Human
	{
		Actor *head, *body, *hands, *feet;
	public:
		Human()
		{
			head = new Actor("ImperialRace", "meshes\\actors\\character\\character assets\\malehead.nif");
			body = new Actor("ImperialRace", "meshes\\clothes\\prisoner\\prisonerclothes_0.nif");
			hands = new Actor("ImperialRace", "meshes\\clothes\\prisoner\\prisonercuffs_0.nif");
			feet = new Actor("ImperialRace", "meshes\\clothes\\prisoner\\prisonershoes_0.nif");
			head->PutDown("gloomgenman");
			body->PutDown("gloomgenman");
			hands->PutDown("gloomgenman");
			feet->PutDown("gloomgenman");
		};

		void Step()
		{
			head->Step();
			body->Step();
			hands->Step();
			feet->Step();
		}
	};

} // namespace gloom

#endif