/*
 * LPTlib
 * Lagrangian Particle Tracking library
 *
 * Copyright (c) 2012-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

#ifndef LPT_L_P_T_H
#define LPT_L_P_T_H

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <mpi.h>
#include "LPT_Args.h"

//forward declaration
namespace DSlib
{
class DSlib;
class Communicator;
class DecompositionManager;
class CommDataBlockManager;
}
namespace PPlib
{
class PPlib;
class ParticleData;
class StartPoint;
class PP_Transport;
}
namespace pm_lib
{
class PerfMonitor;
}

namespace LPT
{
//! @brief LPTのインターフェースルーチンを提供するクラス
//!
//! 計算に必要なパラメータ、他クラスへのポインタを保持し
//! それらを呼び出すことで粒子計算および必要な初期化、後処理を行なう
class LPT
{
private:
    //Singletonパターンを適用
    LPT() : initialized(false)
    {
        NumPolling   = 10000;
        PollingRatio = 0.8;
    }

    LPT(const LPT& obj);
    LPT& operator=(const LPT& obj);
    ~LPT(){}

public:
    static LPT* GetInstance()
    {
        static LPT instance;
        return &instance;
    }

private:
    bool  initialized;  //!< LPT_Initialize()が正常に終了したかどうかを表すフラグ
    int   NumPolling;   //!< データブロックの到着をポーリングする回数
    float PollingRatio; //!< データブロックの到着をポーリングする割合
                        //!< 要求したデータブロック数*PollingRatio < 到着したデータブロック数
                        //!< となったらMPI_Testを止めてMPI_Waitに切り替える

    std::vector<PPlib::StartPoint*> StartPoints;  //!<ソルバー側からLPT_SetStartPoint*() 経由で渡されてきた開始点のインスタンスを一時保存するコンテナ
                                                  //!<PPlibのインスタンス生成後にそっちに渡して中身は破棄する

    int CacheSize;                                //!< データブロックのキャッシュに使う領域の最大サイズ。単位はMByte

    int*   Mask;                                  //!< セルが固体(=0)か流体(=1)かを示すマスク配列
    double CurrentTime;                           //!< 現在時刻
    unsigned int CurrentTimeStep;                 //!< 現在のタイムステップ

    DSlib::DSlib* ptrDSlib;                       //!< DSlibのオブジェクトへのポインタ
    DSlib::DecompositionManager* ptrDM;           //!< DecompositionManagerのオブジェクトへのポインタ
    PPlib::PPlib*                ptrPPlib;        //!< PPlibのオブジェクトへのポインタ
    DSlib::Communicator*         ptrComm;         //!< Communicatorのオブジェクトへのポインタ

    REAL_TYPE RefLength;                          //!< 代表長さ
    REAL_TYPE RefVelocity;                        //!< 代表速度
    bool      OutputDimensional;                  //!<ファイル出力を有次元で行うかどうかのフラグ

    MPI_Win   window_for_rerun_flag;                       //!< データブロックの再送フラグを通信するためのwindows
    bool      work_for_rerun_flag;                         //!< データブロックの再送フラグを通信するためのワーク領域(粒子プロセスのrank0のみが使用)
    int       root_rank_for_rerun_flag;                    //!< データブロックの再送フラグのBcastを行うroot rank

public:
    //!  @brief 粒子計算に必要なパラメータを受け取り、DSlib, PPlib等のインスタンスを生成する
    //! 開始点のインスタンス生成およびデータ分散もこの関数内で処理する
    //! @param args [in] 引数構造体
    int LPT_Initialize(LPT_InitializeArgs args);

    //! @brief 粒子計算を行なう
    //! 返り値はエラーコード
    //! 0:正常
    //! @param args [in] 引数構造体
    int LPT_CalcParticleData(LPT_CalcArgs args);

    //! @brief  呼び出された時点での粒子データをファイルに出力する
    //! ファイル名は "LPT::OutputFileName + Rank番号" とし
    //! プロセス毎に異なるファイルに出力する
    int LPT_OutputParticleData(const int& TimeStep, const double& Time, float* v00);

    //! 粒子データを出力し、LPTの内部で保持している全ての開始点, 粒子, 流速データを破棄する
    int LPT_Post(void);

    //! 開始点のインスタンスを生成するためのインターフェース関数群
    bool LPT_SetStartPoint(REAL_TYPE Coord1[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime);
    bool LPT_SetStartPointMovingPoints(const int& NumPoints, REAL_TYPE* Coords, double* Times, double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime);
    bool LPT_SetStartPointLine(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int SumStartPoints, double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime);
    bool LPT_SetStartPointRectangle(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int NumStartPoints[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime);
    bool LPT_SetStartPointCuboid(REAL_TYPE Coord1[3], REAL_TYPE Coord2[3], int NumStartPoints[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime);
    bool LPT_SetStartPointCircle(REAL_TYPE Coord1[3], int SumStartPoints, REAL_TYPE Radius, REAL_TYPE NormalVector[3], double StartTime, double ReleaseTime, double TimeSpan, double ParticleLifeTime);

    //! MPI_Testによるポーリング回数を取得
    int GetNumPolling()
    {
        return this->NumPolling;
    }

    //! MPI_Testによるポーリング回数を設定
    void SetNumPolling(const int& NumPolling)
    {
        this->NumPolling = NumPolling;
    }

    //! MPI_Testによるポーリング比率を取得
    float GetPollingRatio()
    {
        return this->PollingRatio;
    }

    //! MPI_Testによるポーリング比率を設定
    void SetPollingRatio(const float& PollingRatio)
    {
        this->PollingRatio = PollingRatio;
    }

private:
    //! 送受信バッファの管理に使っていたCommDataBlockManagerのオブジェクトを削除する
    //
    //オブジェクトが保持する個々の領域はデストラクタ内でdeleteされる
    inline void DeleteCommBuff(std::list<DSlib::CommDataBlockManager*>* SendBuff, std::list<DSlib::CommDataBlockManager*>* RecvBuff);

    //! PPlib::Particlesに含まれる全ての粒子移動を再計算する
    inline void ReCalcParticlesAll(PPlib::PP_Transport& Transport, const double& deltaT, const int& divT, const double& CurrentTime, const int& CurrentTimeStep);

    //! 計算済の粒子をPPlib::Particlesに戻す
    inline void MoveBackToParticleContainer(std::vector<std::list<PPlib::ParticleData*>*>& calced, std::vector<PPlib::ParticleData*>& moved);

    //! 指定したデータブロックの到着を確認
    bool is_arrived(DSlib::CommDataBlockManager* RecvBuffManager, const int& polling_counter);
};
} // namespace LPT
#endif