#include "OpcUaServer.hpp"
#include <iostream>

/* ——— ctor / dtor ———————————————————————————————————————— */
OpcUaServer::OpcUaServer(const std::string &nsName)
{
    m_server = UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(m_server));

    m_nsIdx = UA_Server_addNamespace(m_server, nsName.c_str());
    initAddressSpace();

    // register write callback on /Machine/Command
    UA_ValueCallback cb;
    cb.onRead  = nullptr;
    cb.onWrite = &OpcUaServer::writeCallback;
    UA_Server_setVariableNode_valueCallback(m_server, m_cmdNodeId, cb);

    // make this instance available inside callback
    UA_Server_setNodeContext(m_server, m_cmdNodeId, this);
}

OpcUaServer::~OpcUaServer()
{
    UA_Server_delete(m_server);
}

/* ——— public API ———————————————————————————————————————— */
/* OpcUaServer.cpp */
void OpcUaServer::run()
{
    std::cout << "Server on opc.tcp://0.0.0.0:4840\n";
    UA_Server_run(m_server, &m_running);    // ✔ types now match
}

void OpcUaServer::stop() { m_running = false; }

void OpcUaServer::onRunningChanged(const std::function<void(bool)> &cb)
{ m_onRunningChanged = cb; }

/* ——— static write callback ———————————————————————————— */
/* OpcUaServer.cpp */
/* OpcUaServer.cpp */
void OpcUaServer::writeCallback(UA_Server *server,
                                const UA_NodeId *, void *,
                                const UA_NodeId *, void *nodeCtx,
                                const UA_NumericRange *,
                                const UA_DataValue *data)
{
    auto *self = static_cast<OpcUaServer*>(nodeCtx);

    if(!data || !UA_Variant_hasScalarType(&data->value, &UA_TYPES[UA_TYPES_STRING]))
        return;                                         // type mismatch → ignore

    const UA_String uaStr = *static_cast<UA_String*>(data->value.data);
    const std::string cmd(reinterpret_cast<char*>(uaStr.data), uaStr.length);
    bool newState = (cmd == "ON" || cmd == "on");

    /* update /Machine/Status/Running */
    UA_Boolean uaBool = newState;
    UA_Variant v;
    UA_Variant_setScalar(&v, &uaBool, &UA_TYPES[UA_TYPES_BOOLEAN]);
    UA_Server_writeValue(server, self->m_runNodeId, v);

    if (self->m_onRunningChanged)
        self->m_onRunningChanged(newState);
}



/* ——— address-space helper ————————————————————————————— */
void OpcUaServer::initAddressSpace()
{
    /* /Machine folder */
    UA_NodeId machineId;
    UA_ObjectAttributes attr = UA_ObjectAttributes_default;
    attr.displayName = UA_LOCALIZEDTEXT("en-US", "Machine");
    UA_Server_addObjectNode(m_server,
        UA_NODEID_STRING(m_nsIdx, "/Machine"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
        UA_QUALIFIEDNAME(m_nsIdx, "Machine"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE),
        attr, nullptr, &machineId);

    /* /Machine/Status folder */
    UA_NodeId statusId;
    attr.displayName = UA_LOCALIZEDTEXT("en-US", "Status");
    UA_Server_addObjectNode(m_server,
        UA_NODEID_STRING(m_nsIdx, "/Machine/Status"),
        machineId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(m_nsIdx, "Status"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE),
        attr, nullptr, &statusId);

    /* /Machine/Command  (String, R/W) */
    m_cmdNodeId = UA_NODEID_STRING(m_nsIdx, "/Machine/Command");
    UA_VariableAttributes cmdAttr = UA_VariableAttributes_default;
    UA_String emptyStr = UA_STRING("");
    UA_Variant_setScalar(&cmdAttr.value, &emptyStr, &UA_TYPES[UA_TYPES_STRING]);
    cmdAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Command");
    cmdAttr.dataType = UA_TYPES[UA_TYPES_STRING].typeId;
    cmdAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_Server_addVariableNode(m_server,
        m_cmdNodeId,
        machineId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(m_nsIdx, "Command"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
        cmdAttr, nullptr, nullptr);

    /* /Machine/Status/Running (Boolean, R) */
    m_runNodeId = UA_NODEID_STRING(m_nsIdx, "/Machine/Status/Running");
    UA_VariableAttributes runAttr = UA_VariableAttributes_default;
    UA_Boolean initial = false;
    UA_Variant_setScalar(&runAttr.value, &initial, &UA_TYPES[UA_TYPES_BOOLEAN]);
    runAttr.displayName   = UA_LOCALIZEDTEXT("en-US", "Running");
    runAttr.dataType      = UA_TYPES[UA_TYPES_BOOLEAN].typeId;
    runAttr.accessLevel   = UA_ACCESSLEVELMASK_READ;
    UA_Server_addVariableNode(m_server,
        m_runNodeId,
        statusId,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(m_nsIdx, "Running"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
        runAttr, nullptr, nullptr);
}
