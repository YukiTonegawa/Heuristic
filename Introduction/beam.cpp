#include "../lib/template.hpp"

constexpr int N = 26;
constexpr int D = 365;
std::array<int, N> C;
std::array<std::array<int, 26>, D> S;

struct state{
  std::array<int, D> contests; // 開催するコンテストの種類
  std::array<int, N> last;
  int score, next_move;
  state(): score(0){
    last.fill(0);
  }
  // d日目をkに変える
  state point_set(int d, int k){
    state res(*this);
    res.contests[d] = k;
    res.last[k] = d + 1;
    for(int i = 0; i < N; i++){
      long long diff = (d + 1) - last[i];
      res.score -= C[i] * diff;
    }
    res.score += S[d][k];
    return res;
  }
  int get_score(){
    return score;
  }
  int get_score_fixed(){
    return std::max(1000000 + score, 0);
  }
};

struct beam_search{
  static constexpr int height = 20;
  static constexpr int width = 20;
  void solve(state &s){
    // 1ステップではdepth * width個の状態を展開
    // depth * width個の状態に対して, 可能な遷移の数だけ計算する必要がある
    for(int i = 0; i < D; i++){
      std::vector<state> S{s}; // 状態 
      for(int h = 0; h < min(height, D - i); h++){
        std::vector<state> tmp = S;
        S.clear();
        for(state &s_ : tmp){
          for(int c = 0; c < N; c++){
            S.push_back(s_.point_set(i + h, c));
            if(h == 0) S.back().next_move = c;
          }
        }
        int sz = S.size();
        if(sz > width){
          std::nth_element(S.begin(), S.begin() + width, S.end(), [](state &a, state &b){return a.score > b.score;});
          S.erase(S.begin() + width, S.end());
        }
      }
      int max_score = -(1LL<<30), max_pos = -1;
      for(int j = 0; j < S.size(); j++) if(max_score < S[j].score) max_score = S[j].score, max_pos = j;
      s = s.point_set(i, S[max_pos].next_move);
    }
  }
};

int main(){
  io_init();
  int _t;
  std::cin >> _t; // 365
  range(i, 0, N) std::cin >> C[i];
  range(i, 0, D) range(j, 0, N) std::cin >> S[i][j];
  state s;
  beam_search bms;
  bms.solve(s);
  range(i, 0, D) std::cout << s.contests[i] + 1 << '\n';
}