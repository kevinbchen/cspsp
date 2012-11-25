#ifndef _UDPMANAGER_H_
#define _UDPMANAGER_H_

#include "Wlan.h"
#include "Packet.h"
#include <vector>

enum {
	GAMEINFO = 0,
	GAMEINFOEND,
	GUNINFO,
	NEWMAP,
	NEWPLAYER,
	REMOVEPLAYER,
	CONNECT,
	SWITCHTEAM,
	PLAYERMOVE,
	RESPAWN,
	MOVE,
	SWITCHGUN,
	SPAWNGUN,
	PICKUPGUN,
	DROPGUN,
	DROPGUNDEAD,
	NEWGUN,
	MOVEGUN,
	STARTFIRE,
	ENDFIRE,
	NEWBULLET,
	NEWSHOTGUNBULLET,
	REMOVEBULLET,
	STARTRELOAD,
	ENDRELOAD,
	HIT,
	HITINDICATOR,
	DAMAGEINDICATOR,
	KILLEVENT,
	WINEVENT,
	RESETROUND,
	RESETROUNDEND,
	CHAT,
	BUY,
	ERROR1,
	PING,
	PLAYERPING,
	SERVERINFO,
	MAPFILE,
	SERVERMESSAGE,
	ACK,
	ORDEREDACK,
	NEWGRENADE,
	EXPLODEGRENADE,
	RECEIVEFLASH,
	MESSAGE,
	TIMEMULTIPLIER,
	PLAYERICON,
	RESETPLAYERS,
	CTFINFO,
	PICKUPFLAG,
	DROPFLAG,
	RETURNFLAG,
	CAPTUREFLAG,
	TIME
};

class Packet;
//------------------------------------------------------------------------------------------------
class UdpManager
{
private:
	Socket* mSocket;
	Packet mPacket;
	int ackcounter;
	int orderedackcounter;
	float sendtimer;
	int *mRoundBit;
	float mClock;
	std::vector<int> mAcks;
	std::vector<int> mOrderedAcks;

protected:

public:
	std::vector<Packet> mReliablePackets;
	std::vector<Packet> mOrderedPackets;
	std::vector<Packet> mBufferedPackets;
	int mOrderId;

	int uploadsum;
	int upload;
	float timer;

	float *mTime;
	float mSTime;
	float *mPing;

	UdpManager(Socket* socket, float *time, float *ping, int *roundbit);
	~UdpManager();
	void SetSocket(Socket* socket);
	void Send(Packet packet, bool round = true);
	void SendReliable(Packet packet, bool ordered = false, bool round = true);
	void SendAck(int ackid, bool ordered = false);
	void ReceiveAck(int id, bool ordered = false);
	bool HandleSequence(int ackid, Packet &packet, int start);
	//void SendAll();
	void Update(float dt);
	void Reset();
};

#endif
