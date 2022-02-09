/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class Checkpoint;

constexpr auto MAX_NUM_CHECKPOINTS{ 32u };

class CCheckpoints {
public:
    static uint32&     NumActiveCPts; // not used, only initialised (0)
    static CCheckpoint (&m_aCheckPtArray)[MAX_NUM_CHECKPOINTS];

public:
    static void InjectHooks();
    
    static void Init();
    static void Shutdown();
    static void UpdatePos(uint32 id, CVector& posn);
    static void SetHeading(uint32 id, float angle);
    static void Update();
    static CCheckpoint* PlaceMarker(uint32 id, uint16 type, CVector& posn, CVector& dir, float size, uint8 red, uint8 green, uint8 blue, uint8 alpha, uint16 pulsePeriod, float pulseFraction, int16 rotateRate);
    static void DeleteCP(uint32 id, uint16 type);
    static void Render();
};
