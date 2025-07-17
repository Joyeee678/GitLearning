// Copyright (c) 2025 GitLearning Project
// Arc fitting implementation for bambustudio-style applications

#include "ArcFitting.h"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <limits>

namespace cura
{

double ArcFitting::Point2D::distance(const Point2D& other) const
{
    double dx = x - other.x;
    double dy = y - other.y;
    return std::sqrt(dx * dx + dy * dy);
}

ArcFitting::ArcParameters ArcFitting::fitArcThreePoints(const Point2D& p1, const Point2D& p2, const Point2D& p3)
{
    ArcParameters result;
    
    // 检查三点是否共线
    // Check if three points are collinear
    double cross_product = (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
    if (std::abs(cross_product) < 1e-10)
    {
        result.is_valid = false;
        return result;
    }
    
    // 计算圆心
    // Calculate center
    result.center = calculateCircleCenter(p1, p2, p3);
    
    // 计算半径
    // Calculate radius
    result.radius = result.center.distance(p1);
    
    // 计算起始和终止角度
    // Calculate start and end angles
    result.start_angle = calculateAngle(result.center, p1);
    result.end_angle = calculateAngle(result.center, p3);
    
    result.is_valid = true;
    return result;
}

ArcFitting::FittingResult ArcFitting::fitArcLeastSquares(const std::vector<Point2D>& points)
{
    FittingResult result;
    
    if (points.size() < 3)
    {
        result.success = false;
        return result;
    }
    
    // 使用最小二乘法拟合圆
    // Use least squares method to fit circle
    size_t n = points.size();
    
    // 计算质心
    // Calculate centroid
    Point2D centroid(0, 0);
    for (const auto& p : points)
    {
        centroid.x += p.x;
        centroid.y += p.y;
    }
    centroid.x /= n;
    centroid.y /= n;
    
    // 构建系数矩阵 A 和向量 b
    // Build coefficient matrix A and vector b
    // 圆的方程: (x-a)² + (y-b)² = r²
    // 展开为: x² + y² - 2ax - 2by + (a² + b² - r²) = 0
    // 设 A = -2a, B = -2b, C = a² + b² - r²
    // 则: x² + y² + Ax + By + C = 0
    
    double sum_x = 0, sum_y = 0, sum_x2 = 0, sum_y2 = 0, sum_xy = 0;
    double sum_x3 = 0, sum_y3 = 0, sum_x2y = 0, sum_xy2 = 0;
    
    for (const auto& p : points)
    {
        double x = p.x;
        double y = p.y;
        double x2 = x * x;
        double y2 = y * y;
        
        sum_x += x;
        sum_y += y;
        sum_x2 += x2;
        sum_y2 += y2;
        sum_xy += x * y;
        sum_x3 += x2 * x;
        sum_y3 += y2 * y;
        sum_x2y += x2 * y;
        sum_xy2 += x * y2;
    }
    
    // 解线性方程组
    // Solve linear equation system
    double A_matrix[2][2] = {
        {sum_x2, sum_xy},
        {sum_xy, sum_y2}
    };
    
    double b_vector[2] = {
        sum_x3 + sum_xy2,
        sum_x2y + sum_y3
    };
    
    // 计算行列式
    // Calculate determinant
    double det = A_matrix[0][0] * A_matrix[1][1] - A_matrix[0][1] * A_matrix[1][0];
    
    if (std::abs(det) < 1e-10)
    {
        result.success = false;
        return result;
    }
    
    // 使用克拉默法则求解
    // Solve using Cramer's rule
    double A = (b_vector[0] * A_matrix[1][1] - b_vector[1] * A_matrix[0][1]) / det;
    double B = (A_matrix[0][0] * b_vector[1] - A_matrix[1][0] * b_vector[0]) / det;
    
    // 计算圆心和半径
    // Calculate center and radius
    result.arc.center.x = -A / 2.0;
    result.arc.center.y = -B / 2.0;
    
    double C = (sum_x2 + sum_y2 - A * sum_x - B * sum_y) / n;
    double radius_squared = (A * A + B * B) / 4.0 - C;
    
    if (radius_squared <= 0)
    {
        result.success = false;
        return result;
    }
    
    result.arc.radius = std::sqrt(radius_squared);
    
    // 计算起始和终止角度
    // Calculate start and end angles
    result.arc.start_angle = calculateAngle(result.arc.center, points.front());
    result.arc.end_angle = calculateAngle(result.arc.center, points.back());
    
    // 计算拟合误差
    // Calculate fitting error
    double total_error = 0;
    for (const auto& p : points)
    {
        double dist = result.arc.center.distance(p);
        double error = std::abs(dist - result.arc.radius);
        total_error += error * error;
    }
    result.error = std::sqrt(total_error / n);
    
    result.arc.is_valid = true;
    result.success = true;
    
    return result;
}

double ArcFitting::distanceToArc(const Point2D& point, const ArcParameters& arc)
{
    if (!arc.is_valid)
    {
        return std::numeric_limits<double>::infinity();
    }
    
    double dist_to_center = arc.center.distance(point);
    return std::abs(dist_to_center - arc.radius);
}

bool ArcFitting::isArcFittingSuitable(const std::vector<Point2D>& points, double tolerance)
{
    if (points.size() < 3)
    {
        return false;
    }
    
    // 尝试拟合圆弧
    // Try to fit arc
    FittingResult result = fitArcLeastSquares(points);
    
    if (!result.success)
    {
        return false;
    }
    
    // 检查拟合误差是否在容差范围内
    // Check if fitting error is within tolerance
    return result.error <= tolerance;
}

std::vector<ArcFitting::Point2D> ArcFitting::interpolateArc(const ArcParameters& arc, int num_points)
{
    std::vector<Point2D> result;
    
    if (!arc.is_valid || num_points < 2)
    {
        return result;
    }
    
    double start_angle = arc.start_angle;
    double end_angle = arc.end_angle;
    
    // 确保角度差在正确的方向
    // Ensure angle difference is in correct direction
    double angle_diff = end_angle - start_angle;
    if (angle_diff < 0)
    {
        angle_diff += 2 * M_PI;
    }
    
    double angle_step = angle_diff / (num_points - 1);
    
    for (int i = 0; i < num_points; ++i)
    {
        double angle = start_angle + i * angle_step;
        Point2D point;
        point.x = arc.center.x + arc.radius * std::cos(angle);
        point.y = arc.center.y + arc.radius * std::sin(angle);
        result.push_back(point);
    }
    
    return result;
}

ArcFitting::Point2D ArcFitting::calculateCircleCenter(const Point2D& p1, const Point2D& p2, const Point2D& p3)
{
    // 使用三点确定圆心的公式
    // Formula for determining circle center from three points
    double ax = p1.x, ay = p1.y;
    double bx = p2.x, by = p2.y;
    double cx = p3.x, cy = p3.y;
    
    double d = 2 * (ax * (by - cy) + bx * (cy - ay) + cx * (ay - by));
    
    if (std::abs(d) < 1e-10)
    {
        // 三点共线的情况
        // Case where three points are collinear
        return Point2D(0, 0);
    }
    
    double ux = ((ax * ax + ay * ay) * (by - cy) + (bx * bx + by * by) * (cy - ay) + (cx * cx + cy * cy) * (ay - by)) / d;
    double uy = ((ax * ax + ay * ay) * (cx - bx) + (bx * bx + by * by) * (ax - cx) + (cx * cx + cy * cy) * (bx - ax)) / d;
    
    return Point2D(ux, uy);
}

double ArcFitting::calculateAngle(const Point2D& center, const Point2D& point)
{
    double dx = point.x - center.x;
    double dy = point.y - center.y;
    return std::atan2(dy, dx);
}

double ArcFitting::normalizeAngle(double angle)
{
    while (angle < 0)
    {
        angle += 2 * M_PI;
    }
    while (angle >= 2 * M_PI)
    {
        angle -= 2 * M_PI;
    }
    return angle;
}

} // namespace cura