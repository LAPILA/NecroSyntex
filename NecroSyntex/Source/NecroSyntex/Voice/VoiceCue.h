#pragma once

/**
 *  “게임 전체” 음성 이벤트 열거형
 *  – 필요‑시 언제든 뒤에 항목을 추가해도 됩니다.
 */
UENUM(BlueprintType)
enum class EVoiceCue : uint8
{
	/* --- 세션/매치 흐름 --- */
	GameStart,
	MissionStart,
	MissionComplete,
	GameEnd,

	/* --- 전투 기본 --- */
	Fire,
	Reload,
	NoAmmo,
	EquipWeapon_Generic,
	EquipWeapon_Sniper,
	ThrowGrenade,
	LowHP,
	TakeHit,
	Death,
	Heal,                   // 치료 받음 / 자가 회복
	Pickup_Supply,          // 탄약·보급품 획득

	/* --- 스킬&특수 --- */
	Skill1,
	Skill2,

	/* --- AI/환경 --- */
	MonsterFirstSight,
	ObjectiveFail,

	Count        UMETA(Hidden)
};
