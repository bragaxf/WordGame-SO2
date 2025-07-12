# WordGame – SO2

This project was developed as part of the **Operating Systems 2 Course**  at **Instituto Superior de Engenharia de Coimbra (ISEC)**.

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

| File        | Description                         |
|-------------|-------------------------------------|
| `arbitro.c` | Game arbitrator (main server logic) |
| `arbitro.h` | Header for arbitrator definitions   |
| `bot.c`     | Autonomous bot player logic         |
| `jogoui.c`  | Player user interface (client)      |
| `jogoui.h`  | Header for jogoui                   |
| `painel.c`  | Graphical monitor (optional)        |
| `utils.h`   | Common definitions and structures   |
| `SO2 - 2425 - TP v6.pdf`   | Official project specification  |

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
