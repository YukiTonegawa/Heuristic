#ifndef _LIMITED_VARIABLE_H_
#define _LIMITED_VARIABLE_H_
#include <limits>
#include <cassert>
#include <type_traits>
// 代入, 演算をした時に[min_val, max_val]に収まるようにassert
template<typename T, int id>
struct limited_variable{
private:
  T val_;
public:
  static_assert(std::is_arithmetic<T>::value); // assert 算術型
  using lv = limited_variable<T, id>;
  static T min_val, max_val;
  // 値のあり得る範囲を決める
  static void set_range(T min_val_, T max_val_){
    min_val = min_val_;
    max_val = max_val_;
  }
  limited_variable(T x_) : val_(x_){assert(min_val <= x_ && x_ <= max_val);}
  lv operator += (T x){return (*this) = (this->val_ + x);}
  lv operator -= (T x){return (*this) = (this->val_ - x);}
  lv operator *= (T x){return (*this) = (this->val_ * x);}
  lv operator /= (T x){return (*this) = (this->val_ / x);}
  lv operator %= (T x){return (*this) = (this->val_ % x);}
  lv operator += (lv &r){return (*this) = (this->val_ + r.val_);}
  lv operator -= (lv &r){return (*this) = (this->val_ - r.val_);}
  lv operator *= (lv &r){return (*this) = (this->val_ * r.val_);}
  lv operator /= (lv &r){return (*this) = (this->val_ / r.val_);}
  lv operator %= (lv &r){return (*this) = (this->val_ % r.val_);}
  // To do: <, ++など
  T val(){
    assert(min_val <= this->val_ && this->val_ <= max_val);
    return this->val_;
  }
  operator T()const{
    assert(min_val <= this->val_ && this->val_ <= max_val);
    return this->val_;
  }
};
template<typename T, int id>
T limited_variable<T, id>::min_val = std::numeric_limits<T>::min();
template<typename T, int id>
T limited_variable<T, id>::max_val = std::numeric_limits<T>::max();

#endif