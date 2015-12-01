#include "i_render.h"
#include "mouse_renderer.h"
#include "core/i_position_component.h"
#include "core/i_inventory_component.h"
#include "core/i_health_component.h"
#include "core/i_collision_component.h"
#include "core/i_renderable_component.h"
#include "core/actor.h"
#include "recognizer.h"
#include "shader_manager.h"
#include "input/mouse.h"
#include "engine/engine.h"
#include "core/program_state.h"
#include "core/scene.h"

void MouseRenderer::Init()
{
    mVAO.Init();
    mMouseMoveId = EventServer<WorldMouseMoveEvent>::Get().Subscribe( boost::bind( &MouseRenderer::OnMouseMoveEvent, this, _1 ) );
}

MouseRenderer::MouseRenderer()
{
    Init();
}

void MouseRenderer::Draw()
{
    size_t CurSize = 2;
    typedef std::vector<glm::vec2> Positions_t;
    Positions_t Positions;
    Positions.reserve( CurSize );
    typedef std::vector<GLfloat> Floats_t;
    Floats_t Headings;
    Headings.reserve( CurSize );
    Floats_t Sizes;
    Sizes.reserve( CurSize );
    typedef std::vector<glm::vec4> TexCoords_t;
    TexCoords_t TexCoords;
    TexCoords.reserve( CurSize );
    typedef std::vector<glm::vec4> Colors_t;
    Colors_t Colors;
    Colors.reserve( CurSize );

    Sprite const& Spr=RenderableRepo::Get()(AutoId("cross"))(AutoId("default_action"));
    SpritePhase const& Phase = Spr( 0 );
    GLuint TexId = Phase.TexId;
    Opt<Actor> actor=Scene::Get().GetActor(core::ProgramState::Get().mControlledActorGUID);
    if (actor.IsValid())
    {
        Sizes.push_back( ( GLfloat )actor->Get<IInventoryComponent>()->GetSelectedWeapon()->GetScatter().GetCalculated()*300+15);
    }
    else
    {
        Sizes.push_back( ( GLfloat )15 ); 
    }
    Positions.push_back( glm::vec2( mX, mY ) );
    Headings.push_back( ( GLfloat )0.0 );
    TexCoords.push_back( glm::vec4( Phase.Left, Phase.Right, Phase.Bottom, Phase.Top ) );
    Colors.push_back( glm::vec4(0.0,0.0,1.0,0.5) );
    Positions.push_back( glm::vec2( mX, mY ) );
    Headings.push_back( ( GLfloat )0.0 );
    Sizes.push_back( ( GLfloat )15 ); 
    TexCoords.push_back( glm::vec4( Phase.Left, Phase.Right, Phase.Bottom, Phase.Top ) );
    Colors.push_back( glm::vec4(0.0,0.0,1.0,1.0) );

    mVAO.Bind();
    size_t TotalSize = CurSize * ( sizeof( glm::vec4 ) + sizeof( glm::vec2 ) + 2 * sizeof( GLfloat ) + sizeof( glm::vec4 ) );
    glBufferData( GL_ARRAY_BUFFER, TotalSize, NULL, GL_DYNAMIC_DRAW );

    size_t CurrentOffset = 0;
    size_t CurrentSize = CurSize * sizeof( glm::vec4 );
    GLuint CurrentAttribIndex = 0;
    glBufferSubData( GL_ARRAY_BUFFER, CurrentOffset, CurrentSize, &TexCoords[0] );
    glEnableVertexAttribArray( CurrentAttribIndex );
    size_t const TexIndex = CurrentOffset;
    ++CurrentAttribIndex;

    CurrentOffset += CurrentSize;
    CurrentSize = CurSize * sizeof( glm::vec2 );
    glBufferSubData( GL_ARRAY_BUFFER, CurrentOffset, CurrentSize, &Positions[0] );
    glEnableVertexAttribArray( CurrentAttribIndex );
    size_t const PosIndex = CurrentOffset;
    ++CurrentAttribIndex;

    CurrentOffset += CurrentSize;
    CurrentSize = CurSize * sizeof( GLfloat );
    glBufferSubData( GL_ARRAY_BUFFER, CurrentOffset, CurrentSize, &Headings[0] );
    glEnableVertexAttribArray( CurrentAttribIndex );
    size_t const HeadingIndex = CurrentOffset;
    ++CurrentAttribIndex;

    CurrentOffset += CurrentSize;
    CurrentSize = CurSize * sizeof( GLfloat );
    glBufferSubData( GL_ARRAY_BUFFER, CurrentOffset, CurrentSize, &Sizes[0] );
    glEnableVertexAttribArray( CurrentAttribIndex );
    size_t const SizeIndex = CurrentOffset;
    ++CurrentAttribIndex;

    CurrentOffset += CurrentSize;
    CurrentSize = CurSize * sizeof( glm::vec4 );
    glBufferSubData( GL_ARRAY_BUFFER, CurrentOffset, CurrentSize, &Colors[0] );
    glEnableVertexAttribArray( CurrentAttribIndex );
    size_t const ColorIndex = CurrentOffset;

    ShaderManager& ShaderMgr( ShaderManager::Get() );
    ShaderMgr.ActivateShader( "sprite2" );
    ShaderMgr.UploadData( "spriteTexture", GLuint( 1 ) );
    glActiveTexture( GL_TEXTURE0 + 1 );
    CurrentAttribIndex = 0;
    glVertexAttribPointer( CurrentAttribIndex, 4, GL_FLOAT, GL_FALSE, 0, ( GLvoid* )( TexIndex ));
    glVertexAttribDivisor( CurrentAttribIndex, 1 );
    ++CurrentAttribIndex;
    glVertexAttribPointer( CurrentAttribIndex, 2, GL_FLOAT, GL_FALSE, 0, ( GLvoid* )( PosIndex ));
    glVertexAttribDivisor( CurrentAttribIndex, 1 );
    ++CurrentAttribIndex;
    glVertexAttribPointer( CurrentAttribIndex, 1, GL_FLOAT, GL_FALSE, 0, ( GLvoid* )( HeadingIndex ));
    glVertexAttribDivisor( CurrentAttribIndex, 1 );
    ++CurrentAttribIndex;
    glVertexAttribPointer( CurrentAttribIndex, 1, GL_FLOAT, GL_FALSE, 0, ( GLvoid* )( SizeIndex ));
    glVertexAttribDivisor( CurrentAttribIndex, 1 );
    ++CurrentAttribIndex;
    glVertexAttribPointer( CurrentAttribIndex, 4, GL_FLOAT, GL_FALSE, 0, ( GLvoid* )( ColorIndex ));
    glVertexAttribDivisor( CurrentAttribIndex, 1 );
    glBindTexture( GL_TEXTURE_2D, TexId );
    glDrawArraysInstanced( GL_TRIANGLE_STRIP, 0, 4, 2 );
    glActiveTexture( GL_TEXTURE0 );
    mVAO.Unbind();
}

MouseRenderer::~MouseRenderer()
{

}

void MouseRenderer::OnMouseMoveEvent(const WorldMouseMoveEvent& Event)
{
    mX = Event.Pos.x;
    mY = Event.Pos.y;
}