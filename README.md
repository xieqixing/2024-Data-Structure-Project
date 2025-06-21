## 数据结构project



### 1.数据预处理

​	从openstreetmap网站上下载地图数据，本pj实现的功能是上海市的地图寻路功能，所以本次下载的道路数据的地理范围如下图所示：

![屏幕截图 2024-12-17 185202](C:/Users/xqx/Pictures/Screenshots/屏幕截图 2024-12-17 185202.png)

​	将下载出来的数据命名为test1，大小为682MB。由于数据格式是xml格式，使用C++中的tinyxml库读取的话速度过慢，所以需要将xml格式的数据先预处理转换成json格式。而且本project不涉及到地图渲染过程，所以可以忽略建筑信息以及区域信息等。

​	由于只需要完成寻路功能的实现，所以只需要获取xml文件中有关道路的信息和相应的点，将其存在一个新的json文件中即可。这一模块的代码放在了`src	`文件夹里，`preprocessing.h`定义了要从xml中读取的信息：

```c++
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
```

​	在读取了xml文件之后我们先利用`preprocessing(const char*)`读取所有的点和道路，在读取道路的时候对于其中每个点打上need标记。这样在`output_model()`中，就可以通过每一个节点中need是否为0，判断这个点是不是应该写入json文件，然后用`unordered_map`记录新的下标，实现道路节点下标的转变，完成文件压缩：

```c++
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
```

​	接着再将4种道路写入json文件：

```c++
for(auto u : m_ways_1){
    Json::Value way_1;
    for (auto v : u.nodes){
        way_1["nodes"].append(node_cnt_to_cnt[v]);
    }

    root["ways_1"] .append(way_1);       
}
//后面三种道路同样方式处理
```

​	写完预处理程序之后运用cmake进行编译，编译方法是在`build`文件夹里面打开终端，输入：

```
cmake ../src
cmake --build .
```

​	然后程序就会出现在`build`文件夹中的`Debug`文件夹里面了，然后将test1放到相同文件夹里面，运行程序。最终获取json文件`route_data`，大小压缩到了74.9MB，程序大概只需要10秒左右就能够完成读取。（由于test1大小太大，所以并没有上传）





### 2.后端处理：



#### 总体框架：

​	后端代码放在了`js`文件夹中，其代码用c++编写，然后用emscripten编译转化为js文件，以供前端调用，其中主要提供两个接口：

```c++
// emscripten对于c++代码的处理，为了让前端能调用这些函数
EMSCRIPTEN_BINDINGS()
{
    emscripten::function("load", &load);    // json文件的加载
    emscripten::function("get_ways", &get_ways);    // 最短路的计算
}

```

​	在`route_finding.h`文件中，定义了route_finding类，实现数据读取和最短路计算。其中读取文件用了jsoncpp中提供的函数。最短路计算使用了双向A星算法，计算最邻近的点则使用了boost库中的r树。每个函数的具体实现都写到了`route_finding.cpp`中。

​	具体分析一下两个接口函数的实现逻辑。首先是`load()`，其逻辑就是从`route_data.json`中读取点和道路的数据，并在这一过程中判断该点是不是道路口，用来在之后寻路的时候做路径压缩。之后调用`find_neighbors()`建立邻接表，同时在这一步计算出相邻两点之间的距离，调用`load_rtree()`将所有点存入R树，这样模型的加载就完成了。

```c++
void load(){
    Model.load("route_data.json");
}

void route_finding::load(const char* json){
    int cnt=0;

    Json::Reader json_reader;
    Json::Value root;

	// 省略一些代码,就是把节点和道路读进来
    
    find_neighbors();	// 找到与每个点相邻的点，建立邻接表

    load_rtree();		// 建立R树，用来找最邻近的点
}
```

​	而`get_ways()`函数的具体逻辑则如下：

```c++
string get_ways(double a, double b, double c, double d, int mode){
    // 运用R树找到最邻近的点作为起点和终点
    int start = Model.find_closest_node(a, b, mode);
    int end = Model.find_closest_node(c, d, mode);

    Model.bidirectional_a_star_search(start, end, mode);	//调用双向A星算法实现最短路搜寻

    string s = Model.output_final_way();	// 将找到的最短路转化为json格式返回给前端
    
    Model.clear_model(start, end);	// 清理对点进行的操作，以便多次寻路
    return s;
}
```





