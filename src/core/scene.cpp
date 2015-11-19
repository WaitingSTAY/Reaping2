#include "platform/i_platform.h"
#include "core/scene.h"
#include "core/i_position_component.h"
#include "core/i_controller_component.h"
#include "core/i_inventory_component.h"
#include "core/i_health_component.h"
#include "core/component_factory.h"
#include "core/i_collision_component.h"
#include "core/i_renderable_component.h"
#include "core/actor_factory.h"
#include "core/renderable_layer.h"
#include "core/i_remove_on_death_component.h"
#include "core/target_player_controller_component.h"
#include "actor_event.h"
#include "player_controller_component.h"
#include "program_state.h"
#include "ui/ui.h"
#include "ui/widget_factory.h"
#include "ui/text_widget.h"
#include "i_move_component.h"
#include "buffs/i_buff_holder_component.h"
#include "buffs/buff_factory.h"
#include "buffs/max_health_buff.h"
#include "engine/buffs_engine/max_health_buff_sub_system.h"
#include "engine/soldier_spawn_system.h"
#include "map_load_event.h"
#include "map_start_event.h"
using core::ProgramState;

int32_t ActorHolder::ActorDefaultOrderer::operator ()(const Opt<Actor>& Obj)const
{
    return Obj->GetGUID();
}

bool ActorHolder::IsRenderable::operator ()(const Opt<Actor>& Obj)const
{
    return Obj->Get<IRenderableComponent>().IsValid();
}

int32_t ActorHolder::GetLayer::operator ()(const Opt<Actor>& Obj)const
{
    return Obj->Get<IRenderableComponent>().IsValid()?Obj->Get<IRenderableComponent>()->GetLayer():0;
}

int32_t ActorHolder::GetZOrder::operator ()(const Opt<Actor>& Obj)const
{
    return Obj->Get<IRenderableComponent>().IsValid()?Obj->Get<IRenderableComponent>()->GetZOrder():0;
}

int32_t ActorHolder::GetCollisionClass::operator()(const Opt<Actor>& Obj) const
{
    return Obj->Get<ICollisionComponent>().IsValid()?int32_t(Obj->Get<ICollisionComponent>()->GetCollisionClass()):0;
}


void Scene::AddActor( Actor* Object )
{
    L2("AddActor called (GUID:%d)\n",Object->GetGUID());
    mNewActors.push_back( Opt<Actor>(Object) );
}

void Scene::Update( double DeltaTime )
{

    if( IsPaused() )
    {
        return;
    }

    Opt<Actor> player(GetActor(mProgramState.mControlledActorGUID));
    if (player.IsValid())
    {
        Opt<IHealthComponent> healthC=player->Get<IHealthComponent>();
        mMaxHP=healthC->GetMaxHP().Get();
    }
    //TODO: testing
    if (false&&ProgramState::Get().mMode!=ProgramState::Client
        &&rand()%60==1
        &&mActorHolder.mAllActors.size()<1500)
    {
        AddTestCreep(mDimensions.x + ( rand() % ( int )( ( mDimensions.z - mDimensions.x ) ) ) 
            , mDimensions.y + ( rand() % ( int )( ( mDimensions.w - mDimensions.y ) ) ) );
    }
    //testing end

    for( NewActorList_t::iterator it = mNewActors.begin(), e = mNewActors.end(); it != e; ++it )
    {
        EventServer<ActorEvent>::Get().SendEvent( ActorEvent( (*it), ActorEvent::Added ) );
        L2("new actor inserted at update (GUID:%d)\n",(*it)->GetGUID());
        mActorHolder.mAllActors.insert( *it );
    }
    mNewActors.clear();
}

Scene::Scene()
    : mDimensions( -4000, -4000, 4000, 4000 )
    , mTypeId( 0 )
    , mPaused( true )
    , mSceneModel( "scene", &RootModel::Get() )
    , mLoadModel( StringFunc( this, &Scene::Load ), "load", &mSceneModel )
    , mPauseModel( VoidFunc( this, &Scene::Pause ), "pause", &mSceneModel )
    , mResumeModel( VoidFunc( this, &Scene::Resume ), "resume", &mSceneModel )
    , mPlayerModel( "player", &RootModel::Get() )
    , mMaxHP( 0 )
    , mProgramState( core::ProgramState::Get() )
{
}

glm::vec4 const& Scene::GetDimensions()
{
    return mDimensions;
}

Scene::~Scene()
{
    for( NewActorList_t::iterator it = mNewActors.begin(), e = mNewActors.end(); it != e; ++it )
    {
        //EventServer<ActorEvent>::Get().SendEvent( ActorEvent( (*it), ActorEvent::Added ) );
        L2("new actor inserted at destruct (GUID:%d)\n",(*it)->GetGUID());
        mActorHolder.mAllActors.insert( *it );
    }
    mNewActors.clear();

    for( ActorList_t::iterator it = mActorHolder.mAllActors.begin(), e = mActorHolder.mAllActors.end(); it!=e; ++it )
    {
        //EventServer<ActorEvent>::Get().SendEvent( ActorEvent( (*it), ActorEvent::Removed ) );
        L2("actor delete at destruct (GUID:%d)\n",(*it)->GetGUID());
        delete (*it).Get();
    }
    mActorHolder.mAllActors.clear();
    mPlayerModels.clear();
}

