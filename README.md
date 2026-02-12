# Nuclear-Launch code sharing : A CRT Secret Sharing System

A web-based secret sharing application implementing a Chinese Remainder Theorem (CRT) based threshold scheme. This project utilizes a hybrid architecture with a Node.js backend for session management and a C++ core for cryptographic calculations.

## Overview

This system allows an administrator ("dealer") to generate a secret number and split it into multiple shares using modular arithmetic properties. These shares are distributed to "ministers". A predefined threshold of ministers must collaborate to reconstruct the original secret.

### Key Features
- **Threshold Security**: Implements an $(n, k)$ threshold scheme (default: 5 shares, 3 required to reconstruct).
- **Hybrid Architecture**:
  - **C++ Core**: Handles high-precision arithmetic and CRT logic for share generation and secret reconstruction.
  - **Node.js Server**: Manages web sessions, API endpoints, and process orchestration.
- **Secure Session Management**: In-memory session storage with automatic expiration (10-minute cleanup) to prevent data persistence risks.
- **Dual Interface**:
  - **Admin Panel**: For generating secrets and viewing public moduli.
  - **Minister Panel**: For share holders to input their partial data for reconstruction.

## Prerequisites

- **Node.js** (v14 or higher recommended)
- **g++** (MinGW or any standard C++ compiler) for building the core executable.

## Installation

1.  **Clone the repository**:
    ```bash
    git clone <repository-url>
    cd <repository-directory>
    ```

2.  **Install dependencies**:
    ```bash
    npm install
    ```

3.  **Compile the C++ core**:
    Ensure `g++` is in your system PATH.
    ```bash
    g++ NLC.cpp -o NLC.exe
    ```
    *Note: On Linux/Mac, output as `NLC` and update `server.js` to run `./NLC` instead of `NLC.exe`.*

## Usage

1.  **Start the server**:
    ```bash
    npm start
    ```
    The server works on port `3000` by default.

2.  **Generate a Secret**:
    -   Navigate to `http://localhost:3000/admin.html`.
    -   Click **Generate Secret**.
    -   The system will display the **Session ID** and the **Public Moduli** for each minister.
    -   *Note: In a real-world scenario, the actual secret values (shares) are distributed securely to ministers via secure channels (console logs in this demo).*

3.  **Reconstruct a Secret**:
    -   Navigate to `http://localhost:3000/minister.html`.
    -   Enter the **Session ID**.
    -   Ministers input their **ID** and **Share Value**.
    -   Once the threshold (3 shares) is met, click **Reconstruct**.
    -   If the shares are valid, the original secret is revealed.

## Project Structure

-   `NLC.cpp`: Core C++ implementation of the Non-Linear Congruential / CRT logic.
-   `server.js`: Express.js server handling API requests and C++ child processes.
-   `admin.*`: Frontend files for the secret generation interface.
-   `minister.*`: Frontend files for the secret reconstruction interface.

## License

This project is open source.
