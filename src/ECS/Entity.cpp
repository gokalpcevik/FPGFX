#include "Entity.h"

namespace fpgfx
{
    Entity::Entity(Scene *pScene, entt::entity entity) : m_EntityHandle(entity), m_Scene(pScene)
    {
        }

}
