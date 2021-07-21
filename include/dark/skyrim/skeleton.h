#ifndef GLOOM_SKELETON_H
#define GLOOM_SKELETON_H

// part of gloom

#include <libs>
#include <dark/dark.h>

#include <opengl/types.h>
#include <opengl/group.h>

namespace dark
{
	class Bone;
	class Skeleton;
	class Keyframes;
	class Animation;

	class Skeleton
	{
	public:
		Skeleton();
		Skeleton(const char *);

		std::map<ni_ref, Bone *> bones;
		std::map<const std::string, Bone *> bonesNamed;

		Nif *nif;
		Bone *baseBone, *lastBone;
		Animation *animation;

		Bone *nested(Rd *, int);

		void load(const char *);
		void construct();
		void step();
	};

	class Bone
	{
	public:
		Group *group;
		//std::vector<Bone *> bones;
		Bone()
		{
			group = new Group;
		};
		mat4 rest, mod, diff;
	};

	class Keyframes
	{
	public:
		Keyframes(Nif *);
		Nif *model;
		struct ni_controller_sequence_pointer *csp;
		bool loop = true;
	};

	class Animation
	{
	public:
		Animation(Keyframes *keyframes) : keyframes(keyframes)
		{
		}
		void step();
		void simpleNonInterpolated();
		Skeleton *skeleton = nullptr;
		float time = 0;
		bool play = true;
		Keyframes *keyframes;
	};

} // namespace dark

#endif