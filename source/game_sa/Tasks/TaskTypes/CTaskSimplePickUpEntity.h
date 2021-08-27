#pragma once

#include "CTaskSimpleHoldEntity.h"

class CTaskSimplePickUpEntity : public CTaskSimpleHoldEntity {
public:
    float m_fMovePedUntilAnimProgress; // value can be 0.0 to 1.0. Ped Will stop moving when the current `m_fCurrentTime`
                                       // of `m_pAnimBlendAssociation` is equal or greater than this.
    CVector m_vecPickupPosition;

    CTaskSimplePickUpEntity(CEntity* entityToHold, const CVector* position, uint8_t boneFrameId, uint8_t boneFlags, CAnimBlock* animBlock, CAnimBlendHierarchy* animBlendHierarchy, int animFlags, float fMovePedUntilAnimProgress);
    CTaskSimplePickUpEntity(CEntity* entityToHold, const CVector* position, uint8_t boneFrameId, uint8_t boneFlags, int animId, int groupId, float fMovePedUntilAnimProgress);
    // TODO: AnimationId, AssocGroupId
    ~CTaskSimplePickUpEntity();

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimplePickUpEntity* Constructor(CEntity* entityToHold, const CVector* position, uint8_t boneFrameId, uint8_t boneFlags, CAnimBlock* animBlock, CAnimBlendHierarchy* animBlendHierarchy, int  animFlags, float fMovePedUntilAnimProgress);
    CTaskSimplePickUpEntity* Constructor(CEntity* entityToHold, const CVector* position, uint8_t boneFrameId, uint8_t boneFlags, int animId, int groupId, float fMovePedUntilAnimProgress);

public:
    CTask* Clone() const override;
    eTaskType GetId() override;

    // reversed virtual functions
    CTask* Clone_Reversed() const;
    eTaskType GetId_Reversed() { return TASK_SIMPLE_PICKUP_ENTITY; };
};

VALIDATE_SIZE(CTaskSimplePickUpEntity, 0x4C);