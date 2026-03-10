#pragma once
#include "xPES_Assembler.h"
#include "xPES_PacketHeader.h"
#include <cstring>
#include <cstdio>

// Konstruktor
xPES_Assembler::xPES_Assembler() {}

// Destruktor
xPES_Assembler::~xPES_Assembler() {
    delete[] m_Buffer;
}

// Inicjalizacja
void xPES_Assembler::Init(int32_t PID) {
    m_PID = PID;
    m_Buffer = new uint8_t[100000];
    m_BufferSize = 100000;
    m_LastContinuityCounter = 0;
    m_Started = false;
    m_DataOffset = 0;
}

// Reset bufora
void xPES_Assembler::xBufferReset() {
    m_PESH.Reset();
    m_DataOffset = 0;
    m_Started = false;
}

void xPES_Assembler::xBufferAppend(const uint8_t* Data, int32_t size) {

    if (m_Buffer == nullptr) {
        //Bufor nie został zaalokowany
        return;
    }

    if (size <= 0) {
        //Próba dodania pustych danych
        return;
    }

    if (m_DataOffset + size > m_BufferSize) {
        //Błąd: Brak miejsca w buforze
        return;
    }
    memcpy(m_Buffer + m_DataOffset, Data, size);
    m_DataOffset += size;
}


xPES_Assembler::eResult xPES_Assembler::AbsorbPacket(
    const uint8_t* TransportStreamPacket,
    const xTS_PacketHeader* PacketHeader,
    const xTS_AdaptationField* AdaptationField) {

    uint32_t TS_AdaptationLength = 0;
    if (PacketHeader->hasAdaptationField()) {
        TS_AdaptationLength = AdaptationField->getNumBytes();
    } // długość pola adaptacji

    if (m_Started && PacketHeader->getContinuityCounter() != ((m_LastContinuityCounter + 1) % 16)) {
        return eResult::StreamPackedLost;
    }// sprawdzenie ciągłości pakietu

    m_LastContinuityCounter = PacketHeader->getContinuityCounter(); // aktualizacja ostatniego licznika ciągłości

    const uint8_t* payloadStart = TransportStreamPacket + xTS::TS_HeaderLength + TS_AdaptationLength; // wskaźnik na początek danych ładunku

    uint32_t tempSize = xTS::TS_PacketLength - xTS::TS_HeaderLength - TS_AdaptationLength;// rozmiar danych ładunku

    if (PacketHeader->getPayloadStartIndicator()) {
        // nowy PES
        m_Started = true;

        m_PESH.Parse(payloadStart);
        uint32_t pesHeaderLen = m_PESH.getHeaderLength(); // długość nagłówka PES

        // dodaj tylko dane po nagłówku PES
        xBufferAppend(payloadStart + pesHeaderLen, tempSize - pesHeaderLen);

        return eResult::AssemblingStarted;
    } else {
        // kontynuacja lub zakończenie
        xBufferAppend(payloadStart, tempSize);

        if (PacketHeader->hasAdaptationField()) {
            return eResult::AssemblingFinished; // zakończenie, jeśli jest pole adaptacji
        } else {
            return eResult::AssemblingContinue; // kontynuacja, jeśli nie ma pola adaptacji
        }
    }
}


void xPES_Assembler::Parse(const uint8_t* TS_PacketBuffer, const xTS_PacketHeader* TS_PacketHeader, const xTS_AdaptationField* TS_AdaptationField, FILE* File){
    
    xPES_Assembler::eResult Result = xPES_Assembler::AbsorbPacket(TS_PacketBuffer, TS_PacketHeader, TS_AdaptationField);
    switch (Result)
    {
        case xPES_Assembler::eResult::StreamPackedLost:{
            printf("\033[1;31mPacket Lost\n\033[0m");
            break;}
        case xPES_Assembler::eResult::AssemblingStarted:{
            printf("\033[1;33mAssembling Started\n\033[0m");
            xPES_Assembler::PrintPESH();
            break;}
        case xPES_Assembler::eResult::AssemblingContinue:{
            printf("\033[1;34mAssembling Continue\n\033[0m");
            break;}
        case xPES_Assembler::eResult::AssemblingFinished:{
            printf("\033[1;32mAssembling Finished\n\033[0m");
            printf("PES: PcktLen=%d HeadLen=%d DataLen=%d \n", this->getNumPacketBytes(), this-> m_PESH.getHeaderLength(), this->getNumPacketBytes() - this->m_PESH.getHeaderLength());
            fwrite(xPES_Assembler::getPacket(), 1, xPES_Assembler::getNumPacketBytes(), File);
            xBufferReset();
            break;}
        case xPES_Assembler::eResult::UnexpectedPID:{
            printf("\033[1;31mUnexpected PID\n\033[0m");
            break;}
    }
}