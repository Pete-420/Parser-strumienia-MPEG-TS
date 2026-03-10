#pragma once
#include "xPES_PacketHeader.h"
#include "tsCommon.h"

void xPES_PacketHeader::Reset()
{
  m_PacketStartCodePrefix = 0;
  m_StreamId = 0;
  m_PacketLength = 0;
  m_HeaderLength = 0;
}


int32_t xPES_PacketHeader::Parse(const uint8_t* Input) {
  if (!Input) return NOT_VALID;

  // Łączenie 3 bajtów w 24-bitowy Packet Start Code Prefix
  m_PacketStartCodePrefix = (static_cast<uint32_t>(Input[0]) << 16) |
                            (static_cast<uint32_t>(Input[1]) << 8) |
                            static_cast<uint32_t>(Input[2]);

  if (!(Input[0] == 0x00 && Input[1] == 0x00 && Input[2] == 0x01)) {
      // Niepoprawny nagłówek PES, nie parsuj!
      return NOT_VALID;
  }


  // Odczyt Stream ID (typ strumienia: audio/wideo/etc.)
  m_StreamId = Input[3];

  // Odczyt długości pakietu (16 bitów)
  m_PacketLength = (static_cast<uint16_t>(Input[4]) << 8) |
                    static_cast<uint16_t>(Input[5]);

  // Podstawowa długość nagłówka (do bajtu 5 włącznie)
  m_HeaderLength = 6;

  // Sprawdzamy, czy strumień to Private 1 / Audio / Video
  if (m_StreamId == 0xBD || (m_StreamId >= 0xC0 && m_StreamId <= 0xEF)) {
      m_HeaderLength = 9; // Minimum dla rozszerzonego nagłówka PES
       


      if (m_HeaderLength > m_PacketLength) {
            return NOT_VALID; // Zabezpieczenie przed przekroczeniem bufora
      }

      uint8_t flags = Input[7]; // Flagi z 8. bajtu (offset 7)

      // Sprawdzamy, czy PTS i DTS są obecne
      if ((flags & 0xC0) == 0xC0) {
          m_HeaderLength += 10; // PTS + DTS
      } else if ((flags & 0x80) == 0x80) {
          m_HeaderLength += 5;  // tylko PTS
      }

      // Kolejne rozszerzenia (jeśli dana flaga jest ustawiona – zwiększamy HeaderLength)

      if (flags & 0x20) m_HeaderLength += 6;  // ESCR
      if (flags & 0x10) m_HeaderLength += 3;  // ES rate
      if (flags & 0x08) m_HeaderLength += 0;  // DSM trick mode (pomijany)
      if (flags & 0x04) m_HeaderLength += 1;  // additional copy info
      if (flags & 0x02) m_HeaderLength += 2;  // PES CRC

      // Flaga rozszerzenia
      if (flags & 0x01) {
          if (m_HeaderLength >= m_PacketLength) {
                return NOT_VALID; // Zabezpieczenie przed odczytem poza buforem
          }
          int ext_start = m_HeaderLength;    // miejsce, gdzie zaczynają się flagi rozszerzeń
          m_HeaderLength += 1;               // przeskok do bajtu z dodatkowymi flagami

          uint8_t ext_flags = Input[ext_start];

          if (ext_flags & 0x80) m_HeaderLength += 16; // PES_private_data
          if (ext_flags & 0x40) m_HeaderLength += 1;  // pack_header_field
          if (ext_flags & 0x20) m_HeaderLength += 2;  // sequence counter
          if (ext_flags & 0x10) m_HeaderLength += 2;  // P-STD buffer
          if (ext_flags & 0x01) m_HeaderLength += 2;  // dodatkowe dane
      }
  }

  return static_cast<int32_t>(m_PacketStartCodePrefix); // zwracamy prefiks jako wskaźnik sukcesu
}

void xPES_PacketHeader::Print() const {
  printf("PES:");
  printf("PSCP=%d ", m_PacketStartCodePrefix);
  printf("SID=%d ", m_StreamId);
  printf("L=%d \n", m_PacketLength);
}