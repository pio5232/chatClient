#pragma once

#include "resource.h"
#include "PacketDefine.h"
#include "ChattingClient.h"
#include "ServerPacketHandler.h"
#include "UITaskManager.h"

#pragma region MakePacket
C_Network::NetworkErrorCode SendLogInPacket();
C_Network::NetworkErrorCode SendRefreshRoomInfoPacket();
C_Network::NetworkErrorCode SendMakeRoomPacket(WCHAR* roomName);
#pragma endregion
