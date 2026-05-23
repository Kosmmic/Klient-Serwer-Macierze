# Współbieżny Serwer Obliczeniowy IPC (C / Linux)

Projekt realizuje system klient-serwer w środowisku tekstowym Linux przy użyciu mechanizmów IPC standardu System V oraz asynchroniczne zarządzanie procesami w systemie POSIX.

## Funkcjonalności
* **Współbieżny Serwer**: Obsługuje wielu klientów jednocześnie za pomocą procesów potomnych (`fork()`). Posiada mechanizm automatycznego czyszczenia procesów zombie (`SIGCHLD`).
* **Komunikacja System V**: Wymiana danych (macierzy oraz wyników) realizowana jest przez systemowe kolejki komunikatów (`msgget`, `msgsnd`, `msgrcv`).
* **Tekstowy Interfejs Użytkownika (TUI)**: Klient posiada dynamiczny interfejs zbudowany w oparciu o bibliotekę `ncurses`.

### 1. Warstwa komunikacji (Kolejki komunikatów System V)
Wymiana informacji odbywa się przez pojedynczą, systemową kolejkę komunikatów, identyfikowaną unikalnym kluczem numerycznym. Kolejka działa jako asynchroniczny bufor FIFO (First-In, First-Out), który obsługuje dwa rodzaje komunikatów (identyfikowane przez pole `mtype` typu `long`):
*   **Mtype = 1 (Żądanie obliczeniowe)**: Klient umieszcza w kolejce strukturę zawierającą swój unikalny identyfikator procesu (PID), wymiary macierzy ($M \times N$) oraz dwuwymiarową tablicę wartości rzeczywistych (`double`).
*   **Mtype = PID Klienta (Odpowiedź)**: Serwer odsyła strukturę z obliczonym wynikiem (suma elementów). Zastosowanie PID jako typu komunikatu pozwala na pełną izolację danych – w środowisku wieloklienckim każdy klient wybiera z kolejki wyłącznie paczkę zwrotną dedykowaną dla niego.

### 2. Współbieżność serwera i eliminacja procesów zombie
Główny proces serwera (rodzic) działa w nieskończonej pętli, blokując się na funkcji `msgrcv()` i oczekując na komunikaty typu `1`. Po odebraniu struktury, serwer nie wykonuje obliczeń sekwencyjnie. Natychmiast wywołuje funkcję systemową `fork()`, tworząc odizolowany proces potomny (dziecko):
*   **Proces główny (rodzic)**: Natychmiast powraca na początek pętli do funkcji `msgrcv()`, zachowując pełną responsywność na nowe żądania od innych klientów.
*   **Proces potomny (dziecko)**: Przejmuje kopię danych komunikatu, wykonuje algorytm sumowania macierzy, wysyła wynik funkcją `msgsnd()` z `mtype = PID` i kończy działanie poprzez `exit(0)`.

## Wymagania systemowe
* System operacyjny: **Linux**
* Kompilator: `gcc`
* Biblioteka deweloperska: `libncurses-dev` (lub odpowiednik dla danej dystrybucji)

W systemach Ubuntu/Debian bibliotekę ncurses zainstalujesz poleceniem:
```bash
sudo apt update && sudo apt install libncurses5-dev libncursesw5-dev
```

## Kompilacja projektu
Projekt zawiera plik `Makefile`. Aby skompilować oba programy, wpisz w terminalu:
```bash
make
```
Aby wyczyścić pliki binarne:
```bash
make clean
```

## Uruchomienie i testowanie
Mechanizm IPC wymaga, aby **najpierw** uruchomić serwer:

1. W pierwszym oknie terminala uruchom serwer:
   ```bash
   ./serwer
   ```
2. W drugim (lub kilku osobnych oknach) uruchom klienta:
   ```bash
   ./klient
   ```

## Struktura plików
* `serwer.c` - kod źródłowy współbieżnego serwera.
* `klient.c` - kod źródłowy klienta z interfejsem TUI (ncurses).
* `wspolny.h` - wspólny plik nagłówkowy zawierający definicje struktur IPC oraz klucz kolejki.
* `Makefile` - skrypt automatyzujący proces kompilacji.
