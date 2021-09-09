#pragma once


#include <dark/dark.h>
#include <skyrim/skyrim.h>

#include <renderer/renderer.h>
#include <renderer/group.h>
#include <renderer/geometry.h>

namespace skyrim
{
	class Bone;
	class Skeleton;
	class Keyframes;
	class Animation;

	Keyframes *skyrim_get_keyframes(const char *);

	class Skeleton
	{
	public:
		std::map<NiRef, Bone *> bones;
		std::map<const std::string, Bone *> bonesNamed;

		Nif *model;
		Bone *baseBone, *lastBone;

		Animation *animation;

		Skeleton();
		Skeleton(Record);
		Bone *MakeBoneHere(Rd *, int);
		void Load(const char *);
		void Construct();
		void Step();
	};

	class Bone : public Group
	{
	public:
		mat4 rest, mod, diff;
		Bone() : Group()
		{
		};
	};

	class Keyframes
	{
	public:
		Keyframes(Nif *);
		Nif *model;
		bool loop;
		NiControllerSequence *controllerSequence;
	};

	class Animation
	{
	public:
		Skeleton *skeleton;
		float time;
		bool play;
		Keyframes *keyframes;
		Animation(Keyframes *);
		void Step();
		void SimpleNonInterpolated();
	};

}