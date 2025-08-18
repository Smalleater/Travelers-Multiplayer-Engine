# 🗺️ Travelers Multiplayer Engine - Roadmap

## 📊 Project Overview

**Objective**: Develop a high-performance networking engine for multiplayer video games

**Overall Progress**: ~45% ✅

| Component | Status | Progress | Version |
|-----------|--------|----------|---------|
| 🏗️ Core Infrastructure | ✅ Complete | 95% | v0.1 |
| 🔗 TCP Networking | ✅ Complete | 95% | v0.1 |
| 🧪 Basic Testing | ✅ Complete | 80% | v0.1 |
| 📦 Message Protocol | 🔄 In Progress | 15% | v0.2 |
| 📡 UDP Implementation | ❌ To Do | 0% | v0.3 |
| 🎮 Gaming Features | ❌ To Do | 0% | v0.4-0.6 |
| 🚀 Production Ready | ❌ To Do | 0% | v0.7-1.0 |

---

## 🏗️ PHASE 1: CORE INFRASTRUCTURE (COMPLETED ✅) - **VERSION 0.1**

> **Version 0.1 Release Criteria:**
> - Complete core infrastructure with service management
> - Full TCP networking implementation with multi-client support
> - Cross-platform compatibility (Windows/Linux)
> - Basic testing framework with client/server test applications
> - Comprehensive logging system with file/console output
> - Thread-safe networking with graceful connection handling

### ✅ 1.1 Foundation & Architecture
- [x] **Modular Architecture** - Clear separation of responsibilities
- [x] **Interface abstractions** - ISocket, ILogger, IThreadManager
- [x] **RAII Memory Management** - Smart pointers, automatic destructors
- [x] **Cross-platform support** - Windows/Linux with abstractions

### ✅ 1.2 Build System & Configuration
- [x] **CMakeLists.txt** - Multi-platform build configuration
- [x] **Compiler settings** - C++17, optimizations, warnings
- [x] **Shared library export** - DLL/SO with TME_API macros
- [x] **Include directories** - Public/private header structure
- [x] **VS Code integration** - IntelliSense and debugging configuration

### ✅ 1.3 Error Handling & Debugging
- [x] **ErrorCodes enum** - 15+ specialized error codes
- [x] **Error codes mapping** - Unified WSAGetLastError/errno
- [x] **Utils class** - System utility functions
- [x] **Time formatting** - GetSystemTime with custom format
- [x] **Console detection** - HasAnyConsoleOutput() multi-platform

### ✅ 1.4 Logging System
- [x] **ILogger interface** - Logging abstraction
- [x] **ConsoleLogger** - Logging with formatted timestamps
- [x] **FileLogger** - Persistent logs with automatic rotation
- [x] **CompositeLogger** - Multiple destinations (console + file)
- [x] **Thread-safe logging** - Mutex protection for concurrent access
- [x] **Comprehensive logging** - Info, Warning, Error levels

### ✅ 1.5 Threading & Concurrency
- [x] **ThreadManager** - Configurable thread pool
- [x] **Job queue system** - Thread-safe queue with condition variables
- [x] **Worker threads** - Automatic task distribution
- [x] **Thread synchronization** - Mutex and condition variables
- [x] **Graceful shutdown** - Clean shutdown of all threads

### ✅ 1.6 Service Management
- [x] **Service Locator Pattern** - Global access to services
- [x] **ServiceManager** - Coordinated initialization/shutdown
- [x] **ServiceLocator** - Thread-safe global service access
- [x] **Service registration** - ProvideLogger, ProvideThreadManager
- [x] **Automatic cleanup** - Reset() to free all resources
- [x] **Error propagation** - Initialization error handling

### ✅ 1.7 Platform Abstraction
- [x] **WsaInitializer** - Windows Sockets initialization (RAII)
- [x] **Socket types** - Typedef for SOCKET/int per platform
- [x] **NetworkCommon.hpp** - Cross-platform definitions (Windows/POSIX)

---

## 🔗 PHASE 2: TCP NETWORKING (COMPLETED ✅) - **VERSION 0.1**

> **Included in Version 0.1 Release Criteria (continued):**
> - Complete TCP socket implementation with non-blocking operations
> - Server and client connection management
> - Reliable data transmission with error handling
> - Network engine with static API interface
> - Automatic disconnection detection and cleanup

