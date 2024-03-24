#ifndef _TEMPERATURE_SCHEDULAR_H_
#define _TEMPERATURE_SCHEDULAR_H_
#include <cassert>
#include <cmath>

// 指数スケジューリング
// t := 時刻を[0, 1]に正規化したもの
// temp = T0 ^ (1 - t) * T1 ^ t
// Tlimit_ は終了時刻(ms)
struct temperature_scheduler_exp{
  double T0, T1, Tlimit;
  temperature_scheduler_exp(double T0_, double T1_, double Tlimit_): T0(T0_), T1(T1_), Tlimit(Tlimit_){}
  double get(double elapse_ms){
    assert(0 <= elapse_ms && elapse_ms <= Tlimit);
    elapse_ms /= Tlimit;
    return std::pow(T0, 1 - elapse_ms) * std::pow(T1, elapse_ms);
  }
};
// 線形スケジューリング
// t := 時刻を[0, 1]に正規化したもの
// temp = (T1 - T0) * t
// Tlimit_ は終了時刻(ms)
struct temperature_scheduler_linear{
  double T0, T1, Tlimit;
  temperature_scheduler_linear(double T0_, double T1_, double Tlimit_): T0(T0_), T1(T1_), Tlimit(Tlimit_){}
  double get(double elapse_ms){
    assert(0 <= elapse_ms && elapse_ms <= Tlimit);
    elapse_ms /= Tlimit;
    return (T1 - T0) * elapse_ms;
  }
};
// diff_scoreが0以上     1 
//    0未満             e ^ {diff_score / Tcur}
template<typename T>
double p_move(T diff_score, double Tcur){
  return diff_score >= 0 ? 1 : std::exp((double)diff_score / Tcur);
}
#endif