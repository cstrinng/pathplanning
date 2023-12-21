#include <chrono>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "turtlesim/msg/pose.hpp"
#include "geometry_msgs/msg/twist.hpp"

using namespace std::chrono_literals;
using std::placeholders::_1;

class MyNode : public rclcpp::Node {
 private:
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publihser_;
  rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr subscriber_;

  turtlesim::msg::Pose goal_;
  geometry_msgs::msg::Twist twist_;


  void timer_callback() const {
    publihser_->publish(twist_);
  }

  void topic_callback(const turtlesim::msg::Pose::SharedPtr pose) {
    auto dist = hypot(goal_.y - pose->y, goal_.x - pose->x);
    if (dist <= 0.2) {
      twist_.linear.set__x(0);
      twist_.angular.set__z(0);
      input();
    } else {
      twist_.linear.set__x(0.5);
      double angle = atan2(goal_.y - pose->y, goal_.x - pose->x) - pose->theta;
      twist_.angular.set__z(angle);
    }
  }

  void input() {
    std::cout << "목표 x 좌표를 입력하세요: ";
    std::cin >> goal_.x;
    std::cout << "목표 y 좌표를 입력하세요: ";
    std::cin >> goal_.y;
  }

 public:
  MyNode() : Node("my_node") {
    timer_ = this->create_wall_timer(100ms,
                                     std::bind(&MyNode::timer_callback, this));   //create wall timer를 통해 타이머 생성, 100ms는 publisher의 hz 결정-> 10hz
    publihser_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10); //create_publisher 함수를 통해 발행자 생성, publish할 토픽 이름과, 10은 메세지의 크기
    subscriber_ = this->create_subscription<turtlesim::msg::Pose>(
        "/turtle1/pose", 10, std::bind(&MyNode::topic_callback, this, _1));
    RCLCPP_INFO(this->get_logger(), "Initialized");
    input();
  }
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<MyNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}