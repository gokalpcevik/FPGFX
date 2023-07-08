#pragma once
#include "entt/entt.hpp"
#include "CommonComponents.h"

namespace fpgfx
{
    class Entity;

    struct Scene
    {
        Scene() = default;
        Scene(Scene &&) = default;
        Scene(const Scene &) = delete;

        Entity CreateEntity();
        void Destroy(Entity entity);
        void Update(float dt);

        template <typename T>
        auto GetComponent(entt::entity entity) -> T &;

        template <typename T, typename... Args>
        auto AddComponent(entt::entity entity, Args &&...args) -> T &;

        template <typename T>
        void RemoveComponent(entt::entity entity);

        entt::registry m_Registry{};
    };

    template <typename T>
    inline auto Scene::GetComponent(entt::entity entity) -> T &
    {
        return m_Registry.get<T>(entity);
    }

    template <typename T, typename... Args>
    inline auto Scene::AddComponent(entt::entity entity, Args &&...args) -> T &
    {
        return m_Registry.emplace<T>(entity, std::forward<Args>(args)...);
    }

    template <typename T>
    auto Scene::RemoveComponent(entt::entity entity) -> void
    {
        m_Registry.remove<T>(entity);
    }
}
