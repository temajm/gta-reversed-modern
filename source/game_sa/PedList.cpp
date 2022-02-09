#include "StdInc.h"

#include "PedList.h"

void CPedList::InjectHooks() {
    ReversibleHooks::Install("CPedList", "Empty", 0x699DB0, &CPedList::Empty);
    ReversibleHooks::Install("CPedList", "BuildListFromGroup_NoLeader", 0x699DD0, &CPedList::BuildListFromGroup_NoLeader);
    ReversibleHooks::Install("CPedList", "ExtractPedsWithGuns", 0x69A4C0, &CPedList::ExtractPedsWithGuns);
}

// 0x699DB0
void CPedList::Empty() {
    m_nCount = 0;
    memset(m_aPeds, 0, sizeof(m_aPeds));
}

void CPedList::ClearUnused() {
    if (m_nCount < GetCapacity()) {
        memset(m_aPeds[m_nCount], 0, sizeof(m_aPeds[0]) * (GetCapacity() - m_nCount));
    }
}

uint32 CPedList::GetCapacity() const {
    return ARRAYSIZE(m_aPeds);
}

void CPedList::AddMember(CPed* ped) {
    if (m_nCount < GetCapacity())
        m_aPeds[m_nCount++] = ped;
    else {
        assert(0); // NOTSA
    }
}

void CPedList::RemoveMemberNoFill(int32 i) {
    m_aPeds[i] = nullptr;
    m_nCount--;
    // Must call FillUpHoles afterwards!
}

CPed* CPedList::Get(int i) {
    return m_aPeds[i];
}

// 0x699DD0
void CPedList::BuildListFromGroup_NoLeader(CPedGroupMembership& groupMembership) {
    m_nCount = 0;
    for (int i = 0; i < 7; i++) { // TODO: Why 7?
        if (CPed* ped = groupMembership.GetMember(i)) {
            AddMember(ped);
        }
    }
    ClearUnused();
}

void CPedList::FillUpHoles() {
    CPed** copyTo = begin();
    for (CPed** it = begin(); it != end(); it++) {
        if (copyTo != it && *it) { // NOTSA: `copyTo != it` is important to avoid UB
            *copyTo++ = *it;
        }
    }
    ClearUnused(); // Note: Original code differs a little, but does the same thing.
}

// 0x69A4C0
void CPedList::ExtractPedsWithGuns(CPedList& from) {
    for (int i = 0; i < from.m_nCount; i++) {
        if (!from.Get(i)->GetActiveWeapon().IsTypeMelee()) {
            AddMember(from.Get(i));
            from.RemoveMemberNoFill(i);
        }
    }
    from.FillUpHoles();
}
