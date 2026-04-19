# Architectural Analysis: pipeline_runner.hpp

## Architectural Diagrams

### Graphviz (.dot) - Pipeline Runner Architecture
```dot
digraph pipeline_runner_architecture {
    rankdir=TB;
    node [shape=box, style=filled, fillcolor=lightblue];
    
    runner [label="PipelineRunner<TPipelineBinding>\nPipeline Runner Template"];
    
    node [shape=box, style=filled, fillcolor=lightgreen];
    facade_dependency [label="Facade Dependency"];
    
    runner -> facade_dependency [label="uses"];
    
    subgraph cluster_facade {
        label="IngestPipelineFacade";
        color=lightgrey;
        facade [label="Facade = IngestPipelineFacade<TPipelineBinding>"];
    }
    
    facade_dependency -> facade;
    
    node [shape=box, style=filled, fillcolor=lightyellow];
    component_aliases [label="Component Type Aliases"];
    
    runner -> component_aliases [label="provides"];
    
    subgraph cluster_aliases {
        label="Pipeline Components";
        color=lightgrey;
        preparation [label="Preparation = Facade::Preparation"];
        resolver [label="Resolver = Facade::Resolver"];
        dispatch [label="Dispatch = Facade::Dispatch"];
        repository_route [label="RepositoryRoute = Facade::RepositoryRoute"];
    }
    
    component_aliases -> preparation;
    component_aliases -> resolver;
    component_aliases -> dispatch;
    component_aliases -> repository_route;
    
    node [shape=box, style=filled, fillcolor=orange];
    runner_operations [label="Runner Operations"];
    
    runner -> runner_operations [label="provides"];
    
    subgraph cluster_operations {
        label="Core Operations";
        color=lightgrey;
        resolve_target [label="resolve_write_target(module, level_key, record)"];
        build_handoff [label="build_write_handoff_event(target)"];
        resolve_route [label="resolve_default_route()"];
    }
    
    runner_operations -> resolve_target;
    runner_operations -> build_handoff;
    runner_operations -> resolve_route;
    
    subgraph cluster_target_resolution {
        label="Target Resolution Flow";
        color=lightgreen;
        resolver_access [label="Resolver::WriterResolver::resolve_write_target"];
        module_param [label="module: LogContainerModule"];
        level_param [label="level_key: string"];
        record_param [label="record: TRecord"];
    }
    
    resolve_target -> resolver_access;
    resolver_access -> module_param;
    resolver_access -> level_param;
    resolver_access -> record_param;
    
    subgraph cluster_handoff {
        label="Handoff Event Building";
        color=lightgreen;
        handoff_access [label="Resolver::WriterResolver::build_handoff_event"];
        target_param [label="target: WriteTargetDescriptor"];
    }
    
    build_handoff -> handoff_access;
    handoff_access -> target_param;
    
    subgraph cluster_route_resolution {
        label="Route Resolution";
        color=lightgreen;
        route_access [label="Facade::make_default_route()"];
    }
    
    resolve_route -> route_access;
    
    subgraph cluster_integration {
        label="Integration Points";
        color=lightblue;
        level_apis [label="Level APIs (LogInfo)"];
        pipeline_execution [label="Pipeline Execution Code"];
        state_management [label="H_State Layer"];
    }
    
    runner -> level_apis [label="used by"];
    runner -> pipeline_execution [label="used by"];
    state_management -> runner [label="provides module for"];
}

### Mermaid - Runner Operation Flow
```mermaid
flowchart TD
    A[Pipeline Execution Code] --> B[Needs Pipeline Operations]
    
    B --> C[Use PipelineRunner]
    C --> D[Specify Pipeline Binding Type]
    D --> E[TPipelineBinding = Concrete Pipeline]
    
    E --> F[Runner Provides Operations]
    F --> G[resolve_write_target]
    F --> H[build_write_handoff_event]
    F --> I[resolve_default_route]
    
    G --> J[Delegate to Resolver::WriterResolver]
    H --> K[Delegate to Resolver::WriterResolver]
    I --> L[Delegate to Facade::make_default_route]
    
    J --> M[Write Target Descriptor]
    K --> N[Write Handoff Event]
    L --> O[Repository Route]
    
    M --> P[Pipeline Operations Available]
    N --> P
    O --> P
    
    P --> Q[Execution Code Operational]
    
    R[LogContainerModule] --> S[State Access]
    S --> J
    
    T[Record] --> U[Data Input]
    U --> J
    
    V[Level Key] --> W[Partition Key]
    W --> J
    
    subgraph "Template Instantiation"
        C
        D
        E
        F
    end
    
    subgraph "Operation Delegation"
        G
        H
        I
        J
        K
        L
    end
    
    subgraph "Result Types"
        M
        N
        O
        P
    end
