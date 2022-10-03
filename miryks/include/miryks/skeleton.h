#pragma once

#include <miryks/miryks.hpp>

#include <opengl/renderer.h>
#include <ogl/scene_graph.hpp>
#include <opengl/geometry.h>

namespace miryks
{
	class bone;
	class animation;
	class animation_mixer;
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
		animation_mixer *mixer;
		skeleton();
		skeleton(const char *);
		~skeleton();
		GroupDrawer *ShowBones();
		void Load(const char *);
		void Construct();
		void Step();
		void SetFreeze();
		// skeleton *Copy();
		bone *MakeBoneHere(RD, NiNode *);

	protected:
	};

	class bone : public Group
	{
	public:
		const char *name;
		NiNode *block;
		mat4 rest, diff;
		quat curQ, frozenQ;
		vec4 curV, frozenV;
		bone() : Group()
		{
			name = 0;
			block = 0;
			curQ = frozenQ = quat(0, 0, 0, 0);
			curV = frozenV = vec4(0, 0, 0, 0);
			// printf("bone : group\n");
		};
		void Cur() {
			curQ = quat(matrix);
			curV = matrix[3];
		}
	};

	class keyframes
	{
	public:
		keyframes(nif *);
		nif *ni;
		bool loop, repeats;
		NiControllerSequence *controllerSequence;
	};

	class animation
	{
	public:
		bool first;
		bool play;
		bool loop;
		float time;
		float ratio;
		animation *next;
		skeleton *skel;
		keyframes *keyf;
		animation(keyframes *);
		~animation();
		void Step();
		void SimpleInterpolated();
		std::vector<bone *> boneCache;
	};
}