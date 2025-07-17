// Copyright (c) 2025 GitLearning Project
// Arc fitting example and test code

#include "ArcFitting.h"
#include <iostream>
#include <iomanip>

using namespace cura;

void printArcParameters(const ArcFitting::ArcParameters& arc)
{
    std::cout << "圆弧参数 (Arc Parameters):" << std::endl;
    std::cout << "  圆心 (Center): (" << std::fixed << std::setprecision(3) 
              << arc.center.x << ", " << arc.center.y << ")" << std::endl;
    std::cout << "  半径 (Radius): " << arc.radius << std::endl;
    std::cout << "  起始角度 (Start angle): " << arc.start_angle << " rad" << std::endl;
    std::cout << "  终止角度 (End angle): " << arc.end_angle << " rad" << std::endl;
    std::cout << "  有效性 (Valid): " << (arc.is_valid ? "是" : "否") << std::endl;
}

void printFittingResult(const ArcFitting::FittingResult& result)
{
    std::cout << "拟合结果 (Fitting Result):" << std::endl;
    std::cout << "  成功 (Success): " << (result.success ? "是" : "否") << std::endl;
    if (result.success)
    {
        std::cout << "  误差 (Error): " << std::fixed << std::setprecision(6) 
                  << result.error << std::endl;
        printArcParameters(result.arc);
    }
}

int main()
{
    std::cout << "=== BambuStudio 风格圆弧拟合计算示例 ===" << std::endl;
    std::cout << "=== BambuStudio-style Arc Fitting Example ===" << std::endl << std::endl;
    
    // 示例1：三点法圆弧拟合
    // Example 1: Three-point arc fitting
    std::cout << "1. 三点法圆弧拟合 (Three-point arc fitting)" << std::endl;
    ArcFitting::Point2D p1(0, 0);
    ArcFitting::Point2D p2(1, 1);
    ArcFitting::Point2D p3(2, 0);
    
    std::cout << "输入点 (Input points): " 
              << "(" << p1.x << "," << p1.y << "), "
              << "(" << p2.x << "," << p2.y << "), "
              << "(" << p3.x << "," << p3.y << ")" << std::endl;
    
    ArcFitting::ArcParameters arc = ArcFitting::fitArcThreePoints(p1, p2, p3);
    printArcParameters(arc);
    std::cout << std::endl;
    
    // 示例2：最小二乘法圆弧拟合
    // Example 2: Least squares arc fitting
    std::cout << "2. 最小二乘法圆弧拟合 (Least squares arc fitting)" << std::endl;
    std::vector<ArcFitting::Point2D> points;
    
    // 生成半圆弧上的点（带一些噪声）
    // Generate points on a semicircle (with some noise)
    double center_x = 5.0, center_y = 5.0, radius = 3.0;
    for (int i = 0; i <= 10; ++i)
    {
        double angle = i * M_PI / 10;  // 从0到π
        double x = center_x + radius * cos(angle) + 0.01 * (rand() % 21 - 10); // 添加小噪声
        double y = center_y + radius * sin(angle) + 0.01 * (rand() % 21 - 10);
        points.emplace_back(x, y);
    }
    
    std::cout << "输入了 " << points.size() << " 个点进行拟合" << std::endl;
    
    ArcFitting::FittingResult result = ArcFitting::fitArcLeastSquares(points);
    printFittingResult(result);
    std::cout << std::endl;
    
    // 示例3：拟合适用性检查
    // Example 3: Fitting suitability check
    std::cout << "3. 拟合适用性检查 (Fitting suitability check)" << std::endl;
    bool suitable = ArcFitting::isArcFittingSuitable(points, 0.1);
    std::cout << "点序列适合圆弧拟合: " << (suitable ? "是" : "否") << std::endl;
    std::cout << std::endl;
    
    // 示例4：圆弧插值
    // Example 4: Arc interpolation
    if (result.success)
    {
        std::cout << "4. 圆弧插值 (Arc interpolation)" << std::endl;
        std::vector<ArcFitting::Point2D> interpolated = ArcFitting::interpolateArc(result.arc, 5);
        
        std::cout << "插值生成 " << interpolated.size() << " 个点:" << std::endl;
        for (size_t i = 0; i < interpolated.size(); ++i)
        {
            std::cout << "  点 " << i << ": (" 
                      << std::fixed << std::setprecision(3)
                      << interpolated[i].x << ", " 
                      << interpolated[i].y << ")" << std::endl;
        }
        std::cout << std::endl;
    }
    
    // 示例5：距离计算
    // Example 5: Distance calculation
    if (result.success)
    {
        std::cout << "5. 点到圆弧距离计算 (Point to arc distance)" << std::endl;
        ArcFitting::Point2D test_point(center_x, center_y + radius);
        double distance = ArcFitting::distanceToArc(test_point, result.arc);
        
        std::cout << "测试点 (" << test_point.x << ", " << test_point.y 
                  << ") 到圆弧的距离: " << distance << std::endl;
    }
    
    std::cout << std::endl << "=== 圆弧拟合计算完成 ===" << std::endl;
    std::cout << "=== Arc fitting calculation completed ===" << std::endl;
    
    return 0;
}