﻿//
// ゲームグラフィックス特論宿題アプリケーション
//
#include "GgApp.h"

// プロジェクト名
#ifndef PROJECT_NAME
#  define PROJECT_NAME "ggsample04"
#endif

// オブジェクト関連の処理
#include "object.h"

// シェーダー関連の処理
#include "shader.h"

// 変換行列関連の処理
#include "matrix.h"

// Catmull-Rom スプライン
#include "spline.h"

// アニメーションの周期（秒）
constexpr auto cycle{ 5.0 };

//軸と回転角から単位余弦数を求める
//参照：授業スライド
void qmake(GLfloat *q, float x, float y, float z, float a)
{
  const float l{ x * x + y * y + z * z };
  if (l != 0.0f)
  {
    const float s{ sinf(a * 0.5f) / sqrtf(l)};
    q[0] = s * x ;
    q[1] = s * y;
    q[2] = s * z;
    q[3] = cosf(a * 0.5f);
  }
}

//球面線形補完する
//参照：授業スライド
void slerp(GLfloat* p, const float* q, const float* r, const float t)
{
  const float qr{ q[0] * r[0]
                + q[1] * r[1]
                + q[2] * r[2]
                + q[3] * r[3]};

  const float ss{ 1.0f - qr * qr };

  if (ss == 0.0)
  {
    p[0] = q[0];
    p[1] = q[1];
    p[2] = q[2];
    p[3] = q[3];
  }
  else
  {
    const float sp{ sqrt(ss) };
    const float ph{ acos(qr) };
    const float pt{ ph * t };
    const float t1{ sin(pt) / sp };
    const float t0{ sin(ph - pt) / sp };

    p[0] = q[0] * t0 + r[0] * t1;
    p[1] = q[1] * t0 + r[1] * t1;
    p[2] = q[2] * t0 + r[2] * t1;
    p[3] = q[3] * t0 + r[3] * t1;
  }

}

//単位四元数から回転変換行列を得る


//
// アプリケーション本体
//
int GgApp::main(int argc, const char* const* argv)
{
  // ウィンドウを作成する (この行は変更しないでください)
  Window window{ argc > 1 ? argv[1] : PROJECT_NAME };

  // 背景色を指定する
  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

  // プログラムオブジェクトの作成
  const auto program{ loadProgram(PROJECT_NAME ".vert", "pv", PROJECT_NAME ".frag", "fc") };

  // uniform 変数のインデックスの検索（見つからなければ -1）
  const auto mcLoc{ glGetUniformLocation(program, "mc") };

  // ビュー変換行列を mv に求める
  GLfloat mv[16];
  lookat(mv, 3.0f, 4.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

  // 頂点属性
  static const GLfloat position[][3]
  {
    { -0.9f, -0.9f, -0.9f },  // (0)
    {  0.9f, -0.9f, -0.9f },  // (1)
    {  0.9f, -0.9f,  0.9f },  // (2)
    { -0.9f, -0.9f,  0.9f },  // (3)
    { -0.9f,  0.9f, -0.9f },  // (4)
    {  0.9f,  0.9f, -0.9f },  // (5)
    {  0.9f,  0.9f,  0.9f },  // (6)
    { -0.9f,  0.9f,  0.9f },  // (7)
  };

  // 頂点数
  constexpr auto vertices{ static_cast<GLuint>(std::size(position)) };

  // 頂点インデックス
  static const GLuint index[]
  {
    0, 1,
    1, 2,
    2, 3,
    3, 0,
    0, 4,
    1, 5,
    2, 6,
    3, 7,
    4, 5,
    5, 6,
    6, 7,
    7, 4,
  };

  // 稜線数
  constexpr auto lines{ static_cast<GLuint>(std::size(index)) };

  // 頂点配列オブジェクトの作成
  const auto vao{ createObject(vertices, position, lines, index) };

  // 平行移動の経路
  static const float route[][3]
  {
    { -2.0f, -1.0f, -3.0f },
    {  0.0f, -2.0f, -2.0f },
    { -1.0f, -1.0f, -1.0f },
    { -0.5f, -0.5f, -0.5f },
    {  0.0f,  0.0f,  0.0f },
  };

  // 通過時間 (× cycle)
  static const float transit[]
  {
    0.0f,
    0.3f,
    0.5f,
    0.7f,
    1.0f,
  };

  // 通過地点の数
  constexpr auto points{ static_cast<int>(std::size(transit)) };

  // 経過時間のリセット
  glfwSetTime(0.0);

  // ウィンドウが開いている間繰り返す
  while (window)
  {
    // ウィンドウを消去する
    glClear(GL_COLOR_BUFFER_BIT);

    // シェーダプログラムの使用開始
    glUseProgram(program);

    // 時刻の計測
    const auto t{ static_cast<float>(fmod(glfwGetTime(), cycle) / cycle) };

    // 時刻 t にもとづく回転アニメーション
    GLfloat mr[16];                   // 回転の変換行列
    GLfloat q[4];//単位四元数
    
    // 【宿題】ここを解答してください（下の loadIdentity() を置き換えてください）
    loadIdentity(mr);
    //まず，軸と回転角から単位余弦数を求める
    // 次に，単位四元数を線形補完する
    //次に，単位四元数から回転変換行列を得る．


    // 時刻 t にもとづく平行移動アニメーション
    float location[3];                // 現在位置
    spline(location, route, transit, points, t);
    GLfloat mt[16];                   // 平行移動の変換行列
    translate(mt, location[0], location[1], location[2]);

    // モデル変換行列を mm に求め，
    // それとビュー変換行列 mv の積をモデルビュー変換行列 mw に求める
    GLfloat mm[16], mw[16];
    multiply(mm, mt, mr);             // モデル変換 mm ← 移動 mt × 回転 mr
    multiply(mw, mv, mm);             // モデルビュー変換 mw ← ビュー変換 mv × モデル変換 mm

    // 透視投影変換行列を mp に求め，
    // それとモデルビュー変換行列 mw の積をクリッピング座標系への変換行列 mc に求める
    GLfloat mp[16], mc[16];
    perspective(mp, 0.5f, window.getAspect(), 1.0f, 15.0f);
    multiply(mc, mp, mw);             // クリッピング座標系への変換行列 mc ← 投影変換 mp × モデルビュー変換 mw

    // uniform 変数 mc に変換行列 mc を設定する
    glUniformMatrix4fv(mcLoc, 1, GL_FALSE, mc);

    // 描画に使う頂点配列オブジェクトの指定
    glBindVertexArray(vao);

    // 図形の描画
    glDrawElements(GL_LINES, lines, GL_UNSIGNED_INT, 0);

    // 頂点配列オブジェクトの指定解除
    glBindVertexArray(0);

    // シェーダプログラムの使用終了
    glUseProgram(0);

    // カラーバッファを入れ替えてイベントを取り出す
    window.swapBuffers();
  }

  return 0;
}