### ✅ 2.1 Socket Foundation
- [x] **ISocket interface** - Multi-platform socket abstraction
- [x] **TcpSocket class** - Complete TCP implementation
- [x] **Socket operations** - Connect, Bind, Listen, Accept, Send, Receive
- [x] **Non-blocking mode** - SetBlocking with WOULD_BLOCK handling
- [x] **Error handling** - Detailed error codes per operation

### ✅ 2.2 Connection Management
- [x] **Server socket** - Bind and Listen on specified port
- [x] **Client connections** - Connect to remote server
- [x] **Accept loop** - Up to 16 new clients per frame
- [x] **Multi-client support** - Client map with unique networkId
- [x] **Connection state tracking** - IsConnected() with validation
- [x] **Disconnection detection** - Automatic cleanup of disconnected clients
- [x] **Graceful disconnection** - Automatic cleanup on connection loss

### ✅ 2.3 Data Transmission
- [x] **Buffer management** - 4KB buffers with resizing
- [x] **Partial read handling** - Handling incomplete reads
- [x] **Batch processing** - Limit 32 messages per client per frame
- [x] **SendToServerReliable** - Client to server (TCP)
- [x] **SendToClientTcp** - Server to specific client (TCP) 
- [x] **SendToAllClientTcp** - Server to all clients (TCP)
- [x] **ReceiveAllFromClientTcp** - Server receives from all clients
- [x] **ReceiveFromServerTcp** - Client receives from server

### ✅ 2.4 Network Engine Integration
- [x] **NetworkEngine** - Main entry point with static interface
- [x] **NetworkManager** - Core networking coordination
- [x] **Lifecycle management** - Init, Update, Shutdown
- [x] **Server/Client modes** - StartServer, StartClient
- [x] **State queries** - IsInitialized, EnsureInitialized

---

## 🧪 PHASE 3: BASIC TESTING (COMPLETED ✅) - **VERSION 0.1**

> **Included in Version 0.1 Release Criteria (continued):**
> - Working client and server test applications
> - CMake build system for all platforms
> - VS Code integration with debug configurations
> - Manual testing capabilities for network functionality

### ✅ 3.1 Test Infrastructure Setup
- [x] **CMake build system** - Build configuration for tests
- [x] **VS Code integration** - Debug configuration for tests
- [x] **Test project structure** - Separate client/server test projects

### ✅ 3.2 Basic Test Applications
- [x] **Server test application** - Basic server implementation in `/tests/Server/`
- [x] **Client test application** - Basic client implementation in `/tests/Client/`
- [x] **Manual testing capability** - Interactive client-server testing

### 🔄 3.3 Testing Gaps (Minor improvements needed)
- [ ] **Automated integration test** - Scripted client-server communication test
- [ ] **Stress testing** - Multiple clients connecting simultaneously
- [ ] **Error condition testing** - Network failure simulation
- [ ] **Performance benchmarks** - Latency and throughput measurements

---

## 📦 PHASE 4: MESSAGE PROTOCOL (IN PROGRESS 🔄) - **VERSION 0.2**

> **Version 0.2 Release Criteria:**
> - Complete TCP message framing implementation
> - Structured message system with headers and types  
> - Basic serialization for primitive types
> - Heartbeat/ping system for connection monitoring
> - Enhanced integration tests with message validation

### 🔄 4.1 Current State & Critical Issues
- [x] **Basic data transmission** - Raw byte arrays (current implementation)
- [x] **Multi-thread infrastructure** - ThreadManager ready but not used in networking yet
- [ ] **Message framing** - Message delimitation in TCP stream (CRITICAL)
- [ ] **Single-thread execution** - NetworkManager::Update() runs on main thread (by design)
- [ ] **Heartbeat system** - Dead connection detection
- [ ] **Message ordering** - Ensure proper message sequence

### 📝 4.2 Message Structure (To Do)
- [ ] **Message header** - Type, size, ID, timestamp, checksum
- [ ] **Message types** - CONNECT, DISCONNECT, GAME_DATA, PING, PONG, etc.
- [ ] **Message validation** - Integrity and format verification
- [ ] **Message fragmentation** - Support for messages > MTU
- [ ] **Message reassembly** - Fragment reconstruction

