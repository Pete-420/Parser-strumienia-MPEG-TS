# TS-PARSER (MPEG-TS → PES → audio)

Projekt w C++ służący do podstawowego parsowania strumienia MPEG-TS (188 bajtów na pakiet), filtrowania po PID oraz składania ładunku PES do pliku wynikowego.

## Co robi program

- Czyta plik `.ts` pakietami po 188 bajtów.
- Parsuje nagłówek TS (`xTS_PacketHeader`).
- (Opcjonalnie) parsuje Adaptation Field (`xTS_AdaptationField`).
- Dla wybranego PID składa payload w całość PES (`xPES_Assembler`) i zapisuje zbuforowane dane do pliku.

Aktualnie w [TS_parser.cpp](TS_parser.cpp) program jest ustawiony na PID `136` oraz na ścieżki absolutne dla plików wejścia/wyjścia.

## Wymagania

- Kompilator C++17
  - Windows: Visual Studio (MSVC) lub MinGW
  - Linux/macOS: GCC/Clang
- CMake ≥ 3.10

Brak zewnętrznych bibliotek.

## Budowanie (CMake)

### Windows (Visual Studio)

W katalogu projektu:

```bash
cmake -S . -B build
cmake --build build --config Debug
```

Plik wykonywalny znajdziesz zazwyczaj w:

- `build/Debug/TS-PARSER.exe` (dla generatora Visual Studio)

### Linux/macOS

```bash
cmake -S . -B build
cmake --build build
```

## Uruchamianie

W obecnej wersji program używa stałych ścieżek:

- wejście: `C:\Users\piotr\Desktop\example_new.ts`
- wyjście: `C:\Users\piotr\Desktop\PID136.mp2`

Zmień je w [TS_parser.cpp](TS_parser.cpp) na własne pliki (albo dostosuj program do argumentów CLI, jeśli chcesz).

Po uruchomieniu program wypisuje informacje o pakietach TS i stanie składania PES.

## Najważniejsze pliki

- [TS_parser.cpp](TS_parser.cpp) — `main()`: czytanie TS, filtr PID, zapis wyniku
- [tsTransportStream.h](tsTransportStream.h), [tsTransportStream.cpp](tsTransportStream.cpp) — nagłówek TS
- [tsAdaptationField.h](tsAdaptationField.h), [tsAdaptationField.cpp](tsAdaptationField.cpp) — Adaptation Field
- [xPES_Assembler.h](xPES_Assembler.h), [xPES_Assembler.cpp](xPES_Assembler.cpp) — składanie PES z payloadów TS
- [xPES_PacketHeader.h](xPES_PacketHeader.h), [xPES_PacketHeader.cpp](xPES_PacketHeader.cpp) — parsowanie nagłówka PES

## Uwagi

- Wypisywanie kolorów w konsoli w [xPES_Assembler.cpp](xPES_Assembler.cpp) używa sekwencji ANSI (np. `\033[...m`). W klasycznym `cmd.exe` może to nie działać; w Windows Terminal / PowerShell zwykle działa.
- Składanie PES jest „minimalne” i zależy od poprawnej ciągłości pakietów (continuity counter). Przy ubytku pakietów pojawi się `Packet Lost`.

## Licencja

Brak określonej licencji w repozytorium (projekt na potrzeby zajęć). Jeśli planujesz publikację/udostępnianie, warto dopisać licencję.