#### 数据结构使用：

​	除了常用的数据结构如`vector`、`unordered_map`之外，本pj还使用了boost库中的R树以及斐波那契堆这两个数据。其中R树是用来完成最邻近点查找，而斐波那契堆是用来代替最优先队列在双向A星算法中实现下一个点的选择。

​	在将数据全部加载到R树中后，对最邻近点的寻找方法如下所示：

```c++
int route_finding::find_closest_node(double x, double y, int mode) {
    std::vector<Value> result;

    int l, r, ret = -1;
    if(mode == 1) l = 0, r = 2;	// 不同的寻路模式：驾车 or 步行
    else l = 2, r = 3;

    for (int i = l; i <= r; i++) {
        result.clear(); // 每次查询前清空 result

        Point query(x, y);
        rtree[i].query(bgi::nearest(query, 1), std::back_inserter(result));

        if (!result.empty()) { // 确保查询结果非空
            int n_ret = result[0].second; // 最近的节点索引
            
            if(ret == -1) ret = n_ret;
            else{
                double dist_ret = (x - m_nds[ret].x) * (x - m_nds[ret].x) + (y - m_nds[ret].y) * (y - m_nds[ret].y);
                double dist_n_ret = (x - m_nds[n_ret].x) * (x - m_nds[n_ret].x) + (y - m_nds[n_ret].y) * (y - m_nds[n_ret].y);

                if (dist_ret > dist_n_ret) {
                    ret = n_ret; // 更新为更近的节点
                }
            }
        }
    }
    
    return ret; // 返回最近节点索引
}
```

​	这使得每次最邻近点的查找速度在0.001秒左右，可以忽略不记。

​	而将最优先队列换成斐波那契堆的原因在于：在双向A星算法中，需要时常更新堆中节点的键值，在STL提供的priority_queue中并未提供相应的接口，而boost库中提供了update方法来更新堆中节点的权值，而斐波那契堆本身在需要频繁更新键值的场景下就非常高效。考虑到这些优势，本pj使用了斐波那契堆，具体更新操作在函数`increase_list`中：

```c++
// 在入堆的时候保存句柄
auto node = &m_nds[cur];
auto handle = open_list1.push(node);    
handle_map1[node] = handle;

// 在需要更新的时候调用update方法
auto node = &m_nds[cur];
auto it = handle_map1.find(node);
if (it != handle_map1.end()) {
    open_list1.update(it->second, node);
}
```

​	此外，为了让emscripten正常编译代码，需要下载最新版本的boost库，否则会因为版本不兼容报错。





#### 距离与启发函数：

​	在openstreetmap中，是使用经纬度来存放点的位置的。而在地图寻路的时候，直接使用公式`sqrt((lon1 - lon2) * (lon1 - lon2) + (lat1 - lat2) * (lat1 - lat2))`来计算两点之间的距离不够准确，与实际的距离有所差距。所以本pj使用了Haversine公式来准确地计算出两点之间的实际距离：

```c++
double degToRad(double degree) {
    return degree * (M_PI / 180.0);
}

double route_finding::get_distance(int idx1, int idx2, double v){
    double lat1 = degToRad(m_nds[idx1].y);
    double lon1 = degToRad(m_nds[idx1].x);
    double lat2 = degToRad(m_nds[idx2].y);
    double lon2 = degToRad(m_nds[idx2].x);

    // Haversine公式
    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;
    double a = sin(dlat / 2) * sin(dlat / 2) + cos(lat1) * cos(lat2) * sin(dlon / 2) * sin(dlon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    
    // 距离 = 地球半径 * c
    return EARTH_RADIUS * c / v;
}
```

​	而为了在地图寻路中贴合实际，所以我们给不同类型的路附上了不同的速度，以便在寻路的时候选择更快速度的路。所以在计算两点实际距离的时候，我们直接除以相应道路的速度。

​	而在A星算法中，我们往往需要启发函数来评估从当前节点到目标节点的估计代价，为了实现这一目的，这一估计代价一定要小于该点到终点的实际代价，所以对于启发函数最后需要除以一个最大道路速度，以保持寻路算法的正确性。





