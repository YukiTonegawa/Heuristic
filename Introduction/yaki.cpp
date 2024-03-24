#include "../lib/template.hpp"
#include "../lib/timer.hpp"
#include "../lib/temperature_schedular.hpp"
#include "../lib/probability.hpp"


// ビームサーチ
// chokudaiサーチ

constexpr int N = 26;
constexpr int D = 365;
std::array<int, N> C;
std::array<std::array<int, 26>, D + 1> S;
struct state{
  std::array<int, D + 1> contests; // 開催するコンテストの種類
  std::array<std::vector<int>, N> days; // コンテストの種類ごとの開催日
  int score;
  state(const std::array<int, D + 1> &_v): contests(_v), score(calc_score_all(_v)){
    for(int i = 1; i <= D; i++) days[contests[i]].push_back(i);
  }
  static int calc_score_all(const std::array<int, D + 1> &_v){
    int res = 0;
    std::array<int, N> last;
    last.fill(0);
    range(d, 1, D + 1){
      last[_v[d]] = d;
      res += S[d][_v[d]];
      range(i, 0, N) res -= C[i] * (d - last[i]);
    }
    return res;
  }
  std::vector<std::tuple<int, int, int, int>> history_point_set; // {d, a, b, c} d日目の a -> b 元のスコアc

  // d日目をkに変える
  void point_set(int d, int k, bool is_rollback = false){
    int pre = contests[d];
    if(!is_rollback) history_point_set.push_back({d, pre, k, score});
    if(pre == k) return; // 元と変わらない場合
    { 
      int x = 0, y = D + 1;
      int idx = lower_bound(allof(days[pre]), d) - days[pre].begin();
      if(idx) x = days[pre][idx - 1];
      assert(days[pre][idx] == d);
      days[pre].erase(days[pre].begin() + idx);
      idx = upper_bound(allof(days[pre]), d) - days[pre].begin();
      if(idx < days[pre].size()) y = days[pre][idx];
      assert(x < d && d < y);
      {
        long long diff = y - x - 1;
        score -= C[pre] * (diff * (diff + 1)) / 2;
      }
      {
        long long diff = d - x - 1;
        score += C[pre] * (diff * (diff + 1)) / 2;
      }
      {
        long long diff = y - d - 1;
        score += C[pre] * (diff * (diff + 1)) / 2;
      }
      score -= S[d][pre];
    }
    {
      int x = 0, y = D + 1;
      int idx = lower_bound(allof(days[k]), d) - days[k].begin();
      if(idx) x = days[k][idx - 1];
      days[k].insert(days[k].begin() + idx, d);
      idx = upper_bound(allof(days[k]), d) - days[k].begin();
      if(idx < days[k].size()) y = days[k][idx];
      assert(x < d && d < y);
      {
        long long diff = y - x - 1;
        score += C[k] * (diff * (diff + 1)) / 2;
      }
      {
        long long diff = d - x - 1;
        score -= C[k] * (diff * (diff + 1)) / 2;
      }
      {
        long long diff = y - d - 1;
        score -= C[k] * (diff * (diff + 1)) / 2;
      }
      score += S[d][k];
    }
    contests[d] = k;
  }
  void rollback_point_set(){
    auto [a, b, c, d] = history_point_set.back();
    history_point_set.pop_back();
    point_set(a, b, true);
  }
  void swap_two(int di, int dj){
    int a = contests[di];
    int b = contests[dj];
    point_set(di, b);
    point_set(dj, a);
  }
  void rollback_swap(){
    rollback_point_set();
    rollback_point_set();
  }
  int get_score(){
    return score;
  }
  int get_score_fixed(){
    return std::max(1000000 + score, 0);
  }
};

//struct safe_vector

struct simulated_annealing{
private:
  // パラメータ
  static constexpr int endms = 1950;
  int curms = 0;
  double Tc;
  static constexpr double p0 = 0.5; // prob point set
  temperature_scheduler_exp ts;
public:
  simulated_annealing(double _T0, double _T1): ts(_T0, _T1, endms){elapse<0>();}

  // endms以上になったら終了(false)
  bool update_time(){
    curms = elapse<0>();
    return curms < endms;
  }
  // 温度は時間に対して線形
  double update_temp(){
    return Tc = ts.get(curms);
  }
  std::tuple<int, int, int> get_neighbor(){
    static std::random_device seed_gen;
    static std::mt19937 engine(seed_gen());
    bool f = judge(p0);
    int random_day = engine() % D + 1;
    if(f){
      return {0, random_day, engine() % N};
    }else{
      return {1, random_day, engine() % D + 1};
    }
  }
  void solve(state &v){
    for(int i = 0; ; i++){
      // 32回に一回
      if((i & 31) == 0){
        if(!update_time()){
          return;
        }
        update_temp();
      }
      auto [t, a, b] = get_neighbor();
      if(t == 0){
        if(v.contests[a] == b) continue;
        int prescore = v.get_score();
        v.point_set(a, b);
        int dx = v.get_score() - prescore;
        if(!judge(p_move(dx, Tc))) v.rollback_point_set();
      }else{
        if(v.contests[a] == v.contests[b]) continue; 
        int prescore = v.get_score();
        v.swap_two(a, b);
        int dx = v.get_score() - prescore;
        if(!judge(p_move(dx, Tc))) v.rollback_swap();
      }
    }
  }
};

int main(){
  io_init();
  int _t;
  std::cin >> _t; // 365
  range(i, 0, N) std::cin >> C[i];
  range(i, 1, D + 1) range(j, 0, N) std::cin >> S[i][j];
  std::array<int, D + 1> _tmp;
  _tmp.fill(0);
  {
    // 初期解(貪欲)
    std::array<int, N> last;
    last.fill(0);
    range(d, 1, D + 1){
      int max_dx = -(1 << 30), max_j = 0;
      range(j, 0, N){
        int diff = C[j] * (d - last[j]) - S[d][j];
        if(diff > max_dx) max_dx = diff, max_j = j;
      }
      _tmp[d] = max_j;
      last[max_j] = d;
    }
  }
  state s(_tmp);
  simulated_annealing sm(2e3, 6e2);
  sm.solve(s);
  {
    // output
    range(i, 1, D + 1) std::cout << s.contests[i] + 1 << '\n';
  }
  //std::cout << s.get_score_fixed() << '\n';
}

// 安全なvector
// 