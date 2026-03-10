#pragma once
#include "xPES_PacketHeader.h"
#include "tsAdaptationField.h"
#include "tsTransportStream.h"
#include "tsCommon.h"
#include <cstdint>
#include <cinttypes>
#include <cstdio>


class xPES_Assembler
{
public:
enum class eResult : int32_t
{
UnexpectedPID = 1,
StreamPackedLost ,
AssemblingStarted ,
AssemblingContinue,
AssemblingFinished,
};
protected:
//setup
int32_t m_PID;
//buffer
uint8_t* m_Buffer;
uint32_t m_BufferSize;
uint32_t m_DataOffset;
//operation
int8_t m_LastContinuityCounter;
bool m_Started;
xPES_PacketHeader m_PESH;
void xBufferReset ();
void xBufferAppend(const uint8_t* Data, int32_t Size);

public:
xPES_Assembler ();
~xPES_Assembler();
void Init (int32_t PID);
eResult AbsorbPacket(const uint8_t* TransportStreamPacket, const xTS_PacketHeader* PacketHeader, const xTS_AdaptationField* AdaptationField);
void PrintPESH () const { m_PESH.Print(); }
uint8_t* getPacket () { return m_Buffer; }
int32_t getNumPacketBytes() const { return m_DataOffset; }
void Parse(const uint8_t* xTS_PacketBuffer, const xTS_PacketHeader* xTS_PacketHeader, const xTS_AdaptationField* xTS_PacketAdaptationField, FILE* File);
};