#### 双向A星算法：

​	双向A星算法相比于普通的A星算法来说有两个难点：一个是启发函数的选择，另一个是结束条件的判断。

​	对于启发函数，其必须要满足两个条件：它不能大于该点到达终点的实际代价，该启发函数需要是有一致性的，所以正向启发函数与反向启发函数的代码如下所示：

```c++
// 正向的启发函数
double route_finding::p_f(int start, int end, int now, double v){
    return 0.5 *(get_distance(now, end, v) - get_distance(now, start, v) + get_distance(start, end, v));
}

// 反向的启发函数
double route_finding::p_r(int start, int end, int now, double v){
    return 0.5 *(get_distance(now, start, v) - get_distance(now, end, v) + get_distance(start, end, v));
}

/*
满足：
p_f (w) + p_r(w) = p_f (v) + p_r(v)
*/
```

​	而对于结束条件来说，我们需要记录当前找到的最短路径，设其代价为`u`，设此时两堆顶元素的`g_value + h_value`为`top_r`和`top_f`。当满足条件`top_r + top_f > u + p_r(end)`时，就找到了最短的路径。

​	解决这两个难点之后，我们就可以编写双向A星算法的代码了：

```c++
void route_finding::bidirectional_a_star_search(int start, int end, int mode){
    // 将Start 和 end 变为路口
    m_nds[start].crossing++;
    m_nds[end].crossing++;
    
    // 初始化start 和 end
    m_nds[start].visited[0] = true;
    m_nds[start].h_value0 = p_f(start, end, start, speed_limit);
    open_list0.push(&m_nds[start]);

    m_nds[end].visited[1] = true;
    m_nds[end].h_value1 = p_r(start, end, end, speed_limit);
    open_list1.push(&m_nds[end]);

    used_nd.push_back(start);
    used_nd.push_back(end);

    int now, n, idx0 = -1;
    double lowest_cost0 = 0.5 * std::numeric_limits<double>::max();
    double now_cost0, now_cost1;
    double p_r_t = p_r(start, end, end, speed_limit);
    
    while(!open_list0.empty() && !open_list1.empty() && iteration_count <= 600000){
        
        if(open_list1.empty()) break;	// 堆空就退出
        now_cost1 = open_list1.top()->g_value1 + open_list1.top()->h_value1;
        for(int i = iteration_count + 300; iteration_count < i && !open_list0.empty(); iteration_count++){

            if(lowest_cost0 + p_r_t < open_list0.top()->g_value0 + open_list0.top()->h_value0 + now_cost1){
                building_final_way(start, end, idx0);	// 满足退出条件，就建立最终路线
                return;
            }

            now = next_nd(0, mode);		// 目前堆顶元素
            m_nds[now].closed[0] = true;
            
            if(m_nds[now].visited[1]){	// 是否有新的路线出现
                if(m_nds[now].g_value0 + m_nds[now].g_value1 < lowest_cost0){
                    std::cout<<iteration_count<<std::endl;
                    lowest_cost0 = m_nds[now].g_value0 + m_nds[now].g_value1;
                    idx0 = now;
                }
            }else{
                increase_list(start, end, now, mode, 0);	// 加入新的点
            }
        }
        
		// 下半部分代码是反向A星算法，代码结构与上面类似，不做具体展示 
    }

    if(idx0 == -1) std::cout<<"could not find a way"<<std::endl;
    else building_final_way(start, end, idx0);
}
```

​	其中正向，反向A星搜索交替进行，每个方向的A星迭代300次之后转变到另一个方向，当满足退出条件的时候就建立最终道路，不满足的话就继续取堆顶元素搜索道路。





### 3.前端代码：

​	前端代码放在了`js`文件夹的`frontend`里面。而在前端中则是调用了高德地图API进行了地图的显示，实现的功能是从前端地图上选取两个点，将两个坐标传到后端，计算出最邻近的两个点，以及这两点之间的最短路径。将这些数据传回前端，并显示在地图上。

```html
<script>
        
    var Module = {
        onRuntimeInitialized: function () {
            Module.load()
        }
    };

</script>
<script src="test.js"></script>
```

​	首先先对数据进行加载，然后等待用户在前端进行选点：

