#pragma once
#include "tsCommon.h"
#include "tsAdaptationField.h"
#include <string>
#include <cstdint>

/// @brief Reset - reset all TS packet header fields
void xTS_AdaptationField::Reset()
{
    m_AdaptationFieldControl = 0;
    m_AdaptationFieldLength = 0;
    m_DC = 0;
    m_RA = 0;
    m_SP = 0;
    m_PR = 0;
    m_OR = 0;
    m_SF = 0;
    m_TP = 0;
    m_EX = 0;
    m_PCR_base = 0;
    m_PCR_reserved = 0;
    m_PCR_extension = 0;
}
/**
@brief Parse adaptation field
@param PacketBuffer is pointer to buffer containing TS packet
@param AdaptationFieldControl is value of Adaptation Field Control field of
corresponding TS packet header
@return Number of parsed bytes (length of AF or -1 on failure)
*/
int32_t xTS_AdaptationField::Parse(const uint8_t* PacketBuffer, uint8_t
AdaptationFieldControl)
{
    Reset();
    if (!PacketBuffer) return NOT_VALID;
    if ((AdaptationFieldControl & 0x02) == 0) return 0;
    const uint8_t* AdaptationFieldPtr = PacketBuffer + xTS::TS_HeaderLength;
    m_AdaptationFieldControl = AdaptationFieldControl;
    m_AdaptationFieldLength = *AdaptationFieldPtr++;
    if (m_AdaptationFieldLength > 183) return NOT_VALID; // max length of AF is 183
    if (m_AdaptationFieldLength == 0) return xTS::TS_HeaderLength+1;
    m_DC = (*AdaptationFieldPtr >> 7) & 0x01; // discontinuity indicator
    m_RA = (*AdaptationFieldPtr >> 6) & 0x01; // random access indicator
    m_SP = (*AdaptationFieldPtr >> 5) & 0x01; // splicing point flag
    m_PR = (*AdaptationFieldPtr >> 4) & 0x01; // transport private data flag
    m_OR = (*AdaptationFieldPtr >> 3) & 0x01; // adaptation field extension flag
    m_SF = (*AdaptationFieldPtr >> 2) & 0x01; // stuffing length
    m_TP = (*AdaptationFieldPtr >> 1) & 0x01; // transport private data length
    m_EX = *AdaptationFieldPtr++ & 0x01; // adaptation field extension length

    if(m_PR){
        if (m_AdaptationFieldLength >= 6) { // Need at least 6 bytes for PCR
            m_PCR_base = ((uint64_t)AdaptationFieldPtr[0] << 25) |
                      ((uint64_t)AdaptationFieldPtr[1] << 17) |
                      ((uint64_t)AdaptationFieldPtr[2] << 9) |
                      ((uint64_t)AdaptationFieldPtr[3] << 1) |
                      ((AdaptationFieldPtr[4] >> 7) & 0x01);
            
            m_PCR_reserved = AdaptationFieldPtr[4] & 0x7F;
            m_PCR_extension = ((AdaptationFieldPtr[4] & 0x01) << 8 | AdaptationFieldPtr[5]);
            
            AdaptationFieldPtr += 6;
        }
    }
// Calculate remaining bytes in AF (length minus flags byte)
    int32_t remainingBytes = m_AdaptationFieldLength - 1;
    
    // Parse PCR if present
    if (m_PR && remainingBytes >= 6)
    {
        m_PCR_base = ((uint64_t)AdaptationFieldPtr[0] << 25) |
                     ((uint64_t)AdaptationFieldPtr[1] << 17) |
                     ((uint64_t)AdaptationFieldPtr[2] << 9) |
                     ((uint64_t)AdaptationFieldPtr[3] << 1) |
                     ((AdaptationFieldPtr[4] >> 7) & 0x01);
        
        m_PCR_reserved = AdaptationFieldPtr[4] & 0x7F;
        m_PCR_extension = ((uint16_t)(AdaptationFieldPtr[4] & 0x01) << 8 | AdaptationFieldPtr[5]);
        
        AdaptationFieldPtr += 6;
        remainingBytes -= 6;
    }

    // Parse OPCR if present
    if (m_OR && remainingBytes >= 6)
    {
        m_OPCR_base = ((uint64_t)AdaptationFieldPtr[0] << 25) |
                      ((uint64_t)AdaptationFieldPtr[1] << 17) |
                      ((uint64_t)AdaptationFieldPtr[2] << 9) |
                      ((uint64_t)AdaptationFieldPtr[3] << 1) |
                      ((AdaptationFieldPtr[4] >> 7) & 0x01);
        
        m_OPCR_reserved = AdaptationFieldPtr[4] & 0x7F;
        m_OPCR_extension = ((uint16_t)(AdaptationFieldPtr[4] & 0x01) << 8 | AdaptationFieldPtr[5]);
        
        AdaptationFieldPtr += 6;
        remainingBytes -= 6;
    }

    // Parse splicing point if present
    if (m_SF && remainingBytes >= 1)
    {
        // Splicing point - just skip the byte
        AdaptationFieldPtr += 1;
        remainingBytes -= 1;
    }

    // Parse transport private data if present
    if (m_TP && remainingBytes >= 1)
    {
        uint8_t privateDataLength = *AdaptationFieldPtr++;
        remainingBytes -= 1;
        
        if (privateDataLength > 0 && remainingBytes >= privateDataLength)
        {
            // Skip private data
            AdaptationFieldPtr += privateDataLength;
            remainingBytes -= privateDataLength;
        }
    }

    // Parse adaptation field extension if present
    if (m_EX && remainingBytes >= 1)
    {
        uint8_t extensionLength = *AdaptationFieldPtr++;
        remainingBytes -= 1;
        
        if (extensionLength > 0 && remainingBytes >= extensionLength)
        {
            // Skip extension
            AdaptationFieldPtr += extensionLength;
            remainingBytes -= extensionLength;
        }
    }

    // The rest is stuffing
    m_StuffingLength = remainingBytes;
    if (m_StuffingLength > 0)
    {
        // Skip stuffing bytes
        AdaptationFieldPtr += m_StuffingLength;
    }

    return m_AdaptationFieldLength + xTS::TS_HeaderLength;
}
/// @brief Print all TS packet header fields
void xTS_AdaptationField::Print() const
{
    printf("AF: L=%d DC=%d RA=%d SP=%d PR=%d OR=%d SF=%d TP=%d EX=%d",
        m_AdaptationFieldLength, m_DC, m_RA, m_SP, m_PR, m_OR, m_SF, m_TP, m_EX);
}