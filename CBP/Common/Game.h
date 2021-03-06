#pragma once

#include <ctime>

#include <LinearMath/btVector3.h>

#include <skse64/NiLight.h>

namespace Game
{
    extern float* frameTimer;
    extern float* frameTimerSlow;

    namespace Debug
    {
        void Notification(const char* a_message, bool a_cancelIfQueued = true, const char* a_sound = nullptr);
    }

    void AIProcessVisitActors(const std::function<void(Actor*)> &a_func);

    SKMP_FORCEINLINE bool InPausedMenu()
    {
        auto mm = MenuManager::GetSingleton();
        return mm && mm->InPausedMenu();
    }

    namespace Node
    {

        SKMP_FORCEINLINE void Traverse(NiAVObject* parent, const std::function<void(NiAVObject*)> &a_func)
        {
            a_func(parent);

            auto node = parent->GetAsNiNode();
            if (!node)
                return;

            for (UInt16 i = 0; i < node->m_children.m_emptyRunStart; i++)
            {
                auto object = node->m_children.m_data[i];
                if (object)
                    Traverse(object, a_func);
            }
        }


        SKMP_FORCEINLINE bool Traverse2(NiAVObject* parent, const std::function<bool(NiAVObject*)> &a_func)
        {
            if (a_func(parent))
                return true;

            auto node = parent->GetAsNiNode();
            if (!node)
                return false;

            for (UInt16 i = 0; i < node->m_children.m_emptyRunStart; i++)
            {
                auto object = node->m_children.m_data[i];
                if (object) {
                    if (Traverse2(object, a_func))
                        return true;
                }
            }

            return false;
        }

        // skee64
        SKMP_FORCEINLINE bool TraverseBiped(TESObjectREFR* a_ref, const std::function<bool(bool, UInt32, NiNode*, NiAVObject*)> &a_func)
        {
            for (SInt32 k = 0; k <= 1; ++k)
            {
                auto weightModel = a_ref->GetBiped(k);
                if (weightModel && weightModel->bipedData)
                {
                    auto rootNode = a_ref->GetNiRootNode(k);

                    for (int i = 0; i < 42; ++i)
                    {
                        NiAVObject* node = weightModel->bipedData->unk10[i].object;
                        if (node)
                        {
                            if (a_func(k == 1, i, rootNode, node))
                                return true;
                        }
                    }
                    for (int i = 0; i < 42; ++i)
                    {
                        NiAVObject* node = weightModel->bipedData->unk13C0[i].object;
                        if (node)
                        {
                            if (a_func(k == 1, i, rootNode, node))
                                return true;
                        }
                    }
                }
            }

            return false;

        }

        SKMP_FORCEINLINE NiAVObject* Find(NiAVObject* parent, const std::function<bool(NiAVObject*)> &a_func)
        {
            if (a_func(parent))
                return parent;

            auto node = parent->GetAsNiNode();
            if (node)
            {
                for (UInt16 i = 0; i < node->m_children.m_emptyRunStart; i++)
                {
                    auto object = node->m_children.m_data[i];
                    if (object) {
                        auto res = Find(object, a_func);
                        if (res != nullptr)
                            return res;
                    }
                }
            }

            return nullptr;
        }

    }


    // skee64
    SKMP_FORCEINLINE TESObjectARMO* GetActorSkin(Actor* a_actor)
    {
        auto npc = DYNAMIC_CAST(a_actor->baseForm, TESForm, TESNPC);
        if (npc) {
            if (npc->skinForm.skin)
                return npc->skinForm.skin;
        }

        auto actorRace = a_actor->race;
        if (actorRace)
            return actorRace->skin.skin;

        if (npc) {
            actorRace = npc->race.race;
            if (actorRace)
                return actorRace->skin.skin;
        }

        return nullptr;
    }

    SKMP_FORCEINLINE float GetNPCWeight(TESNPC* a_npc)
    {
        if (a_npc->nextTemplate)
        {
            auto templ = a_npc->GetRootTemplate();
            if (templ)
                return templ->weight;
        }

        return a_npc->weight;
    }