### � 4.3 Serialization System (To Do)
- [ ] **ISerializable interface** - Serialization abstraction
- [ ] **Binary serializer** - Efficient binary serializer
- [ ] **Primitive types** - int, float, string, bool
- [ ] **Container support** - vector, map, set
- [ ] **Endianness handling** - Big/Little endian conversion
- [ ] **Version compatibility** - Multi-version protocol support

### 🏭 4.4 Protocol Implementation (To Do)
- [ ] **Message factory** - Typed message creation
- [ ] **Message registry** - Custom type registration
- [ ] **Compression** - LZ4/Zlib for large messages
- [ ] **Encryption hooks** - Preparation for encryption

---

## 📡 PHASE 5: UDP IMPLEMENTATION (TO DO ❌) - **VERSION 0.3**

> **Version 0.3 Release Criteria:**
> - Complete UDP socket implementation with reliability layer
> - **Multi-threading transition** - Move networking to dedicated threads
> - Packet acknowledgment and retransmission system
> - Mixed TCP/UDP support in NetworkManager
> - Channel system for reliable/unreliable communication
> - UDP integration tests and performance benchmarks

### 🧵 5.0 Multi-Threading Transition (New Priority)
- [ ] **Network thread separation** - Move I/O operations to dedicated thread
- [ ] **Thread-safe message queues** - Communication between network and main threads
- [ ] **NetworkManager refactoring** - Async Update() with job submission
- [ ] **Performance monitoring** - Compare single vs multi-thread performance
- [ ] **Race condition testing** - Ensure thread safety under load

### 🌐 5.1 UDP Socket Foundation
- [ ] **UdpSocket class** - ISocket implementation for UDP
- [ ] **Connectionless design** - Source/destination address handling
- [ ] **Port binding** - Bind to specific port
- [ ] **Error handling** - UDP-specific error handling
- [ ] **Broadcast support** - Broadcast/multicast messages

### 🔄 5.2 Reliability Layer
- [ ] **Sequence numbers** - Packet numbering
- [ ] **Acknowledgement system** - ACK/NACK for selective reliability
- [ ] **Retransmission** - Automatic lost packet resending
- [ ] **Duplicate detection** - Detection and removal of duplicates
- [ ] **Timeout management** - Adaptive timeout handling
- [ ] **Flow control** - Basic flow control

### 🔗 5.3 Network Manager Integration
- [ ] **UDP support** - Integration into NetworkManager
- [ ] **Mixed TCP/UDP** - Simultaneous TCP + UDP support
- [ ] **Channel system** - Reliable/unreliable channels
- [ ] **Public API** - UDP methods in NetworkEngine

---

## 🧵 MULTI-THREADING STRATEGY

### 📋 **Current Architecture (v0.1-0.2): Single-Thread with Multi-Thread Infrastructure**

**Status**: ✅ **RECOMMENDED APPROACH**
```cpp
// Current implementation (by design):
ErrorCodes NetworkEngine::Update() {
    return static_cast<NetworkManager*>(m_networkManager)->Update(); // Main thread
}
```

**Why this is optimal:**
- ✅ Infrastructure multi-thread ready (ThreadManager, ServiceLocator)
- ✅ Simple debugging and development
- ✅ Stable protocol development
- ✅ Easy transition when needed

### 🔄 **Transition Strategy (v0.3): Gradual Multi-Threading**

**Phase 1: Message Queue Infrastructure**
```cpp
class NetworkManager {
private:
    std::queue<NetworkMessage> m_incomingMessages;
    std::queue<NetworkMessage> m_outgoingMessages;
    std::mutex m_messageQueueMutex;
};
```

**Phase 2: Dedicated Network Thread**
```cpp
ErrorCodes NetworkManager::Update() {
    // Submit network I/O to dedicated thread
    ServiceLocator::ThreadManager().SubmitJob([this]() {
        this->ProcessNetworkIO();
    });
    
    // Process messages on main thread
    ProcessMessageQueues();
}
```

**Phase 3: Full Multi-Threading (v0.3+)**
```cpp
// Network thread: I/O operations
// Main thread: Game logic and message processing
// Worker threads: Message parsing and validation
```

### 🎯 **Benefits of Gradual Transition:**

1. **v0.1-0.2 (Single-thread)**: Stable protocol development
2. **v0.3 (Multi-thread)**: Better performance for UDP + multiple clients
3. **v0.4+ (Optimized)**: Advanced features with proven threading model

### ⚠️ **Key Considerations:**

