# Protocol Messages

## Temperature Request (Client → Sensor/Actuator)
`GET_TEMP`

## Temperature Response (Sensor/Actuator → Client)
`TEMP:25.5`

## Threshold (Server → Client)
`THRESHOLD:30.0`

## LED State (Client ↔ Sensor/Actuator and Client ↔ Server)
`LED:ON` or `LED:OFF`


### Normal Operation Example

```
┌─────────────────┐                    ┌──────────┐                     ┌───────────────┐
│ Sensor/Actuator │                    │  Client   │                     │  Host Server   │
└────────┬────────┘                    └──┬───────┘                     └────┬──────────┘
         │                                 │                                   │
         │  GET_TEMP (TCP 9080)            │                                   │
         │<─────────────────────────────│                                   │
         │  TEMP:25.5 (TCP 9080)           │                                   │
         │──────────────────────────────>│                                   │
         │                                 │  TEMP:25.5 (UDP 8081)             │
         │                                 │───────────────────────────────>│
         │                                 │                                   │
         │                                 │  THRESHOLD:30.0 (TCP 8080)        │
         │                                 │<───────────────────────────────│
         │                                 │                                   │
         │  GET_TEMP (TCP 9080)            │                                   │
         │<─────────────────────────────│                                   │
         │  TEMP:31.0 (TCP 9080)           │                                   │
         │──────────────────────────────>│                                   │
         │                                 │  TEMP:31.0 (UDP 8081)             │
         │                                 │───────────────────────────────>│
         │                                 │                                   │
         │                                 │  [Temp > Threshold: LED ON]       │
         │                                 │                                   │
         │  LED:ON (TCP 9080)              │                                   │
         │<─────────────────────────────│                                   │
         │                                 │  LED:ON (TCP 8080)                │
         │                                 │───────────────────────────────>│
```

## Communication Summary

### Sensor/Actuator ↔ Client (TCP Port 9080)
- **Request-Response Pattern**: Client polls sensor for temperature
- Client sends: `GET_TEMP`
- Sensor responds: `TEMP:25.5`
- Client sends LED commands: `LED:ON` / `LED:OFF`

### Client → Server (UDP Port 8081)
- **Streaming**: Client forwards temperature data
- Format: `TEMP:25.5`

### Server → Client (TCP Port 8080)
- **Threshold Updates**: Server sends threshold changes
- Format: `THRESHOLD:30.0`
- **LED Status**: Server receives LED state
- Format: `LED:ON` / `LED:OFF`
