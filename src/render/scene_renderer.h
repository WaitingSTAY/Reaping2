#ifndef INCLUDED_RENDER_SCENE_RENDERER_H
#define INCLUDED_RENDER_SCENE_RENDERER_H

class SceneRenderer
{
    VaoBase mVAO;
    RenderableRepo& mRenderables;
    int32_t mActionId;
    int32_t mSceneTypeId;
    GLuint mTextureId;
    glm::vec4 mSceneDimensions;
    void CreateMesh( Scene& Object );
    bool SceneChanged( Scene& Object )const;
public:
    SceneRenderer();
    void Draw( Scene& Object );
};

#endif//INCLUDED_RENDER_SCENE_RENDERER_H