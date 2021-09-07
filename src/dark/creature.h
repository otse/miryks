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
		Record race;
		Creature(const char *, const char *);
		void Place(const char *);
		void Load();
		void SetAnimation(Keyframes *);
		void Step();
	};
}