```html
<script>
    // 允许用户在地图上选择两个点
    let markers = [];
    let polyline;
    map.on('click', function(e) {
        if (markers.length < 2) {
            const marker = new AMap.Marker({
                position: e.lnglat,
                map: map
            });
            markers.push(marker);
        }
    });

    function selectPoints() {
        if (markers.length === 2) {
            if (!isInShanghai(markers[0]) || !isInShanghai(markers[1])) {
                alert('无法寻路，请选择上海市内的点。');
                clearMap(); // 清除当前标记点和路线
                return; // 退出函数，不执行寻路
            }
           
            // 将高德地图坐标转换为GPS坐标
            const startGps = gcj02towgs84(markers[0].getPosition().lng, markers[0].getPosition().lat);
            const endGps = gcj02towgs84(markers[1].getPosition().lng, markers[1].getPosition().lat);

            // 调用get_ways()函数，传入两个点的GPS坐标
            getWays(startGps, endGps);
        }
    }

    function getWays(start, end) {
        // 调用C++代码中的get_ways()函数
        console.log(start)
        console.log(end)
        // 检查路径是否有效
            if (!path || path.length === 0) {
                alert('没有找到道路，请重新选点。');
                clearMap(); // 清除当前标记点和路线
                return; // 退出函数
            }

        var mode = parseInt(document.getElementById('modeSelect').value, 10);
        const pathJson = Module.get_ways(start[0], start[1], end[0], end[1], mode);
        const path = JSON.parse(pathJson);

        // 检查路径是否有效
        if (!path || path.length === 0) {
            alert('没有找到道路，请重新选点。');
            clearMap(); // 清除当前标记点和路线
            return; // 退出函数
        }
        
        // 绘制路径,并完成坐标转换
        var pathArray = path.map(p => wgs84togcj02(p.lon, p.lat));

        // 创建折线对象
        polyline = new AMap.Polyline({
            path: pathArray, // 路径
            strokeColor: "#00FF00", // 线颜色
            strokeWeight: 5, // 线宽
            strokeOpacity: 1 // 线透明度
        });

        // 将折线添加到地图上
        map.add(polyline);        
    }
    </script>
```

​	其中gcj02towgs84以及wgs84togcj02是高德地图坐标系以及GPS坐标系相互转换的函数，使用了来自CSDN的代码:https://blog.csdn.net/weixin_42776111/article/details/124290684。

​	为了实现多次寻路，需要清空选点以及路线：

```html
<script>
        function clearMap() {
            // 清除所有标记点
            markers.forEach(marker => {
                marker.setMap(null);
            });
            markers = [];

            // 清除路线
            if (polyline) {
                polyline.setMap(null);
                polyline = null;
            }
        }
</script>
```



### 4.额外功能实现：

#### 步行 or 驾车：

​	在前端给予用户选择的按钮：

```html
<div id="finding_road"> 
    <button onclick="selectPoints()">寻路</button> 
    <select id="modeSelect">
        <option value= 1>开车</option>
        <option value= 2>步行</option>
    </select>
    <button onclick="clearMap()">清除路线和标记</button>
</div>
```

​	并将该值存到`mode`变量传到后端，后端再将`mode`变量传给相应的函数。此时之前将道路分为四种类型就起到了作用。对于`m_ways[4]`，其中下标为0，1，2是驾车能通过的路线；下标为2，3的是步行能通过的路线，所以可以通过以下代码实现驾车以及步行两种寻路模式的选择：

```c++
if(mode == 1) l = 0, r = 2;
    else l = 2, r = 3;
    
for (int idx = l; idx <= r; idx++){
	for (auto& i : m_neighbors[idx][now]){
        //相应代码
    }
}
```





#### 路径压缩：

​	注意到openstreetmap中记录的点是用来画地图的，所以相邻的两点不一定都是道路口，而在寻路过程中，只有在道路口才能转到另一条路中。所以没有必要将所有节点都放到堆中，我们只需将道路口节点放进堆中就行了，其余节点只需要记录父节点是谁就行了，具体实现代码如下：