- **Message ordering**: Ensure TCP message order preservation
- **Connection state**: Thread-safe client connection management  
- **Error handling**: Propagate network errors to main thread
- **Performance**: Monitor latency impact of thread switching

---

## 🎮 PHASE 6: GAMING FEATURES (TO DO ❌) - **VERSION 0.4**

> **Version 0.4 Release Criteria:**
> - Performance optimizations (delta compression, message batching)
> - Basic client prediction and lag compensation
> - Server authority and anti-cheat foundation
> - Interest management for spatial filtering
> - Working example game (multiplayer pong)

### ⚡ 6.1 Performance Optimizations
- [ ] **Delta compression** - Send only changes
- [ ] **Message batching** - Group small messages
- [ ] **Priority queues** - Critical message prioritization
- [ ] **Interest management** - Spatial filtering of updates
- [ ] **Bandwidth limiting** - Per-client bandwidth limiting

### 🎯 6.2 Client Prediction & Lag Compensation
- [ ] **Clock synchronization** - Client/server clock sync
- [ ] **Client-side prediction** - Local movement prediction
- [ ] **Server reconciliation** - Correction of wrong predictions
- [ ] **Interpolation** - Smooth movement between updates
- [ ] **Extrapolation** - Prediction to compensate lag
- [ ] **Rollback system** - Rollback system for corrections

### 🛡️ 6.3 Anti-Cheat Foundation
- [ ] **Server authority** - Server validation of all actions
- [ ] **Rate limiting** - Message rate limiting per client
- [ ] **Bounds checking** - Movement bounds verification
- [ ] **Anomaly detection** - Suspicious behavior detection
- [ ] **Audit logging** - Secure logs for investigation

---

## 🔄 PHASE 7: ADVANCED SYNCHRONIZATION (TO DO ❌) - **VERSION 0.5**

> **Version 0.5 Release Criteria:**
> - Entity Component System with network replication
> - Advanced state management (snapshots, delta compression)
> - Reliable event system and RPC implementation
> - Ownership and authority delegation system
> - Complex multiplayer game example (simple FPS)

### 🎲 7.1 Entity Component System
- [ ] **Network entities** - Entities with unique global IDs
- [ ] **Replicable components** - Automatically synchronized components
- [ ] **Ownership system** - Who controls which entity
- [ ] **Authority delegation** - Authority transfer between clients
- [ ] **Spatial partitioning** - Spatial optimization of entities

### 📊 7.2 State Management
- [ ] **World snapshots** - Complete world state capture
- [ ] **Delta snapshots** - State differences
- [ ] **State compression** - State compression
- [ ] **Consistent state** - Global consistency guarantee
- [ ] **State prediction** - Future state prediction

### 📡 7.3 Event System
- [ ] **Reliable events** - Events with delivery guarantee
- [ ] **RPC system** - Typed Remote Procedure Calls
- [ ] **Event ordering** - Guaranteed event ordering
- [ ] **Event filtering** - Filtering by client/zone
- [ ] **Custom events** - Custom event support

---

## 🌐 PHASE 8: ADVANCED FEATURES (TO DO ❌) - **VERSION 0.6**

> **Version 0.6 Release Criteria:**
> - Session management and matchmaking system
> - NAT traversal and P2P networking capabilities
> - Voice chat and advanced communication features
> - Load balancing and server clustering
> - Geographic distribution support

### 🏠 8.1 Session Management
- [ ] **Room system** - Room creation and management
- [ ] **Lobby management** - Waiting lobbies and matchmaking
- [ ] **Session migration** - Migration between servers
- [ ] **Reconnection** - Automatic reconnection after disconnection
- [ ] **Spectator mode** - Observer mode for matches

### 🎯 8.2 Matchmaking System
- [ ] **Skill-based matching** - Level-based pairing
- [ ] **Queue management** - Intelligent waiting queues
- [ ] **ELO/Ranking** - Ranking system
- [ ] **Team balancing** - Automatic team balancing
- [ ] **Custom criteria** - Custom matching criteria

### 🌍 8.3 Advanced Networking
- [ ] **NAT Traversal** - STUN/TURN for P2P connections
- [ ] **Peer-to-peer** - P2P networking option for certain games
- [ ] **Load balancing** - Load distribution between servers
- [ ] **Server clustering** - Server cluster management
- [ ] **Geographic distribution** - Geographically distributed servers

