#pragma once


#include <dark/dark.h>
#include <skyrim/skyrim.h>

#include <renderer/renderer.h>
#include <renderer/group.h>
#include <renderer/geometry.h>

namespace skyrim
{
	class SKBone;
	class SKSkeleton;
	class SKKeyframes;
	class SKAnimation;

	SKKeyframes *skyrim_get_keyframes(const char *);

	class SKSkeleton
	{
	public:
		std::map<NiRef, SKBone *> bones;
		std::map<const std::string, SKBone *> bonesNamed;

		NIF model;
		SKBone *baseBone, *root;

		SKAnimation *animation;

		SKSkeleton();
		SKSkeleton(SKRecord);
		void Load(const char *);
		void Construct();
		void Step();
		SKBone *MakeBoneHere(RD, NiNode *);
	protected:
	};

	class SKBone : public Group
	{
	public:
		const char *name;
		NiNode *block;
		mat4 rest, mod, diff;
		SKBone() : Group()
		{
			name = 0;
			block = 0;
		};
	};

	class SKKeyframes
	{
	public:
		SKKeyframes(NIF);
		NIF model;
		bool loop;
		NiControllerSequence *controllerSequence;
	};

	class SKAnimation
	{
	public:
		SKSkeleton *skeleton;
		float time;
		bool play;
		SKKeyframes *keyframes;
		SKAnimation(SKKeyframes *);
		void Step();
		void SimpleNonInterpolated();
	};

}