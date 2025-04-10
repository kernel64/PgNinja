# PgNinja 🥷

**A lightweight Windows tray utility to manage your local PostgreSQL server.**

PgNinja is a minimalist system tray application for Windows that lets you easily **start**, **stop**, and **restart** your locally installed PostgreSQL server with a single click.

It automatically detects your PostgreSQL service, reflects the current server status with a colored tray icon (green for running, red for stopped), and ensures the proper permissions by requesting administrator elevation when needed.

---

## ✨ Features

- Lightweight system tray application (like WAMP)
- Auto-detection of the local PostgreSQL service name
- Start / Stop / Restart your PostgreSQL server
- Tray icon color indicates server status (green/red)
- Elevation prompt to run with administrator privileges
- Fast, responsive, and written in modern C++
- MIT Licensed — open and free to use

---

## Requirements

- Windows 10 or higher
- A local installation of PostgreSQL
- Admin rights to manage Windows services

---

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/kernel64/PgNinja.git
