
#include "scene"

#include "aabb"
#include "group"
#include "material"
#include "shader"
#include "pointlight"


static Pointlight_t *black;

Scene_t::Scene_t()
{
    black = new Pointlight_t;
    black->color = vec3(0);
    black->distance = 0;
    black->intensity = 0;

    ambient = vec3(100.f / 255.f, 100.f / 255.f, 100.f / 255.f);
};

Scene_t::~Scene_t(){};

void Scene_t::Clear()
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

void Scene_t::Add(Pointlight_t *l)
{
    add_nullable<Pointlight_t *>(l, pointlights);
}

void Scene_t::Remove(Pointlight_t *l)
{
    remove_nullable<Pointlight_t *>(l, pointlights);
}

void Scene_t::Add(Group_t *gr)
{
    add_nullable<Group_t *>(gr, groups);
}

void Scene_t::DrawItems()
{
    CalcLights();
    SortLights();

    //for (Rent &item : items)
    //{
    //	item.draw();
    //}

    for (Group_t *gr : groups)
    {
    	gr->DrawClassic(mat4(1));
    }
}

void Scene_t::CalcLights()
{
    for (Pointlight_t *pl : pointlights)
        pl->Calc();
}

#include <algorithm>

void Scene_t::SortLights()
{
    auto sort_distance = [](const Pointlight_t *a, const Pointlight_t *b) -> bool {
        if (a->GetDist() < b->GetDist())
            return true;
        return false;
    };

    sort(pointlights.begin(), pointlights.end(), sort_distance);
}

void Scene_t::BindLights(Shader_t *shader)
{
    shader->SetVec3("ambientLightColor", ambient);

    for (int i = 0; i < 9; i++)
    {
        Pointlight_t *l = black;

        if (i < pointlights.size())
            l = pointlights[i];

        string index = "pointLights[" + std::to_string(i) + "]";

        mat3 package;
        package[0] = l->position_;
        package[1] = l->color_;
        package[2][0] = l->distance;
        package[2][1] = l->decay;
        package[2][2] = -1;

        shader->SetMat3((index + ".package").c_str(), package);
    }
}

void Scene_t::Order()
{
}