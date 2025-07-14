#pragma once
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <atomic>
#include <string>
#include <functional>

/*  Simple OPC UA server that exposes:
 *      /Machine/Command      (String, R/W)
 *      /Machine/Status/Running  (Boolean, R)
 *
 *  When “ON” / “OFF” is written to Command, the callback toggles
 *  the Boolean Status node and notifies application code via an
 *  std::function<void(bool newState)> delegate.
 */
class OpcUaServer
{
public:
    explicit OpcUaServer(const std::string &namespaceName = "Machine Namespace");
    ~OpcUaServer();

    /* non-copyable */
    OpcUaServer(const OpcUaServer&)            = delete;
    OpcUaServer &operator=(const OpcUaServer&) = delete;

    /* start the run loop (blocking) */
    void run();
    /* ask the loop to stop (thread-safe) */
    void stop();

    /* optional: set external handler to be called when state changes */
    void onRunningChanged(const std::function<void(bool)> &cb);

private:

volatile UA_Boolean m_running {true};   // ⬅ replaces std::atomic_bool

static void writeCallback(UA_Server *server,
                          const UA_NodeId *sessionId, void *sessionCtx,
                          const UA_NodeId *nodeId,   void *nodeCtx,
                          const UA_NumericRange *range,
                          const UA_DataValue *data);




    void initAddressSpace();

    UA_Server          *m_server      {nullptr};
    UA_NodeId           m_cmdNodeId;
    UA_NodeId           m_runNodeId;
    std::function<void(bool)> m_onRunningChanged;   // delegate
    std::atomic_bool    m_runningFlag {true};
    UA_UInt16           m_nsIdx       {0};
};
