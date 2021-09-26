#pragma once


#include <dark/dark.h>
#include <skyrim/skyrim.h>

#include <renderer/renderer.h>
#include <renderer/group.h>
#include <renderer/geometry.h>

namespace skyrim
{
	class Bone;
	class Skel;
	class Keyf;
	class Anim;

	Keyf *get_keyframes(const char *);

	class Skel
	{
	public:
		std::map<NiRef, Bone *> bones;
		std::map<const std::string, Bone *> bonesNamed;
		NIF model;
		Bone *baseBone, *root;
		Anim *anim;
		Skel();
		Skel(Rcd);
		void Load(const char *);
		void Construct();
		void Step();
		Bone *MakeBoneHere(RD, NiNode *);
	protected:
	};

	class Bone : public Group
	{
	public:
		const char *name;
		NiNode *block;
		mat4 rest, mod, diff;
		Bone() : Group()
		{
			name = 0;
			block = 0;
		};
	};

	class Keyf
	{
	public:
		Keyf(NIF);
		NIF nif;
		bool loop;
		NiControllerSequence *controllerSequence;
	};

	class Anim
	{
	public:
		float time;
		bool play;
		Skel *skel;
		Keyf *keyf;
		Anim(Keyf *);
		void Step();
		void SimpleNonInterpolated();
	};

}