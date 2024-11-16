#pragma once

#include "resource.h"
#include "PacketDefine.h"
#include "ChattingClient.h"
#include "ServerPacketHandler.h"
#include "UITaskManager.h"

#pragma region MakePacket
C_Network::NetworkErrorCode SendLogInPacket();
#pragma endregion
