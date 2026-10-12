#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <geometry_msgs/msg/point_stamped.hpp>
#include <opencv2/opencv.hpp>
#include <cv_bridge/cv_bridge.hpp>

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>


class VisionNode : public rclcpp::Node
{
public:
    VisionNode() : Node("vision_node")
    {
        this->declare_parameter("lower", std::vector<int64_t>({98, 190, 25}));
        this->declare_parameter("upper", std::vector<int64_t>({118, 255, 230}));    
        this->declare_parameter("min_area", 1000);
        this->declare_parameter("blur_kernel_size", 3);
        stats_start_ = std::chrono::steady_clock::now();
        subscriber_= this->create_subscription<sensor_msgs::msg::Image>(
            "/camera/camera/color/image_raw", rclcpp::SensorDataQoS().keep_last(1), std::bind(&VisionNode::get_image, this, std::placeholders::_1)
        );

        image_publisher_ = this->create_publisher<sensor_msgs::msg::Image>("/vision/image", rclcpp::SensorDataQoS().keep_last(1));
        mask_publisher_ = this->create_publisher<sensor_msgs::msg::Image>("/vision/mask", rclcpp::SensorDataQoS().keep_last(1));
        target_publisher_ = this->create_publisher<geometry_msgs::msg::PointStamped>("/target", rclcpp::SensorDataQoS().keep_last(1));
        timer_ = this->create_wall_timer(std::chrono::milliseconds(100), std::bind(&VisionNode::log_info, this));
    }
    ~VisionNode() {}
private:

    void get_image( const sensor_msgs::msg::Image::ConstSharedPtr msg)
    {
        cv::Mat frame;
        cv::Mat blur, hsv, mask;

        std::vector<int64_t> lower = this->get_parameter("lower").as_integer_array();
        cv::Scalar lo(lower[0], lower[1], lower[2]);
        std::vector<int64_t> upper = this->get_parameter("upper").as_integer_array();
        cv::Scalar hi(upper[0], upper[1], upper[2]);


        geometry_msgs::msg::PointStamped target;

        frame = cv_bridge::toCvCopy(msg, "bgr8")->image;
        cv::GaussianBlur(frame, blur, cv::Size(this->get_parameter("blur_kernel_size").as_int(), this->get_parameter("blur_kernel_size").as_int()), 0);
        cv::cvtColor(blur, hsv, cv::COLOR_BGR2HSV);
        cv::inRange(hsv, lo, hi, mask);
        cv::morphologyEx(mask, mask, cv::MORPH_OPEN, cv::Mat(), cv::Point(-1, -1), 1);
        cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, cv::Mat(), cv::Point(-1, -1), 2);
        
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        
        
        double max_area = 0;
        double min_area = this->get_parameter("min_area").as_int();

        for (const auto& c : contours)
        {
            double area = cv::contourArea(c);
            if (area < min_area)
            {
                continue;
            }
            if (area > max_area)
            {
                cv::Moments M = cv::moments(c);
                if (M.m00 <= 0) { continue; }
                double cx = static_cast<double>(M.m10 / M.m00);
                double cy = static_cast<double>(M.m01 / M.m00);
                max_area = std::max(max_area, area);
                
                double ex = (cx-msg->width/2.0)/(msg->width/2.0);
                double ey = (cy-msg->height/2.0)/(msg->height/2.0);
                double z = area / (msg->width*msg->height);
                
                target.header = msg->header;
                target.point.x = ex;
                target.point.y = ey;
                target.point.z = z;
                detected_ = true;
            }    
        }
        
        sensor_msgs::msg::Image::SharedPtr image_msg (cv_bridge::CvImage(msg->header, "bgr8", frame).toImageMsg());
        sensor_msgs::msg::Image::SharedPtr mask_msg (cv_bridge::CvImage(msg->header, "mono8", mask).toImageMsg());
        
        mask_publisher_->publish(*mask_msg);
        image_publisher_->publish(*image_msg);
        if (max_area > 0)
        {
            target_publisher_->publish(target);
        }

        ++processed_;
    }

    void log_info()
    {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - stats_start_).count();
        if (duration > 0)
        {
            RCLCPP_INFO(this->get_logger(), "FPS: %.2f detected: %s", processed_ * 1000.0 / duration, detected_ ? "true" : "false");
        }
        stats_start_ = now;
        processed_ = 0;
        detected_ = false;
    }

    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscriber_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr image_publisher_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr mask_publisher_;
    rclcpp::Publisher<geometry_msgs::msg::PointStamped>::SharedPtr target_publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    std::chrono::steady_clock::time_point stats_start_;
    int processed_ = 0;
    bool detected_ = false;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<VisionNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}