```c++
int route_finding::next_crossing(int& fa, int cur, int idx, double& new_g_val, int _i){
    if(cur == m_nds[fa].parent[_i]) return -1;	// 目前寻找的方向不是其父节点方向

    while(m_nds[cur].crossing < 2){ 	// 当目前节点不是道路口就继续循环
        int sz = m_neighbors[idx][cur].size();
        
        if(sz == 1)break;	//如果是死路就退出

        m_nds[cur].parent[_i] = fa;
        used_nd.push_back(cur);

        if(m_neighbors[idx][cur][0].idx != fa){		//沿道路继续寻找道路口
            fa = cur;
            cur = m_neighbors[idx][cur][0].idx;
            new_g_val += m_neighbors[idx][fa][0].distance;
        }else{
            fa = cur;
            cur = m_neighbors[idx][cur][1].idx;
            new_g_val += m_neighbors[idx][fa][1].distance;
        }
    }

    return cur;
}
```







### 5.最终效果：

​	先到`js`文件夹中打开终端，输入以下命令来编译程序：

```
emcc -O3 -sALLOW_MEMORY_GROWTH -Wno-deprecated-literal-operator -I D:\Boost\boost_1_87_0_b1\boost_1_87_0 jsoncpp/json_reader.cpp jsoncpp/json_value.cpp jsoncpp/json_writer.cpp route_finding.cpp main.cpp -o frontend/test.js -lembind --preload-file route_data.json -s USE_BOOST_HEADERS=1
```

​	运行方法则是到`frontend`文件夹中输入：

```
python -m http.server
```

​	然后在浏览器中输入127.0.0.1:8000，就可以运行该寻路project。使用方法是先选择开车还是步行模式，然后直接点击上海市里面随机两个点，之后点击寻路，路线就会直接在地图上显示。想要再次寻路只要点击清除路线和标记就行了。具体效果如下图：

![image-20241229132624021](C:/Users/xqx/AppData/Roaming/Typora/typora-user-images/image-20241229132624021.png)

​	看到加载数据用了10秒左右，找到最邻近点时间在0.001秒之内，基本可以忽略。然后在上海中多次随机选点，计算最短路，发现时间大部分都在0.06秒之内。

​	而如果用户选择了上海市以外的地点，程序会提示用户重新选点：

![image-20241229140744191](C:/Users/xqx/AppData/Roaming/Typora/typora-user-images/image-20241229140744191.png)

​	如果找不到道路，程序也会给出相应的提示：

![image-20241229140851560](C:/Users/xqx/AppData/Roaming/Typora/typora-user-images/image-20241229140851560.png)





### 6. 实验过程记录与总结

​	在这一次的pj的编写过程中，遇到的困难在于不断地拓展pj，大型项目中代码的debug，前端的编写以及双向A星算法正确性的证明。在一开始，该项目只是简单地用A星算法进行寻路，用遍历的方法找最邻近点，之后一步步地拓展功能：道路分类，双向A星，给道路加上不同的速度，算节点间真实距离，路劲压缩，添加R树和斐波那契堆……

​	在不断拓展的过程中，面向对象的编程方法给予了我许多帮助，让我的编程层次更加清晰，修改代码也更加方便不容易错。

​	在大型项目中debug也让人感到特别困难，由于程序正确性往往要在前端进行调用才能显示出来，而且因为用了emscripten编译c++代码，所以在前端的报错信息变得很难看懂。我只能逐步排查问题所在，直至找到错误并修改。如双向A星代码出问题时，我会将它与正确的A星代码所显示的结果对比，寻找问题。

​	因为之前没怎么学过前端，所以前端的编写在一开始让我很头疼。我花了挺久时间才搞明白如何在前端后端之间实现数据通信，以及前端的语法以及高德地图的API文档也花了我不少时间去学习。

​	至于双向A星算法的正确性证明，由于一开始我看的是CSDN上错误的双向A星算法证明，所以一直没有搞懂，后来看了普林斯顿大学有关双向A星算法的PPT，终于明白了正确的双向A星算法应该怎么编写。

​	对于这次project的总结，我毫无疑问从中学到了许多东西。我了解了如何从零开始编写一个拥有前端与后端的项目；我深入学习了一种快速的寻路算法——双向A星算法；我明白了json格式数据的优越性所在；我的代码编写水平得到了锻炼。虽然这个project还有许多可以优化的地方，但我对它感到很满意。
