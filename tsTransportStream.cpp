#include "tsTransportStream.h"

//=============================================================================================================================================================================
// xTS_PacketHeader
//=============================================================================================================================================================================


/// @brief Reset - reset all TS packet header fields
void xTS_PacketHeader::Reset()
{
  m_SB = 0;
  m_E = 0;
  m_S = 0;
  m_T = 0;
  m_PID = 0;
  m_TSC = 0;
  m_AFC = 0;
  m_CC = 0;
}

/**
  @brief Parse all TS packet header fields
  @param Input is pointer to buffer containing TS packet
  @return Number of parsed bytes (4 on success, -1 on failure) 
 */
int32_t xTS_PacketHeader::Parse(const uint8_t* Input)
{
  if (!Input) return NOT_VALID;
  //i`m empty
  const uint32_t* HearPtr = (const uint32_t*)Input;
  uint32_t Head = xSwapBytes32(*HearPtr);
  m_SB = (Head >> 24) & 0xFF;
  m_E  = (Head >> 23) & 0x01;
  m_S  = (Head >> 22) & 0x01;
  m_T  = (Head >> 21) & 0x01;
  m_PID = (Head >> 8) & 0x1FFF;
  m_TSC = (Head >> 6) & 0x03;
  m_AFC = (Head >> 4) & 0x03;
  m_CC  =  Head       & 0x0F;
  return xTS::TS_HeaderLength;
}

/// @brief Print all TS packet header fields
void xTS_PacketHeader::Print() const
{
  printf("TS: SB=%d E=%d S=%d P=%d PID=%4d TSC=%d AF=%d CC=%2d ", 
    m_SB, m_E, m_S, m_T, m_PID, m_TSC, m_AFC, m_CC);
}

//=============================================================================================================================================================================
