#include <dark/dark.h>

#include <skyrim/record.h>

namespace dark
{
	class Creature
	{
	public:
		SkinnedMesh *skinnedMesh;
		Skeleton *skeleton;
		Animation *animation;
		DrawGroup *drawGroup;
		Creature(const char *, const char *);
		void Place(const char *);
		void SetAnimation(Keyframes *);
		void Step();
		Record race;
	};
}