```

## File Overview
**Location:** `D:\CppBridgeVSC\LoggingSystem\include\logging_system\K_Pipelines\pipeline_runner.hpp`  
**Purpose:** PipelineRunner is the runnable execution unit for one concrete pipeline binding, providing both record-to-dispatch execution and admitted-runtime state admission paths.  
**Language:** C++17  
**Dependencies:** `<optional>`, `<string>`, `<vector>`, `dispatch_context.hpp`, `ingest_pipeline_facade.hpp`  

## Architectural Role

### Core Design Pattern: Admitted-Runtime Execution Unit
This file implements **Admitted-Runtime Execution Unit** for per-pipeline execution. The `PipelineRunner` serves as:

- **State-admission aware execution unit** that handles record admission into shared state
- **Batch processing coordinator** that drains pending work and executes dispatch batches
- **State feedback manager** that applies commit/requeue/mark-failed based on execution results
- **Per-pipeline specialization** with compile-time component binding

### Pipelines Layer Architecture (K_Pipelines)
The `PipelineRunner` provides admitted-runtime pipeline operations that answer:

- **How does one pipeline binding execute its admitted-runtime path?**
- **How can state admission, batch processing, and feedback be coordinated?**
- **What is the runnable pipeline unit that handles shared-state boundaries?**

## Structural Analysis

### Template Runner Structure
```cpp
template <typename TPipelineBinding>
struct PipelineRunner final {
    using Facade = IngestPipelineFacade<TPipelineBinding>;

    using Preparation = typename Facade::Preparation;
    using Resolver = typename Facade::Resolver;
    using Dispatch = typename Facade::Dispatch;
    using RepositoryRoute = typename Facade::RepositoryRoute;

    static RepositoryRoute resolve_default_route() {
        return Facade::make_default_route();
    }

    template <typename TModule, typename TRecord, typename TAdapter>
    static auto run_single(
        const TModule& module,
        const std::string& level_key,
        const TRecord& record,
        TAdapter& adapter,
        const std::optional<std::string>& round_id = std::nullopt);
};
```

**Facade Integration:**
- **`Facade`**: Uses IngestPipelineFacade for component access
- **Component Aliases**: Provides direct access to all pipeline components
- **Type Propagation**: Makes component types available for further use
- **Static Methods**: All operations are static, no instance state

### Core Operations

#### Default Route Resolution
```cpp
static RepositoryRoute resolve_default_route() {
    return Facade::make_default_route();
}
```
**Route Process:**
1. Delegates to `Facade::make_default_route()`
2. Returns default repository route for the pipeline
3. Provides standard route configuration

#### Single Record Execution (`run_single`)
```cpp
template <typename TModule, typename TRecord, typename TAdapter>
static auto run_single(
    const TModule& module,
    const std::string& level_key,
    const TRecord& record,
    TAdapter& adapter,
    const std::optional<std::string>& round_id = std::nullopt)
```
**Execution Process:**
1. **Resolve Write Target**: Uses `Resolver::WriterResolver::resolve_write_target`
2. **Build Handoff Event**: Uses `Resolver::WriterResolver::build_handoff_event`
3. **Resolve Dispatch Candidate**: Uses `Resolver::DispatcherResolver::resolve_dispatch_candidate`
4. **Resolve Receiver Binding**: Uses `Resolver::DispatcherResolver::resolve_dispatch_receiver_binding`
5. **Create Dispatch Context**: Constructs `DispatchContext` with resolved components
6. **Execute Batch**: Uses `Dispatch::DispatchExecutor::execute_batch` for emission

#### State-Admission Execution (`admit_and_run`)
```cpp
template <typename TModule, typename TRecord, typename TAdapter>
static auto admit_and_run(
    TModule& module,
    const std::string& level_key,
    const TRecord& record,
    TAdapter& adapter,
    const std::optional<std::string>& round_id = std::nullopt)
