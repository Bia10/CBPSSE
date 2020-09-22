#include "pch.h"

namespace CBP
{
    __forceinline static float mmg(float a_val, float a_min, float a_max) {
        return a_min + (a_max - a_min) * a_val;
    }

    __forceinline static float mmw(float a_val, float a_min, float a_max) {
        return mmg(a_val / 100.0f, a_min, a_max);
    }

    SimComponent::Collider::Collider(
        SimComponent& a_parent)
        :
        m_created(false),
        m_active(true),
        m_process(true),
        m_nodeScale(1.0f),
        m_radius(1.0f),
        m_height(0.001f),
        m_parent(a_parent),
        m_shape(ColliderShape::Sphere)
    {}

    bool SimComponent::Collider::Create(ColliderShape a_shape)
    {
        if (m_created)
        {
            if (m_shape == a_shape)
                return false;

            Destroy();
        }

        m_nodeScale = 1.0f;
        m_radius = 1.0f;
        m_height = 0.001f;
        m_transform.setToIdentity();
        SetColliderRotation(0.0f, 0.0f, 0.0f);

        auto world = DCBP::GetWorld();
        auto& physicsCommon = DCBP::GetPhysicsCommon();

        m_body = world->createCollisionBody(m_transform);

        if (a_shape == ColliderShape::Capsule) {
            m_capsuleShape = physicsCommon.createCapsuleShape(m_radius, m_height);
            m_collider = m_body->addCollider(m_capsuleShape, m_transform);
        }
        else {
            m_sphereShape = physicsCommon.createSphereShape(m_radius);
            m_collider = m_body->addCollider(m_sphereShape, m_transform);
        }

        m_collider->setUserData(std::addressof(m_parent));
        m_body->setIsActive(m_process);

        m_created = true;
        m_active = true;
        m_shape = a_shape;

        return true;
    }

    bool SimComponent::Collider::Destroy()
    {
        if (!m_created)
            return false;

        auto world = DCBP::GetWorld();
        auto& physicsCommon = DCBP::GetPhysicsCommon();

        m_body->removeCollider(m_collider);

        if (m_shape == ColliderShape::Capsule)
            physicsCommon.destroyCapsuleShape(m_capsuleShape);
        else
            physicsCommon.destroySphereShape(m_sphereShape);

        world->destroyCollisionBody(m_body);

        m_created = false;

        return true;
    }

    void SimComponent::Collider::Update()
    {
        if (!m_created)
            return;

        auto nodeScale = m_parent.m_obj->m_worldTransform.scale;

        if (!m_active) {
            if (nodeScale > 0.0f)
            {
                m_active = true;
                m_body->setIsActive(m_process);
            }
            else
                return;
        }
        else {
            if (nodeScale <= 0.0f)
            {
                m_active = false;
                m_body->setIsActive(false);
                return;
            }
        }

        auto pos = m_parent.m_obj->m_worldTransform * m_bodyOffset;

        if (m_shape == ColliderShape::Capsule)
        {
            auto& mat = m_parent.m_obj->m_worldTransform.rot;

            r3d::Quaternion quat(r3d::Matrix3x3(
                mat.arr[0], mat.arr[1], mat.arr[2],
                mat.arr[3], mat.arr[4], mat.arr[5],
                mat.arr[6], mat.arr[7], mat.arr[8])
            );

            m_body->setTransform(r3d::Transform(
                r3d::Vector3(pos.x, pos.y, pos.z),
                quat * m_colRot));
        }
        else
        {
            m_transform.setPosition(r3d::Vector3(pos.x, pos.y, pos.z));
            m_body->setTransform(m_transform);
        }

        if (nodeScale != m_nodeScale) {
            m_nodeScale = nodeScale;
            UpdateRadius();
            UpdateHeight();
        }
    }

    void SimComponent::Collider::Reset()
    {
        if (m_created) {
            m_body->setTransform(r3d::Transform::identity());
        }
    }

    SimComponent::SimComponent(
        Actor* a_actor,
        NiAVObject* a_obj,
        const std::string& a_configGroupName,
        const configComponent_t& a_config,
        uint64_t a_parentId,
        uint64_t a_groupId,
        bool a_collisions,
        bool a_movement,
        const configNode_t& a_nodeConf)
        :
        m_configGroupName(a_configGroupName),
        m_oldWorldPos(a_obj->m_worldTransform.pos),
        m_initialNodePos(a_obj->m_localTransform.pos),
        m_initialNodeRot(a_obj->m_localTransform.rot),
        m_collisionData(*this),
        m_parentId(a_parentId),
        m_groupId(a_groupId),
        m_resistanceOn(false),
        m_rotScaleOn(false),
        m_obj(a_obj),
        m_objParent(a_obj->m_parent),
        m_updateCtx({ 0.0f, 0 })
    {
#ifdef _CBP_ENABLE_DEBUG
        m_debugInfo.parentNodeName = a_obj->m_parent->m_name;
#endif
        UpdateConfig(a_actor, a_config, a_collisions, a_movement, a_nodeConf);
        m_collisionData.Update();
    }

