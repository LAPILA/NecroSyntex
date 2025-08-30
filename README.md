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
<h2 align="center">👥 팀 구성 및 역할 (Team & Roles)</h2>

<table align="center">
  <thead>
    <tr>
      <th align="center" width="15%">Member</th>
      <th align="center" width="25%">Role</th>
      <th width="60%">Key Contributions</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td align="center"><strong>송태환</strong> (You)</td>
      <td align="center">💻<br><strong>Lead C++ Gameplay Programmer</strong></td>
      <td>
        <ul>
          <li><strong>코어 게임플레이 시스템 아키텍처 설계 및 C++ 구현</strong>
            <ul>
              <li>플레이어 캐릭터(이동, 생명력, 상태 관리)</li>
              <li>전투 컴포넌트(발사, 재장전, 무기 교체) 및 HUD 시스템 연동</li>
              <li>데이터 기반 무기 및 투사체 시스템</li>
            </ul>
          </li>
          <li><strong>멀티플레이어 네트워크 핵심 기능 구현</strong>
            <ul>
              <li>클라이언트 예측(Client-Side Prediction) 및 서버 되감기(Lag Compensation) 적용</li>
              <li>커스텀 액터(드론)의 부드러운 움직임을 위한 맞춤형 네트워크 동기화 로직</li>
            </ul>
          </li>
          <li><strong>애니메이션 시스템 구현</strong>
            <ul>
              <li>애니메이션 블루프린트(ABP)와 스테이트 머신을 활용한 캐릭터 애니메이션 시스템 구축</li>
            </ul>
          </li>
        </ul>
      </td>
    </tr>
    <tr>
      <td align="center"><strong>박두림</strong></td>
      <td align="center">🤖<br><strong>AI & Server Programmer</strong></td>
      <td>
          <ul>
            <li><strong>몬스터 AI 시스템 전체 구현</strong>
                <ul>
                  <li>비헤이비어 트리(Behavior Tree)와 AI 컨트롤러를 사용한 몬스터 행동 로직 설계</li>
                  <li>서버 권위적 몬스터 전투 및 피격, 죽음 판정 로직</li>
                </ul>
            </li>
            <li><strong>서버 백엔드 및 네트워크 관리</strong>
                <ul>
                  <li>Steam API 연동 및 데디케이티드 서버 환경 구축</li>
                </ul>
            </li>
          </ul>
      </td>
    </tr>
    <tr>
      <td align="center"><strong>박태혁</strong></td>
      <td align="center">🧩<br><strong>Gameplay & Systems Programmer</strong></td>
      <td>
        <ul>
          <li><strong>데이터 기반 도핑(버프/디버프) 시스템 아키텍처 설계 및 구현</strong></li>
          <li><strong>컴포넌트 기반의 동적 미션 시스템 구현</strong>
              <ul>
                <li>미션의 성공/실패 조건 판정 및 클라이언트 UI 동기화</li>
              </ul>
          </li>
          <li><strong>게임플레이 시스템 블루프rint 연동 및 확장</strong></li>
        </ul>
      </td>
    </tr>
    <tr>
      <td align="center"><strong>진교찬</strong></td>
      <td align="center">🎲<br><strong>Game Designer</strong></td>
      <td>
        <ul>
          <li>코어 게임 콘셉트 및 메카닉 기획</li>
          <li>시스템 디자인 (캐릭터 스킬, 미션, 게임 흐름)</li>
        </ul>
      </td>
    </tr>
    <tr>
      <td align="center"><strong>한준성</strong></td>
      <td align="center">🎨<br><strong>Level Artist</strong></td>
      <td>
        <ul>
          <li>게임 월드 레벨 레이아웃 및 환경 구성</li>
          <li>주요 오브젝트 및 애셋 배치</li>
        </ul>
      </td>
    </tr>
  </tbody>
</table>
