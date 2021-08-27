#pragma once

#include "CTaskSimpleGoTo.h"

class CTaskSimpleGoToPoint : public CTaskSimpleGoTo {
public:
    union {
        struct {
            unsigned char m_bMoveTowardsTargetPoint : 1;
            unsigned char m_b02 : 1;
            unsigned char m_b03 : 1;
            unsigned char m_b04 : 1;
            unsigned char m_b05 : 1;
        } gotoPointFlags;
        unsigned char m_GoToPointFlags;
    };
    unsigned char field_21[3]; // padding
    CVector m_vecLastPedPos;

public:
    CTaskSimpleGoToPoint(int moveState, const CVector& targetPoint, float fRadius, bool bMoveTowardsTargetPoint, bool a6);
    ~CTaskSimpleGoToPoint();

    CTask* Clone() const override;
    eTaskType GetId() override { return TASK_SIMPLE_GO_TO_POINT; }
    bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;
    bool ProcessPed(CPed* ped) override;

    // bDontCheckRadius is always false
    void UpdatePoint(const CVector& targetPosition, float fRadius, bool bDontCheckRadius);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimpleGoToPoint* Constructor(int moveState, const CVector& targetPoint, float fRadius, bool bMoveTowardsTargetPoint, bool a6);
    CTask* Clone_Reversed() const;
    bool MakeAbortable_Reversed(CPed* ped, eAbortPriority priority, const CEvent* event);
    bool ProcessPed_Reversed(CPed* ped);
};

VALIDATE_SIZE(CTaskSimpleGoToPoint, 0x30);