    void SimComponent::Release()
    {
        m_collisionData.Destroy();
    }

    bool SimComponent::UpdateWeightData(
        Actor* a_actor,
        const configComponent_t& a_config,
        const configNode_t& a_nodeConf)
    {
        if (a_actor == nullptr)
            return false;

        auto npc = DYNAMIC_CAST(a_actor->baseForm, TESForm, TESNPC);
        if (npc == nullptr)
            return false;

        float weight = std::clamp(npc->weight, 0.0f, 100.0f);

        m_colRad = std::max(mmw(weight, a_config.phys.colSphereRadMin, a_config.phys.colSphereRadMax), 0.0f);
        m_colHeight = std::max(mmw(weight, a_config.phys.colHeightMin, a_config.phys.colHeightMax), 0.001f);
        m_colOffsetX = mmw(weight,
            a_config.phys.offsetMin[0] + a_nodeConf.colOffsetMin[0],
            a_config.phys.offsetMax[0] + a_nodeConf.colOffsetMax[0]);
        m_colOffsetY = mmw(weight,
            a_config.phys.offsetMin[1] + a_nodeConf.colOffsetMin[1],
            a_config.phys.offsetMax[1] + a_nodeConf.colOffsetMax[1]);
        m_colOffsetZ = mmw(weight,
            a_config.phys.offsetMin[2] + a_nodeConf.colOffsetMin[2],
            a_config.phys.offsetMax[2] + a_nodeConf.colOffsetMax[2]);

        return true;
    }

    void SimComponent::UpdateConfig(
        Actor* a_actor,
        const configComponent_t& a_config,
        bool a_collisions,
        bool a_movement,
        const configNode_t& a_nodeConf) noexcept
    {
        m_conf = a_config;
        m_collisions = a_collisions;

        if (a_movement != m_movement) {
            m_movement = a_movement;
            m_applyForceQueue.swap(decltype(m_applyForceQueue)());
        }

        if (!UpdateWeightData(a_actor, a_config, a_nodeConf)) {
            m_colRad = a_config.phys.colSphereRadMax;
            m_colHeight = std::max(a_config.phys.colHeightMax, 0.001f);
            m_colOffsetX = a_config.phys.offsetMax[0] + a_nodeConf.colOffsetMax[0];
            m_colOffsetY = a_config.phys.offsetMax[1] + a_nodeConf.colOffsetMax[1];
            m_colOffsetZ = a_config.phys.offsetMax[2] + a_nodeConf.colOffsetMax[2];
        }

        if (m_collisions &&
            m_colRad > 0.0f)
        {
            m_collisionData.Create(m_conf.ex.colShape);

            m_collisionData.SetRadius(m_colRad);
            m_collisionData.SetOffset(
                m_colOffsetX,
                m_colOffsetY,
                m_colOffsetZ
            );

            if (m_collisionData.GetColliderShape() == ColliderShape::Capsule)
            {
                m_collisionData.SetHeight(m_colHeight);
                m_collisionData.SetColliderRotation(
                    m_conf.phys.colRot[0],
                    m_conf.phys.colRot[1],
                    m_conf.phys.colRot[2]
                );
            }
        }
        else {
            m_collisionData.Destroy();
        }

        m_npCogOffset = NiPoint3(m_conf.phys.cogOffset[0], m_conf.phys.cogOffset[1], m_conf.phys.cogOffset[2]);
        m_npGravityCorrection = NiPoint3(0.0f, 0.0f, m_conf.phys.gravityCorrection);

        if (m_conf.phys.resistance > 0.0f) {
            m_resistanceOn = true;
            m_resistance = std::clamp(m_conf.phys.resistance, 0.0f, 100.0f);
        }
        else
            m_resistanceOn = false;

        bool rot =
            m_conf.phys.rotational[0] != 0.0f ||
            m_conf.phys.rotational[1] != 0.0f ||
            m_conf.phys.rotational[2] != 0.0f;

        if (rot != m_rotScaleOn) {
            m_rotScaleOn = rot;
            m_obj->m_localTransform.rot = m_initialNodeRot;
        }

        m_conf.phys.colRestitutionCoefficient = mmg(std::clamp(m_conf.phys.colRestitutionCoefficient, 0.0f, 1.0f), 0.0f, 1.0f);

        if (m_movement)
            m_conf.phys.mass = std::clamp(m_conf.phys.mass, 1.0f, 1000.0f);
        else
            m_conf.phys.mass = 10000.0f;

        m_conf.phys.colPenMass = std::clamp(m_conf.phys.colPenMass, 1.0f, 100.0f);
    }

