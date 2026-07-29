# cpp_Golf

DirectXによる3Dゴルフゲーム（開発中）

学校の先生から支給された描画フレームワーク（`vn_framework3D_2026_Adv`、DirectX使用）をベースに、行列演算・当たり判定・物理挙動といったゲームの土台部分を自作で実装しています。現時点ではゲームとしての完成（コース・スコア判定を含めた一連のプレイ）には至っておらず、以下のコアシステムの実装を進めている段階です。

## 技術的こだわりポイント

### 1. 自作行列演算ライブラリ（MatrixMath）
平行移動・拡大縮小・XYZ軸回転に加え、任意軸回転（ロドリゲスの回転公式）を自前で実装。地形の法線方向を軸にボールを傾けるなど、ゴルフゲーム特有の見た目の処理にも利用しています。ワールド座標変換用の `MultiplyPoint` と、方向ベクトル変換用の `MultiplyVector` を用途で使い分けている点もポイントです。

### 2. Raycastによる地面・接地判定
`CollisionManager::Raycast` で地形にレイを飛ばし、ボールやプレイヤーが地面に接しているかどうかの接地判定に使用。`MatrixMath::GetHitInfo` では指定座標での高さ・法線を取得し、坂道でのボールの傾きや斜面方向の力（`GetSlopeForce`）の計算に活用しています。

### 3. 形状別の衝突判定システム
`ICollisionShape` インターフェースを介して形状ごとの当たり判定を切り替え可能な構造にし、`SphereShape` で球体の衝突判定を実装。`CollisionManager` が複数の `Collider` を一括管理し、`OverlapSphere`（Unityの `Physics.OverlapSphere` 相当）による範囲判定にも対応しています。

### 4. 物理エンジンの実装（PhysicsBody）
重力による落下、転がり抵抗による減速、反発係数を用いた跳ね返り（バウンド）を自作で計算する物理挙動クラス。一定時間・一定速度以下で自動的に「静止」とみなす停止判定や、ぶつかった勢いに応じて衝突とみなすかどうかの閾値処理も実装しています。

## 自作コード範囲

`vn_framework3D_2026_Adv/public/` 以下（`vn_` プレフィックスの付いたファイル群を除く）

主なファイル：`MatrixMath.cpp/h` / `PhysicsBody.cpp/h` / `Collider.cpp/h` / `CollisionManager.cpp/h` / `SphereShape.cpp/h` / `ICollisionShape.h` / `Contact.h` / `Ball.cpp/h` / `BallsManager.cpp/h` / `HoleBase.cpp/h` / `CameraController.cpp/h` / `ShotInput.cpp/h` / `DirectionController.cpp/h` / `UIManager.cpp/h` など

## 支給フレームワーク

`vn_billboard` / `vn_character` / `vn_effect` / `vn_model` / `vn_object` / `vn_sprite` など `vn_` 系ファイルは学校の先生から提供された描画フレームワークです。

## 開発環境

- 言語：C++
- グラフィックスAPI：DirectX（DirectXMath使用）
- IDE：Visual Studio 2022

## 作者

ウラノチカゲ（浦野 慶夏） uranochikage@gmail.com
