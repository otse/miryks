
#include "scene"

#include "aabb"
#include "group"
#include "material"
#include "shader"
#include "pointlight"

static pointlight_t *black;

scene_t::scene_t()
{
    black = new pointlight_t;
    black->color = vec3(0);
    black->distance = 0;
    black->intensity = 0;

    ambient = vec3(17.f / 255.f, 17.f / 255.f, 17.f / 255.f);
};

scene_t::~scene_t(){};

void scene_t::Clear()
{
    //items.clear();
    //groups2.clear();
}

template <typename T>
bool add_nullable(T t, std::vector<T> &v)
{
    if (t == nullptr)
        return false;
    std::vector<T>::iterator has = std::find(v.begin(), v.end(), t);
    if (has == v.end())
    {
        // We don't have this yet, add it
        v.push_back(t);
        return true;
    }
    return false;
}

template <typename T>
bool remove_nullable(T t, std::vector<T> &v)
{
    if (t == nullptr)
        return false;
    std::vector<T>::iterator has = std::find(v.begin(), v.end(), t);
    if (has != v.end())
    {
        // We have this, erase it
        v.erase(has);
        return true;
    }
    return false;
}

void scene_t::Add(pointlight_t *l)
{
    add_nullable<pointlight_t *>(l, pointlights);
}

void scene_t::Remove(pointlight_t *l)
{
    remove_nullable<pointlight_t *>(l, pointlights);
}

void scene_t::DrawItems()
{
    CalcLights();
    SortLights();

    //for (Rent &item : items)
    //{
    //	item.draw();
    //}

    //for (instance_t ins : instances)
    //{
    //	ins->draw_classic();
    //}
}

void scene_t::CalcLights()
{
    for (pointlight_t *pl : pointlights)
        pl->Calc();
}

#include <algorithm>

void scene_t::SortLights()
{
    auto sort_distance = [](const pointlight_t *a, const pointlight_t *b) -> bool {
        if (a->GetDist() < b->GetDist())
            return true;
        return false;
    };

    sort(pointlights.begin(), pointlights.end(), sort_distance);
}

void scene_t::BindLights(shader_t *shader)
{
    shader->setVec3("ambientLightColor", ambient);

    for (int i = 0; i < 9; i++)
    {
        pointlight_t *l = black;

        if (i < pointlights.size())
            l = pointlights[i];

        string index = "pointLights[" + std::to_string(i) + "]";

        mat3 package;
        package[0] = l->position_;
        package[1] = l->color_;
        package[2][0] = l->distance;
        package[2][1] = l->decay;
        package[2][2] = -1;

        shader->setMat3((index + ".package").c_str(), package);
    }
}

void scene_t::Order()
{
}