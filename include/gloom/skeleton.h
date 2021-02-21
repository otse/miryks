#ifndef GLOOM_SKELETON_H
#define GLOOM_SKELETON_H

// part of gloom

#include <libs>
#include <Gloom/Dark2.h>

#include <OpenGL/Types.h>
#include <OpenGL/Group.h>

namespace gloom
{
	class Bone;
	class Skeleton;
	class Keyframes;
	class Animation;

	class Skeleton
	{
	public:
		Skeleton();

		std::map<ni_ref, Bone *> bones;
		std::map<const std::string, Bone *> bones_named;

		Nif *nif;
		Bone *baseBone, *lastBone;
		Animation *animation;

		Bone *Nested(Rd *, int);

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
			//csp
		}
		void Step();
		void SimpleNonInterpolated();
		Skeleton *skeleton = nullptr;
		float time = 0;
		bool play = true;
		Keyframes *keyframes;
	};

} // namespace gloom

#endif