void Scene::SetType( std::string const& Type )
{
    mTypeId = AutoId( Type );
}

int32_t Scene::GetTypeId() const
{
    return mTypeId;
}

void Scene::Load( std::string const& Level )
{
    EventServer<core::MapLoadEvent>::Get().SendEvent(core::MapLoadEvent("map/"+Level));
    mPaused = false;
    
    for( NewActorList_t::iterator it = mNewActors.begin(), e = mNewActors.end(); it != e; ++it )
    {
        EventServer<ActorEvent>::Get().SendEvent( ActorEvent( (*it), ActorEvent::Added ) );
        L2("new actor inserted at Load (GUID:%d)\n",(*it)->GetGUID());
        mActorHolder.mAllActors.insert( *it );
    }
    mNewActors.clear();

    for( ActorList_t::iterator it = mActorHolder.mAllActors.begin(), e = mActorHolder.mAllActors.end(); it!=e; ++it )
    {
        EventServer<ActorEvent>::Get().SendEvent( ActorEvent( (*it), ActorEvent::Removed ) );
        L2("actor delete at Load (GUID:%d)\n",(*it)->GetGUID());
        delete (*it).Get();
    }
    mActorHolder.mAllActors.clear();
    SetType( "grass" );

    EventServer<core::MapStartEvent>::Get().SendEvent(core::MapStartEvent());

}

void Scene::AddTestCreep(double X, double Y)
{
    std::auto_ptr<Actor> Obj;
    switch(rand()%4)
    {
    case 0:
        Obj=ActorFactory::Get()(AutoId("spider1"));
        break;
    case 1:
        Obj=ActorFactory::Get()(AutoId("spider2"));
        break;
    case 2:
        Obj=ActorFactory::Get()(AutoId("spider1target"));
        break;
    case 3:
        Obj=ActorFactory::Get()(AutoId("spider2target"));
        break;
    }
    Obj->Get<IPositionComponent>()->SetX(X);
    Obj->Get<IPositionComponent>()->SetY(Y);
    AddActor( Obj.release() );
}

void Scene::RemoveActor(ActorList_t::iterator it)
{
    EventServer<ActorEvent>::Get().SendEvent( ActorEvent( (*it), ActorEvent::Removed ) );
    L2("removeActor it (GUID:%d)\n",(*it)->GetGUID());
    delete (*it).Get();
    mActorHolder.mAllActors.erase(it);
}

void Scene::RemoveActor(int32_t guid)
{
    ActorList_t::iterator it = mActorHolder.mAllActors.find(guid);
    if (it!=mActorHolder.mAllActors.end())
    {
        L2("removeActor from existing actors (GUID:%d)\n",(*it)->GetGUID());
        RemoveActor(it);
        return;
    }
    else
    {
        for(NewActorList_t::iterator i=mNewActors.begin(),e=mNewActors.end();i!=e;++i)
        {
            if ((*i)->GetGUID()==guid)
            {
                delete (*i).Get();
                mNewActors.erase(i);
                L2("removeActor from new actors (GUID:%d)\n",(*i)->GetGUID());
                return;
            }
        }
    }
    L1("removeActor is called on an actor that does not exist (no good at all) (GUID:%d)\n",guid);
}

Opt<Actor> Scene::GetActor(int32_t guid)
{
    ActorList_t::iterator it = mActorHolder.mAllActors.find(guid);
    if (it!=mActorHolder.mAllActors.end())
    {
        return (*it);
    }
    for(NewActorList_t::iterator i=mNewActors.begin(),e=mNewActors.end();i!=e;++i)
    {
        if ((*i)->GetGUID()==guid)
        {
            return (*i);
        }
    }
    return Opt<Actor>(NULL);
}

void Scene::SetPlayerModels(Opt<Actor> actor)
{
    mPlayerModels.clear();
    Opt<IHealthComponent> healthC=actor->Get<IHealthComponent>();
    mPlayerModels.push_back( new ModelValue( healthC->GetHP(), "hp", &mPlayerModel ) );
    Opt<IPositionComponent> positionC = actor->Get<IPositionComponent>();
    mPlayerModels.push_back( new ModelValue( positionC->GetX(), "x", &mPlayerModel ) );
    mPlayerModels.push_back( new ModelValue( positionC->GetY(), "y", &mPlayerModel ) );
    mPlayerModels.push_back( new ModelValue( mMaxHP, "max_hp", &mPlayerModel ) );
}

