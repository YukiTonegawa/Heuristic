#ifndef _TIMER_H__
#define _TIMER_H__
#include <chrono>

// ms単位で現在の時刻を取得
long long timems(){
  auto p = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(p).count();
}
// 初回呼び出し時からの経過時間(ms)
template<int id>
long long elapse(){
  static long long s = timems();
  return timems() - s;
}
#endif