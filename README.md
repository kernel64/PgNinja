# PgNinja  <img src="https://mabslabs.com/images/pgNinja.png" alt="PgNinja" style="position: relative;top: 13px;width:50px;"/>

**A lightweight Windows tray utility to manage your local PostgreSQL server.**

PgNinja is a minimalist system tray application for Windows that lets you easily **start**, **stop**, and **restart** your locally installed PostgreSQL server with a single click.

It automatically detects your PostgreSQL service, reflects the current server status with a colored tray icon (green for running, red for stopped), and ensures the proper permissions by requesting administrator elevation when needed.

---

When you need to quickly manage your PostgreSQL server without diving into the command line or complex GUI tools, PgNinja is here to help.

When the local server is running, the icon will turn green.

![PgNinja - PostgreSQL server is stopped](https://mabslabs.com/images/capture1.png)  * Stopped server *

![PgNinja - PostgreSQL server is running](https://mabslabs.com/images/capture2.png) * Server is running *

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

### Building with Visual Studio

1. Clone the repository:

```sh
    git clone https://github.com/kernel64/PgNinja.git
```

2. Open the solution file:

    * Navigate to the repository folder and open the .sln file in Visual Studio.

3. Build the solution:

    * Go to the "Build" menu and select "Build Solution".

    * Ensure that all dependencies are correctly installed.


