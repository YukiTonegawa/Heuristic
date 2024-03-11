#include <random>

struct coord{
  int y_, x_;
  coord(const int y = 0, const int x = 0): y_(y), x_(x){}
};

static constexpr int H = 3;
static constexpr int W = 4;
static constexpr int end_turn = 4;

struct maze_state{
private:
  int points_[H][W] = {};
  int turn_ = 0;
  static constexpr int dx[4] = {1, -1, 0, 0};
  static constexpr int dy[4] = {0, 0, 1, -1};

public:
  coord pos_ = coord();
  int score = 0;
  maze_state(){}
  maze_state(const int seed){
    auto mt_ = std::mt19937(seed);
    pos_.y_ = mt_() % H;
    pos_.x_ = mt_() % W;
    for(int y = 0; y < H; y++){
      for(int x = 0; x < W; x++){
        if(y != pos_.y_ || x != pos_.x_){
          points_[y][x] = mt_() % 10;
        }
      }      
    }
  }
  void advance(const int action){
    pos_.x_ += dx[action];
    pos_.y_ += dy[action];
    int &p = points_[pos_.y_][pos_.x_];
    score += p;
    p = 0;
    turn_++;
  }
  std::vector<int> legal_actions()const{
    return {};
  }
  bool is_done()const{
    return turn_ == end_turn;
  }
};
