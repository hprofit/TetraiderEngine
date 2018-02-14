/*
All event types are listed here. These are event names that will be 
turned into entries within the EventType enum
*/
REGISTER_EVENT_NAME(INVALID_EVENT) // Special case - this should always be first
REGISTER_EVENT_NAME(WINDOW_CLOSED)
REGISTER_EVENT_NAME(RESTART_LEVEL)
REGISTER_EVENT_NAME(EVENT_FPS_UPDATE)

REGISTER_EVENT_NAME(KEY_PRESSED)



REGISTER_EVENT_NAME(EVENT_OnCollide)
REGISTER_EVENT_NAME(EVENT_StaticsLoaded)
REGISTER_EVENT_NAME(EVENT_OnLevelInitialized)
REGISTER_EVENT_NAME(EVENT_OnEnemyHealthZero)
REGISTER_EVENT_NAME(EVENT_OnPlayerHealthZero)
REGISTER_EVENT_NAME(EVENT_OnHealthZero)
REGISTER_EVENT_NAME(EVENT_OnTakeDamage)
REGISTER_EVENT_NAME(EVENT_OnPlayerHeal)
REGISTER_EVENT_NAME(EVENT_FlipScaleX)
REGISTER_EVENT_NAME(EVENT_FlipScaleY)