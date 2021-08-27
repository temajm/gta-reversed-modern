#pragma once

#include "CTaskSimple.h"

enum eHoldEntityBoneFlags  {
    HOLD_ENTITY_FLAG_1                  = 0x1,
    HOLD_ENTITY_UPDATE_TRANSLATION_ONLY = 0x10
};

class CEntity;
class CAnimBlendAssociation;

class CTaskSimpleHoldEntity : public CTaskSimple {
public:
    CEntity* m_pEntityToHold;
    CVector m_vecPosition;
    uint8_t m_bBoneFrameId; // see ePedNode
    unsigned char m_bBoneFlags;
    bool field_1A [2];
    float m_fRotation;
    int m_nAnimId;
    int m_nAnimGroupId;
    int m_animFlags; // m_pAnimBlendAssociation flags
    class CAnimBlock* m_pAnimBlock; 
    class CAnimBlendHierarchy* m_pAnimBlendHierarchy; // If set, m_animID and m_groupID are ignored in StartAnim method
    bool m_bEntityDropped; 
    bool m_bEntityRequiresProcessing;
    bool m_bDisallowDroppingOnAnimEnd;
    bool field_37;
    CAnimBlendAssociation* m_pAnimBlendAssociation;

public:
    // TODO: AnimationId,AssocGroupId
    CTaskSimpleHoldEntity(CEntity* entityToHold, const CVector* position, uint8_t boneFrameId, uint8_t boneFlags, int animId, int groupId, bool bDisAllowDroppingOnAnimEnd);
    CTaskSimpleHoldEntity(CEntity* entityToHold, const CVector* position, uint8_t boneFrameId, uint8_t boneFlags, const char* animName, const char* animBlockName, int animFlags);
    CTaskSimpleHoldEntity(CEntity* entityToHold, const CVector* position, uint8_t boneFrameId, uint8_t boneFlags, CAnimBlock* animBlock, CAnimBlendHierarchy* animHierarchy, int animFlags);
    ~CTaskSimpleHoldEntity();

    // original virtual functions
    CTask* Clone() const override;
    eTaskType GetId() override;
    bool MakeAbortable(class CPed* ped, eAbortPriority priority, const CEvent* event) override;
    bool ProcessPed(class CPed* ped) override;
    bool SetPedPosition(class CPed* ped) override;

    void ReleaseEntity();
    bool CanThrowEntity();
    void PlayAnim(int groupId, int animId);
    static void FinishAnimHoldEntityCB(CAnimBlendAssociation* pAnimAssoc, void* pData);
    void StartAnim(CPed* pPed);
    void DropEntity(CPed* pPed, bool bAddEventSoundQuiet);
    void ChoosePutDownHeight(CPed* pPed);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimpleHoldEntity* Constructor(CEntity* entityToHold, const CVector* position, uint8_t boneFrameId, uint8_t boneFlags, int animId, int groupId, bool bDisAllowDroppingOnAnimEnd);
    CTaskSimpleHoldEntity* Constructor(CEntity* entityToHold, const CVector* position, uint8_t boneFrameId, uint8_t boneFlags, const char* animName, const char* animBlockName, int animFlags);
    CTaskSimpleHoldEntity* Constructor(CEntity* entityToHold, const CVector* position, uint8_t boneFrameId, uint8_t boneFlags, CAnimBlock* animBlock, CAnimBlendHierarchy* animHierarchy, int animFlags);

    // reversed virtual functions
    CTask* Clone_Reversed() const;
    eTaskType GetId_Reversed() { return TASK_SIMPLE_HOLD_ENTITY; };
    bool MakeAbortable_Reversed(class CPed* ped, eAbortPriority priority, const CEvent* event);
    bool ProcessPed_Reversed(class CPed* ped);
    bool SetPedPosition_Reversed(class CPed* ped);
};

VALIDATE_SIZE(CTaskSimpleHoldEntity, 0x3C);

