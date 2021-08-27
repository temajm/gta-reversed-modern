#include "StdInc.h"

void CTaskSimplePickUpEntity::InjectHooks() {
    HookInstall(0x691870, (CTaskSimplePickUpEntity * (CTaskSimplePickUpEntity::*)(CEntity*, const CVector*, uint8_t, uint8_t, CAnimBlock*, CAnimBlendHierarchy*, int, float)) & CTaskSimplePickUpEntity::Constructor);
    HookInstall(0x6917B0, (CTaskSimplePickUpEntity * (CTaskSimplePickUpEntity::*)(CEntity*, const CVector*, uint8_t, uint8_t, int, int, float)) & CTaskSimplePickUpEntity::Constructor);
    HookInstall(0x692A90, &CTaskSimplePickUpEntity::Clone_Reversed);
    HookInstall(0x691810, &CTaskSimplePickUpEntity::GetId_Reversed);
}

CTaskSimplePickUpEntity::CTaskSimplePickUpEntity(CEntity* entityToHold, CVector const* position, uint8_t boneFrameId, uint8_t boneFlags, CAnimBlock* animBlock, CAnimBlendHierarchy* animBlendHierarchy, int  animFlags, float fMovePedUntilAnimProgress) : CTaskSimpleHoldEntity(entityToHold, position, boneFrameId, boneFlags, animBlock, animBlendHierarchy, animFlags) {
    m_fMovePedUntilAnimProgress = fMovePedUntilAnimProgress;
} 

CTaskSimplePickUpEntity::CTaskSimplePickUpEntity(CEntity* entityToHold, const CVector* position, uint8_t boneFrameId, uint8_t boneFlags, int animId, int groupId, float fMovePedUntilAnimProgress) : CTaskSimpleHoldEntity(entityToHold, position, boneFrameId, boneFlags, animId, groupId, false) {
    m_fMovePedUntilAnimProgress = fMovePedUntilAnimProgress;
} 

CTaskSimplePickUpEntity::~CTaskSimplePickUpEntity() {
    // nothing here
}

// 0x691870
CTaskSimplePickUpEntity* CTaskSimplePickUpEntity::Constructor(CEntity* entityToHold, const CVector* position, uint8_t boneFrameId, uint8_t boneFlags, CAnimBlock* animBlock, CAnimBlendHierarchy* animBlendHierarchy, int  animFlags, float fMovePedUntilAnimProgress) {
    this->CTaskSimplePickUpEntity::CTaskSimplePickUpEntity(entityToHold, position, boneFrameId, boneFlags, animBlock, animBlendHierarchy, animFlags, fMovePedUntilAnimProgress);
    return this;
}

// 0x6917B0
CTaskSimplePickUpEntity* CTaskSimplePickUpEntity::Constructor(CEntity* entityToHold, const CVector* position, uint8_t boneFrameId, uint8_t boneFlags, int animId, int groupId, float fMovePedUntilAnimProgress){
    this->CTaskSimplePickUpEntity::CTaskSimplePickUpEntity(entityToHold, position, boneFrameId, boneFlags, animId, groupId, fMovePedUntilAnimProgress);
    return this;
}

// 0x692A90
CTask* CTaskSimplePickUpEntity::Clone() const {
    return CTaskSimplePickUpEntity::Clone_Reversed();
}

// 0x691810
eTaskType CTaskSimplePickUpEntity::GetId() {
    return CTaskSimplePickUpEntity::GetId_Reversed();
}

// reversed virtual functions
CTask* CTaskSimplePickUpEntity::Clone_Reversed() const {
    if (m_pAnimBlendHierarchy) {
        return new CTaskSimplePickUpEntity(
            m_pEntityToHold,
            &m_vecPosition,
            m_bBoneFrameId,
            m_bBoneFlags,
            m_pAnimBlock,
            m_pAnimBlendHierarchy,
            m_animFlags,
            m_fMovePedUntilAnimProgress
        );
    }
    else {
        return new CTaskSimplePickUpEntity(
            m_pEntityToHold,
            &m_vecPosition,
            m_bBoneFrameId,
            m_bBoneFlags,
            m_nAnimId,
            m_nAnimGroupId,
            m_fMovePedUntilAnimProgress
        );
    }
    return nullptr;
}
