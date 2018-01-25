#include "Body.h"
#include "Transform.h"
#include "Shape.h"
#include "DebugManager.h"
#include <iostream>

Body::Body() :
	Component(ComponentType::Body),
	m_isStatic(false)
{
}

Body::~Body() {}

void Body::Update(double dt) {}

void Body::Integrate(double dt) {
	if (m_isStatic) {
		DrawDebugShape();
		return;
	}

	// Update previous position
	m_PrevPosition.Set(m_Position.x, m_Position.y, m_Position.z);

	// Calculate acceleration from forces
	m_Acceleration = m_Forces*m_massInv;

	// Integrate acceleration to velocity
	m_Velocity += m_Acceleration*dt;

	// Integrate velocity to position
	m_Position += m_Velocity*dt;

	// Update transform positions
	m_pTransform->m_position = m_Position;

	// Clear all forces
	ClearForces();

	DrawDebugShape();
}

void Body::Serialize(json j) {
	m_isStatic = ParseBool(j, "isStatic");
	m_mass = ParseFloat(j, "mass");

	if (!m_mass) m_massInv = 0;
	else m_massInv = 1 / m_mass;

	std::string shape = ParseString(j["SHAPE"], "type");

	// TODO: if overriding avoid creating new shapes
	if (shape == "AABB") {
		AABB* pRect = new AABB(ParseFloat(j["SHAPE"], "width"), ParseFloat(j["SHAPE"], "height"));
		m_pShape = pRect;
		m_pShape->pBody = this;
	}
	else if (shape == "circle") {
		Circle* pCircle = new Circle(ParseFloat(j["SHAPE"], "radius"));
		m_pShape = pCircle;
		m_pShape->pBody = this;
	}
	else if (shape == "polygon") {
		Polygon* pPolygon = new Polygon();
		for (unsigned int i = 0; i < j["SHAPE"]["vertices"].size(); ++i) {
			Vector3D vertx(ParseFloat(j["SHAPE"]["vertices"][i], "x"), ParseFloat(j["SHAPE"]["vertices"][i], "y"), ParseFloat(j["SHAPE"]["vertices"][i], "z"));
			pPolygon->m_vertices.push_back(vertx);
		}
		m_pShape = pPolygon;
		m_pShape->pBody = this;
	}
}

void Body::LateInitialize() {
	if (!m_pTransform) {
		if (pGO)
			m_pTransform = static_cast<Transform*>(pGO->GetComponent(ComponentType::Transform));
		else {
			printf("No Game Object found. Body component failed to operate.\n");
			return;
		}

		if (!m_pTransform) {
			printf("No Transform component found. Body component failed to operate.\n");
			return;
		}
	}
}

const Vector3D& Body::GetPosition() { return m_Position; }
const Vector3D& Body::GetPreviousPosition() { return m_PrevPosition; }
const Vector3D& Body::GetVelocity() { return m_Velocity; }

void Body::SetVelocity(const Vector3D& vel) { m_Velocity.Set(vel.x, vel.y, vel.z); }

void Body::SetVelocity(float speed, float angle) {
	m_Velocity = Vector3D::AngleDegrees(angle, Vector3D(0,0,1));
	m_Velocity.Mul(speed);
}

void Body::DrawDebugShape() {
	DebugManager& debugMngr = DebugManager::GetInstance();

	switch (m_pShape->type) {
		case ST_Circle: {
			Circle* pC = static_cast<Circle*>(m_pShape);
			debugMngr.DrawWireCircle(GetPosition(), pC->radius*2.0f, DebugColor::GREEN);
			break;
		}
		case ST_AABB: {
			AABB* pRect = static_cast<AABB*>(m_pShape);
			debugMngr.DrawWireRectangle(GetPosition(), Vector3D(0,0,0), Vector3D(pRect->width, pRect->height, 0), DebugColor::GREEN);
			break;
		}
		case ST_POLYGON: {
			Polygon* pPoly = static_cast<Polygon*>(m_pShape);
			for (unsigned int i = 0; i < pPoly->m_vertices.size(); ++i) {
				Vector3D pointA = pPoly->m_vertices[i] + GetPosition();
				Vector3D pointB = pPoly->m_vertices[i == pPoly->m_vertices.size() - 1 ? 0: i+1] + GetPosition();
				debugMngr.DrawLine(pointA, pointB, DebugColor::GREEN);
			}
			break;
		}
	}
}

void Body::AddForce(const Vector3D& force) { m_Forces.Add(force); }
void Body::ClearForces() { m_Forces.Zero(); }