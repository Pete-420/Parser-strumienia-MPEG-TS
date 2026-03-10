#pragma once
#include "tsCommon.h"
#include "tsTransportStream.h"
#include <string>
#include <cstdint>

class xTS_AdaptationField
{
protected:
//setup
uint8_t m_AdaptationFieldControl;
//mandatory fields
uint8_t m_AdaptationFieldLength;
uint8_t m_DC; // discontinuity indicator
uint8_t m_RA; // random access indicator
uint8_t m_SP; // elementary stream priority indicator
uint8_t m_PR; // program clock reference flag
uint8_t m_OR; // original program clock reference flag
uint8_t m_SF; // splicing point flag
uint8_t m_TP; // transport private data flag
uint8_t m_EX; // adaptation field extension flag


uint64_t m_PCR_base;
uint8_t m_PCR_reserved;
uint32_t m_PCR_extension;
//optional fields - PCR
uint64_t m_OPCR_base;
uint8_t m_OPCR_reserved;
uint16_t m_OPCR_extension;
//optional fields - OPCR    

uint8_t m_StuffingLength;
public:
void Reset();
int32_t Parse(const uint8_t* PacketBuffer, uint8_t AdaptationFieldControl);
void Print() const;
public:
//mandatory fields
uint8_t getAdaptationFieldLength () const { return
m_AdaptationFieldLength ; }
//derived values
uint32_t getNumBytes () const { return m_AdaptationFieldLength + 1; }
uint8_t getAdaptationFieldControl () const { return m_AdaptationFieldControl; }
uint8_t getDC () const { return m_DC; }
uint8_t getRA () const { return m_RA; }
uint8_t getSP () const { return m_SP; }
uint8_t getPR () const { return m_PR; }
uint8_t getOR () const { return m_OR; }
uint8_t getSF () const { return m_SF; }
uint8_t getTP () const { return m_TP; }
uint8_t getEX () const { return m_EX; }
uint64_t getPCR_base () const { return m_PCR_base; }
uint8_t getPCR_reserved () const { return m_PCR_reserved; }
uint32_t getPCR_extension () const { return m_PCR_extension; }
uint64_t getOPCR_base () const { return m_OPCR_base; }
uint8_t getOPCR_reserved () const { return m_OPCR_reserved; }
uint16_t getOPCR_extension () const { return m_OPCR_extension; }
uint8_t getStuffingLength () const { return m_StuffingLength; }
};