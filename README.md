<h1 align="center">🚀 PVE TPS Project: NecroSyntex 🚀</h1>

> 다수의 플레이어가 협력하여 캐릭터 고유의 능력과 다양한 총기를 사용해 몬스터를 처치하고, 실시간으로 주어지는 미션을 클리어하는 3인칭 PVE 슈팅 게임입니다.
> 언리얼 엔진 5.3과 C++를 기반으로 멀티플레이어 환경의 핵심 기능들을 구현했습니다.

<br>

<p align="center">
  <img src="https://img.shields.io/badge/Unreal%20Engine-5.3-blue?style=for-the-badge&logo=unrealengine" alt="Unreal Engine 5.3">
  <img src="https://img.shields.io/badge/C++-17-blue?style=for-the-badge&logo=cplusplus" alt="C++17">
  <img src="https://img.shields.io/badge/Network-Multiplayer-brightgreen?style=for-the-badge" alt="Multiplayer">
</p>

---

## ✨ 주요 기능 (Core Features)

### 🌐 로비 및 게임 시작 (Lobby & Game Start)
<div align="center">

![Lobby](https://github.com/user-attachments/assets/4b9c34d8-a8d6-434b-b6f3-e47fd26e0a09)
*<p align="center">플레이어들이 로비에서 각 캐릭터와 능력(도핑)을 선택하고 함께 게임에 진입합니다.</p>*
</div>

게임에 접속한 플레이어들이 로비에서 만나 캐릭터와 능력(도핑)을 선택하고, 모든 인원이 준비되면 함께 게임에 진입하는 **멀티플레이 세션 흐름**을 구현했습니다. C++ 기반의 `PlayerState`와 `GameMode`를 통해 각 클라이언트의 선택 사항을 동기화하고, 모든 플레이어가 함께 게임을 시작합니다.

<br>

### 🔥 핵심 전투와 도핑 시스템 (Core Combat & Doping System)
<div align="center">

![Combat](https://github.com/user-attachments/assets/20f3042b-54a0-4d4d-9749-9f2f5c276fd2)
*<p align="center">다양한 총기와 도핑 스킬을 활용한 전략적인 PVE 전투.</p>*
</div>

다양한 총기를 사용한 **3인칭 슈팅(TPS)** 전투를 기반으로, '도핑' 시스템을 통해 **캐릭터의 능력치를 일시적으로 강화**하여 위기를 극복하는 전략적인 플레이가 가능합니다. 
모든 발사, 피격 판정, 버프 적용은 **서버 권위적**으로 처리되며, `LagCompensationComponent`를 통해 클라이언트의 랙을 보상하여 정확한 피격 판정을 보장합니다.

<br>

### 🎯 실시간 미션 시스템 (Real-time Mission System)
<div align="center">

![Mission](https://github.com/user-attachments/assets/7b187e56-e5af-4437-a645-a696a6f136fc)
*<p align="center">게임 플레이 중 실시간으로 미션을 부여받 모습.</p>*
</div>

단순한 몬스터 사냥을 넘어, 게임 플레이 도중 일부 구역에 진입하면 **실시간으로 다양한 미션**이 부여됩니다. `Component` 기반으로 설계된 미션 매니저가 서버에서 미션의 시작, 성공, 실패를 판정하고, 모든 클라이언트에게 **UI를 통해 목표를 동기화**하여 플레이어들이 공동의 목표를 향해 협력하도록 유도합니다.

---
