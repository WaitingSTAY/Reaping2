#ifndef INCLUDED_CORE_AOE_COLLISION_COMPONENT_H
#define INCLUDED_CORE_AOE_COLLISION_COMPONENT_H

#include "collision_component.h"
#include "core/property_loader.h"
#include <set>

class AoeCollisionComponent : public CollisionComponent
{
public:
    typedef std::set<int32_t> Damaged_Actor_Ids_t;
    AoeCollisionComponent();
    virtual void AddDamagedActorId(int32_t damagedActorId);
    virtual Damaged_Actor_Ids_t const& GetDamagedActorIds()const;
    virtual void SetDamageOnce(bool damageOnce);
    virtual bool IsDamageOnce()const;
    virtual void SetDamage(int32_t damage);
    virtual int32_t GetDamage()const;
protected:
    friend class ComponentFactory;
    Damaged_Actor_Ids_t mDamagedActorIds;
    bool mDamageOnce;
    int32_t mDamage;
private:
};

class AoeCollisionComponentLoader : public ComponentLoader<AoeCollisionComponent>
{
    virtual void BindValues();
protected:
    AoeCollisionComponentLoader();
    friend class ComponentLoaderFactory;
};

#endif//INCLUDED_CORE_AOE_COLLISION_COMPONENT_H
