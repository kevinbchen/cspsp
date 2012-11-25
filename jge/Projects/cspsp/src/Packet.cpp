#include "Packet.h"

Packet::Packet()
{
	index = 0;
	id = -1;
	//data.resize(1);
}

Packet::Packet(char* buffer, int size)
{
	index = 0;
	id = -1;
	data.resize(size);
	memcpy(&data[0], buffer, size);
}

Packet::~Packet()
{
	data.clear();
}

char* Packet::Data() {
	return &data[0];
}

int Packet::Index() {
	return index;
}

void Packet::WriteId(int value) {
	WriteInt16(value);
	id = value;
}
void Packet::SetId(int value) {
	id = value;
}
int Packet::GetId() {
	return id;
}

int Packet::Length() {
	return data.size();
}

char Packet::ReadInt8() {
	char value = 0;
	if (index >= data.size()) {
		printf("Invalid ReadInt8");
		return 0;
	}
    memcpy(&value, &data[index], sizeof(char));
    index += sizeof(char);	
	return value;
}

void Packet::WriteInt8(char value) {
	data.resize(data.size() + sizeof(char));
	memcpy(&data[data.size() - sizeof(char)], &value, sizeof(char));
}

short int Packet::ReadInt16() {
	short int value = 0;
	if (index >= data.size()) {
		printf("Invalid ReadInt16");
		return 0;
	}
    memcpy(&value, &data[index], sizeof(short int));
    index += sizeof(short int);	
	return value;
}

void Packet::WriteInt16(short int value) {
	data.resize(data.size() + sizeof(short int));
	memcpy(&data[data.size() - sizeof(short int)], &value, sizeof(short int));
}

int Packet::ReadInt32() {
	int value = 0;
	if (index >= data.size()) {
		printf("Invalid ReadInt32");
		return 0;
	}
    memcpy(&value, &data[index], sizeof(int));
    index += sizeof(int);	
	return value;
}

void Packet::WriteInt32(int value) {
	data.resize(data.size() + sizeof(int));
	memcpy(&data[data.size() - sizeof(int)], &value, sizeof(int));
}

float Packet::ReadFloat() {
	float value = 0.0f;
	if (index >= data.size()) {
		printf("Invalid ReadFloat");
		return 0.0f;
	}
    memcpy(&value, &data[index], sizeof(float));
    index += sizeof(float);	
	return value;
}

void Packet::WriteFloat(float value) {
	data.resize(data.size() + sizeof(float));
	memcpy(&data[data.size() - sizeof(float)], &value, sizeof(float));
}

int Packet::ReadChar(char* buffer, int buffersize) {
	int length = ReadInt8();
	if (length <= 0) {
		strcpy(buffer, "");
		return length;
	}

	if (index+length > data.size()) { //uhoh
		strcpy(buffer, "error");
		return length;
	}

	strcpy(buffer,"");
	int i = 0;
	while (i < length) {
		if (i >= buffersize-1) break;

		char temp = data[index+i];
		if (temp != '\0' && (temp < 32 || temp > 128)) temp = 32;

		buffer[i] = temp;
		i++;
	}
	buffer[i] = '\0';

    //memcpy(&buffer, &data[index], length);
	//buffer[length] = 0; //terminate string
    index += length;	
	return length;
}

void Packet::WriteChar(char* value) {
	int len = strlen(value);
	if (len > 127) len = 127;

	WriteInt8(len);

	data.resize(data.size() + len);

	for(int i=0; i!=len; i++) {
		char temp = value[i];
		if (temp != '\0' && (temp < 32 || temp > 128)) temp = 32;

		data[data.size() - len + i] = temp;
		//memcpy(&data[data.size() - strlen(value) + i], &value[i], sizeof(char));
	}
}

int Packet::ReadData(char* buffer) {
	int length = ReadInt16();

	if (index+length > data.size()) { //uhoh
		return 0;
	}
	//char buffer[1024];

	for (int i=0; i<length; i++) {
		buffer[i] = data[index+i];
	}
    //memcpy(&buffer, &data[index], length);
	//buffer[length] = 0; //terminate string
    index += length;	

	//char* value = new char[length];
	//strcpy(value, (char*)buffer);

	//return value;
	return length;
}

void Packet::WriteData(char* value, int length) {
	int len = length; //strlen(value)+1;

	WriteInt16(len);

	data.resize(data.size() + len);

	for(int i=0; i!=len; i++) {
		data[data.size() - len + i] = value[i];
		//memcpy(&data[data.size() - strlen(value) + i], &value[i], sizeof(char));
	}
}

void Packet::WritePacketData(Packet &packet) {
	if (packet.Length() == 0) return;
	data.resize(data.size() + packet.Length());
	memcpy(&data[data.size() - packet.Length()], packet.Data(), packet.Length());
}

void Packet::Clear() {
	//free(data
	data.clear();
	data.resize(0);
	index = 0;
}

void Packet::CopyPacket(Packet &packet, int start, int end) {
	int len = end-start;
	packet.data.resize(len);
	memcpy(&(packet.data[0]),&data[start],len);
}

void Packet::SetTime(float value) {
	time = value;
}

float Packet::GetTime() {
	return time;
}