### 🎙️ 8.4 Voice & Communication
- [ ] **Voice chat** - Real-time voice chat
- [ ] **Audio compression** - Optimized audio compression
- [ ] **Push-to-talk** - Push-to-talk system
- [ ] **Text chat** - Text chat with moderation
- [ ] **Communication channels** - Multiple communication channels

---

## 📊 PHASE 9: MONITORING & ANALYTICS (TO DO ❌) - **VERSION 0.7**

> **Version 0.7 Release Criteria:**
> - Real-time performance monitoring and metrics
> - Analytics and telemetry system
> - Developer tools (debug visualization, network simulation)
> - Comprehensive logging and crash reporting
> - Performance profiling and optimization tools

### 📈 9.1 Real-time Monitoring
- [ ] **Performance metrics** - Latency, throughput, packet loss
- [ ] **Connection monitoring** - Real-time connection status
- [ ] **Server health** - Server health monitoring
- [ ] **Client diagnostics** - Client-side diagnostics
- [ ] **Alert system** - Automatic alerts on anomalies

### 📋 9.2 Analytics & Telemetry
- [ ] **Usage analytics** - Usage statistics
- [ ] **Performance profiling** - Automatic performance profiling
- [ ] **Crash reporting** - Automatic crash reports
- [ ] **Network telemetry** - Detailed network telemetry
- [ ] **Custom metrics** - Customizable metrics

### 🔧 9.3 Developer Tools
- [ ] **Debug visualization** - Network state visualization
- [ ] **Network simulation** - Degraded network condition simulation
- [ ] **Stress testing** - Automated load testing
- [ ] **Profiling tools** - Integrated profiling tools

---

## 🚀 PHASE 10: PRODUCTION READY (TO DO ❌) - **VERSION 1.0** 🎉

> **Version 1.0 Release Criteria (PRODUCTION READY):**
> - Complete test suite with >80% code coverage
> - Full API documentation and integration guides
> - Multiple example games and tutorials
> - Performance benchmarks meeting all targets
> - Security testing and production deployment guides
> - Community-ready with open source examples

### 🧪 10.1 Testing & Validation
- [ ] **Unit tests** - Complete unit tests
- [ ] **Integration tests** - Automated integration tests
- [ ] **Load testing** - High-capacity load tests
- [ ] **Network condition testing** - Tests with latency/loss
- [ ] **Security testing** - Network security tests

### 📚 10.2 Documentation & Examples
- [ ] **API documentation** - Complete API documentation
- [ ] **Integration guides** - Detailed integration guides
- [ ] **Example games** - Concrete examples (Pong, simple FPS)
- [ ] **Best practices** - Best practices guide
- [ ] **Deployment guide** - Production deployment guide

### ⚡ 10.3 Performance & Optimization
- [ ] **Memory optimization** - Memory usage optimization
- [ ] **CPU optimization** - CPU performance optimization
- [ ] **Network optimization** - Network protocol optimization
- [ ] **Platform-specific optimizations** - Per-platform optimizations
- [ ] **Benchmarking suite** - Standardized benchmark suite

---

## 📋 VERSION ROADMAP

### 🎯 **Current: Version 0.1** (August 2025)
✅ **Completed Features:**
- Complete core infrastructure and TCP networking
- Basic testing framework
- Cross-platform support (Windows/Linux)

📋 **v0.1 Release Status:** 
- ⚠️ **Not officially released yet**
- All core features completed but needs final release preparation
- **Release blockers for v0.1:**
  - [ ] Create official release notes
  - [ ] Tag v0.1 in git repository
  - [ ] Package binaries for Windows/Linux
  - [ ] Update README with v0.1 usage instructions

🎯 **Suggested v0.1 Release:** End of August 2025 (after current sprint)

### 🚀 **Next: Version 0.2** (Target: September 2025)
🔄 **In Development:**
- TCP message framing and protocol
- Structured message system
- Basic serialization
- Connection health monitoring
- **Single-thread execution maintained** for stability

### 📅 **Future Versions:**
- **v0.3** (October 2025): **Multi-Threading Transition** + UDP Implementation
- **v0.4** (December 2025): Gaming Features & Performance Optimizations
- **v0.5** (February 2026): Advanced Synchronization
- **v0.6** (April 2026): Advanced Features
- **v0.7** (June 2026): Monitoring & Analytics
- **v1.0** (August 2026): Production Ready 🎉

