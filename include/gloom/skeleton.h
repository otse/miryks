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

		nifp *nif;
		Bone *baseBone, *lastBone;
		Animation *animation;

		Bone *Nested(nifprd *);
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
	};

	class KeyFrames
	{
	public:
		KeyFrames(struct nifp *nif) : model(nif) {}

		struct nifp *model;

		bool loop = false;
	};

	class Animation
	{
	public:
		Animation() {};

		void Animate(Skeleton *);

		float time = 0;

		bool play = true;

		KeyFrames *kf;
	};

} // namespace gloom

#endif