#ifndef GLOOM_SKELETON_H
#define GLOOM_SKELETON_H

#include <gloom/dark2.h>

#include <opengl/types.h>
#include <opengl/group.h>

namespace gloom
{
	class Bone;
	class Skeleton;
	class KeyFrames;
	class Animation;

	class Skeleton
	{
	public:
		Skeleton();

		std::map<int, Bone *> bones;
		std::map<const std::string, Bone *> bones_named;

		nifp *nif;
		Bone *baseBone, *lastBone;
		Animation *animation;

		Bone *Nested(nifprd *, int);

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
		mat4 rest, mod;
	};

	class KeyFrames
	{
	public:
		KeyFrames(struct nifp *);
		struct nifp *model;
		struct ni_controller_sequence_pointer *csp;
		bool loop = true;
	};

	class Animation
	{
	public:
		Animation(KeyFrames *keyframes) : keyframes(keyframes)
		{
			//csp
		}
		void Step();
		void SimpleNonInterpolated();
		Skeleton *skeleton = nullptr;
		float time = 0;
		bool play = true;
		KeyFrames *keyframes;
	};

} // namespace gloom

#endif