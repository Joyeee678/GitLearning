// Copyright (c) 2025 GitLearning Project
// Arc fitting implementation for bambustudio-style applications

#ifndef ARC_FITTING_H
#define ARC_FITTING_H

#include <vector>
#include <cmath>

namespace cura
{

/*!
 * 圆弧拟合计算类
 * Arc fitting calculation class for bambustudio-style applications
 * 
 * 这个类提供了圆弧拟合的核心计算功能，主要用于：
 * This class provides core arc fitting calculation functionality, mainly used for:
 * 1. 将直线段序列拟合为圆弧 (Fitting line segments into arcs)
 * 2. 计算圆弧参数（圆心、半径、起始角度、终止角度）(Calculate arc parameters)
 * 3. 圆弧插值和路径优化 (Arc interpolation and path optimization)
 */
class ArcFitting
{
public:
    /*!
     * 二维点结构
     * 2D Point structure
     */
    struct Point2D
    {
        double x, y;
        Point2D() : x(0), y(0) {}
        Point2D(double x_, double y_) : x(x_), y(y_) {}
        
        Point2D operator+(const Point2D& other) const { return Point2D(x + other.x, y + other.y); }
        Point2D operator-(const Point2D& other) const { return Point2D(x - other.x, y - other.y); }
        Point2D operator*(double scale) const { return Point2D(x * scale, y * scale); }
        double distance(const Point2D& other) const;
    };

    /*!
     * 圆弧参数结构
     * Arc parameters structure
     */
    struct ArcParameters
    {
        Point2D center;      // 圆心 (Center point)
        double radius;       // 半径 (Radius)
        double start_angle;  // 起始角度 (Start angle in radians)
        double end_angle;    // 终止角度 (End angle in radians)
        bool is_valid;       // 是否有效 (Whether the arc is valid)
        
        ArcParameters() : radius(0), start_angle(0), end_angle(0), is_valid(false) {}
    };

    /*!
     * 拟合结果结构
     * Fitting result structure
     */
    struct FittingResult
    {
        ArcParameters arc;
        double error;        // 拟合误差 (Fitting error)
        bool success;        // 是否成功 (Whether fitting was successful)
        
        FittingResult() : error(0), success(false) {}
    };

public:
    ArcFitting() = default;
    ~ArcFitting() = default;

    /*!
     * 使用三点法拟合圆弧
     * Fit arc using three points method
     * 
     * @param p1 第一个点 (First point)
     * @param p2 第二个点 (Second point) 
     * @param p3 第三个点 (Third point)
     * @return 圆弧参数 (Arc parameters)
     */
    static ArcParameters fitArcThreePoints(const Point2D& p1, const Point2D& p2, const Point2D& p3);

    /*!
     * 使用最小二乘法拟合圆弧
     * Fit arc using least squares method
     * 
     * @param points 点序列 (Point sequence)
     * @return 拟合结果 (Fitting result)
     */
    static FittingResult fitArcLeastSquares(const std::vector<Point2D>& points);

    /*!
     * 计算点到圆弧的距离
     * Calculate distance from point to arc
     * 
     * @param point 测试点 (Test point)
     * @param arc 圆弧参数 (Arc parameters)
     * @return 距离 (Distance)
     */
    static double distanceToArc(const Point2D& point, const ArcParameters& arc);

    /*!
     * 检查点序列是否适合圆弧拟合
     * Check if point sequence is suitable for arc fitting
     * 
     * @param points 点序列 (Point sequence)
     * @param tolerance 容差 (Tolerance)
     * @return 是否适合 (Whether suitable)
     */
    static bool isArcFittingSuitable(const std::vector<Point2D>& points, double tolerance = 0.1);

    /*!
     * 将圆弧插值为点序列
     * Interpolate arc into point sequence
     * 
     * @param arc 圆弧参数 (Arc parameters)
     * @param num_points 插值点数 (Number of interpolation points)
     * @return 插值点序列 (Interpolated point sequence)
     */
    static std::vector<Point2D> interpolateArc(const ArcParameters& arc, int num_points);

private:
    /*!
     * 计算三点形成的圆的圆心
     * Calculate center of circle formed by three points
     */
    static Point2D calculateCircleCenter(const Point2D& p1, const Point2D& p2, const Point2D& p3);
    
    /*!
     * 计算角度（考虑象限）
     * Calculate angle (considering quadrant)
     */
    static double calculateAngle(const Point2D& center, const Point2D& point);
    
    /*!
     * 角度标准化到 [0, 2π]
     * Normalize angle to [0, 2π]
     */
    static double normalizeAngle(double angle);
};

} // namespace cura

#endif // ARC_FITTING_H