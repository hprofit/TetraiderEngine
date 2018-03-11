#pragma once
#ifndef LIGHT_BASE_H
#define LIGHT_BASE_H

class LightBase : public Component
{
private:
	Transform * m_pTransform;
	Color m_color;
	float m_distance;

	bool m_layers[RENDER_LAYER::L_NUM_LAYERS];

public:
	LightBase(ComponentType cType);
	~LightBase() {};

	virtual void DeActivate();
	virtual void LateInitialize();
	virtual void Update(float dt) {};
	virtual void LateUpdate(float dt) {};
	virtual void Serialize(const json& j);
	virtual void Override(const json& j);
	virtual void HandleEvent(Event* pEvent) {};

	Vector3D GetPosition() const;
	GLubyte Red() const { return m_color[0]; }
	GLubyte Green() const { return m_color[1]; }
	GLubyte Blue() const { return m_color[2]; }
	GLubyte Alpha() const { return m_color[3]; }
	Color GetColor() const { return m_color; }
	float Distance() const { return m_distance; }

	bool GetLayer(int layer) const { return m_layers[layer]; }
};

#endif
