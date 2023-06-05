#pragma once

#include "skeleton.h"

void AppInputInjectHooks();

bool AttachInputDevices();

RsEventStatus HandleKeyDown(RsKeyStatus* param);
RsEventStatus HandleKeyUp(RsKeyStatus* param);
RsEventStatus KeyboardHandler(RsEvent event, void* param);

RsEventStatus HandlePadButtonDown(RsKeyStatus* param);
RsEventStatus HandlePadButtonUp(RsKeyStatus* param);
RsEventStatus PadHandler(RsEvent event, void* param);
