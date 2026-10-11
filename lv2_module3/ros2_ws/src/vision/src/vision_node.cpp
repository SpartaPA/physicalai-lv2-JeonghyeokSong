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
        this->declare_parameter("lower", std::vector<int64_t>({100, 150, 50}));
        this->declare_parameter("upper", std::vector<int64_t>({130, 255, 255}));    
        this->declare_parameter("min_area", 1000);
        this->declare_parameter("blur_kernel_size", 3);

        subscriber_= this->create_subscription<sensor_msgs::msg::Image>(
            "/camera/camera/color/image_raw", rclcpp::SensorDataQoS().keep_last(1), std::bind(&VisionNode::get_image, this, std::placeholders::_1)
        );

        image_publisher_ = this->create_publisher<sensor_msgs::msg::Image>("/vision/image", rclcpp::SensorDataQoS().keep_last(1));
        mask_publisher_ = this->create_publisher<sensor_msgs::msg::Image>("/vision/mask", rclcpp::SensorDataQoS().keep_last(1));
        target_publisher_ = this->create_publisher<geometry_msgs::msg::PointStamped>("/target", rclcpp::SensorDataQoS().keep_last(1));
    }

    ~VisionNode() {}
private:

    void get_image( const sensor_msgs::msg::Image::ConstSharedPtr msg)
    {
        RCLCPP_INFO(this->get_logger(), "width: %d, height: %d, encoding: %s", msg->width, msg->height, msg->encoding.c_str());
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
                int cx = static_cast<int>(M.m10 / M.m00);
                int cy = static_cast<int>(M.m01 / M.m00);
                max_area = std::max(max_area, area);
                target.header = msg->header;
                target.point.x = cx;
                target.point.y = cy;
                target.point.z = area;
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
    }

    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscriber_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr image_publisher_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr mask_publisher_;
    rclcpp::Publisher<geometry_msgs::msg::PointStamped>::SharedPtr target_publisher_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<VisionNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}