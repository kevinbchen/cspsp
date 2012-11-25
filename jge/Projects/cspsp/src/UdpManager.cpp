
#include "UdpManager.h"
#include "GameApp.h"

//------------------------------------------------------------------------------------------------
UdpManager::UdpManager(Socket* socket, float *time, float *ping, int *roundbit)
{
	mSocket = socket;
	ackcounter = 0;
	orderedackcounter = 0;
	mOrderId = 0;
	sendtimer = 0;
	upload = 0;
	uploadsum = 0;
	timer = 0;
	mRoundBit = roundbit;
	mTime = time;
	mSTime = 0.0f;
	mPing = ping;
	mClock = 0.0f;
}

//------------------------------------------------------------------------------------------------
UdpManager::~UdpManager()
{
}

//------------------------------------------------------------------------------------------------
void UdpManager::SetSocket(Socket* socket)
{
	mSocket = socket;
}


//------------------------------------------------------------------------------------------------
void UdpManager::Send(Packet packet, bool round)
{
	int roundbit = *mRoundBit;
	if (!round) roundbit ^= 128;
	packet.Data()[0] |= roundbit;
	//mPackets.push_back(packet);
	mPacket.WritePacketData(packet);
	
}

//------------------------------------------------------------------------------------------------
void UdpManager::SendReliable(Packet packet, bool ordered, bool round)
{
	int roundbit = *mRoundBit;
	if (!round) roundbit ^= 128;

	if (!ordered) {
		ackcounter++;
		if (ackcounter > 32767) {
			ackcounter = -32768;
		}
		packet.WriteId(ackcounter);
		packet.Data()[0] |= roundbit;
		packet.SetTime(mClock);
		mReliablePackets.push_back(packet);
	}
	else {
		orderedackcounter++;
		if (orderedackcounter > 32767) {
			orderedackcounter = -32768;
		}
		packet.WriteId(orderedackcounter);
		packet.Data()[0] |= roundbit;
		packet.SetTime(mClock);
		mOrderedPackets.push_back(packet);
	}
}

//------------------------------------------------------------------------------------------------
void UdpManager::SendAck(int ackid, bool ordered)
{
	Packet sendpacket = Packet();
	if (!ordered) {
		sendpacket.WriteInt8(ACK);
	}
	else {
		sendpacket.WriteInt8(ORDEREDACK);
	}
	sendpacket.WriteInt16(ackid);
	Send(sendpacket);
}

//------------------------------------------------------------------------------------------------
void UdpManager::ReceiveAck(int id, bool ordered)
{
	std::vector<Packet>* packets;
	if (!ordered) {
		packets = &mReliablePackets;
	}
	else {
		packets = &mOrderedPackets;
	}

	for (int i=0; i<packets->size(); i++) {
		if ((*packets)[i].GetId() == id) {
			packets->erase(packets->begin()+i);
			//i--;
			return;
		}
	}
}

//------------------------------------------------------------------------------------------------
bool UdpManager::HandleSequence(int ackid, Packet &packet, int start) {
	//printf("ack %d\n",ackid);
	int d = ackid-mOrderId;
	if (d < -32768) d += 65536;
	if (d > 1) {
		bool exists = false;
		for (int i=0; i<mBufferedPackets.size(); i++) {
			if (mBufferedPackets[i].GetId() == ackid) {
				exists = true;
				break;
			}
		}
		if (!exists) {
			Packet bufferedpacket = Packet();
			packet.CopyPacket(bufferedpacket,start,packet.Index());
			bufferedpacket.SetId(ackid);
			mBufferedPackets.push_back(bufferedpacket);
		}
		return false;
	}
	else if (d <= 0) {
		return false;
	}
	for (int i=0; i<mBufferedPackets.size(); i++) {
		if (mBufferedPackets[i].GetId() == ackid) {
			mBufferedPackets.erase(mBufferedPackets.begin()+i);
			break;
		}
	}
	//printf("ack %d\n",ackid);
	mOrderId = ackid;
	return true;
}

//------------------------------------------------------------------------------------------------
void UdpManager::Update(float dt)
{
	//mPing = mPing*0.9f + mSPing*0.1f;

	sendtimer += dt;
	timer += dt;
	mClock += dt;
	
	if (timer > 1000) {
		timer = 0;
		upload = uploadsum;
		//std::cout << uploadsum;
		//std::cout << "\n";
		uploadsum = 0;
	}
	if (sendtimer > 40 || dt == 0) {
		Packet packet;

		packet.WriteInt8(NETVERSION);
		packet.WriteInt8(TIME);
		packet.WriteFloat(*mTime);
		packet.WriteFloat(mSTime);

		int largerSize = mOrderedPackets.size();
		if (mReliablePackets.size() > largerSize) {
			largerSize = mReliablePackets.size();
		}
		
		for (int i=0; i<largerSize; i++) {
			if (i < mOrderedPackets.size())  {
				if (mPacket.Length()+mOrderedPackets[i].Length() < 700) {
					if (mClock-mOrderedPackets[i].GetTime() > *mPing*0.5f) {
						mPacket.WritePacketData(mOrderedPackets[i]);
						mOrderedPackets[i].SetTime(mClock);
					}
				}
				else {
					break;
				}
			}
			if (i < mReliablePackets.size()) {
				if (mPacket.Length()+mReliablePackets[i].Length() < 700) {
					if (mClock-mReliablePackets[i].GetTime() > *mPing*0.5f) {
						mPacket.WritePacketData(mReliablePackets[i]);
						mReliablePackets[i].SetTime(mClock);
					}
				}
				else {
					break;
				}
			}
		}
		packet.WritePacketData(mPacket);
		
		sendtimer = 0.0f;
		//printf("%d,%d,%d\n",mReliablePackets.size(),mOrderedPackets.size(),mOrderId);


		if (mPacket.Length() > 0) {
			uploadsum += packet.Length();
			SocketSendUdp(mSocket, packet.Data(), packet.Length());
		}
		mPacket.Clear();
	}
}

//------------------------------------------------------------------------------------------------
void UdpManager::Reset()
{
	mPacket.Clear();
	ackcounter = 0;
	orderedackcounter = 0;
	sendtimer = 0.0f;
	mReliablePackets.clear();
	mOrderedPackets.clear();
	mBufferedPackets.clear();
	mOrderId = 0;
	mSTime = 0.0f;
	mClock = 0.0f;
}

