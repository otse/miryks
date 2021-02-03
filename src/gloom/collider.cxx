#include <Gloom/Dark2.h>
#include <Gloom/Collider.h>

#include <algorithm>

struct Tri
{
	glm::vec3 x, y, z, n;
	float dist;
};

void sort_tris_dist(std::vector<Tri> &tris)
{
	std::sort(
        tris.begin(),
        tris.end(),
        [](const Tri &a, const Tri &b) {
            return a.dist < b.dist;
        });
}

namespace gloom
{
    Collider::Collider() {
        printf("collider");
    }

	void Collider::MoveTo(Collider *collider, glm::vec3 tendency)
    {
        bool collide = false;

	    bool gravity = true;
    }

}