#pragma once

#include <miryks/miryks.h>
#include <miryks/miryks.hpp>

#include <opengl/renderer.h>
#include <opengl/group.h>
#include <opengl/geometry.h>

namespace miryks
{
	class bone;
	class animation;
	class keyframes;

	keyframes *get_keyframes(const char *);

	class skeleton
	{
	public:
		std::map<NiRef, bone *> bones;
		std::map<const std::string, bone *> bonesNamed;
		Nif *model;
		bone *baseBone, *root;
		animation *anim;
		skeleton();
		skeleton(char *);
		void Load(const char *);
		void Construct();
		void Step();
		bone *MakeBoneHere(RD, NiNode *);
	protected:
	};

	class bone : public Group
	{
	public:
		const char *name;
		NiNode *block;
		mat4 rest, mod, diff;
		bone() : Group()
		{
			name = 0;
			block = 0;
		};
	};

	class keyframes
	{
	public:
		keyframes(Nif *);
		Nif *nif;
		bool loop;
		NiControllerSequence *controllerSequence;
	};

	class animation
	{
	public:
		float time;
		bool play;
		skeleton *skel;
		keyframes *keyf;
		animation(keyframes *);
		void Step();
		void SimpleNonInterpolated();
	};

}