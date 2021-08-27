/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "CTaskSimple.h"
#include "CTaskTimer.h"

class CTaskSimpleStandStill : public CTaskSimple {
public:
    int m_nTime;
    CTaskTimer m_timer;
    bool m_bLooped;
    bool m_bUseAnimIdleStance;
    char _pad[2];
    float m_fBlendData;

public:
    CTaskSimpleStandStill(int nTime, bool Looped, bool bUseAnimIdleStance, float fBlendData);
    ~CTaskSimpleStandStill();

    CTask* Clone() const override;
    eTaskType GetId() override { return TASK_SIMPLE_STAND_STILL; };
    bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;
    bool ProcessPed(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimpleStandStill* Constructor(int nTime, bool Looped, bool bUseAnimIdleStance, float fBlendData);

    CTask* Clone_Reversed() const;
    bool MakeAbortable_Reversed(CPed* ped, eAbortPriority priority, const CEvent* event);
    bool ProcessPed_Reversed(CPed* ped);
};

VALIDATE_SIZE(CTaskSimpleStandStill, 0x20);