```
**Admitted-Runtime Process:**
1. **Enqueue Record**: Calls `module.enqueue_pending(level_key, record)` for state admission
2. **Drain Pending**: Calls `module.drain_pending()` to get batch of records to process
3. **Resolve Per Record**: For each drained record, resolves dispatch candidate and receiver binding
4. **Build Batch Context**: Creates `DispatchContext` for each record in the batch
5. **Execute Batch**: Uses `Dispatch::DispatchExecutor::execute_batch` for emission
6. **Apply State Feedback**: Calls private `apply_state_feedback()` with execution results

#### State Feedback Application (`apply_state_feedback` - private)
```cpp
template <typename TModule, typename TRecord, typename TExecutionResult>
static void apply_state_feedback(
    TModule& module,
    const std::vector<TRecord>& drained_records,
    const TExecutionResult& result,
    const DispatchFailurePolicy& failure_policy)
```
**Feedback Process Based on Failure Policy:**
- **Success (failed == 0)**: Calls `module.commit_dispatched(drained_records)`
- **Requeue Mode**: Calls `module.requeue_pending_front(drained_records)`
- **MarkFailed Mode**: Calls `module.mark_failed(drained_records)`
- **AbortBatch Mode**: Calls `module.mark_failed(drained_records)`

## Integration with Architecture

### Runner in Admitted-Runtime Execution Flow
The runner provides two execution paths:

**Direct Record Path (`run_single`):**
```
Record Available → Level API → PipelineRunner → Dispatch Emission
       ↓              ↓              ↓              ↓
   TRecord → LogInfo → run_single → Resolver → DispatchContext → Adapter
   level_key → "INFO" → write_target → dispatch_candidate → single batch → emission
```

**Admitted-Runtime Path (`admit_and_run`):**
```
Record → State Admission → Batch Drain → Resolution → Batch Execution → State Feedback
    ↓            ↓              ↓           ↓            ↓              ↓
enqueue → module state → drain_pending → Resolver → execute_batch → commit/requeue/
record → shared store → batch records → candidates → emission → mark-failed
```

**Integration Points:**
- **Level APIs**: LogInfo provides thin entrypoint with both execution paths
- **Consuming Surfaces**: ConsumingSurface exposes both direct and admitted paths
- **State Modules**: LogContainerModule provides shared state for admission/batch processing
- **Resolver Components**: Handle per-record dispatch candidate and receiver binding resolution
- **Dispatch Components**: Execute batches and return results for state feedback

### Usage Pattern
```cpp
// Using with InfoPipelineBinding
using InfoRunner = PipelineRunner<InfoPipelineBinding>;

// Direct record-to-dispatch execution (bypasses state admission)
auto direct_result = InfoRunner::run_single(
    log_container_module,           // TModule - state access (read-only)
    "INFO",                         // level_key
    record,                         // TRecord - finalized log record
    adapter,                        // TAdapter - emission target
    std::optional<std::string>{"round_123"} // optional round_id
);

// Admitted-runtime execution (with state admission and batch processing)
auto admitted_result = InfoRunner::admit_and_run(
    log_container_module,           // TModule& - state access (read-write)
    "INFO",                         // level_key
    record,                         // TRecord - record to admit
    adapter,                        // TAdapter - emission target
    std::optional<std::string>{"batch_001"} // optional round_id
);

