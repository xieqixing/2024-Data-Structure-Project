#pragma once

#include<vector>
#include<string>

class preprocessing{
public:

    struct Node{
        double x=0.0;   // 经度
        double y=0.0;   // 纬度
        int ref = 0;    // 记录点的下标
        int need = 0;   // 是道路中的点，而不是组成建筑的点
    };

    struct Way{
        std::vector<int> nodes;     // 记录xml文件中点的下标
    };


    preprocessing(const char*);     // 构造函数

    void output_model(const char*); // 写入函数


protected:
    std::vector<Node> m_nodes;      // 记录所有的点
    std::vector<Way> m_ways_1;      // 记录高速路
    std::vector<Way> m_ways_2;      // 记录高架路
    std::vector<Way> m_ways_3;      // 记录普通路
    std::vector<Way> m_ways_4;      // 记录步行街
};