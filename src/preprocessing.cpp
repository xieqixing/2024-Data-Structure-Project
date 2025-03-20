#include<iostream>
#include<unordered_map>
#include<fstream>
#include <iomanip>
#include"preprocessing.h"
#include "jsoncpp/json/json.h"
#include "tinyxml/tinyxml.h"


preprocessing::preprocessing(const char* osm){
    std::cout<<"find the xml?"<<std::endl;

    TiXmlDocument tinyXmlDoc(osm);
    tinyXmlDoc.LoadFile();
    TiXmlElement *root = tinyXmlDoc.RootElement();

    std::cout<<"find the xml"<<std::endl;
    //加载点到load_model的m_nodes里面
    TiXmlElement *nodeElement = root->FirstChildElement("node");
    std::unordered_map<std::string,int> node_id_to_cnt;  //id和vector中下标相对应，方便存到node里面
    for (; nodeElement; nodeElement = nodeElement->NextSiblingElement("node"))
    {
        int cnt = m_nodes.size();
        node_id_to_cnt[nodeElement->Attribute("id")]=cnt;

        m_nodes.emplace_back();
        m_nodes.back().x = atof(nodeElement->Attribute("lon")); //经度
        m_nodes.back().y = atof(nodeElement->Attribute("lat")); //纬度
        m_nodes.back().ref = cnt;
    }
    std::cout<<"load all the nodes"<<std::endl;

    //加载路到load_model的m_ways里面
    TiXmlElement *wayElement = root->FirstChildElement("way");
    for (; wayElement; wayElement = wayElement->NextSiblingElement("way"))
    {
        TiXmlElement *childTag = wayElement->FirstChildElement("tag");
        for (; childTag; childTag = childTag->NextSiblingElement("tag"))
        {
            std::string name = childTag->Attribute("k");
            std::string value = childTag->Attribute("v");

            if(name=="highway"){    //tag里表示它是路就加到路里面
                //排除一些不需要的道路：有轨公交专用道，避险车道，赛道，未知分类的道路，马道，建筑内部路，正在建的道路,未铺设路面的道路
                if(value == "bus_guideway" || value == "escape" || value == "raceway" || value == "road" || value == "busway" || value == "bridleway" || value == "corridor" || value == "path" || value == "proposed" || value == "construction" || value == "track") break;


                //只能车辆通过的高架
                if(value == "motorway" || value == "motorway_link"){
                    int cnt = m_ways_1.size();
                    m_ways_1.emplace_back();

                    TiXmlElement *childNode = wayElement->FirstChildElement("nd");
                    for (; childNode; childNode = childNode->NextSiblingElement("nd"))
                    {
                        std::string ref = childNode->Attribute("ref");
                        m_ways_1.back().nodes.push_back(node_id_to_cnt[ref]);//记录点在m_nodes中的下标就行了
                        m_nodes[node_id_to_cnt[ref]].need = 1;
                    }

                    break;
                }else if(value == "trunk" || value == "trunk_link"){
                    int cnt = m_ways_2.size();
                    m_ways_2.emplace_back();

                    TiXmlElement *childNode = wayElement->FirstChildElement("nd");
                    for (; childNode; childNode = childNode->NextSiblingElement("nd"))
                    {
                        std::string ref = childNode->Attribute("ref");
                        m_ways_2.back().nodes.push_back(node_id_to_cnt[ref]);//记录点在m_nodes中的下标就行了
                        m_nodes[node_id_to_cnt[ref]].need = 1;
                    }

                    break;
                }//只能行人走的路
                else if(value == "living_street" || value == "pedestrian" || value == "footway" || value == "steps"){
                    int cnt = m_ways_4.size();
                    m_ways_4.emplace_back();

                    TiXmlElement *childNode = wayElement->FirstChildElement("nd");
                    for (; childNode; childNode = childNode->NextSiblingElement("nd"))
                    {
                        std::string ref = childNode->Attribute("ref");
                        m_ways_4.back().nodes.push_back(node_id_to_cnt[ref]);//记录点在m_nodes中的下标就行了
                        m_nodes[node_id_to_cnt[ref]].need = 1;
                    }

                    break;
                }else if(value == "cycleway")break;
                else{
                    int cnt = m_ways_3.size();
                    m_ways_3.emplace_back();

                    TiXmlElement *childNode = wayElement->FirstChildElement("nd");
                    for (; childNode; childNode = childNode->NextSiblingElement("nd"))
                    {
                        std::string ref = childNode->Attribute("ref");
                        m_ways_3.back().nodes.push_back(node_id_to_cnt[ref]);//记录点在m_nodes中的下标就行了
                        m_nodes[node_id_to_cnt[ref]].need = 1;
                    }

                    break;
                }
            }
        }
    }

    std::cout<<"load all the ways"<<std::endl;

}

void preprocessing::output_model(const char* json){
    Json::Value root;

    std::unordered_map<int,int> node_cnt_to_cnt;    // 旧的下标对应新的下标
    int cnt = 0;    //新的点的下标
    for (auto u : m_nodes)
    {  
        if(u.need == 0) continue;   // 不是道路中的点就跳过
        Json::Value node;
        node["x"] = u.x;
        node["y"] = u.y;
        node_cnt_to_cnt[u.ref] = cnt;   
        
        root["nodes"].append(node);
        cnt++;
    }

    for(auto u : m_ways_1){
        Json::Value way_1;
        for (auto v : u.nodes){
            way_1["nodes"].append(node_cnt_to_cnt[v]);
        }

        root["ways_1"] .append(way_1);       
    }

    for(auto u : m_ways_2){
        Json::Value way_2;
        for (auto v : u.nodes){
            way_2["nodes"].append(node_cnt_to_cnt[v]);
        }

        root["ways_2"] .append(way_2);       
    }

    for(auto u : m_ways_3){
        Json::Value way_3;
        for (auto v : u.nodes){
            way_3["nodes"].append(node_cnt_to_cnt[v]);
        }

        root["ways_3"] .append(way_3);       
    }

    for(auto u : m_ways_4){
        Json::Value way_4;
        for (auto v : u.nodes){
            way_4.append(node_cnt_to_cnt[v]);
        }

        root["ways_4"] .append(way_4);       
    }

    Json::StreamWriterBuilder builder;
    std::ofstream outfile(json);

    std::string s = Json::writeString(builder, root);
    outfile << s;
    outfile.close();
}