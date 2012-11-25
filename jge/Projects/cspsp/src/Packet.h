#ifndef _PACKET_H_
#define _PACKET_H_

#include <vector>
#include <string.h>
#include <stdio.h>

class Packet
{
private:
	std::vector<char> data;
	int index;
	int id;
	float time;

protected:

public:

	Packet();
	Packet(char* buffer, int size);
	~Packet();

	char* Data();
	int Length();
	int Index();

	void WriteId(int value);
	void SetId(int value);
	int GetId();
	//template<typename T> void Write(T const &t);
	//template<typename T> T Read();

	void WriteInt8(char value);
	char ReadInt8();

	void WriteInt16(short int value);
	short int ReadInt16();

	void WriteInt32(int value);
	int ReadInt32();

	void WriteFloat(float value);
	float ReadFloat();

	void WriteChar(char* value);
	int ReadChar(char *buffer, int buffersize);

	void WriteData(char* value, int length);
	int ReadData(char* buffer);

	void WritePacketData(Packet &packet);

	void Clear();

	void CopyPacket(Packet &packet, int start, int end);

	void SetTime(float value);
	float GetTime();

};

#endif

