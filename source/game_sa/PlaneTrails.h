/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "PlaneTrail.h"

static constexpr auto NUM_PLANE_TRAILS { 32u };

class CPlaneTrails {
public:
    static CPlaneTrail (&aArray)[NUM_PLANE_TRAILS];

public:
    static void InjectHooks();

    static void Init();
    static void Render();
    static void RegisterPoint(CVector point, uint32 trailIndex);
    static void Update();
};
