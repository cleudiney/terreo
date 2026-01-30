# Copilot Instructions for Water Tank Monitoring System

## Project Overview
This is an ESP32-based water tank monitoring system for a building (Edifício Aquários - COPASA). It manages two pump systems, monitors water levels via ultrasonic sensor, tracks flow rates, and provides a web interface with user authentication and real-time alerts.

**Key characteristics:**
- Modular Arduino architecture with separate `.ino` files for each subsystem
- SPIFFS-based persistent data (users, alerts, statistics)
- RESTful API backend with WebServer class
- Bootstrap-based responsive frontend
- Hierarchical access control (6 levels from resident to admin)
- Real-time monitoring with WhatsApp/CallMeBot notifications

## Architecture: Core Patterns & Data Flow

### File Organization Pattern
The project uses a **modular single-sketch pattern** where `caixa_terreo.ino` is the entry point containing only `setup()` and `loop()`. All functionality is split across:

- **variaveis.h/cpp**: Global declarations, enums, structures, constants
- **subsystem files** (sensores.ino, bombas.ino, usuarios.ino, etc.): Implementation of features
- **spiffs.ino**: Persistent storage operations
- **webserver.ino**: RESTful API and HTML routing
- **data/**: SPIFFS filesystem content (HTML pages, CSS, JS)

See [guia descritivo.txt](guia descritivo.txt) for detailed module descriptions.

### Critical Initialization Order (in setup())
The order in `caixa_terreo.ino` is **strictly enforced** and cannot be changed:
1. Serial → 2. GPIOs → 3. SPIFFS (must be before user files)
4. WiFi → 5. NTP → 6. State structures → 7. Sensors → 8. Users DB → 9. Automation rules → 10. Alerts → 11. WebServer

**Why:** SPIFFS contains user credentials and alert history; WiFi must precede time sync; state must initialize before sensor readings.

### State Management: `EstadoCaixa` Structure
All water tank state is centralized in the global `estadoAtual` struct:
```cpp
struct EstadoCaixa {
  float nivelPercentual;        // 0-100%
  float nivelCm;                // raw ultrasonic distance
  bool vazaoEntrada;            // has inflow
  bool bombaAAtiva/bombaBAtiva; // pump status
  EstadoCaixaEnum estado;       // ENCHENDO|ESVAZIANDO|ESTAGNADA|CRITICA|ESTAVEL
  String ultimaAtualizacao;     // timestamp
};
```
Every subsystem **reads from and writes to** this struct. No other state singleton exists—if you need to persist data, modify `estadoAtual` or the respective `.dat` file.

### Sensor Reading Pipeline
Centralized in `lerTodosSensores()`:
1. **Ultrasonic (HC-SR04)**: Takes 5 samples, averages, filters noise (see `lerSensorUltrassom()`)
2. **Flow rate**: Counted via interrupt pulses on GPIO 34
3. State is **timestamped** via `getDataHoraAtual()` (NTP-synced)

New sensors: Add a `lerSensor*()` function and call it from `lerTodosSensores()`.

### Pump Control: Manual vs. Automatic
- **Manual**: API endpoints in `bombas.ino` call `ligarBomba('A'|'B')` / `desligarBomba()`
- **Automatic**: `controleAutomaticoBombas()` applies business rules (e.g., "stop if no inflow and level < 40%")
- **Critical safety**: Both paths call `registrarAviso()` to log every state change
- Pump logic: Bypass `digitalWrite()` and you lose audit trails

### Persistent Data Format (Text-based, SPIFFS)
Located at `/dados/` in SPIFFS (maps to data/ on host). Format is pipe-delimited text:

- **usuarios.dat**: `username;password;nivelacesso\n`
- **avisos.dat**: `datahora|tipo|mensagem|usuario\n` (tipos: "evento", "urgente", "alerta")
- **registros.dat**: Historical events and statistics

**Critical**: File I/O uses `SPIFFS.open()` with explicit `FILE_READ` or `FILE_APPEND`. Always close files. Parsing uses `readStringUntil('\n')` and manual `indexOf()` parsing—no JSON on these files.

### User Authentication & Access Control
In `usuarios.ino`:
- `autenticarUsuario()` checks credentials against usuarios.dat
- `NivelAcesso` enum (0-5): ADMIN > SINDICO > SUBSINDICO > ZELADOR > MORADOR > NENHUM
- Web endpoints check `autenticado` global + `nivelAcessoLogado` string
- Sessions timeout handled in `sessao_web.ino`

**Important**: Always check BOTH conditions before allowing protected operations.

## Development Workflows

### Building & Uploading
Use **PlatformIO** or **Arduino IDE** with the standard ESP32 board definition. The project targets **ESP32** (not S3 or other variants).

Common pre-flight checks:
- SPIFFS must be initialized (see spiffs.ino) before calling file operations
- WiFi credentials must be in `variaveis.cpp` (not shown in repo for security)
- NTP client requires time to sync before RTC calls

### Debugging
- Serial monitor at **115200 baud**: All major functions emit emojis + status messages
- SPIFFS contents can be logged via `listarSPIFFSDebug()` in spiffs.ino
- Use `getDataHoraAtual()` for time-synced debug timestamps

### Testing Sensors Without Hardware
The main loop includes a commented section that **simulates sensor values**. Uncomment lines in `caixa_terreo.ino` loop to test APIs without ultrasonic/flow hardware.

## Integration Points & External Dependencies

### CallMeBot WhatsApp Notifications
Configured in `variaveis.h` with three phone/API key pairs. Called from:
- `enviarMensagemBoot()`: Initial startup alert
- `enviarAlerta()`: Critical thresholds (nivel < 20%, critical state)
- Outbound via HTTP GET to callmebot.com API

**Pattern**: Build a query string and use `HTTPClient` (include in code as needed).

### DuckDNS Dynamic DNS
Called from `iniciarDuckDNS()` in duckdns.ino. Requires:
- DUCKDNS_DOMAIN (e.g., "myCaixa.duckdns.org")
- DUCKDNS_TOKEN (from duckdns.org)

Updates every 5 minutes to keep external IP current. Useful for remote HTTPS access.

### NTP Time Synchronization
Via `NTPClient` library, configured in `inicializarTimeClient()` (conexao.ino). Uses `pool.ntp.org` and must be called **after** WiFi connects. All timestamps use this synced time.

## Project-Specific Conventions

### Naming & Emoji Logging
Functions start with verb: `inicializar*()`, `ler*()`, `ligar*()`. All Serial.println() include emojis for visual scanning (📋, 🔧, ⚠️, etc.). Maintain this style for consistency.

### Error Handling Pattern
No exceptions—use explicit checks and early returns:
```cpp
File f = SPIFFS.open(path, FILE_READ);
if (!f) {
  Serial.println("❌ Error message");
  return; // or return false
}
```

### Configuration Values
Thresholds are in `variaveis.h`:
- `NIVEL_CRITICO` = 20% (pump safety cutoff)
- `NIVEL_ALERTA` = 40% (trigger notifications)
- `NIVEL_NORMAL` = 60%
- `ALTURA_CAIXA_CM` = 200 (ultrasonic sensor height)

Changing these requires recompile—no runtime config system yet.

## Frontend Architecture (data/ folder)

- **index.html**: Main dashboard with water tank visualization
- **login.html / login-localstorage.html**: Authentication UIs (session stored in browser)
- **admin.html, avisos.html, etc.**: Role-based pages
- **js/script.js**: Empty stub (logic in individual page JS files)
- **js/auth-guard.js, navbar-guard.js**: Client-side route protection

**API Contract**: Backend returns JSON for `/api/*` endpoints; pages expect status, nivel, bomba states, avisos array, usuarios array. See webserver.ino for endpoint signatures.

## Common Tasks & Code Patterns

### Adding a New Sensor
1. Declare pin in variaveis.h
2. Create `lerSensor*()` function in new or existing .ino
3. Update `EstadoCaixa` struct if new state needed
4. Call from `lerTodosSensores()` in sensores.ino
5. Expose via `/api/estado` JSON response

### Adding a New User Role
1. Extend `NivelAcesso` enum in variaveis.h
2. Update `apiListarUsuarios()` in usuarios.ino to parse new level
3. Guard protected endpoints with `nivelAcessoLogado` check in webserver.ino

### Adding a New Alert Type
1. Call `registrarAviso("tipo", "msg", "usuario")` from any subsystem
2. Client pages poll `/api/avisos` (real-time via future WebSocket upgrade)
3. Old aviso.dat entries are append-only (no delete mechanism yet)

### Troubleshooting Pump Not Responding
1. Check GPIO pins match `PINO_BOMBA_A` / `PINO_BOMBA_B` in variaveis.h
2. Verify `controleAutomaticoBombas()` isn't overriding manual commands
3. Check `registrarAviso()` logs—if no "ligada" message, API may have failed auth
4. Check WebServer route in webserver.ino is correctly mapped

## Limitations & TODOs
- WebSocket is stubbed (websocket.ino is empty) → upgrades to WebSocket would improve real-time responsiveness
- No OTA (over-the-air) updates yet
- SPIFFS data format is text-based → scales poorly above ~1000 log entries (consider SQLite if expansion needed)
- No backup of users.dat (consider exporting to cloud)
- Session timeout is manual (could be auto-logout)

---

**Last Updated**: January 28, 2026  
**For questions on specific modules**, check [guia descritivo.txt](guia descritivo.txt) for detailed module breakdown.
