#pragma once


#include <dark/dark.h>
#include <skyrim/skyrim.h>

#include <renderer/renderer.h>
#include <renderer/group.h>
#include <renderer/geometry.h>

#define DRAW_BONES 0

namespace skyrim
{
	class Bone;
	class Skeleton;
	class Keyframes;
	class Animation;

	Keyframes *loadAnimDisk(const char *);

	class Skeleton
	{
	public:
		Skeleton();
		Skeleton(const char *);
		Skeleton(Record);

		std::map<NiRef, Bone *> bones;
		std::map<const std::string, Bone *> bonesNamed;

		Nif *nif;
		Bone *baseBone, *lastBone;
		Animation *animation;

		Bone *MakeBoneHere(Rd *, int);

		void Load(const char *);
		void Construct();
		void Step();
	};

	class Bone
	{
	public:
		Group *group;
		//std::vector<Bone *> bones;
		Bone()
		{
			group = new Group;
//#if DRAW_BONES
//			group->geometry = new Geometry();
//			group->geometry->SetupMesh();
//#endif
		};
		mat4 rest, mod, diff;
	};

	class Keyframes
	{
	public:
		Keyframes(Nif *);
		Nif *model;
		NiControllerSequence *csp;
		bool loop = true;
	};

	class Animation
	{
	public:
		Animation(Keyframes *keyframes) : keyframes(keyframes)
		{
		}
		void Step();
		void SimpleNonInterpolated();
		Skeleton *skeleton = nullptr;
		float time = 0;
		bool play = true;
		Keyframes *keyframes;
	};

}