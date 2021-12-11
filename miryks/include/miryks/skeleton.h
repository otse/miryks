#pragma once

#include <miryks/miryks.h>

#include <opengl/renderer.h>
#include <opengl/group.h>
#include <opengl/geometry.h>

namespace miryks
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
		Nif *model;
		Bone *baseBone, *root;
		Anim *anim;
		Skel();
		Skel(record_copy);
		void Load(const char *);
		void Construct();
		void Step();
		Bone *MakeBoneHere(RD, NiNode *);
	protected:
	};

	class Bone : public group_type
	{
	public:
		const char *name;
		NiNode *block;
		mat4 rest, mod, diff;
		Bone() : group_type()
		{
			name = 0;
			block = 0;
		};
	};

	class Keyf
	{
	public:
		Keyf(Nif *);
		Nif *nif;
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