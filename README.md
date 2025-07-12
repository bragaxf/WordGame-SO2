# WordGame – SO2

This project was developed as part of the **Operating Systems 2**  **Instituto Superior de Engenharia de Coimbra (ISEC)**.

## 🧠 Description

**WordGame** is a multiplayer word game implemented in C for the Windows operating system. It uses advanced interprocess communication (IPC) mechanisms including:

- **Named Pipes** (login and dedicated per player)
- **Shared Memory** (to store game state)
- **Synchronization Events**
- **Multithreading**
- **Real-time updates via timer**
- **Bot logic**
- **Optional visual monitor (`painel.c`)**
- **Graphical UI (`jogoui.c`)**

## 🔧 Components

- `arbitro.c` – Manages game logic and shared state
- `bot.c` – Acts as an autonomous player
- `jogoui.c` – Graphical interface for players
- `painel.c` – Visual monitor for game state (optional)
- `utils.h` – Shared headers/utilities

## 🖥️ Technologies

- C (WinAPI)
- Named Pipes (Windows IPC)
- Shared Memory
- Threads and Synchronization (Events)
- Git

## 📚 Academic Context

> Developed during the 2rd year of the **Bachelor in Computer Engineering** at the **Instituto Superior de Engenharia de Coimbra (ISEC)**, for the **Operating Systems 2** course.

## 📝 License

This project is licensed under the MIT License. See `LICENSE` for more information.
