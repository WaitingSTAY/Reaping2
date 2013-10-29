#ifndef INCLUDED_RENDERER_DEFAULT_MODEL_H
#define INCLUDED_RENDERER_DEFAULT_MODEL_H

class DefaultModel : public Singleton<DefaultModel>, public Model
{
	GLuint mTexId;
	friend class Singleton<DefaultModel>;
	DefaultModel();
	~DefaultModel();
public:
	virtual void Draw(Actor const& Object)const;
};

#endif//INCLUDED_RENDERER_DEFAULT_MODEL_H
