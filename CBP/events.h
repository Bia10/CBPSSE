#pragma once

namespace CBP
{
	enum Event {
		OnConfigLoad = 1,
		OnD3D11PreCreate,
		OnD3D11PostCreate,
		OnD3D11PostPostCreate,
		OnCreateWindowEx,
		OnMenuEvent,
		OnMessage,
		OnGameSave,
		OnGameLoad,
		OnFormDelete,
		OnRevert
	};

	typedef void (*EventCallback)(Event, void* );

	template <typename E, typename C>
	class EventTriggerDescriptor
	{
	public:
		EventTriggerDescriptor(E m_code, C callback):
			m_code(m_code), m_callback(callback)
		{}

		int m_code;
		C m_callback;
	};

	class IEvents
	{
		friend class MenuOpenCloseEventInitializer;
	public:
		typedef void(*inihookproc) (void);

		typedef EventTriggerDescriptor<Event, EventCallback> _EventTriggerDescriptor;

		typedef void (*LoadGameEventCallback) (SKSESerializationInterface*, UInt32, UInt32, UInt32);

		static bool Initialize();
		static void RegisterForEvent(Event a_code, EventCallback fn);
		static void RegisterForLoadGameEvent(UInt32 code, LoadGameEventCallback fn);
		static void TriggerEvent(Event a_code, void* args = nullptr);

		void TriggerEventImpl(Event m_code, void* args);

		FN_NAMEPROC("Events")
	private:
		IEvents() = default;

		static void MessageHandler(SKSEMessagingInterface::Message* message);
		static void FormDeleteHandler(UInt64 handle);
		static void SaveGameHandler(SKSESerializationInterface* intfc);
		static void LoadGameHandler(SKSESerializationInterface* intfc);
		static void RevertHandler(SKSESerializationInterface* intfc);

		std::unordered_map<Event, std::vector<_EventTriggerDescriptor>> m_events;
		std::unordered_map<UInt32, LoadGameEventCallback> m_loadgame_events;

		static IEvents m_Instance;
	};

}