#include "tsCommon.h" 
#include "tsTransportStream.h"
#include "tsAdaptationField.h"
#include "xPES_Assembler.h" 
#include "xPES_PacketHeader.h"

//=============================================================================================================================================================================

int main(int argc, char *argv[], char *envp[])
{
    FILE* TransportStreamFile = fopen("C:\\Users\\piotr\\Desktop\\example_new.ts", "rb");
    FILE* AudioFile = fopen("C:\\Users\\piotr\\Desktop\\PID136.mp2", "wb");

    if (TransportStreamFile == nullptr)
    {
        printf("Error opening file\n");
        return EXIT_FAILURE;
    }

    xTS_PacketHeader     TS_PacketHeader;
    xTS_AdaptationField  TS_PacketAdaptationField;
    xPES_Assembler       PES_Assembler;
    xPES_PacketHeader    PES_PacketHeader;
    int32_t              TS_PacketId = 0;
    uint8_t              TS_PacketBuffer[xTS::TS_PacketLength];
    int i = 0;

    PES_Assembler.Init(136);
    while (!feof(TransportStreamFile))
    {
        size_t NumRead = fread(TS_PacketBuffer, 1, xTS::TS_PacketLength, TransportStreamFile);
        if (NumRead != xTS::TS_PacketLength) { break; }

        TS_PacketHeader.Reset();
        TS_PacketHeader.Parse(TS_PacketBuffer);
        TS_PacketAdaptationField.Reset();

        if (TS_PacketHeader.getSyncByte() == 'G' && TS_PacketHeader.getPacketIdentifier() == 136)
        {
            if (TS_PacketHeader.hasAdaptationField())
            {
                TS_PacketAdaptationField.Parse(TS_PacketBuffer, TS_PacketHeader.getAdaptationFieldControl());
                //TS_PacketAdaptationField.Print();
            }
            printf("%010d ", TS_PacketId);
            TS_PacketHeader.Print();
            PES_Assembler.Parse(TS_PacketBuffer, &TS_PacketHeader, &TS_PacketAdaptationField, AudioFile);
        }

        TS_PacketId++;
        //if (++i >= 50) break; 
    }
    fclose(AudioFile);
    fclose(TransportStreamFile);
    return EXIT_SUCCESS;
}

//=============================================================================================================================================================================
