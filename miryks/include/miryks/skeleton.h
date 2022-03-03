#pragma once

#include <miryks/miryks.hpp>

#include <opengl/renderer.h>
#include <ogl/scene_graph.hpp>
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
		nif *model;
		bone *baseBone, *root;
		animation *anim;
		skeleton();
		skeleton(char *);
		GroupDrawer *ShowBones();
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
			printf("bone : group\n");
		};
	};

	class keyframes
	{
	public:
		keyframes(nif *);
		nif *ni;
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