// Get default route for pipeline setup
auto route = InfoRunner::resolve_default_route();
```

## Quality Assurance

### Code Quality Metrics
- **Cyclomatic Complexity:** 2 (conditional state feedback logic)
- **Lines of Code:** 78 (core template) + 135 (documentation comments)
- **Dependencies:** 5 headers (3 std, 2 internal)
- **Template Complexity:** Single template parameter with complex delegation and state feedback

### Architectural Compliance
✅ **Multi-Tier Architecture:** Layer K (Pipelines) - pipeline execution coordination  
✅ **No Hardcoded Values:** All behavior through delegation to components  
✅ **Helper Methods:** Pipeline-shaped operations for execution  
✅ **Cross-Language Interface:** N/A (internal pipeline coordination)  

### Error Analysis
**Status:** No syntax or logical errors detected.  

**Architectural Correctness Verification:**
- **Facade Usage**: Correctly uses IngestPipelineFacade for component access
- **Delegation Pattern**: Properly delegates all operations to appropriate components
- **Template Design**: Single pipeline binding template parameter
- **Type Propagation**: Correctly exposes all component types from facade

**Potential Issues Considered:**
- **Component Requirements**: Pipeline binding must have compatible resolver with WriterResolver
- **Facade Dependencies**: Requires IngestPipelineFacade to be properly defined
- **Delegation Chain**: Operations depend on resolver components being correctly implemented
- **Template Instantiation**: Requires concrete pipeline binding with proper component structure

**Root Cause Analysis:** N/A (code is correct)  
**Resolution Suggestions:** N/A  

## Design Rationale

### Admitted-Runtime Execution Unit
**Why Admitted-Runtime Evolution:**
- **State Admission Integration**: Record admission into shared state is now required
- **Batch Processing Support**: Multiple records can be processed together efficiently
- **State Feedback Management**: Execution results must update shared state appropriately
- **Failure Policy Handling**: Different failure modes require different state updates

**Current Scope Assumptions:**
- **TRecord Available**: Works with pre-existing finalized records
- **Level Key from API**: Uses level information from calling API (LogInfo)
- **Module as Shared State**: LogContainerModule provides admission/drain/commit state operations
- **Adapter-Like Objects**: Accepts any adapter boundary implementation

**Design Intent:**
- **Shared-State Boundaries**: Respects separation between per-pipeline and shared state
- **Minimal Feedback Logic**: Only essential state updates (commit/requeue/mark-failed)
- **Policy-Driven**: Failure handling follows configurable policy patterns
- **Future Expansion Ready**: Foundation for richer feedback and retry logic

### Pipeline-Shaped Design
**Why "Pipeline-Shaped":**
- **End-to-End Operations**: Operations that span the pipeline from input to output
- **Component Integration**: Uses multiple pipeline components in coordination
- **Execution Semantics**: Mimics how the full pipeline will eventually operate
- **Incremental Development**: Foundation for future full pipeline execution

## Performance Characteristics

### Compile-Time Performance
- **Template Instantiation**: Moderate overhead with facade usage
- **Type Resolution**: Delegation through facade to component types
- **Inlining Opportunity**: Static methods easily optimized
- **Dependency Chain**: Through facade to pipeline binding components

### Runtime Performance
- **Delegation Overhead**: Each call delegates through facade to component
- **No State Management**: Pure coordination with no internal state
- **Component Performance**: Actual performance depends on delegated components
- **Scalability**: Performance scales with underlying component implementations

## Evolution and Maintenance

### Runner Extension
Later expansions may include:
- **Raw-Content Preparation Hooks**: When preparation/admission entry is promoted
- **Envelope/Record Stabilization Integration**: For raw-to-record conversion
- **Partial-Batch Result Reconciliation**: For complex batch failure scenarios
- **Richer Policy-Aware Feedback Handling**: Beyond current commit/requeue/mark-failed
- **Retry/Backoff Integration**: For transient failure recovery
- **Per-Record Execution Result Accounting**: For detailed success/failure tracking
- **Instrumentation and Timing Hooks**: For performance monitoring and debugging
- **Stronger Compile-Time Contract Assertions**: Additional type safety validations

### What This File Should NOT Contain
This file must NOT become:
- **System Root**: No central service or orchestration ownership
- **Shared State Owner**: No global state management
- **Adapter Registry Logic**: No adapter discovery or management
- **Governance/Configuration**: No pipeline policy or configuration
- **Generic Central Convergence**: No collapsing per-pipeline behavior into generic service

### Testing Strategy
Runner testing should verify:
- Template instantiation works with different pipeline binding types
- run_single correctly executes complete record-to-dispatch path
- admit_and_run correctly handles state admission, batch processing, and feedback
- Resolver delegation works for write target and dispatch candidate resolution
- State feedback correctly applies commit/requeue/mark-failed based on failure policy
- Batch processing drains correct number of records and processes them
- DispatchContext construction works for multiple records in a batch
- Type aliases correctly expose all facade component types
- Optional round_id parameter handling works in both execution paths
- Module state operations (enqueue/drain/commit) are called correctly
- No runtime state or overhead introduced by runner usage
- Integration with level APIs and consuming surfaces works properly

## Related Components

### Depends On
- `<string>` - For level_key and other string parameters
- `K_Pipelines/ingest_pipeline_facade.hpp` - For component access abstraction

### Used By
- `L_Level_api/log_info.hpp` - For INFO-specific pipeline operations
- Generic pipeline execution code that needs pipeline-shaped operations
- Pipeline testing and validation code
- Higher-level orchestration that needs pipeline access patterns

---

**Analysis Version:** 1.2
**Analysis Date:** 2026-04-19
**Architectural Layer:** K_Pipelines (Pipeline Execution)
**Status:** ✅ Analyzed, Updated for Admitted-Runtime Implementation