---

## 🎯 IMMEDIATE PRIORITIES

### 🚨 URGENT (Week of August 18-24, 2025)
1. **TCP Message Framing Implementation** - ⚠️ CRITICAL: Add message length prefix to TCP stream
   - Implement 4-byte message length header
   - Modify Send/Receive methods to handle framed messages
   - Test with variable-length messages
   - **Keep single-thread execution** for stable development
   
2. **Fix Integration Testing** - Enhance existing test applications
   - Add structured message exchange in client/server tests
   - Implement ping/pong test with message validation
   - Add connection stability testing (auto-reconnect test)
   
3. **Prepare v0.1 Release** - Finalize first official release
   - Create comprehensive release notes for v0.1
   - Tag v0.1 in git repository  
   - Update README with build/usage instructions
   - Test release builds on Windows/Linux

4. **Code Quality & Documentation** - Clean up current implementation
   - Add comprehensive code comments to NetworkManager
   - Fix any compiler warnings in release build
   - Ensure consistent coding style

### 📈 SHORT TERM (August 25 - September 15, 2025)
**Priority:** Complete v0.1 release and start v0.2 development

1. **Finalize v0.1 Release** - Week 1 (Aug 25-31)
   - Complete v0.1 release preparation tasks
   - Official v0.1 tag and release
   - Community announcement and documentation
   
2. **Message Protocol Foundation** - Week 2-3 (Sep 1-14)
   - Design message header structure (type, size, ID, timestamp)
   - Implement basic message types (CONNECT, DISCONNECT, PING, PONG, DATA)
   - Add message validation and error handling
   
3. **Basic Serialization System** - Week 3-4 (Sep 8-15)
   - Create ISerializable interface
   - Implement serialization for primitive types (int, float, string, bool)
   - Add endianness handling for cross-platform compatibility

### 🎮 MEDIUM TERM (September - November 2025)
1. **Complete v0.2 - Message Protocol** - 3-4 weeks (September)
   - Finish TCP message framing and structured messages
   - Heartbeat/ping system implementation
   - **Maintain single-thread architecture** for protocol stability
   
2. **v0.3 Multi-Threading Transition** - 3-4 weeks (October)
   - **Phase 1**: Create thread-safe message queues between threads
   - **Phase 2**: Move NetworkManager::Update() to dedicated network thread
   - **Phase 3**: Implement job-based networking with ThreadManager
   - **Phase 4**: Performance testing and optimization
   
3. **UDP Implementation** - 4-6 weeks (October-November)
   - UdpSocket class with reliability layer
   - Packet acknowledgment and retransmission
   - Integration with multi-threaded NetworkManager
   
4. **Example Applications** - 2-3 weeks (November)
   - Simple chat application demonstrating multi-threading
   - Basic multiplayer pong game
   - Real-time position synchronization demo

### 🚀 LONG TERM (December 2025 - March 2026)
1. **Advanced Networking** - Entity component system integration
2. **Production Ready** - Monitoring, analytics, deployment tools
3. **Documentation & Tutorials** - Complete developer guides
4. **Community & Examples** - Open source examples and best practices

---

## 📈 SUCCESS METRICS

### Performance Targets
- **Latency**: < 50ms in LAN, < 100ms in WAN
- **Throughput**: Support 1000+ simultaneous clients
- **Packet loss**: < 0.1% in normal conditions
- **CPU usage**: < 10% for 100 clients
- **Memory usage**: < 1GB for 1000 clients

### Minimum Viable Features
- **TCP**: ✅ Bidirectional reliable communication
- **Message Protocol**: 🔄 Structured message system (in progress)
- **UDP**: ❌ Fast communication for real-time games
- **Multi-clients**: ✅ Support minimum 64 simultaneous clients
- **Cross-platform**: ✅ Windows and Linux

### Quality Assurance
- **Test Coverage**: Target >80% code coverage
- **Documentation**: Complete API reference and integration guides
- **Examples**: At least 3 working example applications
- **Performance**: Benchmark suite with regression detection

---

*Last update: August 18, 2025*
*Current Version: 0.1*
*Next Release: v0.2 (Target: September 2025)*
*Current Sprint: TCP Message Framing (Aug 18-24)*
*Final Goal: v1.0 Production Ready (August 2026)*
