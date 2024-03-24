#ifndef _PROBABILITY_H_
#define _PROBABILITY_H_

#include <random>

// 確率pの行動を行うか
bool judge(double p){
  static std::random_device seed_gen;
  static std::mt19937 engine(seed_gen());
  static constexpr unsigned inf = std::numeric_limits<unsigned>::max();
  return engine() < (double)inf * p;
}
#endif
