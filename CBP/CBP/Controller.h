#pragma once

//#define _CBP_MEASURE_PERF

namespace CBP
{
    struct ControllerInstruction
    {
        enum class Action : uint32_t 
        {
            AddActor,
            RemoveActor,
            UpdateConfig,
            UpdateConfigAll,
            Reset,
            PhysicsReset,
            NiNodeUpdate,
            NiNodeUpdateAll,
            WeightUpdate,
            WeightUpdateAll,
            AddArmorOverride,
            UpdateArmorOverride,
            UpdateArmorOverridesAll,
            ClearArmorOverrides
        };

        Action m_action;
        Game::ObjectHandle m_handle{ 0 };
    };

    class SKMP_ALIGN(32) ControllerTask :
        public TaskDelegateFixed,
        public TaskQueueBase<ControllerInstruction>,
        protected ILog
    {
        typedef stl::unordered_set<Game::ObjectHandle> handleSet_t;

        class UpdateWeightTask :
            public TaskDelegate
        {
        public:
            UpdateWeightTask(Game::ObjectHandle a_handle);

            virtual void Run() override;
            virtual void Dispose() override;
        private:
            Game::ObjectHandle m_handle;
        };

    public:
        SKMP_DECLARE_ALIGNED_ALLOCATOR(32);

        ControllerTask();

        virtual void Run() override;

    protected:

        void CullActors();
        void ProcessTasks();

        bool m_ranFrame;
        float m_lastFrameTime;

        //PerfTimerInt m_pt{ 1000000 };

    private:

        SKMP_FORCEINLINE void UpdatePhase1();
        SKMP_FORCEINLINE void UpdateActorsPhase2(float a_timeStep);

        SKMP_FORCEINLINE uint32_t UpdatePhysics(Game::BSMain* a_main, float a_interval);

#ifdef _CBP_ENABLE_DEBUG
        SKMP_FORCEINLINE void UpdatePhase3();
#endif

        SKMP_FORCEINLINE uint32_t UpdatePhase2(
            float a_timeStep, 
            float a_timeTick, 
            float a_maxTime);

        SKMP_FORCEINLINE uint32_t UpdatePhase2Collisions(
            float a_timeStep,
            float a_timeTick, 
            float a_maxTime);

        void AddActor(Game::ObjectHandle a_handle);
        void RemoveActor(Game::ObjectHandle a_handle);
        //bool ValidateActor(simActorList_t::value_type &a_entry);
        void UpdateConfigOnAllActors();
        //void UpdateGroupInfoOnAllActors();
        void UpdateConfig(Game::ObjectHandle a_handle);
        void Reset();
        void PhysicsReset();
        void NiNodeUpdate(Game::ObjectHandle a_handle);
        void WeightUpdate(Game::ObjectHandle a_handle);
        void NiNodeUpdateAll();
        void WeightUpdateAll();
        void AddArmorOverrides(Game::ObjectHandle a_handle, Game::FormID a_formid);
        void UpdateArmorOverrides(Game::ObjectHandle a_handle);
        void UpdateArmorOverridesAll();
        void ClearArmorOverrides();

    public:
        void PhysicsTick(Game::BSMain* a_main, float a_interval);

        void ClearActors(bool a_noNotify = false, bool a_release = false);
        void ResetInstructionQueue();

        void ApplyForce(
            Game::ObjectHandle a_handle,
            uint32_t a_steps,
            const std::string& a_component, 
            const NiPoint3& a_force);

        void UpdateDebugRenderer();

        SKMP_FORCEINLINE const auto& GetSimActorList() const {
            return m_actors;
        };

        SKMP_FORCEINLINE auto& GetProfiler() {
            return m_profiler;
        }

        SKMP_FORCEINLINE void UpdateTimeTick(float a_val) {
            m_averageInterval = a_val;
        }

        SKMP_FORCEINLINE void SetMarkedActor(Game::ObjectHandle a_handle) {
            m_markedActor = a_handle;
        }

        SKMP_FORCEINLINE void ResetFrame(float a_frameTime) {
            m_ranFrame = false;
            m_lastFrameTime = a_frameTime;
        }

        FN_NAMEPROC("Controller")

    private:

        void GatherActors(handleSet_t& a_out);

        bool ApplyArmorOverrides(
            Game::ObjectHandle a_handle, 
            const armorOverrideResults_t& a_entry);

        bool BuildArmorOverride(
            Game::ObjectHandle a_handle, 
            const armorOverrideResults_t& a_in,
            armorOverrideDescriptor_t& a_out);

        void DoUpdateArmorOverrides(
            simActorList_t::value_type& a_entry,
            Actor* a_actor);

        SKMP_FORCEINLINE void DoConfigUpdate(
            Game::ObjectHandle a_handle, 
            Actor* a_actor, 
            SimObject& a_obj);

        SKMP_FORCEINLINE const char* GetActorName(Actor* a_actor) {
            return a_actor ? CALL_MEMBER_FN(a_actor, GetReferenceName)() : "nullptr";
        }

        simActorList_t m_actors;
        Game::ObjectHandle m_markedActor;

        float m_timeAccum;
        float m_averageInterval;

        Profiler m_profiler;
        //PerfTimerInt m_pt;
    };

    class ControllerTaskSim :
        public ControllerTask
    {
    public:

        ControllerTaskSim() :
            ControllerTask()
        {
        }

        virtual void Run() override;
    };

}