    SKMP_FORCEINLINE float GetActorWeight(Actor* a_actor)
    {
        auto npc = DYNAMIC_CAST(a_actor->baseForm, TESForm, TESNPC);

        if (npc)
            return GetNPCWeight(npc);

        return CALL_MEMBER_FN(a_actor, GetWeight)();
    }

    // https://github.com/Ryan-rsm-McKenzie/CommonLibSSE/blob/master/include/RE/AI/ProcessLists.h
    class ProcessLists
    {
    public:

        static ProcessLists* GetSingleton();

        bool GuardsPursuing(Actor* a_actor);

        bool                                    runDetection;                                  // 001
        bool                                    showDetectionStats;                            // 002
        uint8_t                                 pad003;                                        // 003
        ActorHandle                             statdetect;                                    // 004
        bool                                    processHigh;                                   // 008
        bool                                    processLow;                                    // 009
        bool                                    processMHigh;                                  // 00A
        bool                                    processMLow;                                   // 00B
        uint16_t                                unk00C;                                        // 00C
        uint8_t                                 unk00E;                                        // 00E
        uint8_t                                 pad00F;                                        // 00F
        int32_t                                 numberHighActors;                              // 010
        float                                   unk014;                                        // 014
        uint32_t                                unk018;                                        // 018
        float                                   removeExcessDeadTimer;                         // 01C
        uint64_t                                movementSyncSema;                              // 020
        uint32_t                                unk028;                                        // 028
        uint32_t                                pad02C;                                        // 02C
        tArray<ActorHandle>                     highActorHandles;                              // 030
        tArray<ActorHandle>                     lowActorHandles;                               // 048
        tArray<ActorHandle>                     middleHighActorHandles;                        // 060
        tArray<ActorHandle>                     middleLowActorHandles;                         // 078
        tArray<ActorHandle>* allProcesses[4];                                                  // 090

    private:

        MEMBER_FN_PREFIX(ProcessLists);
        DEFINE_MEMBER_FN(_GuardsPursuing, uint32_t, 0x6D6D70, Actor* a_actor, int p2, char p3);
    };

    static_assert(offsetof(ProcessLists, highActorHandles) == 0x30);
    static_assert(sizeof(ProcessLists) == 0xB0);

    struct PositionPlayerEvent;
    struct BSGamerProfileEvent;

    // https://github.com/Ryan-rsm-McKenzie/CommonLibSSE/blob/master/include/RE/BSMain/Main.h
    class BSMain :
        BSTEventSink <PositionPlayerEvent>, // 00
        BSTEventSink <BSGamerProfileEvent>  // 08
    {
    public:
        virtual ~BSMain();

        virtual EventResult    ReceiveEvent(PositionPlayerEvent* evn, EventDispatcher<PositionPlayerEvent>* dispatcher) override;
        virtual EventResult    ReceiveEvent(BSGamerProfileEvent* evn, EventDispatcher<BSGamerProfileEvent>* dispatcher) override;

        static BSMain* GetSingleton();

        bool                         quitGame;                        // 010
        bool                         resetGame;                       // 011
        bool                         fullReset;                       // 012
        bool                         gameActive;                      // 013
        bool                         onIdle;                          // 014
        bool                         reloadContent;                   // 015
        bool                         freezeTime;                      // 016
        bool                         freezeNextFrame;                 // 017

    };

    static_assert(offsetof(BSMain, quitGame) == 0x10);
    static_assert(sizeof(BSMain) == 0x18);

    class Unk00
    {
    public:
        static Unk00* GetSingleton();

        void SetGlobalTimeMultiplier(float a_scale, bool a_unk);

    private:
        MEMBER_FN_PREFIX(Unk00);
        DEFINE_MEMBER_FN(_SetGlobalTimeMultiplier, void, 0xC078B0, float a_scale, bool a_unk);
    };


}

