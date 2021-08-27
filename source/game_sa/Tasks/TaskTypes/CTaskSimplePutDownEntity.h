#pragma once

#include "CTaskSimpleHoldEntity.h"

class CTaskSimplePutDownEntity : public CTaskSimpleHoldEntity {
public:
    float m_fPutDownHeightZ;

public:
    CTaskSimplePutDownEntity();
    CTaskSimplePutDownEntity(CEntity* entityToHold, const CVector& position, uint8_t boneFrameId, uint8_t boneFlags, const char* animName, const char* animBlockName, int animFlags, float fPutDownHeightZ);
    CTaskSimplePutDownEntity(CEntity* entityToHold, const CVector& position, uint8_t boneFrameId, uint8_t boneFlags, CAnimBlock* animBlock, CAnimBlendHierarchy* animHierarchy, int animFlags, float fPutDownHeightZ);
    CTaskSimplePutDownEntity(CEntity* entityToHold, const CVector& position, uint8_t boneFrameId, uint8_t boneFlags, int animId, int groupId, bool bDisAllowDroppingOnAnimEnd, float fPutDownHeightZ);
    // TODO: int animId, int groupId -> AnimationId, AssocGroupId
    ~CTaskSimplePutDownEntity();

public:
    CTask* Clone() const override;
    eTaskType GetId() override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimplePutDownEntity* Constructor();

    CTask* Clone_Reversed() const;
    eTaskType GetId_Reversed() { return TASK_SIMPLE_PUTDOWN_ENTITY; };
};

VALIDATE_SIZE(CTaskSimplePutDownEntity, 0x40);