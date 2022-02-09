/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RGBA.h"
#include "Vector.h"

enum eCheckpointType : uint8 {
    CHECKPOINT_TUBE = 0x0,
    CHECKPOINT_ENDTUBE = 0x1,
    CHECKPOINT_EMPTYTUBE = 0x2,
    CHECKPOINT_TORUS = 0x3,
    CHECKPOINT_TORUS_NOFADE = 0x4,
    CHECKPOINT_TORUSROT = 0x5,
    CHECKPOINT_TORUSTHROUGH = 0x6,
    CHECKPOINT_TORUS_UPDOWN = 0x7,
    CHECKPOINT_TORUS_DOWN = 0x8,
    CHECKPOINT_NUM = 0x9,
};

class  CCheckpoint {
public:
    eCheckpointType m_nType;
    bool            m_bIsUsed;
    bool            m_bMustBeRenderedThisFrame;
    int             m_nIdentifier;
    CRGBA           m_colour;
    int16           m_nPulsePeriod;
    int16           m_nRotateRate;
    CVector         m_vecPosition;
    CVector         m_vecDirection;
    float           m_fPulseFraction;
    float           m_fSize;
    float           m_fDistanceToPlayer; //or CameraRange
    float           m_multiSize;

public:
    static void InjectHooks();

    bool UpdateZWithWaterLevel();
    void Render();
};

VALIDATE_SIZE(CCheckpoint, 0x38);
