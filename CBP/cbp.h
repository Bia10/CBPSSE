#pragma once

//#define _CBP_MEASURE_PERF
//#define _CBP_SHOW_STATS

namespace CBP
{

    struct UTTask
    {
        enum UTTAction : uint32_t {
            kActionAdd,
            kActionRemove,
            kActionUpdateConfig,
            kActionUpdateConfigAll,
            kActionReset,
            kActionUIUpdateCurrentActor,
            kActionUpdateGroupInfoAll,
            kActionPhysicsReset,
            kActionNiNodeUpdate,
            kActionNiNodeUpdateAll,
            kActionWeightUpdate,
            kActionWeightUpdateAll
        };

        UTTAction m_action;
        SKSE::ObjectHandle m_handle;
    };

    class UpdateTask :
        public TaskDelegateFixed,
        ILog
    {
        typedef std::unordered_set<SKSE::ObjectHandle> handleSet_t;

        class UpdateWeightTask :
            public TaskDelegate
        {
        public:
            UpdateWeightTask(SKSE::ObjectHandle a_handle);

            virtual void Run();
            virtual void Dispose();
        private:
            SKSE::ObjectHandle m_handle;
        };

    public:
        UpdateTask();

        virtual void Run();

        __forceinline void CullActors();

        __forceinline void UpdatePhase1();
        __forceinline void UpdateActorsPhase2(float a_timeStep);

        __forceinline void UpdatePhase2(float a_timeStep, float a_timeTick, float a_maxTime);
        __forceinline void UpdatePhase2Collisions(float a_timeStep, float a_timeTick, float a_maxTime);

#ifdef _CBP_ENABLE_DEBUG
        void UpdatePhase3();
#endif

        void PhysicsTick();

        void AddActor(SKSE::ObjectHandle handle);
        void RemoveActor(SKSE::ObjectHandle handle);
        void UpdateConfigOnAllActors();
        void UpdateGroupInfoOnAllActors();
        void UpdateConfig(SKSE::ObjectHandle handle);
        void ApplyForce(SKSE::ObjectHandle a_handle, uint32_t a_steps, const std::string& a_component, const NiPoint3& a_force);
        void ClearActors();
        void Reset();
        void PhysicsReset();
        void NiNodeUpdate(SKSE::ObjectHandle a_handle);
        void WeightUpdate(SKSE::ObjectHandle a_handle);
        void NiNodeUpdateAll();
        void WeightUpdateAll();

        void UpdateDebugRenderer();

        void AddTask(const UTTask& task);
        void AddTask(UTTask&& task);
        void AddTask(UTTask::UTTAction a_action);
        void AddTask(UTTask::UTTAction a_action, SKSE::ObjectHandle a_handle);

        inline const auto& GetSimActorList() {
            return m_actors;
        }

        inline auto& GetProfiler() {
            return m_profiler;
        }

        inline void UpdateTimeTick(float a_val) {
            m_averageInterval = a_val;
        }

        FN_NAMEPROC("UpdateTask")
    private:
        bool IsTaskQueueEmpty();
        void ProcessTasks();
        void GatherActors(handleSet_t& a_out);

        simActorList_t m_actors;
        std::queue<UTTask> m_taskQueue;

        ICriticalSection m_taskLock;

        float m_timeAccum;
        float m_averageInterval;

        static std::atomic<uint64_t> m_nextGroupId;

        Profiler m_profiler;
    };

