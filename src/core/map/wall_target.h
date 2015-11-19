#ifndef INCLUDED_CORE_MAP_WALL_TARGET_H
#define INCLUDED_CORE_MAP_WALL_TARGET_H

#include "platform/i_platform.h"
#include "i_target.h"
#include "../scene.h"

namespace map {

class WallTarget:public ITarget
{
public:
    WallTarget( int32_t Id, int32_t cursorId, int32_t actorId );
    virtual void Update( double DeltaTime );
    virtual int32_t GetCursorId() const;
    virtual int32_t GetActorId() const;
    virtual void PutTarget(glm::vec2 position);
protected:
    int32_t mId;
    int32_t mCursorId;
    int32_t mActorId;
private:
    Scene& mScene;
};


} // namespace map
#endif//INCLUDED_CORE_MAP_WALL_TARGET_H