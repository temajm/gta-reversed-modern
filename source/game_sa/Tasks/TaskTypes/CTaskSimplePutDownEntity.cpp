#include "StdInc.h"

#include "CTaskSimplePutDownEntity.h"

void CTaskSimplePutDownEntity::InjectHooks() {
    HookInstall(0x691990, &CTaskSimplePutDownEntity::Constructor);
    HookInstall(0x692B70, &CTaskSimplePutDownEntity::Clone_Reversed);
    HookInstall(0x691900, &CTaskSimplePutDownEntity::GetId_Reversed);
}

CTaskSimplePutDownEntity::CTaskSimplePutDownEntity() : CTaskSimpleHoldEntity(nullptr, nullptr, PED_NODE_RIGHT_HAND, HOLD_ENTITY_FLAG_1, ANIM_ID_NO_ANIMATION_SET, 0, false) {
    m_fPutDownHeightZ = 0.6f;
}

CTaskSimplePutDownEntity::CTaskSimplePutDownEntity(CEntity* entityToHold, const CVector& position, uint8_t boneFrameId, uint8_t boneFlags, int animId, int groupId, bool bDisAllowDroppingOnAnimEnd, float fPutDownHeightZ) : CTaskSimpleHoldEntity(entityToHold, &position, boneFrameId, boneFlags, animId, groupId, bDisAllowDroppingOnAnimEnd) {
    m_fPutDownHeightZ = fPutDownHeightZ;
}

CTaskSimplePutDownEntity::CTaskSimplePutDownEntity(CEntity* entityToHold, const CVector& position, uint8_t boneFrameId, uint8_t boneFlags, const char* animName, const char* animBlockName, int animFlags, float fPutDownHeightZ) : CTaskSimpleHoldEntity(entityToHold, &position, boneFrameId, boneFlags, animName, animBlockName, animFlags) {
    m_fPutDownHeightZ = fPutDownHeightZ;
}

CTaskSimplePutDownEntity::CTaskSimplePutDownEntity(CEntity* entityToHold, const CVector& position, uint8_t boneFrameId, uint8_t boneFlags, CAnimBlock* animBlock, CAnimBlendHierarchy* animHierarchy, int animFlags, float fPutDownHeightZ) : CTaskSimpleHoldEntity(entityToHold, &position, boneFrameId, boneFlags, animBlock, animHierarchy, animFlags) {
    m_fPutDownHeightZ = fPutDownHeightZ;
}

CTaskSimplePutDownEntity::~CTaskSimplePutDownEntity() {
    // nothing here
}

// 0x691990
CTaskSimplePutDownEntity* CTaskSimplePutDownEntity::Constructor() {
    this->CTaskSimplePutDownEntity::CTaskSimplePutDownEntity();
    return this;
}

// 0x692B70
CTask* CTaskSimplePutDownEntity::Clone() const {
    return CTaskSimplePutDownEntity::Clone_Reversed();
}

// 0x691900
eTaskType CTaskSimplePutDownEntity::GetId() {
    return CTaskSimplePutDownEntity::GetId_Reversed();
}

CTask* CTaskSimplePutDownEntity::Clone_Reversed() const {
    if (m_pAnimBlendHierarchy) {
        return new CTaskSimplePutDownEntity(
            m_pEntityToHold,
            m_vecPosition,
            m_bBoneFrameId,
            m_bBoneFlags,
            m_pAnimBlock,
            m_pAnimBlendHierarchy,
            m_animFlags,
            m_fPutDownHeightZ
        );
    }
    else {
        return new CTaskSimplePutDownEntity(
            m_pEntityToHold,
            m_vecPosition,
            m_bBoneFrameId,
            m_bBoneFlags,
            m_nAnimId,
            m_nAnimGroupId,
            false,
            m_fPutDownHeightZ
        );
    }
}