    class DCBP :
        ILog,
        IConfigINI
    {
        class BackLog
        {
            typedef std::vector<std::string> vec_t;

            using iterator = typename vec_t::iterator;
            using const_iterator = typename vec_t::const_iterator;

        public:

            BackLog(size_t a_limit) :
                m_limit(a_limit)
            {
            }

            [[nodiscard]] inline const_iterator begin() const noexcept {
                return m_data.begin();
            }
            [[nodiscard]] inline const_iterator end() const noexcept {
                return m_data.end();
            }

            inline void Lock() {
                m_lock.Enter();
            }

            inline void Unlock() {
                m_lock.Leave();
            }

            inline void Add(const char* a_string)
            {
                m_lock.Enter();

                m_data.emplace_back(a_string);
                if (m_data.size() > m_limit)
                    m_data.erase(m_data.begin());

                m_lock.Leave();
            }

            ICriticalSection m_lock;
            std::vector<std::string> m_data;

            size_t m_limit;
        };

        enum SerializationVersion {
            kDataVersion1 = 1
        };

        class KeyPressHandler : public KeyEventHandler
        {
        public:
            virtual void ReceiveEvent(KeyEvent, UInt32) override;
        private:
            bool combo_down;
        };

        class EventHandler :
            public BSTEventSink <TESObjectLoadedEvent>,
            public BSTEventSink <TESInitScriptEvent>,
            public BSTEventSink <TESFastTravelEndEvent>
        {
        protected:
            virtual EventResult	ReceiveEvent(TESObjectLoadedEvent* evn, EventDispatcher<TESObjectLoadedEvent>* dispatcher) override;
            virtual EventResult	ReceiveEvent(TESInitScriptEvent* evn, EventDispatcher<TESInitScriptEvent>* dispatcher) override;
            virtual EventResult	ReceiveEvent(TESFastTravelEndEvent* evn, EventDispatcher<TESFastTravelEndEvent>* dispatcher) override;
        public:
            static EventHandler* GetSingleton() {
                static EventHandler handler;
                return &handler;
            }
        };

        class ToggleUITask :
            public TaskDelegateStatic
        {
            enum class ToggleResult
            {
                kResultNone,
                kResultEnabled,
                kResultDisabled
            };
        public:
            virtual void Run();
        private:
            ToggleResult Toggle();
        };
        
        class SwitchUITask :
            public TaskDelegate
        {
        public:
            SwitchUITask(bool a_switch) :
                m_switch(a_switch)
            {
            }

            virtual void Run();
            virtual void Dispose() {
                delete this;
            };
        private:
            bool m_switch;
        };

        class ApplyForceTask :
            public TaskDelegate
        {
        public:
            ApplyForceTask(
                SKSE::ObjectHandle a_handle,
                uint32_t a_steps,
                const std::string& a_component,
                const NiPoint3& a_force
            );

            virtual void Run();
            virtual void Dispose() {
                delete this;
            }
        private:
            SKSE::ObjectHandle m_handle;
            uint32_t m_steps;
            std::string m_component;
            NiPoint3 m_force;
        };

        class UpdateActorCacheTask :
            public TaskDelegateStatic
        {
        public:
            virtual void Run();
        };

        typedef void (*mainLoopUpdateFunc_t)(void* p1);

    public:

        static void Initialize();

        static void MainLoop_Hook(void* p1);

        static void DispatchActorTask(Actor* actor, UTTask::UTTAction action);
        static void DispatchActorTask(SKSE::ObjectHandle handle, UTTask::UTTAction action);

        [[nodiscard]] inline static const auto& GetSimActorList() {
            return m_Instance.m_updateTask.GetSimActorList();
        }

        static void UpdateConfigOnAllActors();
        static void UpdateGroupInfoOnAllActors();
        static void ResetPhysics();
        static void NiNodeUpdate();
        static void NiNodeUpdate(SKSE::ObjectHandle a_handle);
        static void WeightUpdate();
        static void ResetActors();
        static void UpdateDebugRendererState();
        static void UpdateDebugRendererSettings();
        static void UpdateProfilerSettings();
        static void ApplyForce(SKSE::ObjectHandle a_handle, uint32_t a_steps, const std::string& a_component, const NiPoint3& a_force);
        [[nodiscard]] static bool ActorHasNode(SKSE::ObjectHandle a_handle, const std::string& a_node);
        [[nodiscard]] static bool ActorHasConfigGroup(SKSE::ObjectHandle a_handle, const std::string& a_cg);
        [[nodiscard]] static bool GlobalHasConfigGroup(const std::string& a_cg);

        static bool SaveAll();

        inline static bool SaveGlobals() {
            return m_Instance.m_serialization.SaveGlobals();
        }

        inline static void MarkGlobalsForSave() {
            m_Instance.m_serialization.MarkForSave(CBP::ISerialization::kGlobals);
        }

        inline static void MarkForSave(CBP::ISerialization::Group a_grp) {
            m_Instance.m_serialization.MarkForSave(a_grp);
        }

        inline static bool SaveCollisionGroups() {
            return m_Instance.m_serialization.SaveCollisionGroups();
        }

        inline static bool SavePending() {
            return m_Instance.m_serialization.SavePending();
        }

        inline static bool SaveToDefaultGlobalProfile() {
            return m_Instance.m_serialization.SaveToDefaultGlobalProfile();
        }

        static bool ExportData(const std::filesystem::path& a_path);
        static bool ImportData(const std::filesystem::path& a_path);
        static bool ImportGetInfo(const std::filesystem::path& a_path, CBP::importInfo_t& a_out);

        [[nodiscard]] inline static const auto& GetLastSerializationException() {
            return m_Instance.m_serialization.GetLastException();
        }

        static void UIQueueUpdateCurrentActor();
        inline static void UIQueueUpdateCurrentActorA() {
            m_Instance.m_uiContext.QueueUpdateCurrentActor();
        }

        inline static void QueueActorCacheUpdate() {
            DTasks::AddTask(&m_Instance.m_updateActorCacheTask);
        }

        [[nodiscard]] inline static auto& GetUpdateTask() {
            return m_Instance.m_updateTask;
        }

        [[nodiscard]] inline static auto GetWorld() {
            return m_Instance.m_world;
        }

        [[nodiscard]] inline static auto& GetPhysicsCommon() {
            return m_Instance.m_physicsCommon;
        }

        static void ResetProfiler();
        static void SetProfilerInterval(long long a_interval);

        [[nodiscard]] inline static auto& GetProfiler() {
            return m_Instance.m_updateTask.GetProfiler();
        }

        inline static void Lock() {
            m_Instance.m_lock.Enter();
        }

        inline static void Unlock() {
            m_Instance.m_lock.Leave();
        }

        inline static auto& GetLock() {
            return m_Instance.m_lock;
        }

        [[nodiscard]] inline static const auto& GetDriverConfig()
        {
            return m_Instance.conf;
        }

        inline static auto &GetRenderer() {
            return m_Instance.m_renderer;
        }

        [[nodiscard]] inline static auto& GetBackLog() {
            return m_Instance.m_backlog;
        }

        FN_NAMEPROC("CBP")
    private:
        DCBP();

        virtual void LoadConfig();

        bool ProcessUICallbackImpl();

        static void MessageHandler(Event m_code, void* args);
        static void OnLogMessage(Event, void* args);

        static void RevertHandler(Event m_code, void* args);
        static void LoadGameHandler(SKSESerializationInterface* intfc, UInt32 type, UInt32 length, UInt32 version);
        static void SaveGameHandler(Event m_code, void* args);

        template <typename T>
        static bool SaveRecord(SKSESerializationInterface* intfc, UInt32 a_type, T a_func);
        template <typename T>
        static bool LoadRecord(SKSESerializationInterface* intfc, UInt32 a_type, T a_func);

        static void OnD3D11PostCreate_CBP(Event code, void* data);
        static void Present_Pre();

        static uint32_t ConfigGetComboKey(int32_t param);

        static bool UICallback();

        void EnableUI();
        void DisableUI();
        bool RunEnableUIChecks();

        struct
        {
            bool ui_enabled;
            bool debug_renderer;
            bool force_ini_keys;

            UInt32 comboKey;
            UInt32 showKey;
        } conf;

        KeyPressHandler inputEventHandler;
        UIContext m_uiContext;
        uint32_t m_loadInstance;

        UpdateTask m_updateTask;
        ToggleUITask m_taskToggle;
        UpdateActorCacheTask m_updateActorCacheTask;

        struct {
            bool show;
            bool lockControls;
        } uiState;

        ISerialization m_serialization;
        std::unique_ptr<CBP::Renderer> m_renderer;

        r3d::PhysicsWorld* m_world;
        r3d::PhysicsCommon m_physicsCommon;

        mainLoopUpdateFunc_t mainLoopUpdateFunc_o;

        BackLog m_backlog;

        ICriticalSection m_lock;

        static DCBP m_Instance;
    };
}