    void SimComponent::Reset()
    {
        if (m_movement)
        {
            m_obj->m_localTransform.pos = m_initialNodePos;
            m_obj->m_localTransform.rot = m_initialNodeRot;
            m_obj->UpdateWorldData(&m_updateCtx);

            m_oldWorldPos = m_obj->m_worldTransform.pos;
        }

        m_collisionData.Update();

        m_velocity = NiPoint3();

        m_applyForceQueue.swap(decltype(m_applyForceQueue)());
    }

    void SimComponent::UpdateMovement(float a_timeStep)
    {
        if (m_movement)
        {
            //Offset to move Center of Mass make rotational motion more significant  
            NiPoint3 target(m_objParent->m_worldTransform * m_npCogOffset);

            NiPoint3 diff(target - m_oldWorldPos);

            float ax = std::fabs(diff.x);
            float ay = std::fabs(diff.y);
            float az = std::fabs(diff.z);

            if (ax > 512.0f || ay > 512.0f || az > 512.0f) {
                Reset();
                return;
            }

            // Compute the "Spring" Force
            NiPoint3 diff2(diff.x * ax, diff.y * ay, diff.z * az);
            NiPoint3 force = (diff * m_conf.phys.stiffness) + (diff2 * m_conf.phys.stiffness2);

            force.z -= m_conf.phys.gravityBias * m_conf.phys.mass;

            if (!m_applyForceQueue.empty())
            {
                auto& current = m_applyForceQueue.front();

                float forceMag = current.force.Length();
                auto vDir = m_objParent->m_worldTransform.rot * current.force;
                vDir.Normalize();

                force += (vDir * forceMag) / a_timeStep;

                if (!current.steps--)
                    m_applyForceQueue.pop();
            }

            float res = m_resistanceOn ?
                (1.0f - 1.0f / ((m_velocity.Length() * 0.025f) + 1.0f)) * m_resistance : 0.0f;

            SetVelocity((m_velocity + ((force / m_conf.phys.mass) * a_timeStep)) -
                (m_velocity * ((m_conf.phys.damping * a_timeStep) * (1.0f + res))));

            auto invRot = m_objParent->m_worldTransform.rot.Transpose();

            auto newPos = m_oldWorldPos + m_velocity * a_timeStep;
            auto ldiff = invRot * (newPos - target);

            ldiff.x = std::clamp(ldiff.x, -m_conf.phys.maxOffset[0], m_conf.phys.maxOffset[0]);
            ldiff.y = std::clamp(ldiff.y, -m_conf.phys.maxOffset[1], m_conf.phys.maxOffset[1]);
            ldiff.z = std::clamp(ldiff.z, -m_conf.phys.maxOffset[2], m_conf.phys.maxOffset[2]);

            m_oldWorldPos = (m_objParent->m_worldTransform.rot * ldiff) + target;

            m_obj->m_localTransform.pos.x = m_initialNodePos.x + (ldiff.x * m_conf.phys.linear[0]);
            m_obj->m_localTransform.pos.y = m_initialNodePos.y + (ldiff.y * m_conf.phys.linear[1]);
            m_obj->m_localTransform.pos.z = m_initialNodePos.z + (ldiff.z * m_conf.phys.linear[2]);

            m_obj->m_localTransform.pos += invRot * m_npGravityCorrection;

            if (m_rotScaleOn)
                m_obj->m_localTransform.rot.SetEulerAngles(
                    ldiff.x * m_conf.phys.rotational[0],
                    ldiff.y * m_conf.phys.rotational[1],
                    (ldiff.z + m_conf.phys.rotGravityCorrection) * m_conf.phys.rotational[2]);

            m_obj->UpdateWorldData(&m_updateCtx);
        }

        m_collisionData.Update();
    }

    void SimComponent::UpdateVelocity()
    {
        if (m_movement)
            return;

        auto newPos = m_obj->m_worldTransform.pos;
        m_velocity = newPos - m_oldWorldPos;
        m_oldWorldPos = newPos;
    }

    void SimComponent::ApplyForce(uint32_t a_steps, const NiPoint3& a_force)
    {
        if (!a_steps || !m_movement)
            return;

        /*if (m_applyForceQueue.size() > 100)
            return;*/

        m_applyForceQueue.emplace(
            Force{ a_steps, a_force }
        );
    }

#ifdef _CBP_ENABLE_DEBUG
    void SimComponent::UpdateDebugInfo()
    {
        m_debugInfo.worldTransform = m_obj->m_worldTransform;
        m_debugInfo.localTransform = m_obj->m_localTransform;

        m_debugInfo.worldTransformParent = m_objParent->m_worldTransform;
        m_debugInfo.localTransformParent = m_objParent->m_localTransform;
    }
#endif
}